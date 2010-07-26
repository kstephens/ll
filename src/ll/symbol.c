#ifndef __rcs_id__
#ifndef __rcs_id_ll_symbol_c__
#define __rcs_id_ll_symbol_c__
static const char __rcs_id_ll_symbol_c[] = "$Id: symbol.c,v 1.28 2008/05/24 20:54:39 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include "symbol.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>


/************************************************************************/


/* Avoid "defined but not used" errors. */
// #define HASH_EXTERN static
#include "hash/charP_hash.c"

#define HASH(X)_ll_symbol_##X
#define HASH_KEY const char*
#define HASH_KEY_HASH(X) strhash(X)
#define HASH_KEY_EQUAL(X,Y) (X[0] == Y[0] && strcmp(X,Y)==0)
#define HASH_KEY_SET(X,Y)((X)=(Y),ll_write_barrier(&(X)))

#define HASH_DATA ll_v
#define HASH_DATA_SET(X,Y)((X)=(Y),ll_write_barrier(&(X)))

#define HASH_MALLOC(X) ll_malloc(X)
#define HASH_REALLOC(X,Y) ll_realloc(X,Y)
#define HASH_FREE(X) ll_free(X)

#include "hash/hash.c"
#include "hash/hash_end.def"


/************************************************************************/


#define symtable ll_runtime(symbol_table)


void _ll_symtable_init(int size)
{
  if ( ! symtable ) {
    symtable = ll_malloc(sizeof(*symtable));
    _ll_symbol_TableInit(symtable, size);
    _ll_symbol_TableSize(symtable);
    _ll_symbol_TableNEntries(symtable);
    _ll_symbol_TableCollisions(symtable);
    _ll_symbol_TableCollisions(symtable);
  }
}


void _ll_intern_symbol(ll_v sym, ll_v str)
{
  /* Do not intern anonymous symbols. */
  if ( ll_EQ(str, ll_f) ) {
    return;
  }

  ll_set_g(_symbols, ll_cons(sym, ll_g(_symbols)));

  {
    const char *name = ll_ptr_string(str);
    _ll_symbol_TablePush(symtable, name, sym);
  }
}


ll_v _ll_interned_symbol_(const char *name)
{
  ll_v *x;

  /* Handle interned symbol of _ll_name_symbol(uninterned_symbol) */
  if ( ! name ) {
    return ll_f;
  }

  x = _ll_symbol_TableGet(symtable, name);
  return x ? *x : ll_f;
}


ll_v _ll_interned_symbol(ll_v str)
{
  const char *name = ll_ptr_string(str);
  return _ll_interned_symbol_(name);
}


static struct strmap {
  const char *in;
  const char *out;
} _smap[] = {
  { "NEG", "neg" },
#define BOP(N,O) { #N, #O },
  { "EQ", "=" },
#include "cops.h"
  { "AS", "=" },
  { "__", "->" },
  { "_", "-" },
  { "P", "%" },
  { "C", ":" },
  { "D", "." },
  { "Q", "?" },
  { "E", "!" },
  { "S", "*" },
  { " ", "-" },
  { 0 , 0 }
};


static
const char *strprefix(const char *str, const char *pre)
{
  while ( *pre ) {
    if ( *pre != *str )
      return 0;
    pre ++;
    str ++;
  }
  return str;
}


char *_ll_escape_symbol(const char *in, char *buf)
{
  char *out = buf;

 restart:
  while ( *in == '_' ) {
    *(out ++) = '%';
    in ++;
  }
  while ( *in ) {
    if ( *in == '_' || isupper(*in) ) {
      struct strmap *map = _smap;
      while ( map->in ) {
	const char *e;
	if ( (e = strprefix(in, map->in)) ) {
	  const char *t = map->out;
	  in = e;
	  while ( *t ) {
	    *(out ++) = *(t ++);
	  }

	  if ( ! isalnum(out[-1]) )
	    goto restart;

	  goto next_in;
	}
	map ++;
      }
    }
    *(out ++) = *(in ++);
  next_in:
    ;

  }
  *out = '\0';

  return buf;
}


