#ifndef __rcs_id__
#ifndef __rcs_id_ll_lookup_c__
#define __rcs_id_ll_lookup_c__
static const char __rcs_id_ll_lookup_c[] = "$Id: lookup.c,v 1.35 2008/05/27 04:06:41 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "call_int.h"
#include "assert.h"
#include "sig.h" /* ll_sig_service */


/*************************************************************************/

#ifndef ll_LOOKUP_CACHE_STATS
#define ll_LOOKUP_CACHE_STATS 1
#endif


/*************************************************************************/


/*
** _ll_add_method() is in here with _ll_lookup() because the two are so interdependent.
*/

void _ll_add_method(ll_v type, ll_v op, ll_v meth)
{
  ll_tsa_type *t = ll_THIS_ISA(type, type);
  ll_tsa_operation *o = ll_THIS_ISA(operation, op);

  ll_assert_ref(type);
  ll_assert_ref(op);
  if ( ll_NE(meth, ll_undef) ) {
    ll_assert_ref(meth);
  }

  if ( ll_EQ(meth, ll_f) ) {
    _ll_remove_method(type, op);
    goto done;
  }

  /* Validate types. */
  _ll_typecheck(ll_type(operation), &op);
  _ll_typecheck(ll_type(method), &meth);
  
  /* For the first time OR already a lambda? */
  if ( ll_EQ(o->_lambdaQ, ll_undef) || ll_NE(o->_lambdaQ, ll_f) ) {
    if ( ll_NE(o->_lambda_type, ll_undef) && ll_NE(o->_lambda_type, type) ) {
      /*
      ** A lambda method already exists for this op.
      ** keep the old op->meth assoc by forcing storage into
      ** the old cache-type.
      ** Put the op's lambda? method into cache-type, and continue.
      ** Add a new entry!
      */
#if ll_USE_TYPE_METH_ALIST
      o->_type_meth_alist = ll_cons(ll_cons(o->_lambda_type, o->_lambdaQ), o->_type_meth_alist);
#endif
#if ll_USE_OP_METH_ALIST
      {
      ll_tsa_type *ot = ll_THIS_ISA(type, o->_lambda_type);
      ot->_op_meth_alist = ll_cons(ll_cons(op, o->_lambdaQ), ot->_op_meth_alist);
      }
#endif
      o->_lambda_type = ll_undef;
    } else {
      /*
      ** A lambda method has never been add to this method.
      ** Don't bother adding to type's op-meth-alist;
      ** just store the type and the method in the operation.
      */
      o->_lambdaQ = meth;
      o->_lambda_type = type;
      
      /* 
      ** Invalidate op's lookup cache,
      ** because we don't know what the type offset is yet.
      */
      o->_cache_type = ll_f;
      goto done;
    }
  }

  /* Invalidate op's lookup cache. */
  o->_lambdaQ = 
    o->_cache_type = 
    o->_cache_impl = 
    o->_cache_method =
    ll_f;
  o->_cache_offset = ll_BOX_fixnum(0);

  /* Add a new method entry or set existing. */
#if ll_USE_TYPE_METH_ALIST
  _ll_assq_set(&o->_type_meth_alist, type, meth);
#endif
#if ll_USE_OP_METH_ALIST
  _ll_assq_set(&t->_op_meth_alist, op, meth);
#endif

 done:
#if ll_LOOKUP_CACHE_STATS
  ll_g(_lookup_op_cache_change) = 
    ll_BOX_fixnum(ll_UNBOX_fixnum(ll_g(_lookup_op_cache_change)) + 1);
#endif

  /* Bump the operation version. */
#if ll_USE_LCACHE
  o->_version = ll_BOX_fixnum(ll_UNBOX_fixnum(o->_version) + 1);
#endif
}


