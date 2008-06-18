#ifndef __rcs_id__
#ifndef __rcs_id_ll_init_c__
#define __rcs_id_ll_init_c__
static const char __rcs_id_ll_init_c[] = "$Id: init.c,v 1.24 2008/05/25 23:11:49 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"

#include <string.h> /* strcmp(), strchr() */

#include "assert.h"

#ifdef ll_DEBUG_PRINTF
#undef ll_DEBUG_PRINTF
#define ll_DEBUG_PRINTF(N,F)(fprintf(stderr, "%s", #N), fprintf(stderr, F), 0)
#endif

/***************************************************************************/
/* initialize validation */

typedef struct ll_validator {
  const char *name;
  void (*func)(void);
} ll_validator;

static ll_validator validators[20];
static int nvalidators = 0;

void _ll_add_validator(const char *name, void *func)
{
  validators[nvalidators].name = name;
  validators[nvalidators].func = func;
  nvalidators ++;
}


ll_v _ll_validate_ref(ll_v x)
{
  ll_v isa;

  ll_assert_ref(x);
  ll_assert_ref(isa = ll_TYPE(x));
  ll_assert_msg(init,ll_EQ(ll_TYPE(isa),ll_type(type)),("%s isa = %s", ll_po(x), ll_po(isa)));

  return isa;
}

void _ll_validate()
{
  int i;

  for ( i = 0; i < nvalidators; i ++ ) {
    /* fprintf(stderr, "ll: validate %s...", validators[i].name); */
    (validators[i].func)();
    /* fprintf(stderr, " ok.\n"); */
  }
}

/***************************************************************************/

int ll_initialized, ll_initializing;

static int init_cmp(const void *a, const void *b)
{
  return (*(_ll_init**) a)->pri - (*(_ll_init**) b)->pri;
}


ll_INIT(argv, 900, "argv, envp")
{
  ll_v list, *lp;
  int i;

  /* Save arg list. */
  list = ll_nil; lp = &list;
  for ( i = 0; (*_argvp)[i]; ++ i ) {
    *lp = ll_cons(ll_make_copy_string((*_argvp)[i], -1), ll_nil);
    lp = &ll_CDR(*lp);
  }
  ll_g(llCSargvS) = list;

  /* Save environment list. */
  list = ll_nil; lp = &list;
  for ( i = 0; (*_envp)[i]; ++ i ) {
    const char *key_s = (*_envp)[i], *key_e, *eq_s, *val_s;
    ll_v key, val;

    if ( (eq_s = strchr(key_s, '=')) ) {
      eq_s = key_s;
      key_e = eq_s;
      val_s = eq_s + 1;
    } else {
      key_e = strchr(key_s, '\0');
      val_s = key_s;
    }

    key = ll_make_copy_string(key_s, key_e - key_s);
    val = ll_make_copy_string(val_s, -1);
    *lp = ll_cons(ll_cons(key, val), ll_nil);
    lp = &ll_CDR(*lp);
  }
  ll_g(llCSenvS) = list;

  return 0;
}


ll_INIT(libll, 1000, "libll complete")
{
  return 0;
}


static
void ll_init_print(_ll_init *p, const char *msg)
{
  char file_line[32];
  char name[32];

  snprintf(file_line, sizeof(file_line) - 1, "%s:%d", p->file, p->line);
  strcpy(name, p->name);
  {
    char *s = name;
    while ( *s && ! strchr("0123456789", *s) )
      ++ s;
    *s = 0;
  }
  fprintf(stderr, "ll: init %4d %-16s %s: %s%s", p->pri, file_line, name, p->desc, msg ? msg : "");
}


int ll_init(int *argcp, char ***argvp, char ***envp)
{
  int rtn = 0;
  int print_inits = 0;
  const char *env;

  print_inits = (env = getenv("LL_DEBUG_INIT")) ? atoi(env) : 0;

  if ( ! (ll_initialized || ll_initializing) ) {
    int i;
    _ll_init *p;
    int pri_clash = 0;

    CCont_init(argvp);

    ll_initializing ++;

    /* Sort initializers by priority. */
    for (i = 0; _ll_inits[i]; i ++)
      ;
    qsort(_ll_inits, i, sizeof(_ll_inits[0]), init_cmp);

    for ( i = 0; (p = _ll_inits[i ++]); ) {
      int clash = 0;

      if ( ! p->name ) {
	p->name = "";
      }
      if ( ! p->desc ) {
	p->desc = "";
      }

      if ( (clash = _ll_inits[i] && p->pri >= _ll_inits[i]->pri) ) {
	ll_warn_msg(init, p->pri >= _ll_inits[i]->pri, ("init(%s) >= init(%s)", p->name, _ll_inits[i]->name));
	pri_clash ++;
      }

      if ( print_inits || clash ) {
	ll_init_print(p, 0);
	if ( clash ) {
	  fprintf(stderr, " <= CLASH\n");
	}
      }

      rtn = (p->func)(argcp, argvp, envp);

      if ( print_inits ) {
	fprintf(stderr, "%s\n", rtn ? ": FAILED!" : ".");
      }

      if ( rtn ) {
	if ( ! print_inits ) {
	  ll_init_print(p, ": FAILED!\n");
	}

	break;
      }
      
      _ll_validate();
    }

    ll_assert_msg(init, i > 1, ("inits.h might be empty"));
    ll_assert_msg(init, pri_clash == 0, ( "%d inits clashed", pri_clash));

    ll_initializing --;
    ll_initialized ++;

    /* ll_call(ll_o(load), _1(ll_make_string("lib/ll/test/test.scm", -1))); */
    /* ll_call(ll_o(load), _1(ll_make_string("${HOME}/.ll_init.scm", -1))); */
  }

  return rtn;
}
