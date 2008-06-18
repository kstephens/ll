#ifndef __rcs_id__
#ifndef __rcs_id_ll_cadr_h__
#define __rcs_id_ll_cadr_h__
static const char __rcs_id_ll_cadr_h[] = "$Id: cadr.h,v 1.2 1999/06/28 13:59:28 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

#ifndef _ll_CADR
#define _ll_CADR(A,D) ll_v ll_c##A##D##r(ll_v x);
#endif

_ll_CADR(a,a)
_ll_CADR(a,d)
_ll_CADR(d,a)
_ll_CADR(d,d)

_ll_CADR(aa,a)
_ll_CADR(aa,d)
_ll_CADR(ad,a)
_ll_CADR(ad,d)
_ll_CADR(da,a)
_ll_CADR(da,d)
_ll_CADR(dd,a)
_ll_CADR(dd,d)

_ll_CADR(aaa,a)
_ll_CADR(aaa,d)
_ll_CADR(aad,a)
_ll_CADR(aad,d)
_ll_CADR(ada,a)
_ll_CADR(ada,d)
_ll_CADR(add,a)
_ll_CADR(add,d)
_ll_CADR(daa,a)
_ll_CADR(daa,d)
_ll_CADR(dad,a)
_ll_CADR(dad,d)
_ll_CADR(dda,a)
_ll_CADR(dda,d)
_ll_CADR(ddd,a)
_ll_CADR(ddd,d)

_ll_CADR(aaaa,a)
_ll_CADR(aaaa,d)
_ll_CADR(aaad,a)
_ll_CADR(aaad,d)
_ll_CADR(aada,a)
_ll_CADR(aada,d)
_ll_CADR(aadd,a)
_ll_CADR(aadd,d)
_ll_CADR(adaa,a)
_ll_CADR(adaa,d)
_ll_CADR(adad,a)
_ll_CADR(adad,d)
_ll_CADR(adda,a)
_ll_CADR(adda,d)
_ll_CADR(addd,a)
_ll_CADR(addd,d)
_ll_CADR(daaa,a)
_ll_CADR(daaa,d)
_ll_CADR(daad,a)
_ll_CADR(daad,d)
_ll_CADR(dada,a)
_ll_CADR(dada,d)
_ll_CADR(dadd,a)
_ll_CADR(dadd,d)
_ll_CADR(ddaa,a)
_ll_CADR(ddaa,d)
_ll_CADR(ddad,a)
_ll_CADR(ddad,d)
_ll_CADR(ddda,a)
_ll_CADR(ddda,d)
_ll_CADR(dddd,a)
_ll_CADR(dddd,d)

#undef _ll_CADR

/*********************************************************/
