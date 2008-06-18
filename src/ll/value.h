#ifndef _ll_value_h
#define _ll_value_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_value_h__
#define __rcs_id_ll_value_h__
static const char __rcs_id_ll_value_h[] = "$Id: value.h,v 1.13 2008/05/26 02:25:46 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll/config.h"

#if ll_v_WORD

/*************************************************************************/
/*
** Use a low 2-bit tag.
**
** 33222222222211111111110000000000
** 10987654321098765432109876543210
** --------------------------------
** fixnum           (immediate) |00
** locative           (address) |01
** flonum           (immediate) |10
** boxed object       (address) |11
**
*/

typedef unsigned long ll_v;

#define ll_INITIALIZED(X) (X)

/***********************************************************************/
/* eq? */

#define ll_EQ(X,Y) ((X) == (Y))
#define ll_NE(X,Y) ((X) != (Y))

/***********************************************************************/
/* tag extraction */

#define ll_TAG(X) (((unsigned int) (X)) & 3)

/***********************************************************************/
/* fixnum */

#define ll_TAG_fixnum        0
#define ll_BOX_fixnum(X)     (((long)(X)) << 2)
#define ll_make_fixnum(X)    ll_BOX_fixnum(X)
#define ll_UNBOX_fixnum(X)   (((long)(X)) >> 2)
long ll_unbox_fixnum(ll_v x);
/* #define ll_unbox_fixnum(X)ll_UNBOX_fixnum(X) */
#define ll_ISA_fixnum(X)     (ll_TAG(X) == ll_TAG_fixnum)
#define ll_TYPE_fixnum(X)    ll_type(fixnum)
#define ll_MIN_fixnum        (((long)~0UL) >> 2)
#define ll_MAX_fixnum        ((long)(~0UL >> 2))

/***********************************************************************/
/* locative */

#define ll_TAG_locative      1
#define ll_BOX_locative(X)   (((unsigned long)(X)) + ll_TAG_locative)
#define ll_make_locative(X)  ll_BOX_locative(X)
#define ll_UNBOX_locative(X) ((ll_v*)((X) - ll_TAG_locative))
/* #define ll_unbox_locative(X)ll_UNBOX_locative(X) */
ll_v *ll_unbox_locative(ll_v x);
#define ll_ISA_locative(X)   (ll_TAG(X) == ll_TAG_locative)
#define ll_TYPE_locative(X)  ll_type(locative)

/***********************************************************************/
/* flonum */

#define ll_TAG_flonum        2
#ifdef i386

union ll_v_flonum {
  ll_v  v;
  float f;
};

static __inline ll_v ll_BOX_flonum(float f)
{
  union ll_v_flonum vf;
  vf.f = f;
  vf.v &= ~ 3UL;
  vf.v |= ll_TAG_flonum;
  return vf.v;
}

static __inline float ll_UNBOX_flonum(ll_v v)
{
  union ll_v_flonum vf;
  vf.v = v;
  vf.v &= ~ 3UL;
  return vf.f;
}

#else
#error flonum only implemented for i386
#endif

#define ll_make_flonum(X)    ll_BOX_flonum(X)
float ll_unbox_flonum(ll_v x);
/* #define ll_unbox_flonum(X)ll_UNBOX_flonum(X) */
#define ll_ISA_flonum(X)     (ll_TAG(X) == ll_TAG_flonum)
#define ll_TYPE_flonum(X)    ll_type(flonum)

/***********************************************************************/
/* boxed object reference */

#define ll_TAG_ref           3
#define ll_BOX_ref(X)        (((unsigned int)(X)) + ll_TAG_ref)
#define ll_make_ref(X)       ll_BOX_ref(X)
#define ll_UNBOX_ref(X)      ((void*)((X) - ll_TAG_ref))
#define ll_unbox_ref(X)      ll_UNBOX_ref(X)
#define ll_ISA_ref(X)        (ll_TAG(X) == ll_TAG_ref)
#define ll_TYPE_ref(X)       (ll_SLOTS(X)[0])

