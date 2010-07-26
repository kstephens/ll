#ifndef __rcs_id__
#ifndef __rcs_id_ll_write_c__
#define __rcs_id_ll_write_c__
static const char __rcs_id_ll_write_c[] = "$Id: write.c,v 1.32 2008/05/24 20:56:34 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include "ll/floatcfg.h"
#include <string.h> /* strchr, strcat */
#include "ll/assert.h"


/********************************************************************/
/* circular reference detection protocol.
 *


(define x (cons 'a 'b))
x
(set-cdr! x x)
x

(define y (vector 'a 'b 'c 'd))
y
(vector-set! y 2 x)
y

(set-cdr! x y)
y

(vector-set! y 1 y)
y

(set-cdr! x x)
y


* Top-level output methods

  (write obj . port)
  (display obj . port)

These will default port to *current-output-port*.
Port may be wrapped with a <write-port> to handle
tracking of circular references.

write and display will dispatch to

  (%write-port obj port op)

where op is write or display.

%write-port gives the object a chance to elect for
circular reference detection.

Atoms (string, numbers, symbols, etc.) will call:

  (%output-string port str)
 
directly, while most sequences or other objects will call:

  (%write-object-deep port obj op)

%write-object-deep will scan the object for multiple references then
output objects by calling:

  (%write-port-deep obj port op)


*/
/********************************************************************/


/****************************************************************/
/* Main port interface. */

#define GET_PORT(I) \
ll_v port = ll_ARGC > (I) ? ll_ARGV[I] : ll_call(ll_o(current_output_port), _0())


ll_define_primitive(object, write, __1(obj, port), _0())
{
  GET_PORT(1);
  ll_call_tail(ll_o(_write_port), _3(ll_ARG_0, port, ll_o(write)));
}
ll_define_primitive_end


ll_define_primitive(object, display, __1(obj, port), _0())
{
  GET_PORT(1);
  ll_call_tail(ll_o(_write_port), _3(ll_ARG_0, port, ll_o(display)));
}
ll_define_primitive_end


/****************************************************************/
/* (newline) (write_char) */


ll_define_primitive(object, newline, __0(port), _0())
{
  GET_PORT(0);
  ll_call_tail(ll_o(_output_char), _2(port, ll_make_char('\n')));
}
ll_define_primitive_end


ll_define_primitive(char, write_char, __1(char, port), _0())
{
  GET_PORT(1);
  ll_call_tail(ll_o(_output_char), _2(port, ll_SELF));
}
ll_define_primitive_end


#undef GET_PORT


/****************************************************************/
/* Scan and write circular references. */


#define PORT ll_SELF
#define OBJ ll_ARG_1
#define OP ll_ARG_2


ll_define_primitive(output_port, _write_object_deep, _3(port, obj, op), _0())
{
  /*
  fprintf(stderr, "output_port _write_object_deep %s %s %s\n",
	  ll_po(PORT),
	  ll_po(OBJ),
	  ll_po(OP));
  */

  /* If the target port is already a <write-port>,
   * Just dispatch to (%write-object-deep SELF object op),
   * This <write-port> is either in 'scan or 'write mode.
   */
  if ( ll_EQ(ll_TYPE(PORT), ll_type(write_port)) ) {
    ll_call_tail(ll_o(_write_object_deep), _3(PORT, OBJ, OP));
  }
  /* Otherwise,
   *
   * This is an top-level entry via:
   *   (display obj . port)
   * or
   *   (write obj .port)
   */
  else {
    /* Create a circular reference tracking port. */
    ll_v write_port = ll_call(ll_o(make), _2(ll_type(write_port), PORT));

    /* Scan object. */
    ll_call(ll_o(_write_port_scan_object), _3(write_port, OBJ, OP));

    /* Write object. */
    ll_call_tail(ll_o(_write_port_write_object), _3(write_port, OBJ, OP));
  }
}
ll_define_primitive_end


/****************************************************************/
/* <write-port> */


ll_define_primitive(write_port, initialize, _2(port, impl), _0())
{
  PORT = ll_call_super(ll_o(initialize), ll_f, _2(PORT, ll_ARG_1));

  ll_THIS->_mode = ll_s(scan);
  ll_THIS->_obj_ref_alist = ll_nil;

  ll_THIS->_obj_id_next = ll_BOX_fixnum(0);
  ll_THIS->_obj_id_alist = ll_nil;

  ll_return(PORT);
}
ll_define_primitive_end


