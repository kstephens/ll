#ifndef __rcs_id__
#ifndef __rcs_id_ll_toplevel_c__
#define __rcs_id_ll_toplevel_c__
static const char __rcs_id_ll_toplevel_c[] = "$Id: toplevel.c,v 1.17 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"


/****************************************************************************/
/* A simple history mechanism */


static
void _save_expr(ll_v expr)
{
  ll_set_fluid(ll_s(llCtop_levelCexprs), ll_cons(
					 ll_cons(ll_fluid(ll_s(llCtop_levelCprompt_id)), expr), ll_fluid(ll_s(llCtop_levelCexprs))));
}


ll_define_primitive(char, llCtop_levelCread_macroCexpr, _2(char, port), _0())
{
  ll_v index;

  index = ll_call(ll_o(read), _1(ll_ARG_1));
  ll_LIST_LOOP(ll_fluid(ll_s(llCtop_levelCexprs)), x);
  {
    if ( ll_eqvQ(ll_car(x), index) ) {
      ll_return(ll_cons(ll_cdr(x), ll_nil));
    }
  }
  ll_LIST_LOOP_END;

  ll_return(ll_nil);
}
ll_define_primitive_end


static
void _save_result(ll_v expr)
{
  ll_set_fluid(ll_s(llCtop_levelCresults), ll_cons(
					 ll_cons(ll_fluid(ll_s(llCtop_levelCprompt_id)), expr), ll_fluid(ll_s(llCtop_levelCresults))));
}


ll_define_primitive(char, llCtop_levelCread_macroCresult, _2(char, port), _0())
{
  ll_v index;

  index = ll_call(ll_o(read), _1(ll_ARG_1));
  ll_LIST_LOOP(ll_fluid(ll_s(llCtop_levelCresults)), x);
  {
    if ( ll_eqvQ(ll_car(x), index) ) {
      ll_return(ll_cons(ll_cdr(x), ll_nil));
    }
  }
  ll_LIST_LOOP_END;

  ll_return(ll_nil);
}
ll_define_primitive_end


/****************************************************************************/


ll_define_primitive(object, top_level, __0(result), _0())
{
  /* Continues within the innermost (ll:top-level) read-eval-print loop, or begins a new top-level. */
  ll_v toplevel = ll_fluid(ll_s(llCtop_levelCrestart));
  ll_v result = ll_ARGC > 0 ? ll_ARG_0 : ll_unspec;

  if ( ll_unbox_boolean(toplevel) ) {
    ll_call_tail(toplevel, _1(result));
  } else {
    ll_call_tail(ll_o(llCtop_level), _0());
  }
}
ll_define_primitive_end


ll_define_primitive(object, llCtop_levelCprompt_read_eval, __0(prompts), _0())
{
  /* Prompt the user on the ll:interactive-output-port, read an expression from ll:interactive-input-port, evaluate it and print the result on ll:interactive-output-port. */
  ll_v expr = ll_undef, result = ll_undef;

  ll_v prompt_port = ll_undef, input_port, output_port; 
  ll_v prompt_id;

  prompt_id = ll__ADD(ll_fluid(ll_s(llCtop_levelCprompt_id)), 
		      ll_make_fixnum(1));
  ll_set_fluid(ll_s(llCtop_levelCprompt_id), prompt_id);

  /* Set up ports. */
  prompt_port = ll_make_string(0, 0);

  input_port = ll_fluid(ll_s(llCtop_levelCinput_port));
  if ( ! ll_unbox_boolean(input_port) ) {
    input_port = ll_g(SllCinteractive_input_portS);
  }

  output_port = ll_fluid(ll_s(llCtop_levelCoutput_port));
  if ( ! ll_unbox_boolean(output_port) ) {
    output_port = ll_g(SllCinteractive_output_portS);
  }

  {
    ll_v prompt_string = ll_make_string(0, 0);

    /* Format the prompt */
    if ( ll_ARGC == 0 ) {
      ll_format(prompt_string, " ll ~S> ", 1, prompt_id);
    } else if ( ll_ARGC == 1 ) {
      ll_format(prompt_string, " ~S ~S> ", 2, ll_ARG_0, prompt_id);
    } else if ( ll_ARGC >= 2 ) {
      ll_format(prompt_string, " ~S~S ~S> ", 3, ll_ARG_0, ll_ARG_1, prompt_id);
    }

    if ( ll_unbox_boolean(ll_call(ll_o(readline_portQ), _1(input_port))) ) {
      prompt_port = input_port;
    } else {
      prompt_port = ll_f;
    }

    if ( ll_unbox_boolean(prompt_port) ) {
      ll_call(ll_o(_output_string), _2(prompt_port, prompt_string));
      ll_call(ll_o(flush), _1(prompt_port));
    }
  }

  expr = ll_call(ll_o(read), _1(input_port));

  if ( ll_EQ(expr, ll_eos) ) {
    result = ll_eos;
  } else {
    /* Evaluate the expression */
    _save_expr(expr);
    
    result = ll_call(ll_o(eval), _1(expr));
    
    /* Print the result */
    if ( ll_NE(result, ll_unspec) ) {
      if ( ll_unbox_boolean(prompt_port) ) {
	ll_call(ll_o(write), _2(result, output_port));
	ll_call(ll_o(newline), _1(output_port));
      }
    }
    
    /* Save the result */
    _save_result(result);
  }
  ll_return(result);
}
ll_define_primitive_end


