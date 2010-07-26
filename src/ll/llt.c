#ifndef __rcs_id__
#ifndef __rcs_id_ll_llt_c__
#define __rcs_id_ll_llt_c__
static const char __rcs_id_ll_llt_c[] = "$Id: llt.c,v 1.12 2008/05/26 07:57:42 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include <stdio.h> /* printf() */
#include <string.h> /* strcmp() */

int main(int argc, char **argv, char **envp)
{
  int interactive = 1;
  int print_result = 0;
  ll_v result;

  ll_init(&argc, &argv, &envp);

  result = ll_undef;

  {
    int argi;
    for ( argi = 1; argi < argc; ++ argi ) {
      const char *arg = argv[argi];
      if ( arg[0] == '-' ) {
	if ( ! strcmp("-h", arg) || ! strcmp("--help", arg) ) {
	  if ( interactive == 1 ) {
	    interactive = 0;
	  }
	  printf(
		 "%s [ -h | -d | -p | -i | -e expr ]\n"
		 "\n"
		 "  -h \n"
		 "    This help message. \n"
		 "  -d \n"
		 "    Invoke debugger on error. \n"
		 "  -p \n"
		 "    Print result of -e eval. \n"
		 "  -i \n"
		 "    Force interactive REPL. \n"
		 "  -e expr \n"
		 "    Evaluate expression (disables interactive unless -i). \n"
		 , argv[0]);
	} else
	if ( ! strcmp("-d", arg) ) {
	  ll_bind_fluid(ll_s(error_handler), ll_o(error_start_debugger));
	} else
	if ( ! strcmp("-p", arg) ) {
	  print_result = 1;
	} else
	if ( ! strcmp("-i", arg) ) {
	  interactive = 2;
	} else
	if ( ! strcmp("-e", arg) ) {
	  if ( interactive == 1 ) {
	    interactive = 0;
	  }

	  result = ll_eval_string(argv[++ argi], (size_t) -1);

	  if ( print_result ) {
	    if ( ll_NE(result, ll_unspec) ) {
	      ll_format(ll_undef, "~S\n", 1, result);
	    }
	  }	  
	} else {
	  fprintf(stderr, "ll: invalid argument: \"%s\"\n", arg);
	  exit(1);
	}
      }
    }
  }

  if ( interactive ) {
    ll_call(ll_o(llCtop_level), _0());
  }

  return 0;
}

