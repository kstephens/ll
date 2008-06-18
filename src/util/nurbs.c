/* $Id: nurbs.c,v 1.2 2001/08/06 11:15:39 stephens Exp $ */
/* Author: kurtstephens@acm.org 2001/08/06 */

#ifndef _util_NURBS_H
#include "nurbs.h"
#endif

#include <assert.h>
//#include <alloca.h>


NURBS(EXPORT) int NURBS(_check_pi)(const NURBS(NURBS_T) *n, int i)
{
  assert(i >= 0);
  assert(i < n->nv);
  return i;
}


NURBS(EXPORT) int NURBS(_check_ui)(const NURBS(NURBS_T) *n, int i)
{
  assert(i >= 0);
  assert(i < n->nu);
  return i;
}


#define deg_ (n->degree)
#define nv n->nv
#define vdim n->vdim
#define P(i) (n->v + n->voff + n->vskip * NURBS(_check_pi)(n, (i)))
#define W(i) (n->w + n->woff + n->wskip * NURBS(_check_pi)(n, (i)))[0]
#define U(i) (n->u[NURBS(_check_ui)(n, (i))])


#ifndef NURBS_EXPORT
#define NURBS_EXPORT
#endif


NURBS(EXPORT) NURBS(T) NURBS(_min_knot)(NURBS(NURBS_T) *n)
{
  return n->u[0];
}


NURBS(EXPORT) NURBS(T) NURBS(_max_knot)(NURBS(NURBS_T) *n)
{
  return n->u[n->nu - n->degree];
}


NURBS(EXPORT) int NURBS(_find_span) (NURBS(NURBS_T) *n, NURBS(T) u)
{
  if ( u >= U(nv) ) 
    return nv - 1 ;
  if ( u <= U(deg_) )
    return deg_ ;

  {
    int low  = 0 ;
    int high = nv + 1 ; 
    int mid;
    
    while ( mid = (low + high) / 2, 
	    u < U(mid) || u >= U(mid + 1) ) {
      if ( u < U(mid) )
	high = mid ;
      else
	low = mid ;
    }

    return mid ;
  }
}



NURBS(EXPORT) void NURBS(_basis_funs) (NURBS(NURBS_T) *n, NURBS(T) u, int i, NURBS(T) *N)
{
  NURBS(T)* left = (NURBS(T)*) alloca(2 * (deg_+1) * sizeof(NURBS(T))) ;
  NURBS(T)* right = &left[deg_ + 1] ;
  int j;

  N[0] = 1.0 ;
  for ( j = 1; j <= deg_ ; ++ j ) {
    int r;
    NURBS(T) saved = 0.0;

    left[j] = u - U(i + 1 - j) ;
    right[j] = U(i + j) - u ;

    for ( r = 0 ; r < j; ++ r ) {
      NURBS(T) temp = N[r] / (right[r + 1] + left[j - r]) ;
      N[r] = saved + right[r + 1] * temp ;
      saved = left[j - r] * temp ;
    }

    N[j] = saved ;
  }  
}


NURBS(EXPORT) void NURBS(_basis_funs_der)(NURBS(NURBS_T) *n, NURBS(T) u, int i, int d, NURBS(T) *funs)
{
}



NURBS(EXPORT) NURBS(T) * NURBS(_eval) (NURBS(NURBS_T) *n, NURBS(T) u, NURBS(T) *p)
{
  int span = NURBS(_find_span)(n, u);
  NURBS(T) *Nb = (NURBS(T) *) alloca(sizeof(Nb[0]) * (deg_ + 1));
  int i, j;
  NURBS(T) w;

  NURBS(_basis_funs)(n, u, span, Nb);

  i = deg_;
  for ( j = 0; j < vdim; ++ j ) {
    p[j] = Nb[i] * P(span - deg_ + i)[j] * (n->w ? W(span - deg_ + i) : 1);
  }
  if ( n->w ) {
    w = Nb[i] * W(span - deg_ + i);
  }

  for ( -- i; i >= 0; -- i ) {
    for ( j = 0; j < vdim; ++ j ) {
      p[j] += Nb[i] * P(span - deg_ + i)[j] * (n->w ? W(span - deg_ + i) : 1);
    }
    if ( n->w ) {
      w += Nb[i] * W(span - deg_ + i);
    }
  }

  /* Homogenous division. */
  if ( n->w ) {
    for ( j = 0; j < vdim; ++ j ) {
      p[j] /= w;
    }
  }

  return p;
}


NURBS(EXPORT) NURBS(T) * NURBS(_eval_der)(NURBS(NURBS_T) *n, NURBS(T) u, int d, NURBS(T) *p)
{

  return p;
}



#undef deg_
#undef nv
#undef vdim
#undef P
#undef W
#undef U