void _ll_remove_method(ll_v type, ll_v op)
{
  ll_tsa_type *t = ll_THIS_ISA(type, type);
  ll_tsa_operation *o = ll_THIS_ISA(operation, op);

  _ll_typecheck(ll_type(operation), &op);

  /* Invalidate the lookup cache */
  o->_cache_type = ll_f;

  /* Removed lambda? */
  if ( ll_EQ(o->_lambda_type, type) ) {
    o->_lambdaQ = o->_lambda_type = ll_undef;
  } else {
    /* Remove from method alist. */
#if ll_USE_TYPE_METH_ALIST
    _ll_assq_delete(&o->_type_meth_alist, type);
#endif
#if ll_USE_OP_METH_ALIST
    _ll_assq_delete(&t->_op_meth_alist, op);
#endif
  }
}


/*************************************************************************/

static
int _ll_lookup1(ll_v type)
{
  ll_tsa_type *t = ll_THIS_ISA(type, type);
  ll_tsa_operation *o = ll_THIS_ISA(operation, ll_AR_OP);
  ll_v x;

  ll_DEBUG_PRINT(lookup, ("_ll_lookup1(op = %s, type = %s)\n", ll_po(ll_AR_OP), ll_po(type)));

  /* Check for lambda. */
  if ( ll_EQ(o->_lambda_type, type) ) {
#if ll_LOOKUP_CACHE_STATS
  ll_g(_lookup_op_cache_lambda) = 
    ll_BOX_fixnum(ll_UNBOX_fixnum(ll_g(_lookup_op_cache_lambda)) + 1);
#endif

    /* Put the method in the current message. */
    ll_AR_METH = o->_lambdaQ;

    /* Remember which type we found the method in. */
    ll_AR_TYPE = type;

    return 1;
  }


#if ll_USE_TYPE_METH_ALIST
  /* Search operation's type-meth map. */
  x = _ll_assq_to_front(&o->_type_meth_alist, type);
#else
#if ll_USE_OP_METH_ALIST
  /* Search type's op-meth map. */
  x = _ll_assq_to_front(&t->_op_meth_alist, ll_AR_OP);
#else
#error ll_USE_TYPE_METH_ALIST or ll_USE_OP_METH_ALIST must be 1 
#endif
#endif

  if ( ll_unbox_boolean(x) ) {
    /* Put the method in the current message. */
    ll_AR_METH = ll_CDR(x);
    
    /* Remember which type we found the method in. */
    ll_AR_TYPE = type;

    return 1;
  }


  /* Not found yet, try supers. */
  x = t->_supers;
  while ( ll_NE(x, ll_nil) ) {
    if ( _ll_lookup1(ll_CAR(x)) )
      return 1;
    x = ll_CDR(x);
  }

  return 0;
}


static
void _ll_get_type_offset(ll_v isa)
{
  ll_tsa_type *t = ll_THIS_ISA(type, isa);
  ll_v x;

  x = _ll_assq_to_front(&t->_type_offset_alist, ll_AR_TYPE);

  if ( ll_unbox_boolean(x) ) {
    ll_AR_TYPE_OFFSET = ll_CDR(x);
    return;
  }

  _ll_error(ll_fe(type), 3, 
	    ll_s(reason), ll_s(cannot_find_type_offset), 
	    ll_s(type), ll_AR_TYPE, 
	    ll_s(supertype), isa);
}


void _ll_print_trace(ll_v super)
{
  size_t i, argc;

  fprintf(stderr, "ll: trace: ");

  i = _ll_val_sp_end - _ll_val_sp;
  fprintf(stderr, "%4d ", i);

  while ( i -- ) {
    fprintf(stderr, " ");
  }

  if ( ll_NE(super, ll_undef) ) {
    fprintf(stderr, "SUPER: (%s ", ll_po(super));
  } else {
    fprintf(stderr, "(");
  }

  fprintf(stderr, "%s", ll_po(ll_AR_OP));

  for ( i = 0, argc = ll_AR_ARGC; i < argc; ++ i ) {
    fprintf(stderr, " %s", ll_po(ll_AR_ARGV[i]));
  }

  fprintf(stderr, ") in %s\n", ll_po(_ll_ar_sp[1]._meth));

  fflush(stderr);
}


