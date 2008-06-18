#ifndef __rcs_id__
#ifndef __rcs_id_ll_ops_c__
#define __rcs_id_ll_ops_c__
static const char __rcs_id_ll_ops_c[] = "$Id: ops.c,v 1.8 2008/06/02 20:44:11 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "ll/op.h"
#include "ll/global.h"
#include "op.h"
#include "assert.h"
#include <string.h> /* strncmp */


/************************************************************************/
/* operation inits */


_ll_op_init _ll_op_inits[] = {
#ifndef ll_o_def
#define ll_o_def(X) { #X, ll_gi(X) },
#include "ll/ops.h"
#endif
  { 0 }
};


/************************************************************************/
/* initialization */


static int is_locater_of(const char *nj, const char *ni)
{
  if ( nj == ni )
    return 0;

  /* if nj starts with "locative-" */
  if ( (strncmp(nj, "locative_", 9) == 0) ||
       (strncmp(nj, "locative-", 9) == 0)
       ) {
    nj += 9;

    return strcmp(nj, ni) == 0;
  }

  return 0;
}


static int is_setter_of(const char *nj, const char *ni)
{
  if ( nj == ni )
    return 0;

  /* If nj starts with "set-" */
  if ( (nj[0] == 's') && 
       (nj[1] == 'e') && 
       (nj[2] == 't') &&
       (nj[3] == '_' || nj[3] == '-') ) {
    nj += 4;

    while ( *nj ) {
      /* If at end of ni, */
      if ( (ni[0] == '\0') ) {
	/* If nj ends with "!", okay */
	if (
	    (nj[0] == 'E' || nj[0] == '!') && 
	    (nj[1] == '\0')
	    ) {
	  return 1;
	} else {
	  return 0;
	}
      }

      /* Match rest of string */
      if ( *nj != *ni )
	return 0;

      nj ++;
      ni ++;
    }

    return 0;
  } else {
    while ( *nj ) {
      /* If at end of ni, or ni ends with '-ref' */
      if ( (ni[0] == '\0') || 
	   (
	    (ni[0] == '-' || ni[0] == '_') &&
	    (ni[1] == 'r') &&
	    (ni[2] == 'e') &&
	    (ni[3] == 'f') &&
	    (ni[4] == '\0')
	    )
	   ) {
	/* If nj ends with "-set!", okay */
	if (
	    (nj[0] == '-' || nj[0] == '_') && 
	    (nj[1] == 's') &&
	    (nj[2] == 'e') &&
	    (nj[3] == 't') &&
	    (nj[4] == 'E' || nj[4] == '!') && 
	    (nj[5] == '\0') 
	    ) {
	  return 1;
	} else {
	  return 0;
	}
      }

      /* Match rest of string */
      if ( *nj != *ni )
	return 0;

      ++ nj;
      ++ ni;
    }

    return 0;
  }

  return 0;
}


static int _ll_validate_op()
{
  int i;
  const char *ni;
  _ll_op_init *x = _ll_op_inits;

  for ( i = 0; (ni = x[i].name); ++ i ) {
    ll_v gop = _ll_get_gi(x[i].gi);
    ll_v xop = x[i].op;
    ll_v isa;

    ll_assert_msg(init, ll_EQ(gop, xop), ("operation \"%s\": gop = %s, xop = %s", x[i].name, ll_po(gop), ll_po(xop)));

    isa = _ll_validate_ref(x[i].op);
    /* fprintf(stderr, "%s ", ni); */
    ll_assert_msg(init,
		  ll_EQ(isa, ll_type(operation)) || 
		  ll_EQ(isa, ll_type(settable_operation)) || 
		  ll_EQ(isa, ll_type(locatable_operation)),
		  ("isa = %s", ll_po(isa)));
  }

  return 0;
}