/* Delegate to port-impl. */
ll_define_primitive(write_port, _output_string, _2(port, string), _0())
{
  /* Only if actually in write mode. */
  if ( ll_EQ(ll_THIS->_mode, ll_s(write)) ) {
    ll_call_tail(ll_o(_output_string), _2(ll_call(ll_o(port_impl), _1(PORT)), OBJ));
  }
}
ll_define_primitive_end


ll_define_primitive(write_port, _write_port_scan_object, _3(port, obj, op), _0())
{
  /*
  fprintf(stderr, "\nwrite_port _write_port_scan_object %s %s %s\n",
	  ll_po(PORT),
	  ll_po(OBJ),
	  ll_po(OP));
  */
  ll_THIS->_mode = ll_s(scan);

  ll_call_tail(ll_o(_write_object_deep), _3(PORT, OBJ, OP));
}
ll_define_primitive_end


ll_define_primitive(write_port, _write_port_write_object, _3(port, obj, op), _0())
{
  /*
  fprintf(stderr, "\nwrite_port _write_port_write_object %s %s %s\n",
	  ll_po(PORT),
	  ll_po(OBJ),
	  ll_po(OP));
  */

  ll_THIS->_mode = ll_s(write);

  /* Remove all obj_ref_alist entries where ref < 2. */
  {
    ll_v *x = &ll_THIS->_obj_ref_alist;
    while ( ! ll_nullQ(*x) ) {
      ll_v obj_ref = ll_car(*x);
      if ( ll_unbox_fixnum(ll_cdr(obj_ref)) < 1 ) {
	*x = ll_CDR(*x);
      } else {
	x = &ll_CDR(*x);
      }
    }
  }

  ll_call_tail(ll_o(_write_object_deep), _3(PORT, OBJ, OP));
}
ll_define_primitive_end


ll_define_primitive(write_port, _write_object_deep, _3(port, obj, op), _0())
{
  /*
  fprintf(stderr, "\nwrite_port _write_object_deep %s %s %s %s\n",
	  ll_po(PORT),
	  ll_po(OBJ),
	  ll_po(OP),
	  ll_po(ll_THIS->_mode));
  */

  ll_v chain_op = ll_undef;

  if ( ll_pairQ(OP) ) {
    chain_op = ll_car(OP);
    OP = ll_cdr(OP);
  } else {
    chain_op = ll_o(_write_port_deep);
  }

  /* Keep track of each object's reference count during
   * writes into the stream.
   */
  if ( ll_EQ(ll_THIS->_mode, ll_s(scan)) ) {
    if ( ! ll_unbox_boolean(ll_call(ll_o(_write_object_many_refsQ), _3(PORT, OBJ, ll_t))) ) {
      /* Scan object. */
      ll_call_tail(chain_op, _3(OBJ, PORT, OP));
    }
  }
  /* In write mode. */
  else if ( ll_EQ(ll_THIS->_mode, ll_s(write)) ) {
    ll_v obj_id = ll_assq(OBJ, ll_THIS->_obj_id_alist);

    /* Already written with ref id? */
    if ( ll_unbox_boolean(obj_id) ) {
      /* Output id of the object. */
      ll_format(PORT, "#~A#", 1, ll_cdr(obj_id));
    }
    /* Object may not have been written yet.
     */
    else {
      ll_v obj_ref = ll_assq(OBJ, ll_THIS->_obj_ref_alist);

      /* Object has more than one reference? */
      if ( ll_unbox_boolean(obj_ref) &&
	   ll_unbox_fixnum(ll_cdr(obj_ref)) > 1 ) {
	/* Assign an id to the object. */
	obj_id = ll_BOX_fixnum(ll_unbox_fixnum(ll_THIS->_obj_id_next) + 1);

	ll_THIS->_obj_id_next = obj_id;

	ll_THIS->_obj_id_alist = 
	  ll_cons(ll_cons(OBJ, obj_id), 
		  ll_THIS->_obj_id_alist);
	
	/* Prefix with the id. */
	ll_format(PORT, "#~A=", 1, obj_id);
      } 

      /* Write object. */
      ll_call_tail(chain_op, _3(OBJ, PORT, OP));
    }
  }
}
ll_define_primitive_end


