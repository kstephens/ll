#ifndef __rcs_id__
#ifndef __rcs_id_ll_char_c__
#define __rcs_id_ll_char_c__
static const char __rcs_id_ll_char_c[] = "$Id: char.c,v 1.14 2008/05/26 00:19:34 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include <ctype.h>
#include "ll/char.h"

/*************************************************************************/


int ll_unbox_char(ll_v x)
{
  _ll_typecheck(ll_type(char), &x);
  return ll_UNBOX_char(x);
}


/*************************************************************************/


ll_INIT(char, 140, "all <char> objects")
{
  int i;

  for ( i = 0; i <= ll_MAX_char; i ++ ) {
    ll_v x;

#if ll_CHAR_STATIC_ALLOC
    assert(sizeof(ll_tsa_char) == sizeof(ll_v) * 2);
    x = ll_BOX_char(i);
    ll_SLOTS(x)[0] = ll_type(char);
#else
    x = _ll_allocate_type(ll_type(char));
#endif
    ll_THIS_ISA(char, x)->_value = ll_make_fixnum(i);
    ll_write_barrier(x);

#if ! ll_CHAR_STATIC_ALLOC
    ll_write_barrier_ptr(&ll_BOX_char(i));
#endif
  }

  return 0;
}


ll_define_primitive(char, char_upcase, _1(char), _1(no_side_effect,"#t"))
{
  int x = ll_UNBOX_char(ll_SELF);
  ll_return(ll_make_char(toupper(x)));
}
ll_define_primitive_end


ll_define_primitive(char, char_downcase, _1(char), _1(no_side_effect,"#t"))
{
  int x = ll_UNBOX_char(ll_SELF);
  ll_return(ll_make_char(tolower(x)));
}
ll_define_primitive_end


ll_define_primitive(char, char__integer, _1(char), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_value);
}
ll_define_primitive_end


ll_define_primitive(fixnum, integer__char, _1(self), _1(no_side_effect,"#t"))
{
  _ll_rangecheck(ll_s(integer), &ll_SELF, 0, ll_MAX_char);
  ll_return(ll_make_char(ll_UNBOX_fixnum(ll_SELF)));
}
ll_define_primitive_end


#ifdef tolower
#undef tolower
#endif


#define CHAR_TYPE_PRED(NAME,OP) \
ll_define_primitive(char, char_##NAME##Q, _1(char), _1(no_side_effect,"#t")) \
{ \
  int x = ll_UNBOX_char(ll_SELF); \
  ll_return(ll_make_boolean(OP)); \
} \
ll_define_primitive_end

CHAR_TYPE_PRED(alphabetic, isalpha(x))
CHAR_TYPE_PRED(numeric, isdigit(x))
CHAR_TYPE_PRED(whitespace, isspace(x))
CHAR_TYPE_PRED(upper_case, isupper(x))
CHAR_TYPE_PRED(lower_case, islower(x))
#undef CHAR_TYPE_PRED


#define CHAR_CMP_PRED(NAME,OP) \
ll_define_primitive(char, char##NAME##Q, _2(char1, char2), _1(no_side_effect,"#t")) \
{ \
  ll_return(ll_make_boolean(ll_UNBOX_char(ll_SELF) OP ll_unbox_char(ll_ARG_1))); \
} \
ll_define_primitive_end \
ll_define_primitive(char, char_ci##NAME##Q, _2(char1, char2), _1(no_side_effect,"#t")) \
{ \
  ll_return(ll_make_boolean(tolower(ll_UNBOX_char(ll_SELF)) OP tolower(ll_unbox_char(ll_ARG_1)))); \
} \
ll_define_primitive_end


CHAR_CMP_PRED(AS,==)
CHAR_CMP_PRED(LT,<)
CHAR_CMP_PRED(GT,>)
CHAR_CMP_PRED(LE,<=)
CHAR_CMP_PRED(GE,>=)


#undef CHAR_CMP_PRED