ll_define_primitive(object, llCtop_level, _0(), _0())
{
  int errors = 0;
  ll_v result = ll_unspec;

  do {
    ll_v restart = ll_undef;

    (void) restart;
    ll_CATCH_BEGIN(ll_type(object), restart);
    {
      errors = 0;
      
      ll_let_fluid();
      {
	ll_v expr;

	ll_bind_fluid(ll_s(error_handler), ll_o(error_start_debugger));
	ll_bind_fluid(ll_s(llCtop_levelCrestart), restart);
	ll_bind_fluid(ll_s(llCtop_levelClevel), ll__ADD(ll_fluid(ll_s(llCtop_levelClevel)), ll_make_fixnum(1)));

	/* Prompt and read an expression until eof */
	while ( 
	       expr = ll_call(ll_o(llCtop_levelCprompt_read_eval), _1(ll_s(ll))),
	       ll_NE(expr, ll_eos)
	       )
	  result = expr;
      }
      ll_let_fluid_END();
    }
    ll_CATCH_VALUE(v);
    {
      (void) v;
      errors = 1;
      _save_result(ll_undef);
    }
    ll_CATCH_END;
  } while ( errors );

  ll_return(result);
}
ll_define_primitive_end


ll_v ll_eval_string(const char *s, size_t l)
{
  ll_v result = ll_unspec;

  ll_let_fluid();
  {
    /* Create a <string> object. */
    ll_v expr, port;

    expr = ll_make_copy_string((char *) s, l);

    /* Remember the string in case of error. */
    _ll_additional_error_properties(1,
				    ll_s(in_string), expr,
				    ll_undef);

    /* Create an <input-port> using the <string>. */
    port = ll_call(ll_o(make), _2(ll_type(input_port), expr));

    /* Load from input-port. */
    result = ll_call(ll_o(load), _1(port));    
  }
  ll_let_fluid_END();

  return result;
}


ll_INIT(toplevel, 290, "definitions, read macros")
{
  ll_bind_fluid(ll_s(llCtop_levelCinput_port), ll_f);
  ll_bind_fluid(ll_s(llCtop_levelCoutput_port), ll_f);
  ll_bind_fluid(ll_s(llCtop_levelClevel), ll_make_fixnum(0));
  ll_bind_fluid(ll_s(llCtop_levelCrestart), ll_f);
  ll_bind_fluid(ll_s(llCtop_levelCprompt_id), ll_make_fixnum(0));
  ll_bind_fluid(ll_s(llCtop_levelCexprs), ll_nil);
  ll_bind_fluid(ll_s(llCtop_levelCresults), ll_nil);

  /* Add our read macros */
  ll_call(ll_o(llCreadCdefine_macro_char), _2(ll_make_char('^'), ll_o(llCtop_levelCread_macroCexpr)));
  ll_call(ll_o(llCreadCdefine_macro_char), _2(ll_make_char('V'), ll_o(llCtop_levelCread_macroCresult)));

  return 0;
}