ll_INIT(op1, 180, "determine <operation> object types")
{
  int i, j;
  const char *ni, *nj;
  _ll_op_init *x = _ll_op_inits;

  /* Clear types. */
  for ( i = 0; (ni = x[i].name); ++ i ) {
    x[i].type = 0;
    x[i].locater = 0;
    x[i].setter = 0;
  }

  /* Scan for locative-<op>, <op> pairs. */
  for ( i = 0; (ni = x[i].name); ++ i ) {
    for ( j = 0; (nj = x[j].name); ++ j ) {
      if ( is_locater_of(nj, ni) ) {
	if ( ll_DEBUG(init_op) ) {
	  fprintf(stderr, "\n  %-16s is a <locatable-operation>", ni);
	  fprintf(stderr, "\n  %-16s is (locater %s)", nj, ni);
	}
	
	ll_assert_msg(init, ! ll_INITIALIZED(x[i].type), ("i %d op %s", i, ni));
	x[i].type = ll_type(locatable_operation);
	ll_write_root(&x[i].op);
	
	ll_assert_msg(init, ! x[i].locater, ("j %d op %s", j, nj));
	x[i].locater = &x[j];

	break;
      }
    }
  }

  /* Scan for set-<op>!, <op> pairs. */
  for ( i = 0; (ni = x[i].name); ++ i ) {
    for ( j = 0; (nj = x[j].name); ++ j ) {
      if ( is_setter_of(nj, ni) ) {
	if ( ! ll_INITIALIZED(x[i].type) ) {
	  if ( ll_DEBUG(init_op) ) {
	    fprintf(stderr, "\n  %-16s is a <settable-operation>\n", ni);
	  }
	  x[i].type = ll_type(settable_operation);
	}

	if ( ll_DEBUG(init_op) ) {
	  fprintf(stderr, "  %-16s is (setter %s)\n", nj, ni);
	}
	ll_assert_msg(init, ! x[i].setter, ("op %s", nj));
	x[i].setter = &x[j];
	
	break;
      }
    }
  }

  /* Everything else is simple <operation>. */
  for ( i = 0; (ni = x[i].name); ++ i ) {
    if ( ! ll_INITIALIZED(x[i].type) ) {
      if ( ll_DEBUG(init_op) ) {
	fprintf(stderr, "  %-16s is an <operation>\n", ni);
      }
      x[i].type = ll_type(operation);
      ll_write_root(&x[i].op);
    }
  }

  return 0;
}


ll_INIT(op2, 181, "create and install the <operation> objects")
{
  int i;
  const char *ni;
  _ll_op_init *x = _ll_op_inits;

  /* Create operations. */

  for ( i = 0; (ni = x[i].name); ++ i ) {
    ll_v op = ll_undef;

    if ( ll_EQ(x[i].type, ll_type(locatable_operation)) ) {
      op = _ll_make_locatable_operation();
    }
    else if ( ll_EQ(x[i].type, ll_type(settable_operation)) ) {
      op = _ll_make_settable_operation();
    } 
    else if ( ll_EQ(x[i].type, ll_type(operation)) ) {
      op = _ll_make_operation();
    } 
    else {
      ll_abort();
    }

    _ll_set_gi(x[i].gi, 
	       x[i].op = op);
  }

  /* Add validator */
  _ll_add_validator("op", &_ll_validate_op);

  return 0;
}


ll_INIT(op3, 182, "attach setter and locater <operation> objects")
{
  int i;
  const char *ni;
  _ll_op_init *x = _ll_op_inits;

  /* Scan for set-<op>!, <op> pairs */

  for ( i = 0; (ni = x[i].name); ++ i ) {
    ll_v op = x[i].op;
    ll_v op2;

    ll_assert_ref(op);

    if ( x[i].locater ) {
      op2 = x[i].locater->op;
      ll_assert_ref(op2);
      ll_LOCATER(op) = op2;
    }

    if ( x[i].setter ) {
      op2 = x[i].setter->op;
      ll_assert_ref(op2);
      ll_SETTER(op) = op2;
    }
  }

  return 0;
}


ll_INIT(op4, 320, "make op vars readonly")
{
  int i;
  const char *ni;
  _ll_op_init *x = _ll_op_inits;

  /* Make op vars readonly. */
  for ( i = 0; (ni = x[i].name); ++ i ) {
    ll_v sym, binding;

    sym = _ll_make_symbol_escape(ni);
    binding = ll_call(ll_o(_binding), _1(sym));
    ll_call(ll_o(make_readonlyE), _1(binding));
  }

  return 0;
}

