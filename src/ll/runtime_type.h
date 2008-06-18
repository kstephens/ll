/******************************************************************/
/* The run-time */

ll_type_def(_runtime, type)
ll_type_def_supers(_runtime)
  ll_type_def_super(_runtime, object)
  ll_type_def_super(_runtime, properties_mixin)
ll_type_def_supers_end(_runtime)
ll_type_def_slots(_runtime)
  /* Stacks */
  ll_type_def_slot(_runtime, ll_ar_p, ar_sp)
  ll_type_def_slot(_runtime, ll_ar_p, ar_sp_bottom)
  ll_type_def_slot(_runtime, ll_ar_p, ar_sp_base)
  ll_type_def_slot(_runtime, ll_stack_buffer_p, ar_stack_buffer)

  ll_type_def_slot(_runtime, ll_val_p, val_sp)
  ll_type_def_slot(_runtime, ll_val_p, val_sp_beg)
  ll_type_def_slot(_runtime, ll_val_p, val_sp_end)
  ll_type_def_slot(_runtime, ll_stack_buffer_p, val_stack_buffer)

  /* Generic properties */
  ll_type_def_slot(_runtime, ll_v, properties)

  /* Current environment vector. */
  ll_type_def_slot(_runtime, int,   env_bindings_n)
  ll_type_def_slot(_runtime, ll_v*, env_bindings)
  ll_type_def_slot(_runtime, ll_v,  env)

  /* Constant objects */
  ll_type_def_slot(_runtime, ll_tsa_boolean, _t)
  ll_type_def_slot(_runtime, ll_tsa_boolean, _f)
  ll_type_def_slot(_runtime, ll_tsa_null,    _nil)
  ll_type_def_slot(_runtime, ll_tsa_char_table, _char_table)

  /* Call-site lookup cache */
  ll_type_def_slot(_runtime, ll_lcache*, lcache_list)
  ll_type_def_slot(_runtime, size_t,     lcache_count)

  /* Core types */
#undef _ll_type_def
#define _ll_type_def(N,MT)ll_type_def_slot(_runtime, ll_tsa_##MT, t_##N)
#include "ll/types.h"

  /* Core symbols */
  ll_type_def_slot(_runtime, void*, symbol_table)
#if 0
#undef _ll_s_def
#define _ll_s_def(N)ll_type_def_slot(_runtime, ll_tsa_symbol, s_##N)
#include "ll/symbols.h"
#endif

ll_type_def_slots_end(_runtime)
  ll_type_def_option(_runtime, top_wiredQ, "#t")
ll_type_def_end(_runtime)

