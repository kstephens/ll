#ifndef __rcs_id__
#ifndef __rcs_id_ll_type_c__
#define __rcs_id_ll_type_c__
static const char __rcs_id_ll_type_c[] = "$Id: type.c,v 1.29 2007/12/20 20:59:15 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include "call_int.h"
#include "assert.h"

#include <string.h> /* strcmp() */

/*************************************************************************/

#if 0
#define DBX(X) \
fprintf(stderr, "%s:%d: %p: <%s>:%s: %s = %p\n", __FILE__, __LINE__, (void*) _ll_val_sp, ll_PRIM_TYPE_NAME, ll_PRIM_OP_NAME, #X, (void*) X)
#else
#define DBX(X) do{}while(0)
#endif


ll_define_primitive(type,_init_type_offset_alist, _2(type, src), _0())
{
  DBX(ll_SELF);

  ll_v src = ll_ARG_1;

  /* Not already scanned src into our type-offset-alist? */
  if ( ll_EQ(ll_assq(src, ll_THIS->_type_offset_alist), ll_f) ) {
    /* Do src's supers first */
    ll_v src_supers = ll_call(ll_o(type_supers), _1(src));
    ll_LIST_LOOP(src_supers, super);
    {
      ll_call(ll_o(_init_type_offset_alist), _2(ll_SELF, super));
    }
    ll_LIST_LOOP_END;

    /* Add */ 
    DBX(ll_SELF);
    ll_THIS->_type_offset_alist = ll_cons(ll_immutable_cons(src, ll_THIS->_size), ll_THIS->_type_offset_alist);
    ll_write_barrier_SELF();

    /* Increment our size based on the src slot-size */
    DBX(ll_SELF);
    ll_THIS->_size = ll__ADD(ll_THIS->_size, ll_call(ll_o(type_slots_size), _1(src)));
    ll_write_barrier_SELF();
  }

  DBX(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(type,initialize, __3(self,supertypes,slot_names,doc), _0())
{
  DBX(ll_SELF);

  ll_call_super(ll_o(initialize), ll_f, _1(ll_SELF));

  /* copy supers */
  {
    ll_v supers = ll_nil, *xp = &supers;

    ll_LIST_LOOP(ll_ARG_1, super);
    {
      *xp = ll_immutable_cons(super, ll_nil);
      /* WRITE BARRIER */
      xp = &ll_CDR(*xp);
    }
    ll_LIST_LOOP_END;

    ll_THIS->_supers = ll_ARG_1;
    ll_write_barrier_SELF();
  }

  /* copy and compute slot offsets, and slots size */
  DBX(ll_SELF);
  {
    ll_v slots = ll_nil, *xp = &slots;
    size_t offset = 0;

    ll_LIST_LOOP(ll_ARG_2, slot_def);
    {
      ll_v slot_name, slot_offset;
      ll_v slot_item;

      slot_name = slot_def;
      slot_offset = ll_make_fixnum(offset);
      offset += sizeof(ll_v);

      slot_item = ll_immutable_cons(slot_name, ll_immutable_cons(slot_offset, ll_nil));

      *xp = ll_immutable_cons(slot_item, ll_nil);
      /* WRITE BARRIER */
      xp = &ll_CDR(*xp);
    }
    ll_LIST_LOOP_END;

    ll_THIS->_slots = slots;
    ll_THIS->_slots_size = ll_make_fixnum(offset);
    ll_write_barrier_SELF();
  }

  /* We'll fill this in later */
  ll_THIS->_size = ll_make_fixnum(0);
  ll_write_barrier_SELF();

#if ll_USE_OP_METH_ALIST
  /* A new type as no methods */
  ll_THIS->_op_meth_alist = ll_nil;
#endif

  /* We'll fill this in later */
  ll_THIS->_type_offset_alist = ll_nil;

  /* It's not top wired and it's not immutable */
  ll_THIS->_top_wiredQ = ll_f;
  ll_THIS->_immutableQ = ll_f;

  /* Create anon coerce and type-check ops */
  DBX(ll_SELF);
  ll_THIS->_tester = ll_f;
  ll_write_barrier_SELF();
  ll_THIS->_coercer = _ll_make_operation();
  ll_write_barrier_SELF();
  ll_THIS->_typechecker = _ll_make_operation();
  ll_THIS->_properties = ll_nil;
  ll_write_barrier_SELF();

  /* Remember any documentation. */
  if ( ll_ARGC >= 4 ) {
    ll_call(ll_o(set_llCdocE), _2(ll_SELF, ll_ARG_3));
  }

  /* Fill in the size and type offset alist */
  DBX(ll_SELF);
  ll_call(ll_o(_init_type_offset_alist), _2(ll_SELF, ll_SELF));

  /* Add methods for coerce and type-check ops */
  DBX(ll_SELF);
  _ll_add_method(ll_SELF, ll_THIS->_coercer, _ll_p_identity);
  _ll_add_method(ll_SELF, ll_THIS->_typechecker, _ll_p_identity);

  DBX(ll_SELF);
  ll_return(ll_SELF);
}
ll_define_primitive_end


/*************************************************************************/


ll_v ll_type_typechecker(ll_v type)
{
  if ( ll_TYPE(type) == ll_type(type) ) {
    return ll_THIS_ISA(type, type)->_typechecker;
  } else {
    return ll_call(ll_o(typechecker), _1(type));
  }
}


ll_define_primitive(type,make, __1(type,args), _0())
{
  /* Get the instance's size */
  size_t _size;
  ll_v instance;

  ll_assert_fixnum(ll_THIS->_size);
  _size = ll_unbox_fixnum(ll_THIS->_size);

  if ( _size < sizeof(ll_v) ) {
    ll_return(_ll_error(ll_e(uninstantiable,error), 1, ll_s(type), ll_SELF));
  }

  /* Allocate an instance */
  instance = ll_make_ref(ll_malloc(_size));

  /* Set the instance's isa slot */
  ll_SLOTS(instance)[0] = ll_SELF;
  /* WRITE BARRIER */

  /* Clear the instance's slots */
  {
    size_t i;
    for ( i = 1; i < _size / sizeof(ll_v); i ++ ) {
      ll_SLOTS(instance)[i] = ll_undef;
    }
  }

  if ( 0 ) {
    extern int _ll_bc_debug;
    _ll_bc_debug = 1;
  }

  /* (make <type> ...) -> (initialize <instance> ...) */
  ll_SELF = instance;
  __ll_call_tailv(ll_o(initialize),ll_ARGC);
}
ll_define_primitive_end


/*************************************************************************/


ll_define_primitive(type, locative_properties, 
		    _1(self), 
		    _2(no_side_effect, "#t",
		       doc, "Returns a locative to the type's properties list."))
{
  ll_return(ll_make_locative(&ll_THIS->_properties));
}
ll_define_primitive_end


ll_define_primitive(type,type_size,
		    _1(type), 
		    _2(no_side_effect,"#t",
		       doc, "Returns the size, in bytes, of an instance."))
{
  ll_return(ll_THIS->_size);
}
ll_define_primitive_end


ll_define_primitive(type,type_slots_size,
		    _1(type), 
		    _2(no_side_effect, "#t",
		       doc, "Return the number of slots for an instance."))
{
  ll_return(ll_THIS->_slots_size);
}
ll_define_primitive_end


ll_define_primitive(type,type_supers,
		    _1(type), 
		    _2(no_side_effect,"#t",
		       doc, "Returns a list of supertypes."))
{
  ll_return(ll_THIS->_supers);
}
ll_define_primitive_end


ll_define_primitive(type,type_slots,
		    _1(type), 
		    _2(no_side_effect, "#t",
		       doc, "Returns a list of slot descriptors."))
{
  ll_return(ll_THIS->_slots);
}
ll_define_primitive_end


#if ll_USE_OP_METH_ALIST
ll_define_primitive(type,type_op_meth_alist,
		    _1(type), 
		    _2(no_side_effect, "#t",
		       doc, "Returns an alist of operation and methods."))
{
  ll_return(ll_THIS->_op_meth_alist);
}
ll_define_primitive_end
#endif

ll_define_primitive(type,type_type_offset_alist,
		    _1(type), 
		    _2(no_side_effect, "#t",
		       doc, "Returns an alist of type and type offsets."))
{
  ll_return(ll_THIS->_type_offset_alist);
}
ll_define_primitive_end


ll_define_primitive(type,top_wiredQ,
		    _1(type), 
		    _2(no_side_effect, "#t",
		       doc, "Return true if the type is top-wired."))
{
  ll_return(ll_THIS->_top_wiredQ);
}
ll_define_primitive_end


ll_define_primitive(type,immutableQ,
		    _1(type), 
		    _2(no_side_effect, "#t",
		       doc, "Return true if the type is immutable."))
{
  ll_return(ll_THIS->_immutableQ);
}
ll_define_primitive_end


ll_define_primitive(type, type_tester,
		    _1(type), 
		    _2(no_side_effect,"#t",
		       doc, "Returns the type's tester operation."))
{
  if ( ! ll_unbox_boolean(ll_THIS->_tester) ) {
    ll_v op = _ll_make_operation();
    ll_THIS->_tester = op;
    _ll_add_method(ll_type(object), op, _ll_p_false);
    _ll_add_method(ll_SELF, op, _ll_p_true);
  }
  ll_return(ll_THIS->_tester);
}
ll_define_primitive_end


ll_define_primitive(type,coercer,
		    _1(type), 
		    _2(no_side_effect,"#t",
		       doc, "Returns the type's coercer operation."))
{
  ll_return(ll_THIS->_coercer);
}
ll_define_primitive_end


ll_define_primitive(type,typechecker,
		    _1(type), 
		    _2(no_side_effect,"#t",
		       doc, "Returns the type's typechecker operation."))
{
  ll_return(ll_THIS->_typechecker);
}
ll_define_primitive_end


/*************************************************************************/


ll_define_primitive(type,_add_method,
		    _3(type, op, meth), 
		    _1(doc, "Added a method implementation for an operation to the type."))
{
  _ll_add_method(ll_SELF, ll_ARG_1, ll_ARG_2);
  ll_return(ll_ARG_1);
}
ll_define_primitive_end


ll_define_primitive(type,remove_method,
		    _2(type, op), 
		    _1(doc, "Removes a method implementation for an operation from the type."))
{
  _ll_remove_method(ll_SELF, ll_ARG_1);
}
ll_define_primitive_end


/*************************************************************************/


