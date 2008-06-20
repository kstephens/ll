#ifndef __rcs_id__
#ifndef __rcs_id_ll_type_c__
#define __rcs_id_ll_type_c__
static const char __rcs_id_ll_type_c[] = "$Id: types.c,v 1.8 2008/05/24 20:44:08 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include "call_int.h"
#include "assert.h"

#include <string.h> /* strcmp() */

/*************************************************************************/
/* type bootstrapping. */

typedef struct ll_typedef_slot {
  const char *type;
  size_t _sizeof;
  const char *name;
  size_t offset;
} ll_typedef_slot;

typedef struct ll_typedef {
  const char *name;
  struct ll_typedef *type; /* Points to metatype def. */
  struct ll_typedef *supers[6];
  ll_typedef_slot slots[125];

  ll_v *_type;
  _ll_gi _tester_i;
  _ll_gi _global_index;
  size_t _runtime_offset;

  short visited;

  size_t _sizeof;
  size_t _sizeof_all;

  int _type_supers_n;
  struct ll_typedef *_type_supers[16];
  size_t _type_offsets[16];

} ll_typedef;


/*******************************************************************/

#define _ll_type_def(N,T) static ll_typedef _ll_td_##N;
#include "ll/types_begin.h"
#include "ll/type_defs.h"
#include "ll/types_end.h"

#define _ll_type_def(N,T) static ll_typedef _ll_td_##N = { #N, &_ll_td_##T,
#define _ll_type_def_supers(N) { 
#define _ll_type_def_super(N,T) &_ll_td_##T,
#define _ll_type_def_supers_end(N) 0 },
#define _ll_type_def_slots(N) {
#define _ll_type_def_slot(N,T,S) { #T, sizeof(T), #S, ll_OFFSET_OF(ll_ts_##N, _##S) },
#define _ll_type_def_slots_end(N) { 0 } },
#define _ll_type_def_end(N) 0, ll_gi(N##Q), ll_gi(LT##N##GT), ll_OFFSET_OF(ll_tsa__runtime, _t_##N) };
#include "ll/types_begin.h"
#include "ll/type_defs.h"
#include "ll/types_end.h"

static ll_typedef *_types[] = {
#define _ll_type_def(N,T) &_ll_td_##N,
#include "ll/types_begin.h"
#include "ll/type_defs.h"
#include "ll/types_end.h"
  0
};



/***************************************************************************/


static
void _ll_init_type_size(ll_typedef *t, ll_typedef *s)
{
  int i;

  if ( s->visited )
    return;
  s->visited = 1;

  for ( i = 0; s->supers[i]; i ++ ) {
    _ll_init_type_size(t, s->supers[i]);
  }
  t->_type_offsets[t->_type_supers_n] = t->_sizeof_all;
  t->_type_supers[t->_type_supers_n] = s;
  t->_type_supers_n ++;
  t->_sizeof_all += s->_sizeof;
}


static
ll_typedef *_ll_td_for_type(ll_v type)
{
  ll_typedef **t;
  for ( t = _types; *t; t ++ ) {
    if ( ll_EQ(*((*t)->_type), type) )
      return *t;
  }
  return 0;
}


static
int  _ll_td_slot_offset(ll_typedef *t, ll_typedef *o, const char *name)
{
  int i, j;

  for ( i = 0; i < t->_type_supers_n; ++ i ) {
    if ( t->_type_supers[i] == o ) {
      for ( j = 0; o->slots[j].type; ++ j ) {
	if ( name[0] == o->slots[j].name[0] && 
	     strcmp(name, o->slots[j].name) == 0 ) {
	  /*
	    fprintf(stderr, "slot offset: %s %s %s = %d\n", t->name, o->name, name, t->_type_offsets[i] + o->slots[j].offset);
	  */
	  return t->_type_offsets[i] + o->slots[j].offset;
	}
      }
    }
  }

  fprintf(stderr, "_ll_td_slot_offset(%s, %s, %s): FAILED\n", t->name, o->name, name);
  abort();

  return -1;
}


static
ll_v  *_ll_slot(ll_v x, ll_v type, const char *name)
{
  char *v = ll_UNBOX_ref(x);
  ll_v isa = ll_TYPE(x);

  return (ll_v*)(v + 
		 _ll_td_slot_offset(_ll_td_for_type(isa), 
				    _ll_td_for_type(type), 
				    name)
		 );
}
#define SLOT(X,T,N) *_ll_slot(X,T,#N)

static int _type_size_index;


#define VARS \
  ll_typedef **t = (t = 0), **t2 = (t2 = 0); \
  ll_typedef_slot *s = (s = 0); \
  int i = (i = 0); \


