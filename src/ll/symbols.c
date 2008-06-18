#ifndef __rcs_id__
#ifndef __rcs_id_ll_symbols_c__
#define __rcs_id_ll_symbols_c__
static const char __rcs_id_ll_symbols_c[] = "$Id: symbols.c,v 1.5 2008/05/24 20:44:08 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll/ll.h"

#include "ll/symbol.h"

typedef struct _ll_symbol_def {
  const char *name;
  ll_tsa_symbol *sp;
  ll_v *vp;
} _ll_symbol_def;


#if ll_MULTIPLICTY

ll_v
#ifndef ll_s_def
#define ll_s_def(X) _ll_s_##X,
#include "ll/symbols.h"
#endif
  _ll_s_;
#else

ll_tsa_symbol
#ifndef ll_s_def
#define ll_s_def(X) _ll_s_##X,
#include "ll/symbols.h"
#endif
  _ll_s_;

#endif

_ll_symbol_def _ll_symbol_defs[] = {
#ifndef ll_s_def
#if ll_MULTIPLICTY
#define ll_s_def(X) { #X, 0, &_ll_s_##X },
#else
#define ll_s_def(X) { #X, &_ll_s_##X, 0 },
#endif

#include "ll/symbols.h"
#endif
  { 0 }
};


/********************************************************************/


ll_INIT(symbol, 150, "create interned symbols")
{
  int i;
  _ll_symbol_def *def;

  ll_DEBUG_PRINT(init_symbol, ("BEGIN\n"));
  ll_set_g(_symbols, ll_nil);

  /* Compute number of initial interned symbols. */
  for ( i = 0; _ll_symbol_defs[i].name; i ++ ) {
    /* NOTHING */
  }

  _ll_symtable_init(i * 2);

  def = _ll_symbol_defs;
  for ( def = _ll_symbol_defs; def->name; ++ def ) {
    char sname[256]; 
    ll_v sym;

    _ll_escape_symbol(def->name, sname);

#if ll_GLOBAL_VARS
    sym = *(def->vp) = _ll_make_symbol_(sname);
    ll_write_root(def->vp);
#else
    sym = ll_BOX_ref(def->sp);
    _ll_initialize_symbol_(def->sp, sname);
    _ll_intern_symbol(sym, def->sp->_name);
#endif

    ll_DEBUG_PRINT(init_symbol, (" '%s' => (string->symbol \"%s\") => %p\n", def->name, sname, ll_UNBOX_ref(sym)));
  }

  ll_DEBUG_PRINT(init_symbol, ("END\n"));

  return 0;
}


