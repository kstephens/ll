#ifndef _ct_cv_h
#define _ct_cv_h

#ifndef __rcs_id__
#ifndef __rcs_id_ct_cv_h__
#define __rcs_id_ct_cv_h__
static const char __rcs_id_ct_cv_h[] = "$Id: cv.h,v 1.3 1999/04/22 05:04:30 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

#include "ct/ct.h"

typedef struct ct_v {
  ct_t _t;
  union ct_vu {
#define CT(N,T) T _##N;
#define CT_NO_VOID
#include "ct/ct.def"
    void* _voidP;
    ct_t _enum;
  } _u;
} ct_v;

#define ct_v_UNBOX(X,T) ((X)._u._##T)
#define ct_v_BOX(X,T,V) ((X)._t = ct_t_##T, ct_v_UNBOX(X,T) = (V))

int ct_v_eqQ(const ct_v *a, const ct_v *b);
#define ct_v_EQ(X,Y) ct_v_eqQ(&(X), &(Y))

#define ct_v_NE(X,Y) (!ct_v_EQ(X,Y))

#endif
