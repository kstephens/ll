#include "ll.h"

/* Activation Record (message) */

ll_define_primitive(message, _write_default_contents, _3(obj, port, op), _0())
{
  ll_format(ll_ARG_1, "op ~S type ~S argc ~S meth ~S", 
	    4,
	    ll_THIS->_op,
	    ll_THIS->_type,
	    ll_make_fixnum(ll_THIS->_argc),
	    ll_THIS->_meth
	    );
}
ll_define_primitive_end


ll_define_primitive(message, message_op, _1(ar), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_op);
}
ll_define_primitive_end


ll_define_primitive(message, message_argc, _1(ar), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_fixnum(ll_THIS->_argc));
}
ll_define_primitive_end


ll_define_primitive(message, message_type, _1(ar), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_type);
}
ll_define_primitive_end


ll_define_primitive(message, message_type_offset, _1(ar), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_type_offset);
}
ll_define_primitive_end


ll_define_primitive(message, message_meth, _1(ar), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_meth);
}
ll_define_primitive_end


ll_define_primitive(message, message_file, _1(ar), _1(no_side_effect, "#t"))
{
#if ll_USE_FILE_LINE
  const char *x = ll_THIS->_file ? ll_THIS->_file : "?";
  ll_return(ll_make_string(x, -1));
#else
  ll_return(ll_f);
#endif
}
ll_define_primitive_end


ll_define_primitive(message, message_line, _1(ar), _1(no_side_effect, "#t"))
{
#if ll_USE_FILE_LINE
  ll_return(ll_make_fixnum(ll_THIS->_line));
#else
  ll_return(ll_f);
#endif
}
ll_define_primitive_end


ll_define_primitive(message, print_frame, __1(self, port), _0())
{
  /* Print a stack backtrace */
  ll_v port = ll_ARGC >= 2 ? ll_ARG_1 : ll_undef;
  
  /* ll_THIS->_db_at_rtn, */

  port = ll_format(port, "  ~S: (~N", 
		   2, 
		   ll_make_fixnum(_ll_ar_sp_bottom - (ll_message*) ll_UNBOX_ref(ll_SELF)), 
		   ll_THIS->_op);
  {
    size_t i = 0, l = ll_THIS->_argc;
    ll_v meth, formals;

    meth = ll_THIS->_meth;

    formals = ll_NE(meth, ll_undef) ? ll_call(ll_o(method_formals), _1(meth)) : ll_undef;

    for ( i = 0; i < l; i ++ ) {
      ll_v formal;

      if ( ll_NE(formals, ll_undef) && ! ll_nullQ(formals) ) {
	if ( ll_unbox_boolean(ll_call(ll_o(symbolQ), _1(formals))) ) {
	  formal = ll_make_fixnum(i);
	  formals = ll_undef;
	} else {
	  formal = ll_car(formals);
	  formals = ll_cdr(formals);
	}
      } else {
	formal = ll_make_fixnum(i);
      }

      ll_format(port, " #|~S|#~F ~N", 
		2, 
		formal,
		ll_THIS->_argv[i]);
    }
  }

  ll_format(port, ")\n        ~A:~A", 
	    2, 
	    ll_call(ll_o(message_file), _1(ll_SELF)),
	    ll_call(ll_o(message_line), _1(ll_SELF))
	    );

  ll_format(port, "\n        ~N ~N\n", 
	    2, 
	    ll_THIS->_type,
	    ll_THIS->_meth
	    );
}
ll_define_primitive_end


/* (print-backtrace <ar> <depth>? <mark>? <port>?) */
ll_define_primitive(message, print_backtrace, __1(ar, depth), _0())
{
  /* Print a stack backtrace */
  ll_message *ar = (void*) ll_UNBOX_ref(ll_SELF);
  int n;
  ll_message *mark = ll_ARGC >= 3 ? ll_UNBOX_ref(ll_ARG_2) : 0;
  ll_v port = ll_ARGC >= 4 ? ll_ARG_3 : ll_undef;

  if ( ll_ARGC >= 2 ) {
    n = ll_unbox_fixnum(ll_ARG_1);
  } else {
    n = -1;
  }

  while ( n != 0 && ar != _ll_ar_sp_bottom ) {
    ll_format(port, ar == mark ? "=> " : "   ", 0);
    ll_call(ll_o(print_frame), _2(ll_make_ref(ar), port));
    ar ++;
    if ( n > 0 ) 
      n --;
  }
}
ll_define_primitive_end


ll_define_primitive(message, get_arg, _2(db, var), _0())
{
  /* Returns a locative to a argument on the stack, var can be an integer or a variable name. */

  int i = -1, l = ll_THIS->_argc;

  do {
    /* If a numeric idenitifier is specified, make sure its in range. */
    while ( ll_ISA_fixnum(ll_ARG_1) ) {
      i = ll_UNBOX_fixnum(ll_ARG_1);
      if ( ! (0 <= i && i < l) ) {
	ll_ARG_1 = _ll_range_error(ll_s(var), ll_ARG_1, 0, l - 1);
      }
      goto found_arg;
    }

    /* Must have been a formal name. */
    if ( i == -1 ) {
      ll_v meth, formals;
      
      /* Special keywords */
      if ( ll_EQ(ll_ARG_1, ll_s(_m)) ) {
	ll_return(ll_make_locative(&ll_THIS->_meth));
      } else if ( ll_EQ(ll_ARG_1, ll_s(_t)) ) {
	ll_return(ll_make_locative(&ll_THIS->_type));
      } else if ( ll_EQ(ll_ARG_1, ll_s(_d)) ) {
	ll_return(ll_make_locative(&ll_THIS->_db_at_rtn));
      } else if ( ll_EQ(ll_ARG_1, ll_s(_o)) ) {
	ll_return(ll_make_locative(&ll_THIS->_op));
      }
      
      meth = ll_THIS->_meth;
      
      formals = ll_NE(meth, ll_undef) ? ll_call(ll_o(method_formals), _1(meth)) : ll_undef;
      
      for ( i = 0; i < l; i ++ ) {
	ll_v formal;
	
	if ( ll_NE(formals, ll_undef) && ! ll_nullQ(formals) ) {
	  if ( ll_unbox_boolean(ll_call(ll_o(symbolQ), _1(formals))) ) {
	    if ( ll_EQ(formals, ll_ARG_1) ) {
	      goto found_arg;
	    }
	  } else {
	    formal = ll_car(formals);
	    if ( ll_EQ(formal, ll_ARG_1) ) {
	      goto found_arg;
	    }
	    formals = ll_cdr(formals);
	  }
	}
      }

      ll_ARG_1 = _ll_undefined_variable_error(ll_ARG_1);
    }
  } while (1);

 found_arg:
  ll_return(ll_make_locative(&ll_THIS->_argv[i]));
}
ll_define_primitive_end