/***********************************************************************/


ll_v _ll_method_not_found_error()
{
  /*
  fprintf(stderr, "\nll: method_not_found %s %s\n", 
	  ll_po(ll_AR_OP),
	  ll_po(ll_TYPE(ll_SELF)));
  abort();
  ll_abort();

  */

  return(_ll_error(ll_re(method_not_found), 0));
}


ll_define_primitive(object, _method_not_found, __0(args), _0())
{
  ll_return(_ll_method_not_found_error());
}
ll_define_primitive_end


static
void _ll_lookup_failed()
{
  /*
  ** Note: this code should do something like:
  ** Box up the message and attempt to forward it.
  */

  /* Use the <object>::%method-not-found method to generate an error. */
  ll_AR_TYPE = ll_type(object);
  ll_AR_TYPE_OFFSET = ll_make_fixnum(0);
  ll_AR_METH = _ll_p_method_not_found;
}

/***********************************************************************/

__inline
void  _ll_lookup_op_check(void)
{
#if 0
  if ( ! ll_initializing ) {
#endif
    ll_v op_type;
    
    while ( op_type = ll_TYPE(ll_AR_OP), 
	    (ll_NE(op_type, ll_type(operation)) && 
	     ll_NE(op_type, ll_type(settable_operation)) && 
	     ll_NE(op_type, ll_type(locatable_operation))) ) {
      ll_AR_OP = _ll_typecheck_error(ll_type(operation), ll_AR_OP);
    }
#if 0
  }
#endif

  ll_assert_ref(ll_AR_OP);
}


#if 1
#define _ll_lookup_op_check() ((void)0)
#endif


void _ll_lookup()
{
  /* FIXME: lcache may cause async signals to starve. */
  /* service any async signals */
  ll_sig_service();

  {
  ll_tsa_operation *op;
  ll_v isa = ll_RCVR_TYPE;

  ll_assert_ref(isa);
  ll_assert_msg(lookup, 0 <= ll_AR_ARGC, ("corrupted ar?"));
  ll_assert_msg(lookup, ll_AR_ARGC <= 256, ("corrupted ar?"));

  if ( ll_DEBUG(trace) ) {
    _ll_print_trace(ll_undef);
  }

  _ll_lookup_op_check();

  op = ll_THIS_ISA(operation, ll_AR_OP);

  ll_DEBUG_PRINT(lookup, ("_ll_lookup(op = %s, isa = %s)\n", ll_po(ll_AR_OP), ll_po(isa)));

#if ll_LOOKUP_CACHE_STATS
  ll_g(_lookup_op_cache_lookup) = 
    ll_BOX_fixnum(ll_UNBOX_fixnum(ll_g(_lookup_op_cache_lookup)) + 1);
#endif

  /* Is the cache valid? */
  if ( ! ll_EQ(op->_cache_type, isa) ) {
    
    /* If not, do full lookup. */
    if ( ! _ll_lookup1(isa) ) {
      /* We did not find a method. */
      _ll_lookup_failed();
      return;
    }

    /* Figure out the type offset. */
    _ll_get_type_offset(isa);
    
    /* Fill cache. */
    op->_cache_type = isa;
    op->_cache_impl = ll_AR_TYPE;
    op->_cache_offset = ll_AR_TYPE_OFFSET;
    op->_cache_method = ll_AR_METH;
    
#if ll_LOOKUP_CACHE_STATS
    ll_g(_lookup_op_cache_miss) = 
      ll_BOX_fixnum(ll_UNBOX_fixnum(ll_g(_lookup_op_cache_miss)) + 1);
#endif

    ll_DEBUG_PRINT(lookup, 
		   ("_ll_lookup(op = %s, isa = %s, imp = %s, meth = %s)\n", 
		    ll_po(ll_AR_OP), 
		    ll_po(isa), 
		    ll_po(ll_AR_TYPE), 
		    ll_po(ll_AR_METH)));
    
    return;
  }

#if ll_LOOKUP_CACHE_STATS
  ll_g(_lookup_op_cache_hit) = 
    ll_BOX_fixnum(ll_UNBOX_fixnum(ll_g(_lookup_op_cache_hit)) + 1);
#endif

  /* Load from cache. */
  ll_AR_TYPE =        op->_cache_impl;
  ll_AR_TYPE_OFFSET = op->_cache_offset;
  ll_AR_METH =        op->_cache_method;

  ll_DEBUG_PRINT(lookup, 
		 ("_ll_lookup(op = %s, isa = %s, imp = %s, meth = %s): CACHED\n", 
		  ll_po(ll_AR_OP), 
		  ll_po(isa), 
		  ll_po(ll_AR_TYPE), 
		  ll_po(ll_AR_METH)));

  }
}