ll_define_primitive(write_port, _write_object_many_refsQ, __2(port, obj, update), _0())
{
  ll_v result = ll_undef;

  int update = ll_unbox_boolean(ll_ARGC > 2 ? ll_ARG_2 : ll_f);
  ll_v obj_ref = ll_assq(OBJ, ll_THIS->_obj_ref_alist);
  int have_ref = 0;

  /* Has object been scanned before? */
  if ( (have_ref = ll_unbox_boolean(obj_ref)) &&
       ll_unbox_fixnum(ll_cdr(obj_ref)) > 1 ) {
    /* Skip it */
    result = ll_t;
  } else {
    ll_v ref_count;
    
    /* Increment the object's ref count. */
    if ( have_ref ) {
      result = ll_t;

      if ( ll_NE(ll_THIS->_mode, ll_s(scan)) || ! update ) {
	ll_return(result);
      }

      ref_count = ll_BOX_fixnum(2);

      ll_set_cdrE(obj_ref, ref_count);
    } else {
      result = ll_f;

      if ( ll_NE(ll_THIS->_mode, ll_s(scan)) || ! update ) {
	ll_return(result);
      }

      ref_count = ll_BOX_fixnum(1);

      ll_THIS->_obj_ref_alist = 
	ll_cons(ll_cons(OBJ, ref_count), 
		ll_THIS->_obj_ref_alist);

    }
    
    /*
    fprintf(stderr, "\n  %write-object-many-refs? %s %s => ref_count = %s => %s\n",
	    ll_po(ll_THIS->_mode),
	    ll_po(OBJ),
	    ll_po(ref_count),
	    ll_po(result));
    */
  }

  ll_return(result);
}
ll_define_primitive_end

#undef PORT
#undef OBJ
#undef OP


/****************************************************************
 * Internal interface via (%write-port obj port op)
 */

#define OBJ ll_SELF
#define PORT ll_ARG_1
#define OP ll_ARG_2


/****************************************************************
 * Constants and intrinsics. 
 */

ll_define_primitive(object, _write_port, _3(obj, port, op), _0())
{
  ll_call_tail(ll_o(_write_object_deep), _3(PORT, OBJ, OP));
}
ll_define_primitive_end


ll_define_primitive(boolean, _write_port, _3(obj, port, op), _0())
{
  if ( ll_EQ(OBJ, ll_f) ) {
    ll_write_string(PORT, "#f", 2); 
  } else {
    ll_write_string(PORT, "#t", 2);
  }   /* tail? */
}
ll_define_primitive_end


ll_define_primitive(undefined, _write_port, _3(obj, port, op), _0())
{
  ll_write_string(PORT, "#u", 2);   /* tail? */ 
}
ll_define_primitive_end


ll_define_primitive(unspecified, _write_port, _3(obj, port, op), _0())
{
  ll_write_string(PORT, "#<unspecified>", -1);   /* tail? */
}
ll_define_primitive_end


ll_define_primitive(eof_object, _write_port, _3(obj, port, op), _0())
{
  ll_write_string(PORT, "#e", 2);   /* tail? */
}
ll_define_primitive_end


ll_define_primitive(fixnum, _write_port, _3(obj, port, op), _0())
{
  char buf[sizeof(ll_v) * 8 * 2];
  if ( sizeof(ll_v) == sizeof(long long) ) {
    sprintf(buf, "%lld", (long long) ll_unbox_fixnum(OBJ));
  } else {
    sprintf(buf, "%ld", (long) ll_unbox_fixnum(OBJ));
  }
  ll_write_string(PORT, buf, (size_t) -1); 
}
ll_define_primitive_end


ll_define_primitive(flonum, _write_port, _3(obj, port, op), _0())
{
  char buf[32];
  sprintf(buf, "%." ll_STRINGTIZE(ll_FLO_DIGITS) "g", (double) ll_unbox_flonum(OBJ));
  if ( ! (strchr(buf, 'e') || strchr(buf, '.')) ) {
    strcat(buf, ".0");
  }
  ll_write_string(PORT, buf, (size_t) -1);   /* tail? */
}
ll_define_primitive_end


ll_define_primitive(ratnum, _write_port, _3(obj, port, op), _0())
{
  ll_call(ll_o(_write_port), _3(ll_THIS->_numerator, PORT, OP));
  ll_write_string(PORT, "/", 1);
  ll_call_tail(ll_o(_write_port), _3(ll_THIS->_denominator, PORT, OP));
}
ll_define_primitive_end


ll_define_primitive(locative, _write_port, _3(obj, port, op), _0())
{
  ll_format(PORT, "#<locative ~L>", 1, OBJ); /* tail? */
}
ll_define_primitive_end


