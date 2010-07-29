#include "ll.h"

/**************************************************************************/
/* mutable/immutable */

ll_define_primitive(constant, immutableQ, _1(object), _1(no_side_effect,"#t"))
{
  ll_return(ll_t);
}
ll_define_primitive_end

ll_define_primitive(object, immutableQ, _1(object), _1(no_side_effect,"#t"))
{
  ll_return(ll_f);
}
ll_define_primitive_end

ll_define_primitive(immutable, immutableQ, _1(object), _1(no_side_effect,"#t"))
{
  ll_return(ll_t);
}
ll_define_primitive_end

ll_define_primitive(mutable, immutableQ, _1(object), _1(no_side_effect,"#t"))
{
  ll_return(ll_f);
}
ll_define_primitive_end

/**************************************************************************/
