
#ifndef __rcs_id__
#ifndef __rcs_id_ll_constant_c__
#define __rcs_id_ll_constant_c__
static const char __rcs_id_ll_constant_c[] = "$Id: constant.c,v 1.5 2007/09/08 23:51:19 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"

/**************************************************************************/

ll_define_primitive(constant, clone, _1(object), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end

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
