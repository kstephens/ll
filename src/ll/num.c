#include "ll.h"
#include <math.h>

#ifndef _ll_NUM
#error Must define _ll_NUM
#endif

#ifndef _ll_NUM_BOX
#error Must define _ll_NUM_BOX
#endif

#ifndef _ll_NUM_UNBOX
#error Must define _ll_NUM_UNBOX
#endif


/************************************************************************/

ll_define_primitive(_ll_NUM, initialize, __1(self,args), _1(no_side_effect,"#t"))
{
  ll_return(_ll_NUM_BOX(0));
}
ll_define_primitive_end

/************************************************************************/

ll_define_primitive(_ll_NUM, abs, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(_ll_NUM_UNBOX(ll_SELF) < 0 ? _ll_NUM_BOX(- _ll_NUM_UNBOX(ll_SELF)) : ll_SELF);
}
ll_define_primitive_end

/************************************************************************/

ll_define_primitive(_ll_NUM, zeroQ, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_boolean(! _ll_NUM_UNBOX(ll_SELF)));
}
ll_define_primitive_end

ll_define_primitive(_ll_NUM, positiveQ, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_boolean(_ll_NUM_UNBOX(ll_SELF) > 0));
}
ll_define_primitive_end

ll_define_primitive(_ll_NUM, negativeQ, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_boolean(_ll_NUM_UNBOX(ll_SELF) < 0));
}
ll_define_primitive_end

/************************************************************************/

ll_define_primitive(_ll_NUM, exp, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_flonum(exp((double) _ll_NUM_UNBOX(ll_SELF))));
}
ll_define_primitive_end

ll_define_primitive(_ll_NUM, log, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_flonum(log((double) _ll_NUM_UNBOX(ll_SELF))));
}
ll_define_primitive_end

ll_define_primitive(_ll_NUM, sin, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_flonum(sin((double) _ll_NUM_UNBOX(ll_SELF))));
}
ll_define_primitive_end

ll_define_primitive(_ll_NUM, cos, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_flonum(sin((double) _ll_NUM_UNBOX(ll_SELF))));
}
ll_define_primitive_end

ll_define_primitive(_ll_NUM, tan, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_flonum(tan((double) _ll_NUM_UNBOX(ll_SELF))));
}
ll_define_primitive_end

ll_define_primitive(_ll_NUM, asin, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_flonum(asin((double) _ll_NUM_UNBOX(ll_SELF))));
}
ll_define_primitive_end

ll_define_primitive(_ll_NUM, acos, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_flonum(asin((double) _ll_NUM_UNBOX(ll_SELF))));
}
ll_define_primitive_end

ll_define_primitive(_ll_NUM, atan, __1(y,x), _1(no_side_effect,"#t"))
{
  if ( ll_ARGC == 1 ) {
    ll_return(ll_make_flonum(atan((double) _ll_NUM_UNBOX(ll_SELF))));
  } else if ( ll_ARGC == 2 ) {
    ll_v xt = ll_TYPE(ll_ARG_1);
    double x;

    if ( ll_EQ(xt, ll_type(fixnum)) ) {
      x = ll_unbox_fixnum(ll_ARG_1);
    } else if ( ll_EQ(xt, ll_type(flonum)) ) {
      x = ll_unbox_flonum(ll_ARG_1);
    } else {
      ll_return(_ll_typecheck(ll_type(real), &ll_ARG_1));
    }
    ll_return(ll_make_flonum(atan2((double) _ll_NUM_UNBOX(ll_SELF), x)));
  }
}
ll_define_primitive_end


/************************************************************************/

ll_define_primitive(_ll_NUM, sqrt, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_flonum(sqrt((double) _ll_NUM_UNBOX(ll_SELF))));
}
ll_define_primitive_end

ll_define_primitive(_ll_NUM, expt, _2(z1,z2), _1(no_side_effect,"#t"))
{
  ll_v z2t = ll_TYPE(ll_ARG_1);
  if ( ll_EQ(z2t, ll_type(fixnum)) ) {
    ll_return(_ll_NUM_BOX(pow((double) _ll_NUM_UNBOX(ll_SELF), (double) ll_unbox_fixnum(ll_ARG_1))));
  } else if ( ll_EQ(z2t, ll_type(flonum)) ) {
    ll_return(ll_make_flonum(pow((double) _ll_NUM_UNBOX(ll_SELF), (double) ll_unbox_flonum(ll_ARG_1))));
  } else {
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1));
  }
}
ll_define_primitive_end

/************************************************************************/

ll_define_primitive(_ll_NUM, magnitude, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(_ll_NUM_UNBOX(ll_SELF) < 0 ? _ll_NUM_BOX(- _ll_NUM_UNBOX(ll_SELF)) : ll_SELF);
}
ll_define_primitive_end


/************************************************************************/
#undef _ll_NUM
#undef _ll_NUM_BOX
#undef _ll_NUM_UNBOX

