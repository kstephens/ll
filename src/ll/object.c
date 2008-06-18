#ifndef __rcs_id__
#ifndef __rcs_id_ll_object_c__
#define __rcs_id_ll_object_c__
static const char __rcs_id_ll_object_c[] = "$Id: object.c,v 1.16 2008/05/26 07:48:08 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include <string.h> /* memcpy() */


/***********************************************************/
/* Oaklisp 2.2 Operations on Objects Type */


ll_define_primitive(object, get_type, _1(object), _1(no_side_effect,"#t"))
{
  ll_return(ll_TYPE(ll_SELF));
}
ll_define_primitive_end


/***********************************************************/
/* Oaklisp 2.5 Type Predicates */


ll_define_primitive(object, is_aQ, _2(object, type), _1(no_side_effect, "#t"))
{
  if ( ll_EQ(ll_TYPE(ll_SELF), ll_ARG_1) ) {
    ll_return(ll_t);
  }
  ll_call_tail(ll_call(ll_o(type_tester), _1(ll_ARG_1)), _1(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(type, subtypeQ, _2(type1, type2), _1(no_side_effect, "#t"))
{
  /* IMPLEMENT */
}
ll_define_primitive_end


/***********************************************************/
/* Oaklisp 2.3 Operations on Types */


ll_define_primitive(object, initialize, __1(object, args), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


/***********************************************************/
/* Direct slot access */


ll_define_primitive(object, __slot, _2(object, slot_index), _1(no_side_effect,"#t"))
{
  ll_return(ll_SLOTS(ll_SELF)[ll_unbox_fixnum(ll_ARG_1)]);
}
ll_define_primitive_end


ll_define_primitive(object, __slotE, _3(object, slot_index, value), _0())
{
  ll_SLOTS(ll_SELF)[ll_unbox_fixnum(ll_ARG_1)] = ll_ARG_2;
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(object, __slot_locative, _2(object, slot_index), _1(no_side_effect,"#t"))
{
  ll_return(ll_BOX_locative(&ll_SLOTS(ll_SELF)[ll_unbox_fixnum(ll_ARG_1)]));
}
ll_define_primitive_end

/***********************************************************/
/* Object mutablity */


ll_define_primitive(object, immutable_type, _1(object), _1(no_side_effect,"#t"))
{
  ll_return(ll_TYPE(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(object, mutable_type, _1(object), _1(no_side_effect,"#t"))
{
  ll_return(ll_TYPE(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(immediate, make_immutable, _1(object), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


/* Do we really want all objects to respond to make-immutable? */
ll_define_primitive(object, make_immutable, _1(object), _0())
{
  ll_TYPE_ref(ll_SELF) = ll_call(ll_o(immutable_type), _1(ll_SELF));
}
ll_define_primitive_end


/***********************************************************/
/* Object cloning */


ll_define_primitive(object, initialize_clone, _2(object, src), _0())
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(object, clone, _1(object), _0())
{
  ll_v t = ll_TYPE(ll_SELF);
  size_t size = ll_unbox_fixnum(ll_call(ll_o(type_size), _1(t)));
  ll_v x = _ll_allocate_type(t);
  memcpy(ll_UNBOX_ref(x), ll_UNBOX_ref(ll_SELF), size);
  ll_call_tail(ll_o(initialize_clone), _2(x, ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(object, initialize_clone_mutable, _2(object, src), _0())
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(object, clone_mutable, _1(object), _0())
{
  ll_v t = ll_call(ll_o(mutable_type), _1(ll_SELF));
  size_t size = ll_unbox_fixnum(ll_call(ll_o(type_size), _1(t)));
  ll_v x = _ll_allocate_type(t);

  /* Copy over all slots.
   * This will also whack <object>:type slot (slot 0).
   */
  memcpy(ll_UNBOX_ref(x), ll_UNBOX_ref(ll_SELF), size);

  /* Set the type back. */
  ll_TYPE_ref(x) = t;

  ll_call_tail(ll_o(initialize_clone_mutable), _2(x, ll_SELF));
}
ll_define_primitive_end


/***********************************************************/
/* Value tags */


ll_define_primitive(object, _get_tag, _1(object), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_fixnum(ll_TAG(ll_SELF)));
}
ll_define_primitive_end


