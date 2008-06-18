#ifndef __rcs_id__
#ifndef __rcs_id_ll_prims_c__
#define __rcs_id_ll_prims_c__
static const char __rcs_id_ll_prims_c[] = "$Id: prims.c,v 1.6 2008/05/24 20:44:08 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll/prim.h"

#ifndef ll_p_def
#define ll_p_def(T,O,ARGS,OPTS) \
ll_declare_primitive(T,O);
#include "ll/prims.h"
#endif

_ll_prim_init _ll_prim_inits[] = {
#ifndef ll_p_def
#define ll_p_def(T,O,ARGS,OPTS) { ll_gi(LT##T##GT), ll_gi(O), &ll_prim(T,O) },
#include "ll/prims.h"
#endif
  { 0 }
};


/***************************************************************************/

#include "ll.h"
#include <string.h>

ll_INIT(prim1, 190, "create the <primitive> methods")
{
  int i, j;
  _ll_prim_init *x = _ll_prim_inits;

  for ( i = 0; x[i].prim; ++ i ) {
    char buf[256];
    ll_v type, op, prim;
    ll_v formals = ll_nil, props = ll_nil;

    strcpy(buf, "<");
    _ll_escape_symbol(x[i].prim->_type, strrchr(buf, '\0'));
    strcat(buf, ">:");
    _ll_escape_symbol(x[i].prim->_op, strrchr(buf, '\0'));

#if 0
    fprintf(stderr, "%s\n", buf);
#endif

    type = _ll_get_gi(x[i].type);
    op = _ll_get_gi(x[i].op);

    {
      const char *name;
      ll_v *xp;

      /* formals */
      for ( xp = &formals, j = 0; (name = x[i].prim->_formals[j]); ++ j ) {
	ll_v sym = _ll_make_symbol_escape(name);

	if ( (! x[i].prim->_formals[j + 1]) && x[i].prim->_restarg ) {
	  *xp = sym;
	} else {
	  *xp = ll_cons(sym, ll_nil);
	  xp = &ll_CDR(*xp);
	}
      }

      /* props */
      for ( xp = &props, j = 0; (name = x[i].prim->_props[j]); ++ j ) {
	const char *value_str = x[i].prim->_props_values[j];
	ll_v value = ll_undef;

	ll_assert(prim,value_str);
	if ( strcmp(value_str, "#t") == 0) {
	  value = ll_t;
	}
	else if ( strcmp(value_str, "#f") == 0 ) {
	  value = ll_f;
	}
	else {
	  value = ll_make_string((char*) value_str, -1);
	}

	*xp = ll_cons(ll_cons(_ll_make_symbol_escape(name), value), ll_nil);
	xp = &ll_CDR(*xp);
      }
    }

    prim =
      _ll_make_primitive(
			 x[i].prim->_func, 
			 buf,
			 formals,
			 props);

    x[i].prim->_prim = prim;

  }

  return 0;
}


ll_INIT(prim2, 210, "install the <primitive> methods")
{
  int i;
  _ll_prim_init *x = _ll_prim_inits;

  /* Install the primitive methods in their respective types. */
  for ( i = 0; x[i].prim; ++ i ) {
    ll_v type, op, prim;

    type = _ll_get_gi(x[i].type);
    op = _ll_get_gi(x[i].op);
    prim = x[i].prim->_prim;

    if ( ll_DEBUG(init_prim) ) {
      fprintf(stdout, "<%s>:%s\n", x[i].prim->_type, x[i].prim->_op);
    }

    _ll_add_method(type, op, prim);
  }


  /* Do lookup on all system primitives to make sure. */
  for ( i = 0; x[i].prim; ++ i ) {
    ll_v type, op, prim, meth;

    type = _ll_get_gi(x[i].type);
    op = _ll_get_gi(x[i].op);
    prim = x[i].prim->_prim;

    meth = __ll_lookup(type, op, ll_undef);
    ll_assert_msg(lookup,ll_EQ(meth, prim), ("primitive method <%s>:%s lost", x[i].prim->_type, x[i].prim->_op));
  }


  return 0;
}