static
ll_v _ll_allocate_symbol(ll_v str)
{
  ll_v sym;
  ll_tsa_symbol *x;

  sym = _ll_allocate_type(ll_type(symbol));
  x = ll_THIS_ISA(symbol, sym);
  x->_name = str;
  x->_properties = ll_nil;
  return sym;
}


ll_v _ll_make_symbol(ll_v str)
{
  const char *name;

  if ( ll_unbox_boolean(str) ) {
    name = ll_ptr_string(str);
  } else {
    name = 0;
  }

  return _ll_make_symbol_(name);
}


void _ll_initialize_symbol_(ll_tsa_symbol *x, const char *name)
{
  ll_v str;

  if ( ! (name && *name) ) {
    /* gensym: not interned.  */
    str = ll_f;
  } else {
    if ( *name == '\001' ) {
      name = _ll_escape_symbol(name + 1, alloca(strlen(name) + 1));
    }
    str = ll_make_copy_string(name, -1);
  }

  x->super_object._isa = ll_type(symbol);
  x->_name = str;
  x->_properties = ll_nil;
}


ll_v _ll_make_symbol_(const char *name)
{
  ll_v sym, str;

  if ( ! (name && *name) ) {
    /* gensym: not interned.  */
    sym = _ll_allocate_symbol(ll_f);
  } else {
    if ( *name == '\001' ) {
      name = _ll_escape_symbol(name + 1, alloca(strlen(name) + 1));
    }
    
    sym = _ll_interned_symbol_(name);

    if ( ll_EQ(sym, ll_f) ) {
      str = ll_make_copy_string(name, -1);

      sym = _ll_allocate_symbol(str);

      _ll_intern_symbol(sym, str);
    }
  }

  return sym;
}


ll_v _ll_make_symbol_escape(const char *name)
{
  char *buf = alloca(strlen(name) + 1);
  return _ll_make_symbol_(_ll_escape_symbol(name, buf));
}


/********************************************************************/


ll_define_primitive(symbol, symbol__string, _1(sym), _1(no_side_effect,"#t"))
{
  ll_THIS->_name = ll_call(ll_o(make_immutable), _1(ll_THIS->_name));
  ll_write_barrier_SELF();
  ll_return(ll_THIS->_name);
}
ll_define_primitive_end


ll_define_primitive(symbol, initialize, __1(sym, name), _0())
{
  ll_v sym;

  ll_THIS->_name = ll_ARGC >= 2 ? ll_ARG_1 : ll_f;
  ll_THIS->_properties = ll_ARGC >= 3 ? ll_ARG_2 : ll_nil;
  ll_write_barrier_SELF();

  if ( ll_NE(ll_THIS->_name, ll_f) ) {
    /* Look for an interned symbol of the same name */
    sym = _ll_interned_symbol(ll_THIS->_name);

    if ( ll_NE(sym, ll_f) ) {
      ll_return(sym);
    }

    /* Save a copy of the name and intern ourselves */
    ll_THIS->_name = ll_call(ll_o(clone), _1(ll_THIS->_name));
    ll_write_barrier_SELF();

    _ll_intern_symbol(ll_SELF, ll_THIS->_name);
  }
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(symbol, locative_properties, _1(self), _1(no_side_effect,"#t"))
     /* Access properties. */
{
  ll_return(ll_make_locative(&ll_THIS->_properties));
}
ll_define_primitive_end


ll_define_primitive(object, _gensym, _0(), _0())
     /* Generates a unique uninterned symbol. */
{
  ll_call_tail(ll_o(make), _1(ll_type(symbol)));
}
ll_define_primitive_end


ll_define_primitive(object, llChashstats, _0(), _0())
     /* Dumps symbol hash table stats */
{
  _ll_symbol_TableStats(symtable, stderr);
}
ll_define_primitive_end


/********************************************************************/


const char *_ll_name_symbol(ll_v v)
{
  ll_v t = ll_TYPE(v);
  if ( ll_eqQ(t, ll_type(symbol)) ) {
    ll_tsa_symbol *x = ll_THIS_ISA(symbol,v);
    return ll_NE(x->_name, ll_f) ? ll_ptr_string(x->_name) : 0;
  } else {
    return ll_ptr_string(ll_call(ll_o(symbol__string), _1(v)));
  }
}


/********************************************************************/

/* EOF */
