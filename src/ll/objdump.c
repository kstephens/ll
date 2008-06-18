#ifndef __rcs_id__
#ifndef __rcs_id_ll_objdump_c__
#define __rcs_id_ll_objdump_c__
static const char __rcs_id_ll_objdump_c[] = "$Id: objdump.c,v 1.7 2007/12/20 22:50:15 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll/ll.h"

#define GET_PORT(I) \
ll_v port = ll_ARGC > (I) ? ll_ARGV[I] : ll_call(ll_o(Scurrent_output_portS), _0())


ll_define_primitive(type, llC_obj_dump, _4(super, obj, port, visited), _0())
{
  ll_v type = ll_ARG_0;
  ll_v port = ll_ARG_2;
  ll_v obj = ll_ARG_1;
  ll_v obj_type = ll_TYPE(obj);
  ll_v *visited = ll_UNBOX_locative(ll_ARG_3);

  /* Visit supers first to ensure proper slot ordering. */
  if ( ll_eqQ(ll_memq(type, *visited), ll_f) ) {
    *visited = ll_cons(type, *visited);

    /* Do our supers first */
    ll_LIST_LOOP(ll_THIS->_supers, super);
    {
      ll_call(ll_o(llC_obj_dump), _4(super, obj, port, ll_ARG_3));
    }
    ll_LIST_LOOP_END;
  }

  {
    ll_v type_offset, slots_size;
    
    type_offset = ll_assq(type, ll_call(ll_o(type_type_offset_alist), _1(obj_type)));
    
    type_offset = ll_cdr(type_offset);

    slots_size = ll_THIS->_slots_size;

    ll_format(port, "  ~S\t~S\t~S\n", 3, type, type_offset, slots_size);
  
    if ( ll_ISA_ref(obj) ) {   
    const char *obj_base = (const char*) ll_UNBOX_ref(obj);

    ll_LIST_LOOP(ll_THIS->_slots, slot);
    {
      ll_v slot_name = ll_car(slot);
      ll_v slot_offset = ll_car(ll_cdr(slot));
      ll_v slot_type = ll_cdr(ll_cdr(slot));
      
      size_t offset = ll_unbox_fixnum(type_offset) + ll_unbox_fixnum(slot_offset);

      ll_format(port, "    ~S\t~S\n      ", 2, slot_name, ll_make_fixnum(offset));

      /* No slot type?  Print shallow */
      if ( ll_EQ(slot_type, ll_nil) ) {
	ll_format(port, "~O\n", 1, * (ll_v*) (obj_base + offset));
      } else {
	/* Print as ptr */
	char buf[32];
	sprintf(buf, "#p%p\n", * (void**) (obj_base + offset));
	ll_format(port, buf, 0);
      }
      
    }
    ll_LIST_LOOP_END;
    }
  }
}
ll_define_primitive_end
 

ll_define_primitive(object, llCobj_dump, __1(obj, port), _0())
{
  ll_v visited = ll_nil;
  ll_v obj = ll_SELF;
  ll_v obj_type = ll_TYPE(ll_SELF);

  GET_PORT(1);
  ll_format(port, "#<\n~S ~W ~S\n", 3, obj_type, obj, ll_call(ll_o(type_size), _1(obj_type)));
  ll_call(ll_o(llC_obj_dump), _4(obj_type, obj, port, ll_make_locative(&visited)));
  ll_format(port, ">\n", 0);
}
ll_define_primitive_end