ll_INIT(type1, 30, "calculate type slot sizes and offsets")
{
  VARS;

  for ( t = _types; *t; ++ t ) {
    size_t t_sizeof = 0;

    ll_assert_msg(init,(*t)->slots[(sizeof((*t)->slots)/sizeof((*t)->slots[0]))-1].name == 0, ("ll_typedef.slots[] too small"));
    for ( s = (*t)->slots; s->type; s ++ ) {
      /* Make sure the C struct is layed out like the ll structure */
      ll_assert_msg(init,s->offset == t_sizeof, ("sizeof(ll_tsa_%s) != calculated size; multiple inheritance?", (*t)->name));

      /* s->_offset = t_sizeof; */
      t_sizeof += s->_sizeof;    
    }

    (*t)->_sizeof = t_sizeof;
    /* printf("%s _sizeof = %lu\n", (*t)->name, (*t)->_sizeof); */
  }

  return 0;
}


ll_INIT(type2, 40, "calculate type total sizes")
{
  VARS;

  for ( t = _types; *t; ++ t ) {
    for ( t2 = _types; *t2; ++ t2 ) {
      (*t2)->visited = 0;
    }
    (*t)->_sizeof_all = 0;
    (*t)->_type_supers_n = 0;
    _ll_init_type_size((*t), (*t));

    if ( ll_DEBUG(init_type) ) {
      _ll_DEBUG_PRINT("%s _sizeof_all = %lu { ", (*t)->name, (*t)->_sizeof_all);
      for ( i = 0; i < (*t)->_type_supers_n; i ++  ) {
	_ll_DEBUG_PRINT("  %s @ %ld, ", (*t)->_type_supers[i]->name, (*t)->_type_offsets[i]);
      }
      _ll_DEBUG_PRINT("}\n");
    }
  }

  return 0;
}


ll_INIT(type3, 50, "allocate type objects; initialize type variables and globals; get the slot index for the <type>::size slot")
{
  VARS;

  for ( t = _types; *t; ++ t ) {
    void *runtime = ll_UNBOX_ref(ll_runtime_current());
    ll_v tt;

#if ll_TYPES_STATIC
    /*
    fprintf(stderr, "\n  type3 %s %d\n", (*t)->name, (*t)->_runtime_offset); 
    */
    tt = ll_make_ref(runtime + (*t)->_runtime_offset);
#else
    tt = ll_make_ref(ll_malloc((*t)->type->_sizeof_all));
#endif
    ll_assert_ref(tt);

    _ll_set_gi((*t)->_global_index, tt);
    (*t)->_type = &_ll_get_gi((*t)->_global_index);
    ll_write_root(*t);
  }

  _type_size_index = _ll_td_slot_offset(&_ll_td_type, &_ll_td_type, "size") / sizeof(ll_v);
  ll_assert(type, _type_size_index > 0);

  return 0;
}


ll_INIT(type4, 60, "slots: isa, size, slots_size")
{
  VARS;

  for ( t = _types; *t; ++ t ) {
    ll_v tt = *((*t)->_type);

    /*
      fprintf(stderr, "\n  type4 %s %s %p", (*t)->name, (*t)->type->name, tt);
    */
    /* ll_typedef *mt = (*t)->type; */

    ll_assert_ref(tt);
    ll_TYPE_ref(tt) = *((*t)->type->_type);
    ll_assert_ref(ll_TYPE_ref(tt));
    ll_write_barrier(tt);
    SLOT(tt, ll_type(type), size) = ll_make_fixnum((*t)->_sizeof_all);
    SLOT(tt, ll_type(type), slots_size) = ll_make_fixnum((*t)->_sizeof);
  }

  return 0;
}


ll_INIT(type5, 130, "slots: supers, type-offset-alist")
{
  VARS;

  for ( t = _types; *t; ++ t ) {
    ll_v tt = *((*t)->_type);

    /* ll_typedef *mt = (*t)->type; */

    ll_TYPE_ref(tt) = *((*t)->type->_type);
    SLOT(tt, ll_type(type), supers) = ll_nil;
    SLOT(tt, ll_type(type), slots) = ll_nil;
#if ll_USE_OP_METH_ALIST
    SLOT(tt, ll_type(type), op_meth_alist) = ll_nil;
#endif
    SLOT(tt, ll_type(type), type_offset_alist) = ll_nil;
    SLOT(tt, ll_type(type), top_wiredQ) = ll_f;
    SLOT(tt, ll_type(type), immutableQ) = ll_f;

    SLOT(tt, ll_type(type), properties) = ll_nil;
    ll_write_barrier_pure(tt);

    /* supers */
    {
      ll_v x = ll_nil, *xp = &x;
      
      for ( i = 0; (*t)->supers[i]; i ++ ) {
	*xp = ll_immutable_cons(*((*t)->supers[i]->_type), ll_nil);
	ll_write_barrier(xp);
	xp = &ll_CDR(*xp);
      }

      SLOT(tt, ll_type(type), supers) = x;
      ll_write_barrier_pure(tt);
    }

    /* type-offset-alist */
    {
      ll_v x = ll_nil, *xp = &x;
      
      for ( i = 0; i < (*t)->_type_supers_n; i ++ ) {
	ll_v a = ll_immutable_cons(*((*t)->_type_supers[i]->_type), ll_make_fixnum((*t)->_type_offsets[i]));
	*xp = ll_cons(a, ll_nil);
	ll_write_barrier(xp);
	xp = &ll_CDR(*xp);
      }

      SLOT(tt, ll_type(type), type_offset_alist) = x;
      ll_write_barrier_pure(tt);
    }
  }

  return 0;
}


