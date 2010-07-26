#ifndef __rcs_id__
#ifndef __rcs_id_ll_cadr_c__
#define __rcs_id_ll_cadr_c__
static const char __rcs_id_ll_cadr_c[] = "$Id: cadr.c,v 1.6 2007/12/18 10:37:00 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"

/*********************************************************/

#define _ll_CADR(_X,_Y) \
__inline ll_v ll_c##_X##_Y##r(ll_v p) { return ll_c##_X##r(ll_c##_Y##r(p)); } \
void ll_set_c##_X##_Y##rE(ll_v p, ll_v v) { ll_set_c##_X##rE(ll_c##_Y##r(p), v); } \
__inline ll_v ll_locative_c##_X##_Y##r(ll_v p) { return ll_locative_c##_X##r(ll_c##_Y##r(p)); } \
ll_define_primitive(pair, c##_X##_Y##r, _1(pair), _1(no_side_effect,"#t")) \
{ \
  ll_return(ll_c##_X##r(ll_THIS->_c##_Y##r)); \
} \
ll_define_primitive_end \
\
ll_define_primitive(pair, set_c##_X##_Y##rE, _2(pair, v), _1(no_side_effect,"#t")) \
{ \
  ll_set_c##_X##rE(ll_THIS->_c##_Y##r, ll_ARG_1); \
} \
ll_define_primitive_end \
\
ll_define_primitive(pair, locative_c##_X##_Y##r, _1(pair), _1(no_side_effect,"#t")) \
{ \
  ll_return(ll_locative_c##_X##r(ll_THIS->_c##_Y##r)); \
} \
ll_define_primitive_end

#include "ll/cadr.h"


#undef _ll_CADR

/*********************************************************/
