#ifndef __rcs_id__
#ifndef __rcs_id_ll_string_c__
#define __rcs_id_ll_string_c__
static const char __rcs_id_ll_string_c[] = "$Id: string.c,v 1.23 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include "call_int.h" /* __ll_call_tailv */
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>


/***************************************************************************/


const char *ll_cstr_string(ll_v x)
{
 again: {
    char *xp = ll_ptr_string(x);
    size_t xl = ll_len_string(x);
    if ( xp[xl] != '\0' ) {
      x = ll_make_copy_string(xp, xl);
      goto again;
    }

    /* Scan for embedded null. */
    {
      char *sp = xp;
      size_t sl = xl;
      while ( sl -- ) {
	if ( *(sp ++) == '\0' ) { 
	  x = _ll_error(ll_re(coersion_error),
			2,
			ll_s(value), x,
			ll_s(reason), ll_make_string("contains null", -1),
			ll_undef
			);
	  goto again;
	}
      }
    }
    return xp;
  }
}


#define _ll_VEC string
#define _ll_VEC_ELEM_TYPE char
#define _ll_VEC_COMPUTE_SIZE(v) (v ? strlen(v) : 0)
#define _ll_VEC_TERM 1
#define _ll_VEC_TERM_VALUE '\0'
#define _ll_VEC_UNBOX_VALUE(X) ll_unbox_char(X)
#define _ll_VEC_BOX_VALUE(X) ll_make_char(X)


#include "vec.c"


/***************************************************************************/

/* K&R A2.5.3 Character Constants */
static
const char C_char_escapes[] = "n\nt\tv\vb\br\rf\fa\a\\\\\"\"\'\'";


ll_define_primitive(string, _string_escape, _1(str), _1(no_side_effect,"#t"))
{
  const unsigned char *v = (const unsigned char*) ll_THIS->_array;
  size_t l = ll_THIS->_length;
  char *b = alloca(l * 4 + 1);
  char *p = b;

  while ( l -- ) {
    int c = *(v ++);

    {
      const char *m = C_char_escapes;

      while ( *m ) {
	if ( c == m[1] ) {
	  *(p ++) = '\\';
	  c = m[0];
	  goto got_c;
	}
	m += 2;
      }
    }

    if ( ! (0x20 <= c && c <= 0x7e) ) {
      /* Did not find escape char */

      *(p ++) = '\\';
      *(p ++) = "01234567"[(c >> 6) & 7];
      *(p ++) = "01234567"[(c >> 3) & 7];
      c = "01234567"[c & 7];
    }

  got_c:
    *(p ++) = c;
  }

  *p = '\0';

  ll_return(ll_make_copy_string(b, p - b));
}
ll_define_primitive_end


ll_define_primitive(string, _string_unescape, _1(str), _1(no_side_effect,"#t"))
{
  const unsigned char *v = (const unsigned char *) ll_THIS->_array;
  size_t l = ll_THIS->_length;
  char *b = alloca(l + 1);
  char *p = b;

#define PEEK() (l ? *v : EOF)
#define READ() (l ? (l --, *(v ++)) : EOF)

  while ( l ) {
    int c = PEEK();
    if ( c == '\\' ) {
      READ();
      switch ( c = READ() ) {

      case EOF:
      ll_return(ll_f); /* IMPLEMENT RAISE ERROR */
        break;

      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
	{
	  int d;
	  int ndigits = 0;
	  c = c - '0';
	  while ( ndigits < 3 && (d = PEEK()) && '0' <= d && d <= '7' ) {
	    READ();
	    c *= 8;
	    c += d - '0';
	    ndigits ++;
	  }
	}
	break;
	
      case 'x': case 'X':
	{
	  int d;
	  int ndigits = 0;
	  c = 0;
	  while ( ndigits < 2 && (d = PEEK()) != EOF ) {
	    if ( '0' <= d && d <= '9' ) {
	      READ();
	      c *= 16;
	      c += d - '0';
	    } else if ( 'a' <= d && d <= 'f' ) {
	      READ();
	      c *= 16;
	      c += d - 'a' + 10;
	    } else if ( 'A' <= d && d <= 'F' ) {
	      READ();
	      c *= 16;
	      c += d - 'A' + 10;
	    } else {
	      break;
	    }
	    ndigits ++;
	  }
	}
        break;
      
      default:
	{
	  const char *m = C_char_escapes;
	  while ( *m ) {
	    if ( c == m[0] ) {
	      c = m[1];
	      goto got_c;
	    }
	    m += 2;
	  }
	}
	ll_return(ll_f);
	break;
      }
    } else {
      c = READ();
    }

  got_c:
    *(p ++) = c;
  }

  *p = '\0';

  ll_return(ll_make_copy_string(b, p - b));
}
ll_define_primitive_end


