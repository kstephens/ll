#ifndef __rcs_id__
#ifndef __rcs_id_ll_eval_c__
#define __rcs_id_ll_eval_c__
static const char __rcs_id_ll_eval_c[] = "$Id: eval.c,v 1.21 2008/05/24 21:04:05 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"

/************************************************************************/
/* The evaluator */

ll_define_primitive(constant, eval, __1(object, env), _0())
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(object, Seval_hookS, _1(object), _0())
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(list, eval_list, __1(list, env), _0())
{
  int eval_each = 1; // getenv("ll_LOAD_EVAL_EACH") ? 1 : 0;
  ll_v exprs = ll_SELF;

  if ( eval_each ) {
    ll_v result = ll_undef;

    ll_LIST_LOOP(exprs, expr);
    {
      result = ll_call(ll_o(eval), _1(expr));
    }
    ll_LIST_LOOP_END;

    ll_return(result);
  } else {
    exprs = ll_call(ll_o(map), _2(ll_o(Seval_hookS), exprs));

    exprs = ll_call(ll_o(map), _2(ll_o(macro_expand), exprs));

    {
      ll_v ir;
      ll_v op;
      
      ir = ll_call(ll_o(make), _2(ll_type(_ir),
				  exprs               /* body */
				  ));
      
      op = ll_call(ll_o(_ir_operation), _1(ir));
      
      ll_call_tail(op, _0());
    }
  }
}
ll_define_primitive_end


ll_define_primitive(object, eval, __1(obj, env), _0())
{
  ll_v obj;
  ll_v env;
  ll_v expr;
  ll_v ir;
  ll_v op;

  obj = ll_SELF;
  env = ll_ARGV[1];

  expr = ll_call(ll_o(eval_stage_1), _2(obj, env));

  ir = ll_call(ll_o(eval_stage_2), _2(expr, env));
  
  if ( ! getenv("ll_CONST_FOLD") ) {
    /* Disable constant folding */
    ll_call(ll_o(set_propertyE), _3(ir, ll_s(no_const_folding), ll_t));
  }

  ir = ll_call(ll_o(eval_stage_3), _2(ir, env));
  
  op = ll_call(ll_o(_ir_operation), _1(ir));
  
  ll_call_tail(op, _0());
}
ll_define_primitive_end


ll_define_primitive(object, eval_no_const_fold, __1(obj, env), _0())
{
  ll_v obj;
  ll_v env;
  ll_v expr;
  ll_v ir;
  ll_v op;

  obj = ll_SELF;
  env = ll_ARGV[1];

  expr = ll_call(ll_o(eval_stage_1), _2(obj, env));

  ir = ll_call(ll_o(eval_stage_2), _2(expr, env));
  
  /* Disable constant folding */
  ll_call(ll_o(set_propertyE), _3(ir, ll_s(no_const_folding), ll_t));

  ir = ll_call(ll_o(eval_stage_3), _2(ir, env));

  op = ll_call(ll_o(_ir_operation), _1(ir));
  
  ll_call_tail(op, _0());
}
ll_define_primitive_end


/***********************************************************************/

ll_define_primitive(object, eval_stage_1, __1(obj, env), _0())
{
  ll_v expr;
  ll_v ir;
  ll_v op;

  expr = ll_SELF;

  if ( getenv("ll_EVAL_DEBUG") ) {
    ll_format(ll_undef, "  eval:\n  ~S\n", 1, expr);
  }

  expr = ll_call(ll_o(Seval_hookS), _1(expr));

  expr = ll_call(ll_o(macro_expand), _1(expr));
  
  if ( getenv("ll_EVAL_DEBUG") ) {
    ll_format(ll_undef, "  expanded:\n  ~S\n", 1, expr);
  }

  ll_return(expr);
}
ll_define_primitive_end


ll_define_primitive(object, eval_stage_2, __1(obj, env), _0())
{
  ll_v expr;
  ll_v ir;

  expr = ll_SELF;

  /*
    ll_format(ll_undef, "\neval: after macro-expand: ~S\n\n", 1, expr);
  */

  ir = ll_call(ll_o(make), _2(ll_type(_ir),
			       ll_cons(expr, ll_nil)  /* body */
			       ));

  ll_return(ir);
}
ll_define_primitive_end


ll_define_primitive(object, eval_stage_3, __1(obj, env), _0())
{
  ll_v ir;

  ir = ll_SELF;

  /* Do constant folding. */
  if ( ! ll_unbox_boolean(ll_call(ll_o(property), _2(ir, ll_s(no_const_folding)))) ) {
    /* 
       ll_format(ll_undef, "const folding ~S\n", 1, ll_THIS->_body);
    */

    ll_call(ll_o(_ir_const_fold), _2(ir, ir));
    
    /*
      ll_format(ll_undef, "const folded ~S\n", 1, ll_THIS->_body);
    */
  } else {
    /* ll_format(ll_undef, "NO const folding ~S\n", 1, ll_THIS->_body); */
  }

  ll_return(ir);
}
ll_define_primitive_end



ll_define_primitive(object, eval_stage_4, __1(obj, env), _0())
{
  ll_v ir;

  ir = ll_SELF;

  /**********************************************************************/
  /* Begin emiting code */

  ll_call(ll_o(_ir_compile2_body), _3(ir, ir, ll_t));
  
  ll_return(ir);
}
ll_define_primitive_end

