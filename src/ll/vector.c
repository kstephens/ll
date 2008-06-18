#ifndef __rcs_id__
#ifndef __rcs_id_ll_vector_c__
#define __rcs_id_ll_vector_c__
static const char __rcs_id_ll_vector_c[] = "$Id: vector.c,v 1.14 2008/05/28 11:48:49 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "call_int.h" /* __ll_call_tailv() */
#include <stdlib.h>
#include <string.h>


#define _ll_VEC vector
#define _ll_VEC_ELEM_TYPE ll_v
#define _ll_VEC_UNDEF_TYPE ll_undef
#define _ll_VEC_ELEM_EQUAL(X,Y) ll_equalQ((X), (Y))
#define _ll_VEC_ELEM_LOCATIVE 1

#include "vec.c"


#define X ll_SELF
#define Y ll_ARG_1

/*
** Reimplement equal? here to make it tail-recurse on the last pair
** of elements.
*/
ll_define_primitive(vector, _ref0, _1(v), _1(no_side_effect,"#t"))
{
  ll_return(ll_BOX_locative(ll_THIS->_array));
}
ll_define_primitive_end


ll_define_primitive(vector, equalQ, _2(x, y), _1(no_side_effect,"#t"))
{
  size_t l;

  if ( ll_EQ(X, Y) )
    ll_return(ll_t);

  if ( ll_NE(ll_TYPE(X), ll_TYPE(Y)) ) {
    ll_v tc = ll_call(ll_o(vectorQ), _1(ll_ARG_1));
    if ( ! ll_unbox_boolean(tc) )
      ll_return(tc);
  }

  l = ll_THIS->_length;

  if ( l != ll_unbox_fixnum(ll_call(ll_o(vector_length), _1(Y))) )
    ll_return(ll_f);

  if ( l ) {
    ll_v *x, *y;
    
    x = ll_THIS->_array;
    y = ll_unbox_locative(ll_call(ll_o(_ref0), _1(Y)));
    
    while ( -- l > 0 ) {
      if ( ! ll_equalQ(*(x ++), *(y ++)) )
	ll_return(ll_f);
    }
    
    X = *x;
    Y = *y;
    __ll_call_tailv(ll_o(equalQ), 2);
  } else {
    ll_return(ll_t);
  }
}
ll_define_primitive_end


#undef X
#undef Y
