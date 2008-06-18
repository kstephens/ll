#ifndef __rcs_id__
#ifndef __rcs_id_ll_format_c__
#define __rcs_id_ll_format_c__
static const char __rcs_id_ll_format_c[] = "$Id: format.c,v 1.21 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include <string.h> /* strlen() */
#include <stdarg.h>
#include <ctype.h>

static
ll_v _ll_formatv(ll_v port, const char *f, size_t fsize, const ll_v *argv, size_t argc)
{
  const char *f_end = f + fsize;
  const char *b, *e;

  ll_v trace_save = ll_DEBUG(trace);
  ll_DEBUG_SET(trace, ll_make_fixnum(0));

  if ( ll_EQ(port, ll_undef) ) {
    port = ll_call(ll_o(current_output_port), _0());
    ll_assert_ref(port);
  }

  if ( ll_EQ(ll_TYPE(port), ll_type(mutable_string)) ) {
    port = ll_call(ll_o(make), _2(ll_type(output_port), port));
    ll_assert_ref(port);
  }

  e = f;
  while ( e < f_end ) {
    b = e;

    while ( e < f_end && *e != '~' ) {
      e ++;
    }

    if ( e - b ) {
      ll_write_string(port, b, e - b);
    }

    if ( e < f_end ) {
      if ( *e == '~' ) {
	ll_v x;

	e ++;
	switch ( *(e ++) ) {
#define GET (argc -- < 0 ? _ll_error(ll_re(format), 1, ll_s(reason), ll_s(too_many_formats)) : *(argv ++))

	case '%':
	  ll_call(ll_o(display), _2(ll_make_char('\n'), port));

	case 'A':
	  ll_call(ll_o(display), _2(GET, port));
	  break;
	  
	case 'S':
	  /*
	  {
	    ll_v obj = GET;
	    fprintf(stderr, "\nformat: (write %s %s)\n",
		    ll_po(obj),
		    ll_po(port));
	    ll_call(ll_o(write), _2(obj, port)); 
	  }
	  */
	  /* */ 
	  ll_call(ll_o(write), _2(GET, port));
	  /* */
	  break;

	case 'O':
	  x = GET;
	do_default:
	  ll_call(ll_o(_write_default), _3(x, port, ll_o(write))); 
	  break;
	 
	case 'F':
	  ll_call(ll_o(flush), _1(port));
	  break;

	  /* named object */
	case 'N':
	  {
	    const char *name;

	    x = GET;
	    name = (char*) ll_po_(x);
	    if ( name ) {
	      ll_write_string(port, name, (size_t) -1);
	    } else {
	      goto do_default;
	    }
	  }
	  break;

	  /* named object */
	case 'n':
	  {
	    const char *name;

	    x = GET;
	    name = (char*) ll_po(x);
	    if ( name ) {
	      ll_write_string(port, name, (size_t) -1);
	    } else {
	      goto do_default;
	    }
	  }
	  break;

	  /* weak ptr */
	case 'W':
	  {
	    x = GET;

	  do_weak_ptr:

	    if ( ll_ISA_ref(x) ) {
	      char buf[32];

	      sprintf(buf, "#p%ld", (unsigned long) ll_UNBOX_ref(x));
	      ll_write_string(port, buf, (size_t) -1);
	    } else {
	      ll_call(ll_o(write), _2(x, port)); 
	    }
	  }
	  break;

	  /* locative */
	case 'L': 
	  {
	    char buf[32];

	    sprintf(buf, "#l%ld", (unsigned long) ll_unbox_locative(GET));
	    ll_write_string(port, buf, (size_t) -1);
	  }
	  break;
	  
	default:
	  return(_ll_error(ll_re(format), 2, ll_s(reason), ll_s(bad_format_char), ll_s(char), ll_make_char(e[-1])));
	}
      }
    }
  }
 
  if ( argc ) {
    return(_ll_error(ll_re(format), 1, ll_s(reason), ll_s(too_many_arguments)));
  }

  ll_DEBUG_SET(trace, trace_save);

  return port;
}


ll_v ll_format(ll_v port, const char *f, int n, ...)
{
  ll_v *v;
  va_list vap;
  va_start(vap, n);

#if 0 /* ll_C_ARGS_ON_STACK */
  v = (void*) vap;
#else
  v = alloca(sizeof(v[0]) * n);
  {
    int i;

    v = alloca(sizeof(v[0]) * n);
    for ( i = 0; i < n; i ++ ) {
      v[i] = va_arg(vap, ll_v);
    }
  }
#endif
  va_end(vap);

  return _ll_formatv(port, f, strlen(f), v, n);
}


/***************************************************************************/

ll_define_primitive(output_port, llCformat, __2(self, format, args), _0())
{
  size_t l = ll_len_string(ll_ARG_1);
  const char *f = ll_ptr_string(ll_ARG_1);
  ll_return(_ll_formatv(ll_SELF, f, l, ll_ARGV + 2, ll_ARGC - 2));
}
ll_define_primitive_end


/***************************************************************************/

ll_define_primitive(char, llCformatCread_macroCweak, _2(char, port), _0())
{
  /* Read macro to handle #l<digits> and #p<digits> */
  unsigned long weak = 0;
  int loc = ll_UNBOX_char(ll_SELF) == 'l';
  ll_v c;

  c = ll_call(ll_o(peek_char), _1(ll_ARG_1));
  while ( ll_NE(c, ll_eos) ) {
    int cc = ll_unbox_char(c);

    if ( ! isdigit(cc) )
      break;

    weak *= 10;
    weak += cc - '0';

#if 0
    fprintf(stderr, " weak got char '%c', weak = %lu\n", (int) cc, (unsigned long) weak);
#endif

    ll_call(ll_o(read_char), _1(ll_ARG_1));
    c = ll_call(ll_o(peek_char), _1(ll_ARG_1));
  }
  
  if ( weak ) {
    if ( loc ) {
      c = ll_make_locative(weak);
    } else {
      c = ll_make_ref(weak);
    }
    
    c = ll_listn(2, ll_s(quote), c);
    c = ll_cons(c, ll_nil);
  } else {
    c = ll_nil;
  }

  ll_return(c);

}
ll_define_primitive_end


/***************************************************************************/

ll_INIT(format, 300, "read macros")
{
  /* Add our read macros */
  ll_call(ll_o(llCreadCdefine_macro_char), _2(ll_make_char('l'), ll_o(llCformatCread_macroCweak)));

  ll_call(ll_o(llCreadCdefine_macro_char), _2(ll_make_char('p'), ll_o(llCformatCread_macroCweak)));

  return 0;
}


/***************************************************************************/
