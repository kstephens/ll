#ifndef __rcs_id__
#ifndef __rcs_id_ll_fluid_c__
#define __rcs_id_ll_fluid_c__
static const char __rcs_id_ll_fluid_c[] = "$Id: fluid.c,v 1.17 2007/12/23 20:09:45 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"


ll_define_primitive(symbol, Pfluid_bind, _2(var, value), _0())
{
  ll_v old_bindings = ll_g(_fluid_bindings);
  ll_v binding = ll_cons(ll_SELF, ll_cons(ll_ARG_1, ll_nil));
  ll_set_g(_fluid_bindings, ll_cons(binding, ll_g(_fluid_bindings)));
  ll_return(old_bindings);
}
ll_define_primitive_end


ll_define_primitive(symbol, Pfluid_binding, __1(var, top_levelQ), _1(no_side_effect, "#t"))
{
  ll_v x = ll_g(_fluid_bindings);

  while ( ! ll_nullQ(x) ) {
    ll_v binding = ll_car(x);
    if ( ll_EQ(ll_car(binding), ll_SELF) ) {
      ll_return(binding);
    }

    x = ll_cdr(x);
  }

  /* insert at (cdr %top-level-fluid_bindings) */
  if ( ll_ARGC > 1 ) {
    ll_v binding = ll_cons(ll_ARG_0, ll_cons(ll_ARG_1, ll_nil));
    ll_v bindings = ll_cons(binding, ll_CDR(ll_g(_top_level_fluid_bindings)));
    ll_set_cdrE(ll_g(_top_level_fluid_bindings), bindings);
  }

  ll_return(ll_f);
}
ll_define_primitive_end


/*************************************************************************/


ll_define_primitive(symbol, Pfluid, _1(var), _1(no_side_effect, "#t"))
{
  do {
    ll_v binding = ll_call(ll_o(Pfluid_binding), _1(ll_SELF));
    if ( ll_unbox_boolean(binding) ) {
      ll_return(ll_car(ll_cdr(binding)));
    } else {
      ll_SELF = _ll_undefined_variable_error(ll_listn(2, ll_s(fluid), ll_SELF));
    }
  } while ( 1 );
}
ll_define_primitive_end


ll_define_primitive(symbol, set_PfluidE, _2(var, value), _0())
{
  do {
    ll_v binding = ll_call(ll_o(Pfluid_binding), _1(ll_SELF));

    if ( ll_unbox_boolean(binding) ) {
      ll_call_tail(ll_o(set_carE), _2(ll_cdr(binding), ll_ARG_1));
    } else {
      ll_SELF = _ll_undefined_variable_error(ll_listn(2, ll_s(fluid), ll_SELF));
    }
  } while ( 1 );
}
ll_define_primitive_end


ll_define_primitive(symbol, define_Pfluid, _2(var, value), _0())
{
  ll_v binding = ll_call(ll_o(Pfluid_binding), _2(ll_SELF, ll_ARG_1));
  if ( ll_unbox_boolean(binding) ) {
    ll_call(ll_o(set_carE), _2(ll_cdr(binding), ll_ARG_1));
  }
  /* ll_return(ll_g(_fluid_bindings)); */
}
ll_define_primitive_end


/*********************************************************/


ll_define_macro(symbol, fluid, _1(var))
{
  /* (fluid <sym>) => (%fluid (quote <sym>)) */
  ll_return(ll_listn(2, ll_s(Pfluid), ll_listn(2, ll_s(quote), ll_ARG_0)));
}
ll_define_macro_end


ll_define_macro(symbol, define_fluid, _2(var, val))
{
  /* (define-fluid <sym> <val>) => (define-%fluid (quote <sym>) <val>) */
  ll_return(ll_listn(3, ll_s(define_Pfluid), ll_listn(2, ll_s(quote), ll_ARG_0), ll_ARG_1));
}
ll_define_macro_end


/*************************************************************************/


ll_v ll_bind_fluid(ll_v name, ll_v value)
{
  ll_assert_ref(name);
  return ll_call(ll_o(Pfluid_bind), _2(name, value));
}


void ll_unbind_fluid(ll_v prev_bindings)
{
  ll_set_g(_fluid_bindings, prev_bindings);
}


ll_v ll_fluid(ll_v name)
{
  ll_assert_ref(name);
  return ll_call(ll_o(Pfluid), _1(name));
}


void ll_set_fluid(ll_v name, ll_v value)
{
  ll_assert_ref(name);
  ll_call(ll_o(set_PfluidE), _2(name, value));
}


void ll_define_fluid(ll_v name, ll_v value)
{
  ll_assert_ref(name);
  ll_call(ll_o(define_Pfluid), _2(name, value));
}


/*************************************************************************/


ll_INIT(fluid1, 90, "bindings list")
{
  ll_set_g(_fluid_bindings, ll_nil);
  ll_set_g(_top_level_fluid_bindings, ll_nil);

  return 0;
}


ll_INIT(fluid2, 255, "top-level fluid bindings")
{
  ll_bind_fluid(ll_s(_top_level_fluid_bindings), ll_f);
  ll_set_g(_top_level_fluid_bindings, ll_g(_fluid_bindings));

  return 0;
}


/*************************************************************************/