ll_define_primitive(char, _write_port, _3(obj, port, op), _0())
{
  if ( ll_EQ(OP, ll_o(write)) ) {
    int c = ll_unbox_fixnum(ll_THIS->_value);
    switch ( c ) {
    case ' ':
      ll_write_string(PORT, "#\\space", -1);
      break;
      
    case '\n':
      ll_write_string(PORT, "#\\newline", -1);
      break;
      
    default: 
      {
	char buf[4] = "#\\x";
	buf[2] = c;
	ll_write_string(PORT, buf, 3); 
      }
    } /* tail? */
  } else {
    ll_call_tail(ll_o(_output_char), _2(PORT, OBJ));
  }
}
ll_define_primitive_end



ll_define_primitive(string, _write_port, _3(obj, port, op), _0())
{
  if ( ll_EQ(OP, ll_o(write)) ) {
    ll_write_string(PORT, "\"", 1);
    ll_call(ll_o(_output_string), _2(PORT, ll_call(ll_o(_string_escape), _1(OBJ))));
    ll_write_string(PORT, "\"", 1); /* tail? */
  } else {
    ll_call_tail(ll_o(_output_string), _2(PORT, OBJ));
  }
}
ll_define_primitive_end


/* Need this because <mutable-string> is a <mutable-sequence> before its a <string>. */
ll_define_primitive(mutable_string, _write_port, _3(obj, port, op), _0())
{
  if ( ll_EQ(OP, ll_o(write)) ) {
    ll_write_string(PORT, "\"", 1);
    ll_call(ll_o(_output_string), _2(PORT, ll_call(ll_o(_string_escape), _1(OBJ))));
    ll_write_string(PORT, "\"", 1); /* tail? */
  } else {
    ll_call_tail(ll_o(_output_string), _2(PORT, OBJ));
  }
}
ll_define_primitive_end


/****************************************************************/
/* semi-intrinsics */

ll_define_primitive(symbol, _write_port, _3(obj, port, op), _0())
{
  if ( ll_EQ(ll_THIS->_name, ll_f) ) {
    ll_call_tail(ll_o(_write_default), _3(OBJ, PORT, OP));
  } else {
    /* FIXME: Handle symbol escapes for non-terminating characters. */
    ll_call_tail(ll_o(_output_string), _2(PORT, ll_THIS->_name));
  }
}
ll_define_primitive_end


/********************************************************************/


/********************************************************************/
/* All sequences may generate circular references. */

ll_define_primitive(sequence, _write_port, _3(obj, port, op), _0())
{
  ll_call_tail(ll_o(_write_object_deep), _3(PORT, OBJ, OP));
}
ll_define_primitive_end


/********************************************************************/


ll_define_primitive(null, _write_port, _3(obj, port, op), _0())
{
  ll_write_string(PORT, "()", 2);   /* tail? */
}
ll_define_primitive_end



ll_define_primitive(pair, _write_port_deep, _3(obj, port, op), _0())
{
  /* If pair is (?a ?x), */
  if ( ll_pairQ(ll_THIS->_cdr) 
    && ll_nullQ(ll_cdr(ll_THIS->_cdr)) ) {
    /* Check for quote and quasiquote syntax. */

    ll_v x = ll_car(ll_THIS->_cdr);
    if ( ll_EQ(ll_THIS->_car, ll_s(quote)) ) {
      ll_write_string(PORT, "'", 1);
      ll_call_tail(ll_o(_write_port), _3(x, PORT, OP));
      /* NOTREACHED */
    } else if ( ll_EQ(ll_THIS->_car, ll_s(quasiquote)) ) {
      ll_write_string(PORT, "`", 1);
      ll_call_tail(ll_o(_write_port), _3(x, PORT, OP));
      /* NOTREACHED */
    } else if ( ll_EQ(ll_THIS->_car, ll_s(unquote)) ) {
      ll_write_string(PORT, ",", 1);
      ll_call_tail(ll_o(_write_port), _3(x, PORT, OP));
      /* NOTREACHED */
    } else if ( ll_EQ(ll_THIS->_car, ll_s(unquote_splicing)) ) {
      ll_write_string(PORT, ",@", 2);
      ll_call_tail(ll_o(_write_port), _3(x, PORT, OP));
      /* NOTREACHED */
    }
  }

  ll_write_string(PORT, "(", 1);

  /* Keep going until end of list. */
  {
    ll_v pair = ll_SELF;

    while ( ll_NE(pair, ll_nil) ) {
      ll_v car = ll_car(pair);
      ll_v cdr = ll_cdr(pair);
      
      ll_call(ll_o(_write_port), _3(car, PORT, OP));
      
      if ( ll_nullQ(cdr) ) {
	/* DONE. */
	break;
      }
      /* If:
       *   there are multiple references to the cdr,
       * OR 
       *   cdr is not a cons.
       */
      else if ( ll_unbox_boolean(ll_call(ll_o(_write_object_many_refsQ), _2(PORT, cdr))) ||
		! ll_pairQ(cdr)
		) {
	/* Dotted list. */
	ll_write_string(PORT, " . ", 3);
	
	ll_call(ll_o(_write_port), _3(cdr, PORT, OP));
	
	/* At end of list. */
	break;
      } else {
	/* Put separator. */
	ll_write_string(PORT, " ", 1);
	
	/* Process cdr. */
	pair = cdr;
      }
    }
  }

  ll_write_string(PORT, ")", 1);  /* tail? */
}
ll_define_primitive_end