void _ll_lookup_super(ll_v super)
{
  /* FIXME: lcache may cause async signals to starve. */
  /* service any async signals */
  ll_sig_service();

  {
  ll_tsa_operation *op;
  ll_v isa = ll_RCVR_TYPE;

  ll_assert_ref(isa);
  ll_assert_ref(super);

  if ( ll_DEBUG(trace) ) {
    _ll_print_trace(super);
  }

  _ll_lookup_op_check();

  op = ll_THIS_ISA(operation, ll_AR_OP);

  /*
  ** If super is #f, begin lookup in the previous message's 
  ** implementor's supertypes.
  */
  if ( ll_EQ(super,ll_f) ) {
    ll_v x;
    
    for ( x = ll_THIS_ISA(type,ll_AR_TYPE)->_supers; 
	  ll_NE(x, ll_nil); 
	  x = ll_CDR(x)
	  ) {
      if ( _ll_lookup1(ll_CAR(x)) )
	goto get_offset;
    }
    goto not_found;
  } else {
    /* Lookup within the specified supertypes */
    if ( ! _ll_lookup1(super) ) {
    not_found:
      /* We did not find a method */
      _ll_lookup_failed();
      return;
    }
  }
 get_offset:
  _ll_get_type_offset(isa);
  }
}


/***********************************************************************/


ll_v __ll_lookup(ll_v type, ll_v op, ll_v super)
{
  ll_v phony_object[1] = { type };
  ll_v meth;

  ll_assert_ref(type);
  ll_assert_ref(op);

  _ll_VS_PROBE(1);
  _ll_PUSH(ll_make_ref(phony_object));

  __ll_lookup_begin(op, 1, (ll_NE(super, ll_undef) ? _ll_lookup_super(super) : _ll_lookup()));
  meth = ll_AR_METH;
  __ll_lookup_end();

  _ll_POP();
  _ll_VS_POP(_ll_val_sp);

  return meth;
}


ll_define_primitive(type, lookup, __2(type, op, super), _1(no_side_effect, "#t"))
{
  ll_v type = ll_SELF;
  ll_v op = ll_ARG_1;
  ll_v super = ll_ARGC >= 3 ? ll_ARG_2 : ll_undef;
  ll_v meth = __ll_lookup(type, op, super);

  if ( ll_EQ(meth, _ll_p_method_not_found) ) {
    meth = ll_f;
  }

  ll_return(meth);
}
ll_define_primitive_end


/***********************************************************************/

ll_INIT(lookup1, 121, "op cache stats")
{
  ll_g(_lookup_op_cache_change) = ll_BOX_fixnum(0);
  ll_g(_lookup_op_cache_lookup) = ll_BOX_fixnum(0);
  ll_g(_lookup_op_cache_lambda) = ll_BOX_fixnum(0);
  ll_g(_lookup_op_cache_hit) = ll_BOX_fixnum(0);
  ll_g(_lookup_op_cache_miss) = ll_BOX_fixnum(0);

  return 0;
}

