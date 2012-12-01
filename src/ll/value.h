#ifndef _ll_value_h
#define _ll_value_h

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

//#if sizeof(void*) == sizeof(long long)
#if defined(__x86_64)
typedef unsigned long long ll_v;
typedef long long ll_v_word;
#define ll_v_BITS 64
#define ll_TAG_BIT 1LL
#define ll_TAG_ZERO 0ULL
#else
#define ll_v_BITS 32
typedef unsigned long ll_v;
typedef long ll_v_word;
#define ll_TAG_BIT 1L
#define ll_TAG_ZERO 0UL
#endif

#define ll_INITIALIZED(X) (X)

/***********************************************************************/
/* eq? */

#define ll_EQ(X,Y) ((X) == (Y))
#define ll_NE(X,Y) ((X) != (Y))

/***********************************************************************/
/* tag extraction */

#ifndef ll_TAG_BITS
#define ll_TAG_BITS 2
#endif
#define ll_TAG_MASK ((ll_TAG_BIT << ll_TAG_BITS) - 1)
#define ll_TAG(X) (((ll_v) (X)) & ll_TAG_MASK)
#define ll_WORD_BITS (sizeof(ll_v) * 8)

/***********************************************************************/
/* fixnum */

#define ll_TAG_fixnum        0
#define ll_BOX_fixnum(X)     (((ll_v_word)(X)) << ll_TAG_BITS)
#define ll_UNBOX_fixnum(X)   (((ll_v_word)(X)) >> ll_TAG_BITS)
#define ll_make_fixnum ll_make_integer
long ll_unbox_fixnum(ll_v x);
#define ll_ISA_fixnum(X)     (ll_TAG(X) == ll_TAG_fixnum)
#define ll_TYPE_fixnum(X)    ll_type(fixnum)
#define ll_MAX_fixnum        ((ll_v_word)((~ll_TAG_ZERO) >> (ll_TAG_BITS + 1)))

#ifndef ll_SYMETRICAL_fixnum
#define ll_SYMETRICAL_fixnum 1
#endif
#if ll_SYMETRICAL_fixnum
#define ll_MIN_fixnum        (-ll_MAX_fixnum)
#else
#define ll_MIN_fixnum        (-((ll_v_word)(ll_TAG_BIT << (ll_WORD_BITS - ll_TAG_BITS - 1))))
#endif

/***********************************************************************/
/* locative */

#define ll_TAG_locative      1
#define ll_BOX_locative(X)   (((ll_v)(X)) + ll_TAG_locative)
#define ll_UNBOX_locative(X) ((ll_v*)((X) - ll_TAG_locative))
#define ll_make_locative(X)  ll_BOX_locative(X)
ll_v *ll_unbox_locative(ll_v x);
#define ll_ISA_locative(X)   (ll_TAG(X) == ll_TAG_locative)
#define ll_TYPE_locative(X)  ll_type(locative)

/***********************************************************************/
/* flonum */


#if defined(i386) /* && sizeof(float) == sizeof(long) */
typedef float ll_flonum_imm_t;
#define _ll_flonum_imm_supported 4
#endif
#if defined(__x86_64) /* && sizeof(double) == sizeof(long long) */
typedef double ll_flonum_t;
#define _ll_flonum_imm_supported 8
#endif

#if _ll_flonum_imm_supported
#define ll_TAG_flonum        2
union ll_v_flonum {
  ll_v  v;
  ll_flonum_t f;
};

static __inline ll_v ll_BOX_flonum(ll_flonum_t f)
{
  union ll_v_flonum vf;
  vf.f = f;
  vf.v &= ~ ll_TAG_MASK;
  vf.v |= ll_TAG_flonum;
  return vf.v;
}

static __inline ll_flonum_t ll_UNBOX_flonum(ll_v v)
{
  union ll_v_flonum vf;
  vf.v = v;
  vf.v &= ~ ll_TAG_MASK;
  return vf.f;
}
#endif

#define ll_make_flonum(X)    ll_BOX_flonum(X)
float ll_unbox_flonum(ll_v x);
/* #define ll_unbox_flonum(X)ll_UNBOX_flonum(X) */
#define ll_ISA_flonum(X)     (ll_TAG(X) == ll_TAG_flonum)
#define ll_TYPE_flonum(X)    ll_type(flonum)

/***********************************************************************/
/* boxed object reference */

#define ll_TAG_ref           3
#define ll_BOX_ref(X)        (((ll_v)(X)) + ll_TAG_ref)
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

/* FIXME: x86_64 */
#define ll_TAG_fixnum       ct_t_long
#define ll_BOX_fixnum(X)    ct_v_long(X)
#define ll_make_fixnum(X)   ll_BOX_fixnum(X)
#define ll_UNBOX_fixnum(X)  ct_v_UNBOX(X, long)
#define ll_unbox_fixnum(X)  ll_UNBOX_fixnum(X)
#define ll_ISA_fixnum(X)    (ll_TAG(X) == ll_TAG_fixnum)
#define ll_TYPE_fixnum(X)   ll_type(fixnum)
#define ll_MIN_fixnum       (((ll_v_word)~ll_TAG_ZERO) >> 1)
#define ll_MAX_fixnum       ((ll_v_word)(~ll_TAG_ZERO >> 1))

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
