#include "ll.h"

/************************************************************************/


ll_define_primitive(rational, inexact__exact, _1(z), _1(no_side_effect, "#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


/************************************************************************/


