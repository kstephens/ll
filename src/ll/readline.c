#ifndef __rcs_id__
#ifndef __rcs_id_ll_readline_c__
#define __rcs_id_ll_readline_c__
static const char __rcs_id_ll_readline_c[] = "$Id: readline.c,v 1.12 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "readline/readline.h"

#ifndef HAS_HISTORY
#define HAS_HISTORY 1
#endif

#if HAS_HISTORY
#include "readline/history.h" /* add_history(), ... */

#ifndef ll_HISTORY_FILE
#define ll_HISTORY_FILE "~/.llhist"
#endif
#endif

#include <string.h>
#include <stdlib.h>


/*************************************************************************/
/* Completion support */


static
char *
_ll_completion_generator (text, state)
     char *text;
     int state;
{
  static int len;
  static ll_v symbols; /* not thread safe. */

  /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0. */
  if ( ! state ) {
    len = strlen(text);
    symbols = ll_g(_symbols);
  }

  /* Return the next name which partially matches from the command list. */
  while ( ! ll_nullQ(symbols) ) {
    ll_v sym = ll_car(symbols);
    const char *name = (void*) _ll_name_symbol(sym);
    symbols = ll_cdr(symbols);
    if (strncmp (name, text, len) == 0)
      return strcpy(malloc(strlen(name) + 1), name);
  }

  /* If no names matched, then return NULL. */
  return ((char *)NULL);
}


static
char **
_ll_completion (text, start, end)
     char *text;
     int start, end;
{
  char **matches;

  matches = rl_completion_matches (text, _ll_completion_generator);

  return (matches);
}


/*************************************************************************/


ll_define_primitive(readline_port, initialize, _1(port), _0())
{
  ll_THIS->_buffer = ll_make_string(0, 0);
  ll_write_barrier_SELF();
  ll_THIS->_prompt_buffer = ll_make_string(" ll > ", -1);
  ll_write_barrier_SELF();
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(readline_port, flush, _1(port), _0())
{
  /* NOTHING */
}
ll_define_primitive_end


ll_define_primitive(readline_port, _close_port, _1(port), _0())
{
  /* NOTHING */
}
ll_define_primitive_end


/******************************************************************/

ll_define_primitive(readline_port, _fill_buffer, _1(port), _0())
{
  if ( ll_NE(ll_THIS->_buffer, ll_f) ) {
    size_t length = ll_len_string(ll_THIS->_buffer);
    char *s;
    
    if ( length == 0 ) {
    again:
      /* Allow conditional parsing of the ~/.inputrc file. */
      rl_readline_name = "ll";

      /* Tell the completer that we want a crack first. */
      rl_attempted_completion_function = (CPPFunction *)_ll_completion;

      /* Call readline */
      s = readline(ll_cstr_string(ll_THIS->_prompt_buffer));
      if ( s ) {
	if ( *s ) {
#if HAS_HISTORY
	  if ( s[0] != '\0' ) {
	    add_history(s);
	    write_history(ll_HISTORY_FILE);
	  }
#endif

	  /* append the result to the read buffer */
	  ll_write_string(ll_THIS->_buffer, s, strlen(s));

	  /* readline does not terminate with "\n" */
	  ll_write_string(ll_THIS->_buffer, "\n", 1);
	} else {
	  goto again;
	}
      } else {
	ll_THIS->_buffer = ll_f;
	ll_write_barrier_SELF();
      }
    }
  }
}
ll_define_primitive_end


/******************************************************************/
/* readline as <input-port> */


ll_define_primitive(readline_port, read_char, _1(port), _0())
{
  ll_call(ll_o(_fill_buffer), _1(ll_SELF));
  if ( ll_unbox_boolean(ll_THIS->_buffer) ) {
    ll_call_tail(ll_o(read_char), _1(ll_THIS->_buffer));
  } else {
    ll_return(ll_eos);
  }
}
ll_define_primitive_end


ll_define_primitive(readline_port, peek_char, _1(port), _0())
{
  ll_call(ll_o(_fill_buffer), _1(ll_SELF));
  if ( ll_unbox_boolean(ll_THIS->_buffer) ) {
    ll_call_tail(ll_o(peek_char), _1(ll_THIS->_buffer));
  } else {
    ll_return(ll_eos);
  }
}
ll_define_primitive_end


/******************************************************************/
/* readline as <output-port> */

/* IMPLEMENT: this doesn't quite work yet */


ll_define_primitive(readline_port, _output_string, _2(port, string), _0())
{
  size_t count = ll_len_string(ll_ARG_1);
  ll_THIS->_prompt_buffer = ll_ARG_1;
  ll_write_barrier_SELF();
  ll_return(ll_make_fixnum(count));
}
ll_define_primitive_end


ll_define_primitive(readline_port, _output_char, _2(port, char), _0())
{
  char buf[1];
  buf[0] = ll_unbox_char(ll_ARG_1);
  ll_write_string(ll_SELF, buf, 1);
}
ll_define_primitive_end


/******************************************************************/


ll_INIT(readline,230,"GNU readline <port>")
{
#if HAS_HISTORY
  /* Initialize the history library */
  using_history();
  read_history(ll_HISTORY_FILE);
#endif

  ll_set_g(SllCreadlineS, ll_call(ll_o(make), _1(ll_type(readline_port))));
  ll_set_g(SllCinteractive_input_portS, ll_g(SllCreadlineS));
  ll_set_g(SllCinteractive_output_portS, ll_g(_current_error_port));

  return 0;
}

/******************************************************************/