ll_define_primitive(string, string__symbol, _1(str), _1(no_side_effect,"#t"))
{
  ll_call_tail(ll_o(make), _2(ll_type(symbol), ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(string, string__number, __1(str,radix), _1(no_side_effect,"#t"))
{
  const char *v = (const char *) ll_THIS->_array;
  size_t l = ll_THIS->_length;
  int is_rational = 0;
  ll_v numerator = ll_f;
  ll_v denominator = ll_f;
  int default_radix = 0;

  if ( ll_ARGC > 1 ) {
    default_radix = ll_unbox_fixnum(ll_ARG_1);
    if ( ! (0 < default_radix && default_radix <= 36) ) {
      ll_return(ll_f);
    }
  } else {
    default_radix = 10;
  }

 again:
  {
  double acc_d = 0;
  ll_v_word acc_i = 0;
  int acc_i_overflow = 0;
  int neg = 0;
  int radix = default_radix;
  int has_sign = 0;
  int has_digits = 0;
  int has_radix = 0;
  int has_hash_digit = 0;
  int has_dot = 0;
  int has_exp = 0;
  int has_exact = 0;
  int is_float = 0;
  int exponent = 0;
  int c;
  ll_v n;
  const char *first_digit = 0;
  const char *last_digit;

  if ( PEEK() == '+' ) {
    READ();
    has_sign = 1;
  } else if ( PEEK() == '-' ) {
    if ( ! first_digit ) first_digit = v;
    READ();
    has_sign = 1;
    neg = 1;
  } else
  if ( PEEK() == '#' ) {
    READ();
    while ( l ) {
      c = PEEK();
      switch ( c ) {
      case 'b': case 'B':
	radix = 2;
	/* FALL THROUGH */
      check_for_radix:
	if ( has_radix ) {
	  ll_return(ll_f);
	}
	has_radix = 1;
	READ();
	break;

      case 'o': case 'O':
	radix = 8;
	goto check_for_radix;

      case 'd': case 'D':
	radix = 10;
	goto check_for_radix;

      case 'x': case 'X':
	radix = 16;
	goto check_for_radix;

	/* #e exact specifier */
      case 'e': case 'E':
	if ( has_exact ) {
	  ll_return(ll_f);
	}
	has_exact = 'e';
	READ();
	break;

	/* #i inexact specifier */
      case 'i': case 'I':
	if ( has_exact ) {
	  ll_return(ll_f);
	}
	has_exact = 'i';
	READ();
	break;

	/* Unknown # specifier */
      default:
	ll_return(ll_f);
      }
    }
  }

  if ( ! first_digit ) 
    first_digit = v;
  last_digit = 0;

  while ( l ) {
    c = READ();
    /* [0-9] */
    if ( isdigit(c) ) {
      c -= '0';
    } else if ( c == '#' ) {
      c = 0;
      has_hash_digit = 1;
    } else if ( isalpha(c) ) {
      if ( islower(c) ) {
	/* [a-z] */
	c -= 'a' - 10;
      } else {
	/* [A-Z] */
	c -= 'A' - 10;
      }
      /* a '_' within a digit string is padding */
    } else if ( (has_digits || has_radix) && c == '_' ) {
      continue;
    } else if ( c == '/' ) {
      last_digit = v - 1;
      if ( is_rational || has_dot || has_exp || ! has_digits ) {
	ll_return(ll_f);
      }
      is_rational = 1;
      break;
    } else if ( c == '.' ) {
      if ( has_dot || has_radix )
	ll_return(ll_f);
      has_dot = 1;
      is_float = 1;
      continue;
    } else if ( tolower(c) == 'e' ) {
      if ( has_exp || has_radix )
	ll_return(ll_f);
      has_exp = 1;
      is_float = 1;
      {
	int exp_neg = 0;
	int e = 0;

	c = READ();
	if ( c == '-' ) {
	  exp_neg = 1;
	  c = READ();
	} else if ( c == '+' ) {
	  c = READ();
	}
	while ( l ) {
	  if ( isdigit(c) ) {
	    e *= 10;
	    e += c - '0';
	  } else {
	    return ll_f;
	  }
	}
	if ( exp_neg )
	  e = - e;

	exponent += e;
	break;
      }
#if 0
    } else if ( isspace(c) ) {
      continue;
#endif
    } else {
      ll_return(ll_f);
    }

    /* c is within the radix? */
    if ( 0 <= c && c < radix ) {
      has_digits = 1;
      acc_d *= radix;
      acc_d += c;
      {
	ll_v_word new_acc_i = (acc_i * radix + c);
	/* Check for int overflow. */
	if ( new_acc_i < acc_i ) {
	  acc_i_overflow = 1;
	} else {
	  acc_i = new_acc_i;
	}
      }
      if ( has_dot ) {
	exponent --;
      }
    } else {
      ll_return(ll_f);
    }
  }

  if ( ! last_digit ) 
    last_digit = v;

  /* If we didn't get any digits, it's not a number */
  if ( ! has_digits ) {
    ll_return(ll_f);
  }

  /* Handle sign */
  if ( neg ) {
    acc_d = - acc_d;
    acc_i = - acc_i;
  }

  /* Was it "15##"?  Make it float */
  if ( has_hash_digit ) {
    is_float = 1;
  }

  /* Is is a float */
  if ( is_float ) {
    acc_d *= pow(radix, exponent);
 
    /* fprintf(stderr, "string->number: %.22g == %.22g\n", (double) acc_d, (double) ll_unbox_flonum(ll_make_flonum(acc_d))); */

    n = ll_make_flonum(acc_d);
  } else {
    /* Check for fixnum overflow. */
    if ( ! acc_i_overflow && ll_MIN_fixnum <= acc_i && acc_i <= ll_MAX_fixnum ) {
      n = ll_make_fixnum(acc_i);
    } else {
      ll_v str = ll_make_copy_string(first_digit, last_digit - first_digit);
      // ll_format(ll_f, "  string->number: ~S => bignum ...\n", 1, str); 
      n = ll_call(ll_o(make), _3(ll_type(bignum), str, ll_BOX_fixnum(radix)));
      // ll_format(ll_f, "  string->number: ~S => bignum ~S\n", 2, str, n);
    }
  }
  
  if ( is_rational ) {
    if ( ll_EQ(numerator, ll_f) ) {
      numerator = n;
      // fprintf(stderr, "  string->number: numerator %.22g %lld\n", acc_d, (long long) acc_i); 
      goto again;
    } else {
      if ( l ) {
	ll_return(ll_f);
      }
      denominator = n;
      // fprintf(stderr, "  string->number: denominator %.22g %lld\n", acc_d, (long long) acc_i); 
      n = ll_make_ratnum(numerator, denominator);
      // ll_format(ll_f, "  string->number: ~S => ratnum ~S\n", 2, ll_SELF, n);
    }
  }

  ll_return(n);
  }
}
ll_define_primitive_end

#undef PEEK
#undef READ


/***************************************************************************/

#define _SGN(X) ((X) < 0 ? -1 : (X) > 0 ? 1 : 0)

ll_define_primitive(string, _string_cmp, _2(string1,string2), _1(no_side_effect,"#t"))
{
  size_t i, l;
  const char *v;
  _ll_typecheck(ll_type(string), &ll_ARG_1);
  l = ll_THIS_ISA(string, ll_ARG_1)->_length;
  v = ll_THIS_ISA(string, ll_ARG_1)->_array;
  for ( i = 0; i < ll_THIS->_length && i < l; i ++) {
    int c;
    if ( ! ( c = ll_THIS->_array[i] - v[i]) ) {
      ll_return(ll_BOX_fixnum(_SGN(c)));
    }
  }
  ll_return(ll_BOX_fixnum(_SGN(ll_THIS->_length - l)));
}
ll_define_primitive_end


ll_define_primitive(string, _string_cmp_ci, _2(string1, string2), _1(no_side_effect,"#t"))
{
  size_t i, l;
  const char *v;
  _ll_typecheck(ll_type(string), &ll_ARG_1);
  l = ll_THIS_ISA(string, ll_ARG_1)->_length;
  v = ll_THIS_ISA(string, ll_ARG_1)->_array;
  for ( i = 0; i < ll_THIS->_length && i < l; i ++) {
    int c;
    if ( ! (c = tolower(ll_THIS->_array[i]) - tolower(v[i])) ) {
      ll_return(ll_BOX_fixnum(_SGN(c)));
    }
  }
  ll_return(ll_BOX_fixnum(_SGN(ll_THIS->_length - l)));
}
ll_define_primitive_end

#define CHAR_CMP_PRED(NAME,OP) \
ll_define_primitive(string, string##NAME##Q, _2(string1, string2), _1(no_side_effect,"#t")) \
{									\
  ll_v c = ll_call(ll_o(_string_cmp), _2(ll_ARG_0, ll_ARG_1));		\
  ll_return(ll_make_boolean(c OP 0));					\
}									\
 ll_define_primitive_end						\
 ll_define_primitive(string, string_ci##NAME##Q, _2(string1, string2), _1(no_side_effect,"#t")) \
 {									\
   ll_v c = ll_call(ll_o(_string_cmp_ci), _2(ll_ARG_0, ll_ARG_1));	\
   ll_return(ll_make_boolean(c OP 0));					\
 }									\
ll_define_primitive_end


CHAR_CMP_PRED(AS,==)
CHAR_CMP_PRED(LT,<)
CHAR_CMP_PRED(GT,>)
CHAR_CMP_PRED(LE,<=)
CHAR_CMP_PRED(GE,>=)

#undef CHAR_CMP_PRED


/***************************************************************************/


#define X ll_SELF
#define Y ll_ARG_1


ll_define_primitive(string, equalQ, _2(x, y), _1(no_side_effect,"#t"))
{
  size_t l;

  if ( ll_EQ(X, Y) )
    ll_return(ll_t);

  if ( ll_NE(ll_TYPE(X), ll_TYPE(Y)) ) {
    ll_v tc = ll_call(ll_o(stringQ), _1(ll_ARG_1));
    if ( ! ll_unbox_boolean(tc) )
      ll_return(tc);
  }

  l = ll_THIS->_length;

  if ( l != ll_unbox_fixnum(ll_call(ll_o(string_length), _1(Y))) )
    ll_return(ll_f);

  if ( l ) {
    char *x, *y;
    
    x = ll_THIS->_array;
    y = ll_THIS_ISA(string, Y)->_array;
    
    while ( -- l > 0 ) {
      if ( *(x ++) != *(y ++) )
	ll_return(ll_f);
    }

  }

  ll_return(ll_t);
}
ll_define_primitive_end


#undef X
#undef Y
