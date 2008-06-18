#ifndef __rcs_id__
#ifndef __rcs_id_ll_floatcfg_c__
#define __rcs_id_ll_floatcfg_c__
static const char __rcs_id_ll_floatcfg_c[] = "$Id: floatcfg.c,v 1.5 2006/03/08 02:22:14 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll/value.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char **argv)
{
  int verbose = argc > 1;
  int i = 0;
  double max_error = 0;
  double eps = 0.1;

  while ( 1.0 + eps != 1.0 && i < 1000 ) {
    double x = x + eps;
    ll_v v = ll_BOX_flonum(x);
    double vx = ll_UNBOX_flonum(v);
    double v_vx = fabs(v - vx);
    double error = fabs(1.0 - v / v_vx);
    
    eps *= 0.1;
    i ++;

    /*
      fprintf(stderr, "i = %d, eps = %g\n", i, eps);
    */

    if ( verbose ) {
      printf("x = %.22g, v = %lu, vx = %.22g, v / (v - vx) = %.22g\n", 
	     (double) x, 
	     (unsigned long) v, 
	     (double) vx, 
	     (double) error);
    }
    if ( max_error < error ) {
      max_error = error;
    }
  }

  printf("#define ll_FLO_MAX_ERROR %.22g\n", (double) max_error);
  printf("#define ll_FLO_DIGITS %d\n", (int) (double) - log10(max_error));

  return 0;
}