ll_INIT(type6, 170, "slots: slots")
{
  VARS;

  for ( t = _types; *t; ++ t ) {
    ll_v tt = *((*t)->_type);
    /* ll_typedef *mt = (*t)->type; */

    /* slots */
    {
      ll_typedef_slot *slot;

      ll_v x = ll_nil, *xp = &x;
      
      for ( i = 0; (slot = &(*t)->slots[i])->type; i ++ ) {
	ll_v slot_name, slot_offset, slot_type;
	ll_v slot_item;

	slot_name = _ll_make_symbol_escape(slot->name);
	slot_offset = ll_make_fixnum(slot->offset);

	/* Don't add slot types for non ll_v types */
	if ( strcmp(slot->type, "ll_v") ) {
	  slot_type = _ll_make_symbol_(slot->type);
	} else {
	  slot_type = ll_nil;
	}
	
	/* a type's slots' item: (<slot-name> <slot-offset> . <slot-type>) */
	slot_item = ll_immutable_cons(slot_name, ll_immutable_cons(slot_offset, slot_type));
      
	/* Add to list */
	*xp = ll_immutable_cons(slot_item, ll_nil);
	xp = &ll_CDR(*xp);
      }

      /* set the slots */
      SLOT(tt, ll_type(type), slots) = x;
      ll_write_barrier_pure(tt);
    }
  }

  return 0;
}


ll_INIT(type7, 200, "add type query, coercer, and typechecker methods")
{
  VARS;

  for ( t = _types; *t; ++ t ) {
    ll_v tt = *((*t)->_type);
    ll_v op;

    ll_assert_msg(type, ll_ISA_ref(_ll_p_false), ("in type %s", (*t)->name));
    ll_assert_msg(type, ll_ISA_ref(_ll_p_true), ("in type %s", (*t)->name));
    ll_assert_msg(type, ll_ISA_ref(_ll_p_identity), ("in type %s", (*t)->name));

    /* Do false first so object? is true for <object>! */
    /* ((type X) tester) => #t or #f */
    op = 
    SLOT(tt, ll_type(type), tester) = _ll_get_gi((*t)->_tester_i);
    ll_assert_ref(op);
    ll_assert_msg(type, ll_ISA_ref(op), ("in type %s", (*t)->name));
    ll_write_barrier_pure(tt);

    _ll_add_method(ll_type(object), op, _ll_p_false);
    _ll_add_method(tt, op, _ll_p_true);

    /* (((type X) coercer) X) => X or error */
    op =
    SLOT(tt, ll_type(type), coercer) = _ll_make_operation();
    ll_write_barrier_pure(tt);

    _ll_add_method(tt, op, _ll_p_identity);

    /* (((type X) typechecker) X) => X or error */
    op = 
    SLOT(tt, ll_type(type), typechecker) = _ll_make_operation();
    ll_write_barrier_pure(tt);
 
    _ll_add_method(ll_type(object), op, _ll_p_typecheck_error);
    _ll_add_method(tt, op, _ll_p_identity);
  }

  return 0;  
}


ll_INIT(type8, 310, "make type vars readonly")
{
  VARS;

  for ( t = _types; *t; ++ t ) {
    char sname[256];
    ll_v sym, binding;

    {
      char buf[256];
      
      buf[255] = '\0';
      sprintf(buf, "LT%sGT", (*t)->name);
      ll_assert(type, buf[255] == '\0');
      _ll_escape_symbol(buf, sname);
    }

    sym = _ll_make_symbol_escape(sname);
    binding = ll_call(ll_o(_binding), _1(sym));
    ll_call(ll_o(make_readonlyE), _1(binding));
  }

  return 0;
}


#undef VARS

/*************************************************************************/


ll_v _ll_allocate_type(ll_v type)
{
  size_t size;
  ll_v x;

  ll_assert_ref(type);
  size = ll_unbox_fixnum(ll_SLOTS(type)[_type_size_index]);
  if ( ! size ) {
    return _ll_error(ll_e(uninstantiable, error), 1, ll_s(type), type);
  }
  x = ll_make_ref(ll_malloc(size));
  ll_SLOTS(x)[0] = type;
  /*
    ll_write_barrier(x);
  */

  return x;
}