/***********************************************************************/

#define ll_TYPE(X)					\
  (							\
   ll_ISA_ref(X) ?					\
   ll_TYPE_ref(X) :					\
   ll_ISA_fixnum(X) ?					\
   ll_TYPE_fixnum(X) :					\
   ll_ISA_flonum(X) ?					\
   ll_TYPE_flonum(X) :					\
   ll_TYPE_locative(X)					\
							)

/***********************************************************************/

#define ll_SLOTS(X) ((ll_v*) (ll_UNBOX_ref(X)))

/***********************************************************************/

#else

/*************************************************************************/
/*
** Use the ct_v value.
*/
#include "ct/cv.h"

typedef ct_v ll_v;

/*
** ll_v._t ----------> ct_t_user()->[0] --------> ll_tsa_type*
** ll_v._u._voidP ---> ll_tsa_<type>*;
*/

/***********************************************************************/
/* eq? */

#define ll_EQ(X,Y) cv_v_eq(&(X), &(Y))

/***********************************************************************/
/* tag extraction */

#define ll_TAG(X) ((X)._t)

/***********************************************************************/
/* fixnum */

#define ll_TAG_fixnum       ct_t_long
#define ll_BOX_fixnum(X)    ct_v_long(X)
#define ll_make_fixnum(X)   ll_BOX_fixnum(X)
#define ll_UNBOX_fixnum(X)  ct_v_UNBOX(X, long)
#define ll_unbox_fixnum(X)  ll_UNBOX_fixnum(X)
#define ll_ISA_fixnum(X)    (ll_TAG(X) == ll_TAG_fixnum)
#define ll_TYPE_fixnum(X)   ll_type(fixnum)
#define ll_MIN_fixnum       (((long)~0UL) >> 1)
#define ll_MAX_fixnum       ((long)(~0UL >> 1))

/***********************************************************************/
/* locative */

#define ll_TAG_locative      ct_t_voidP
#define ll_BOX_locative(X)   ct_v_voidP(X)
#define ll_make_locative(X)  ll_BOX_locative(X)
#define ll_UNBOX_locative(X) ((ll_v*) ct_v_UNBOX(X, voidP))
#define ll_unbox_locative(X) ll_UNBOX_locative(X)
#define ll_ISA_locative(X)   (ll_TAG(X) == ll_TAG_locative)
#define ll_TYPE_locative(X)  ll_type(locative)

/***********************************************************************/
/* flonum */

#define ll_TAG_flonum        ct_t_double
#define ll_BOX_flonum(X)     ct_v_double(X)
#define ll_UNBOX_flonum(X)   ct_v_UNBOX(X, double)
#define ll_make_flonum(X)    ll_BOX_flonum(X)
#define ll_unbox_flonum(X)   ll_UNBOX_flonum(X)
#define ll_ISA_flonum(X)     (ll_TAG(X) == ll_TAG_flonum)
#define ll_TYPE_flonum(X)    ll_type(flonum)

/***********************************************************************/
/* boxed object reference */

#define ll_TAG_ref           3
#define ll_BOX_ref(X)        ct_v_ptr(X, ((ll_v*)(X))->_u._voidP);
#define ll_make_ref(X)       ll_BOX_ref(X)
#define ll_UNBOX_ref(X)      ct_v_UNBOX(X, voidP)
#define ll_unbox_ref(X)      ll_UNBOX_ref(X)
#define ll_ISA_ref(X)        (ll_TAG(X) == ll_TAG_ref)
#define ll_TYPE_ref(X)       (ll_SLOTS(X)[0])

/*************************************************************************/

#define ll_TYPE(X) _ll_v_type(&X)

/*************************************************************************/

#endif

#define _ll_THIS_ISA(T,X) ((ll_tsa_##T*) ll_UNBOX_ref(X))
#define ll_THIS_ISA(T,X)  _ll_THIS_ISA(T, X)

/***********************************************************************/


#ifndef ll_NE
#define ll_NE(X,Y) (! ll_EQ(X, Y))
#endif

#endif