/********************************************************************/
/* (CAR . CDR) .vs. (CAR ... OTHER) support. */


ll_define_primitive(pair, _write_cdr, _3(obj, port, op), _0())
{
  ll_write_string(PORT, " ", 1);
  ll_call(ll_o(_write_port), _3(ll_THIS->_car, PORT, OP));
  ll_call_tail(ll_o(_write_port), _3(ll_THIS->_cdr, PORT, OP));
}
ll_define_primitive_end


ll_define_primitive(null, _write_cdr, _3(obj, port, op), _0())
{
}
ll_define_primitive_end


ll_define_primitive(object, _write_cdr, _3(obj, port, op), _0())
{
  ll_write_string(PORT, " . ", 3);
  ll_call_tail(ll_o(_write_port), _3(OBJ, PORT, OP));
}
ll_define_primitive_end


/********************************************************************/


ll_define_primitive(vector, _write_port_deep, _3(obj, port, op), _0())
{
  size_t i, l;

  l = ll_THIS->_length;

  ll_write_string(PORT, "#(", 2);
  if ( l ) {
    i = 0;
    do {
      ll_v *v = ll_THIS->_array;
      ll_call(ll_o(_write_port), _3(v[i], PORT, OP));
      if ( ++ i < l ) {
	ll_write_string(PORT, " ", 1);
      } else {
	break;
      }
    } while ( 1 );
  }
  ll_write_string(PORT, ")", 1); /* tail? */
}
ll_define_primitive_end


/****************************************************************/
/* write-default:
 * For unformatted objects.
 */


ll_define_primitive(object, _write_port_deep, _3(obj, port, op), _0())
{
  ll_call_tail(ll_o(_write_default), _3(OBJ, PORT, OP));
}
ll_define_primitive_end



ll_define_primitive(object, _write_default, _3(obj, port, op), _0())
{
  ll_v port = PORT;

  ll_write_string(port, "#<", 2);
  {
    const char *str = ll_po(ll_TYPE(OBJ));
    ll_write_string(port, str, strlen(str));
  }
  ll_write_string(port, " ", 1);

  ll_call(ll_o(_write_default_contents), _3(OBJ, port, OP));

  ll_write_string(port, ">", 1); /* tail? */
}
ll_define_primitive_end


/*
** If the object is named
*/
ll_define_primitive(object, _write_default_contents, _3(obj, port, op), _0())
{
  ll_v port = PORT;
  const char *named = (void*) ll_po_(OBJ);
  if ( named ) {
    ll_write_string(port, named, strlen(named));
  } else {
    ll_format(port, "~W", 1, OBJ);
  } /* tail? */
}
ll_define_primitive_end


/**/
ll_define_primitive(constant, _write_default_contents, _3(obj, port, op), _0())
{
  ll_call_tail(ll_o(_write_port), _3(OBJ, PORT, OP));
}
ll_define_primitive_end


ll_define_primitive(method, _write_default_contents, _3(obj, port, op), _0())
{
  ll_v port = PORT;
  if ( ll_unbox_boolean(ll_call(ll_o(symbolQ), _1(ll_THIS->_code))) ) {
    ll_format(port, "~S", 1, ll_THIS->_code);
  } else {
    ll_format(port, "~W", 1, OBJ);
  } /* tail? */
}
ll_define_primitive_end


#undef OBJ
#undef PORT
#undef OP



/****************************************************************/


ll_INIT(write1, 257, "write support")
{
  /* */
  ll_bind_fluid(ll_s(Sprint_circleS), ll_t);
  ll_bind_fluid(ll_s(Sprint_baseS), ll_make_fixnum(10));
  ll_bind_fluid(ll_s(Sprint_radixS), ll_f);

  return 0;
}
