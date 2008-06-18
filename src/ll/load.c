#ifndef __rcs_id__
#ifndef __rcs_id_ll_load_c__
#define __rcs_id_ll_load_c__
static const char __rcs_id_ll_load_c[] = "$Id: load.c,v 1.21 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include <string.h> /* strchr() */
#include <unistd.h> /* access(), R_OK */
#include <stdio.h> /* snprintf() */


ll_define_primitive(input_port, load, _1(port), _0())
{
  int eval_each = getenv("ll_LOAD_EVAL_EACH") ? 1 : 0;
  ll_v result = ll_undef;
  ll_v port = ll_SELF;
  ll_v exprs = ll_nil;

  /* Read all exprs */
  {
    ll_v *ep = &exprs;
    ll_v expr = ll_undef;

    while ( (expr = ll_call(ll_o(read), _1(port))), ll_NE(expr, ll_eos) ) {
      if ( eval_each ) {
	//ll_format(ll_undef, "   load: ~S\n", 1, expr);
	result = ll_call(ll_o(eval), _1(expr));
      } else {
	*ep = ll_cons(expr, ll_nil);
	ep = &ll_CDR(*ep);
      }
    }
  }

  if ( ! eval_each ) {
    /*
      ll_format(ll_undef, "evaluating them all\n", 0);
    */

    /* Evaluate them all */
    result = ll_call(ll_o(eval_list), _1(exprs));
  }

  ll_return(result);
}
ll_define_primitive_end


ll_define_primitive(string, load, _1(file), _0())
{
  ll_v result = ll_undef;
  ll_v path = ll_SELF;
  ll_v load_path = ll_nil;
  ll_v port;

  const char *file = ll_ptr_string(path);

  /* Scan in directories for readable file, if filename is not absolute. */
  if ( file[0] != '/' ) {
    if ( access(file, R_OK) < 0 ) {
      ll_v load_path_dyn = ll_fluid(ll_s(load_path_dynamic));

      load_path = ll_g(llCSload_pathS);
      load_path = ll_append(load_path_dyn, load_path);

      /*
	ll_format(ll_undef, "load_path ~S\n", 1, load_path);
      */

      ll_LIST_LOOP(load_path, x);
      {
	char buf[1024];

	snprintf(buf, sizeof(buf) - 1, "%s/%s", ll_ptr_string(x), file);
	
	if ( access(buf, R_OK) >= 0 ) {
	  path = ll_make_copy_string(buf, -1);
	  break;
	}
      }
      ll_LIST_LOOP_END;
    }
  }

  ll_let_fluid();
  {
    ll_v path_dir = ll_undef;
    ll_v loading_path = ll_nil;

    /* Get the dir of the file path to
     * be loaded.
     */
    {
      char *p = ll_ptr_string(path);
      char *x = strrchr(p, '/');
      
      path_dir = x ? 
	ll_make_copy_string(p, x - p) : 
	ll_make_string(".", 1);
    }

    /*
     * Save it as the dynamic load path to try if
     * (load) is called recursively.
     */
    path_dir = ll_cons(path_dir, ll_nil);
    ll_bind_fluid(ll_s(load_path_dynamic), path_dir); 

    /* Save the current file loaded. */
    loading_path = ll_cons(path, ll_fluid(ll_s(loading)));
    ll_bind_fluid(ll_s(loading), loading_path);

    _ll_additional_error_properties(2,
				    ll_s(loading), loading_path,
				    ll_s(load_path), load_path
				    );
    
    /* Open file port */
    port = ll_call(ll_o(open_input_file), _1(path));
    
    /* Load from port */
    result = ll_call(ll_o(load), _1(port));
    
    /* Close port */
    ll_call(ll_o(close_input_port), _1(port));
  }
  ll_let_fluid_END();

  ll_return(result);
}
ll_define_primitive_end


/***************************************************************************/


ll_INIT(load_path, 500, "initializing ll:*load-path*")
{
  const char *path = getenv("LL_LOAD_PATH");
  ll_v list = ll_nil;
  ll_v *lp = &list;

  /* Initialize ll:*load-path* from LL_LOAD_PATH envvar. */
  if ( path ) {
    int path_sep = strchr(path, ';') ? ';' : ':';
    const char *b, *e;

    e = path;
    while ( *(b = e) ) {
      while ( *e && *e != path_sep ) {
	++ e;
      }
      
      *lp = ll_cons(ll_make_copy_string(b, e - b), ll_nil);
      lp = &ll_CDR(*lp);

      if ( *e ) {
	++ e;
      }
    }
  }
  
  ll_g(llCSload_pathS) = list;

  ll_bind_fluid(ll_s(load_path_dynamic), ll_nil);
  ll_bind_fluid(ll_s(loading), ll_nil);

#if 0
  ll_format(ll_undef, "~S=~S\n", 2, ll_s(llCSload_pathS), list);
#endif

  return 0;
}

