#ifndef _ll_symbol_h
#define _ll_symbol_h

#include "ll/ll.h" /* ll_v */

void _ll_symtable_init(int size);

ll_v _ll_make_symbol(ll_v str);
ll_v _ll_make_symbol_(const char *name);
ll_v _ll_make_symbol_escape(const char *name);

const char *_ll_name_symbol(ll_v sym);
char *_ll_escape_symbol(const char *in, char *out);

ll_v _ll_make_symbol_(const char *name);
void _ll_initialize_symbol_(ll_tsa_symbol *x, const char *name);
void _ll_intern_symbol(ll_v sym, ll_v str);
ll_v _ll_interned_symbol_(const char *name);


#ifndef ll_s_def

#if ll_GLOBAL_VARS

/* Symbols structures are dynamically allocated. */

extern ll_v 
#define ll_s_def(X) _ll_s_##X,
#include "ll/symbols.h"
 _ll_s_;

/* Returns the ll_v for a symbol. */
#define ll_s(X) _ll_s_##X

#else

/* Symbols structures are statically allocated. */
extern ll_tsa_symbol
#define ll_s_def(X) _ll_s_##X,
#include "ll/symbols.h"
 _ll_s_;

/* Returns the ll_v for a symbol. */
#define ll_s(X) ll_BOX_ref(&_ll_s_##X)

#endif /* ll_GLOBAL_VARS */

#endif /* ll_s_def */

#endif
