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
    double x = 1.0 + eps;
#if _ll_flonum_imm_supported
    ll_v v = ll_BOX_flonum(x);
    double vx = ll_UNBOX_flonum(v);
#else
    ll_v v = 0;
    double vx = x;
#endif
    double abs_err = fabs(x - vx);
    double rel_err = abs_err / x;
    
    eps *= 0.1;
    i ++;

    if ( verbose ) {
      printf("/* i = %d, eps = %.30g, 1 + eps = %.30g*/\n", i, eps, 1.0 + eps);
      printf("/* x = %.30g, v = %llu, abs_err = %.30g, rel_err = %.30g */\n", 
	     (double) x, 
	     (unsigned long long) v, 
	     (double) abs_err, 
	     (double) rel_err);
    }
    if ( max_error < rel_err ) {
      max_error = rel_err;
    }
  }

  printf("#define ll_FLO_MAX_ERROR %.22g\n", (double) max_error);
  printf("#define ll_FLO_DIGITS %d\n", (int) i);

  return 0;
}
