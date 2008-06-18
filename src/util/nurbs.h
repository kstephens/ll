#ifndef _util_NURBS_H
#define _util_NURBS_H

#ifndef NURBS_T
#define NURBS_T double
#endif


#define __NURBS(X)NURBS_##X
#define _NURBS(X)__NURBS(X)
#define NURBS(X)_NURBS(X)


typedef struct NURBS(NURBS_T) {
  short degree;

  int nv;

  NURBS(T) *v;
  short vdim;
  short vskip;
  short voff;

  NURBS(T) *w;
  short wskip;
  short woff;

  int nu;
  NURBS(T) *u; /* Length must be nv + (degree * 2) - 2. */

} NURBS(NURBS_T);


#ifndef NURBS_IMPORT
#define NURBS_IMPORT /* extern */
#endif


NURBS(IMPORT) NURBS(T) NURBS(_min_knot)(NURBS(NURBS_T) *n);

NURBS(IMPORT) NURBS(T) NURBS(_max_knot)(NURBS(NURBS_T) *n);

NURBS(IMPORT) int NURBS(_find_span) (NURBS(NURBS_T) *n, NURBS(T) u);

NURBS(IMPORT) void NURBS(_basis_funs) (NURBS(NURBS_T) *n, NURBS(T) u, int span, NURBS(T) *funs);

NURBS(IMPORT) void NURBS(_basis_funs_der)(NURBS(NURBS_T) *n, NURBS(T) u, int span, int d, NURBS(T) *funs);

NURBS(IMPORT) NURBS(T) *NURBS(_eval) (NURBS(NURBS_T) *n, NURBS(T) u, NURBS(T) *r);

NURBS(IMPORT) NURBS(T) *NURBS(_eval_der)(NURBS(NURBS_T) *n, NURBS(T) u, int d, NURBS(T) *r);


#endif
