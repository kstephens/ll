#ifndef _ll_init_h
#define _ll_init_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_init_h__
#define __rcs_id_ll_init_h__
static const char __rcs_id_ll_init_h[] = "$Id: init.h,v 1.3 2003/02/15 09:32:12 stephens Exp $";
#endif
#endif /* __rcs_id__ */


extern int ll_initialized;
extern int ll_initializing;

/* Declarator for the initializer function */
#define ll_INIT_DECL(X) int X (int *_argcp, char ***_argvp, char ***_envp)

typedef struct _ll_init { /* <DOC STRUCT>Initializer object. */
  const char *name;
  int pri;         
  ll_INIT_DECL((*func));
  const char *desc;    
  const char *file;
  int line;
} _ll_init; /* </DOC STRUCT> */

ll_INIT_DECL(ll_init);

#ifndef _ll_INIT
#define _ll_INIT(NAME,PRI,DESC) \
  ll_INIT_DECL(_ll_if_##NAME); \
  _ll_init _ll_i_##NAME = { \
     #NAME, \
     PRI, \
     &_ll_if_##NAME, \
     DESC, \
     __FILE__, \
    __LINE__ \
  }; \
  ll_INIT_DECL(_ll_if_##NAME)
#endif /* _ll_INIT */

#ifndef ll_INIT
#define ll_INIT(NAME,PRI,DESC)_ll_INIT(NAME,PRI,DESC)
#endif

#ifndef ll_INIT_END
#define ll_INIT_END
#endif

extern _ll_init *_ll_inits[];

#endif

