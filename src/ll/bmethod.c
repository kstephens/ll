#include "ll.h"
#include "ll/config.h"
#include "call_int.h"
#include "bcode.h"
#include <string.h>
#include "assert.h"


#ifndef ll_BC_RUN_DEBUG
int ll_BC_RUN_DEBUG = 0;
#endif


#ifndef ll_BC_DUMP
int ll_BC_DUMP = 0;
#endif


/* Enables debugging. */
int _ll_bc_debug = 0;

/* Enables debugging at method start. */
int _ll_bc_debug_at_start = 0;



/*******************************************************************/
/* The byte-code method interpreter */

/*
  ll_bc(NAME, NARGS, STACK_MOTION, NSTACK_ARGS)

  NAME - the name of the op.
  NARGS - the number of arguments after the op code coded as 4-byte
  signed integers.
  STACK_MOTION - the difference between the number of values pushed and
  popped from the value stack.
  NSTACK_ARGS - the number of arguments on the value stack.

*/

const char *_ll_bc_name(int bc)
{
  static const char *__ll_bc_name[] = {
  "",
#ifndef ll_bc_def
#define ll_bc_def(X,NARGS,SM,NSA) #X,
#include "ll/bcs.h"
#endif
  "LAST",
  0
};

  if ( bc < 0 || bc >= (sizeof(__ll_bc_name)/sizeof(__ll_bc_name[0])) - 2 ) {
    static char buf[32];
    sprintf(buf, "((ll_bc)%d)", bc);
    return buf;
  }

  return __ll_bc_name[bc];
}


int _ll_bc_nargs(int bc)
{
  static int __ll_bc_nargs[] = {
    0,
#ifndef ll_bc_def
#define ll_bc_def(X,NARGS,SM,NSA) NARGS,
#include "ll/bcs.h"
#endif
    0,
  };

  if ( bc < 0 || bc >= (sizeof(__ll_bc_nargs)/sizeof(__ll_bc_nargs[0])) - 2 ) {
    return -1;
  }

  return __ll_bc_nargs[bc];
}


#define GET_1() *(pc ++)
#define GET_4() (pc+=4,(long)pc[-4]|((long)pc[-3]<<8)|((long)pc[-2]<<16)|((long)pc[-1]<<24))

#define maxnargs 4

int _ll_bc_get_args(const ll_bc_t **_bc, long *args)
{
  const ll_bc_t *pc = *_bc;
  int nargs;

  nargs = _ll_bc_nargs(GET_1());
  {
    int i;
    
    ll_assert(bc, 0 <= nargs && nargs < maxnargs);
    for ( i = 0; i < nargs; i ++ ) {
      args[i] = GET_4();
    }
  }
  
  *_bc = pc;

  return nargs;
}


int _ll_bc_stack_motion(const ll_bc_t **_bc)
{
  int bcode = **_bc;
  long args[maxnargs];
  int nargs = (nargs = _ll_bc_get_args(_bc, args));
  int sm = 0;

#define ARG(X) args[X]

  switch ( bcode ) {
  case 0:
    break;
#ifndef ll_bc_def
#define ll_bc_def(N,NARGS,SM,NSA) case ll_bc_##N: sm = SM; break;
#include "ll/bcs.h"
#endif
  default:
    ll_assert_msg(bc,0,("bad bc %d", bcode));
  }
  
#undef ARG
  
  return sm;
}


int _ll_bc_max_stack_depth(const ll_bc_t *pc)
{
  int depth = 0, max_depth = 0;

  while ( *pc ) {
    int sm = _ll_bc_stack_motion(&pc);
    depth += sm;
    ll_assert(bc, "depth >= 0");
    if ( max_depth < depth )
      max_depth = depth;
  }

  /* fprintf(stderr, "_ll_bc_max_stack_depth() = %ld\n", (long) max_depth); */

  return max_depth;
}


int _ll_bc_nsa(const ll_bc_t **_bc)
{
  int bcode = **_bc;
  long args[maxnargs];
  int nargs = (nargs = _ll_bc_get_args(_bc, args));
  int sm = 0;
  
#define ARG(X) args[X]
  
  switch ( bcode ) {
  case 0:
    break;
#ifndef ll_bc_def
#define ll_bc_def(N,NARGS,SM,NSA) case ll_bc_##N: sm = NSA; break;
#include "ll/bcs.h"
#endif
  default:
    ll_assert_msg(bc,0,("bad bc %d", bcode));
  }
  
#undef ARG
  
  return sm;
}


/*******************************************************************/


static
int prompt(const char *pr)
{
  int result = 0;
  char cmd[16];

 again:
  fflush(stderr);
  fprintf(stderr, "\n    %s [q]uit, [d]ebug, [CR] continue%s: ", pr, _ll_bc_debug_at_start ? " + " : "");
  fflush(stderr);

  cmd[0] = '\0';
  if ( ! fgets(cmd, sizeof(cmd), stdin) )
    return -1;

  switch ( cmd[0] ) {
  case 'q': case 'Q': case '\0':
    result = 1;
    break;

  case 'd': case 'D':
    ll_call(ll_o(debugger), _0());
    goto again;
    break;

  case '+':
    ++ _ll_bc_debug_at_start;
    goto again;
    break;

  case '-':
    if ( -- _ll_bc_debug_at_start < 0 )
      _ll_bc_debug = 0;
    goto again;
    break;

  default:
    break;
  }

  return result;
}


/* #define CODE  ((ll_bc_t*) _ll_ptr_string(METH->_code)) */
#define CODE ((ll_bc_t*) ll_THIS_ISA(string, METH->_code)->_array)
#define _CONSTS METH->_consts
#define CONSTS ll_THIS_ISA(vector, _CONSTS)->_array
#define ll_V(X) (ll_THIS_ISA(vector,(X))->_array)

int _ll_bc_dump(const ll_bc_t **_pc, int *stack_depth, ll_v meth, int show_stack_args)
{
  const ll_bc_t *pc, *pc_save;
  int bcode;
  char buf[128], *bp = buf;
  long args[maxnargs];
  int i, nargs;
  ll_v port = ll_undef;
  ll_v op_arg = ll_undef;

#define METH ll_THIS_ISA(method,meth)

  pc = *_pc;
  if ( pc == 0 ) {
    pc = CODE;
  }
  pc_save = pc;
  bcode = *pc;

  sprintf(bp, "  BC: %p: %04ld : %p : %03lu : %12s", 
	  (void*) ll_UNBOX_ref(METH->_code), 
	  (long) (pc - CODE), 
	  (void*) _ll_val_sp, 
	  (unsigned long) (stack_depth ? *stack_depth : 0),
	  (const char*) _ll_bc_name(bcode));
  bp = strchr(bp, '\0');
  
  /* Print the code args. */
  nargs = _ll_bc_get_args(&pc, args);
  
  for ( i = 0; i < nargs; i ++ ) {
    sprintf(bp, " %-3ld", (long) (args[i]));
    bp = strchr(bp, '\0');
  }
  if ( nargs == 0 ) {
    sprintf(bp, " %-3s", "");
    bp = strchr(bp, '\0');
  }
    
  ll_write_string(port, buf, (size_t) -1);
    
  /* Get the symbolic opcode arguments */
  
  if ( bcode == ll_bc_const  ) {
    op_arg = CONSTS[args[0]];
  } else if ( bcode == ll_bc_slot ) {
    op_arg = CONSTS[args[0]];
  } else if ( bcode == ll_bc_glo ) {
    op_arg = ll_V(CONSTS[0])[args[0]];
  } else if ( bcode == ll_bc_glo_ ) {
    op_arg = ll_V(CONSTS[1])[args[0]];
  } else if ( bcode == ll_bc_jf || bcode == ll_bc_jmp ) {
    long offset = (pc - CODE) + args[0];
    op_arg = ll_BOX_fixnum(offset);
  }

  if ( stack_depth ) {
    const ll_bc_t *tmp = pc_save;
    *stack_depth += _ll_bc_stack_motion(&tmp);
  }

  if ( show_stack_args ) {
    const ll_bc_t *tmp = pc_save;
    /* Print stack args. */
    nargs = _ll_bc_nsa(&tmp);

    ll_write_string(port, " |", 2);

    ll_format(port, " ~n ", 1, ll_BOX_fixnum(nargs));

    for ( i = 0; i < nargs; i ++ ) {
      ll_format(port, " ~n", 1, _ll_val_sp[i]);
    }
  }

  /* Print the opcode arguments */
  if ( ll_NE(op_arg, ll_undef) ) {
    ll_format(port, " [~n]", 1, op_arg);
  }

  ll_write_string(port, "\n", 1);

  *_pc = pc;

#undef METH

  return bcode;
}


/********************************************************************/


ll_define_primitive(byte_code_method, _dump, _1(bmeth), _0())
{
#define METH ll_THIS_ISA(method, ll_SELF)
  int lines = 0;
  int stack_depth = 0;

  ll_bc_t *pc = 0;

  ll_format(ll_undef, "\n  BC: method ~W: consts ~S\n", 2, ll_SELF, _CONSTS);

  do {
    int bcode;

    lines ++;
    if ( lines > 100 ) {
      if ( prompt("BC: dump: ") )
	break;
      lines = 0;
    }

    bcode = _ll_bc_dump((const ll_bc_t **) &pc, &stack_depth, ll_SELF, 0);

    if ( bcode == 0 )
      break;
  } while ( 1 );

  prompt("BC: dump DONE: ");

#undef METH
}
ll_define_primitive_end


/*******************************************************************/


/* Find the offset of slot s in type t */
static int _ll_slot_offset(ll_v t, ll_v s)
{
  if ( 0 ) {
    ll_format(ll_undef, "_ll_slot_offset(~S, ~S) in ~S\n", 3, t, s, ll_THIS_ISA(type,t)->_slots); 
  }

  ll_LIST_LOOP(ll_THIS_ISA(type,t)->_slots, x);
  {
    ll_v slot_name = ll_car(x);

    if ( ll_EQ(slot_name, s) ) {
      ll_v st;

      if ( ll_BC_RUN_DEBUG ) {
	ll_format(ll_undef, "_ll_slot_offset(~S, ~S) => ~S\n", 3, t, s, x); 
      }

      /* Does the slot have a specific type? */
      if ( ! ll_nullQ(st = ll_cdr(ll_cdr(x))) ) {
	_ll_error(ll_ee(invalid_slot), 4, 
		  ll_s(reason), ll_s(bad_type), 
		  ll_s(type), t, 
		  ll_s(slot), s, 
		  ll_s(slot_type), st);

	return -1;
      }

      return ll_unbox_fixnum(ll_car(ll_cdr(x)));
    }
  }
  ll_LIST_LOOP_END;

  _ll_error(ll_ee(invalid_slot), 3, 
	    ll_s(reason), ll_s(not_found), 
	    ll_s(type), t, 
	    ll_s(slot), s);

  return -1;
}


/* Find the global binding for int x */
static ll_v _ll_global_binding(int index, ll_v *consts)
{
  ll_v binding;

  /* ll_format(ll_undef, "bc: global bindings = ~S\n", 1, consts[1]); */
  if ( ! ll_unbox_boolean(binding = ll_V(consts[1])[index]) ) {
    /* ll_format(ll_undef, "bc: global names = ~S\n", 1, consts[0]); */
    
    ll_V(consts[1])[index] = 
      binding = 
      ll_call(ll_o(_binding_or_error), 
	      _3(ll_call(ll_o(SenvironmentS), _0()), 
		 ll_V(consts[0])[index], 
		 ll_t));
  }

  return(binding);
}


/*******************************************************************/


#ifndef ll_BC_METER
#define ll_BC_METER 0
#endif

#if ll_BC_METER

unsigned long _ll_bc_meter[_ll_bc_LAST + 1][_ll_bc_LAST];

static void _ll_bc_meter_dump(FILE *file, int small)
{
  int i, j;
  
  char *fs = "\t";

  fprintf(file, "ll: bc: meter 1.0\n\n");

  fprintf(file, "[legend]\n");
  fprintf(file, "\n\
  [0][0] => total bytecodes executed.\n\
  [0][x] => total x bytecodes executed.\n\
  [x + 1][y] => total x before y executed.\n\
  [1][y] => total y at beginning.\n\
  [x + 1][0] => total x at beginning.\n\
\n");

  fprintf(file, "[bytecodes]\n");

  for ( i = 0; i < sizeof(_ll_bc_meter)/sizeof(_ll_bc_meter[0]); i ++ ) {
    fprintf(file, "%3d%s%s\n", i, fs, _ll_bc_name(i));
  }
  fprintf(file, "\n");
  
  fprintf(file, "%6s%s", "", fs);
  for ( i = 0; i < sizeof(_ll_bc_meter[0])/sizeof(_ll_bc_meter[0][0]); i ++ ) {
    fprintf(file, "%s%s", _ll_bc_name(i), fs);
  }
  fprintf(file, "\n");

  for ( i = 0; i < sizeof(_ll_bc_meter[0])/sizeof(_ll_bc_meter[0][0]); i ++ ) {
    fprintf(file, "%-6d%s", i, fs);
  }
  fprintf(file, "\n");

  for ( i = 0; i < sizeof(_ll_bc_meter)/sizeof(_ll_bc_meter[0]); i ++ ) {
    fprintf(file, "%6d%s", i, fs);
    for ( j = 0; j < sizeof(_ll_bc_meter[0])/sizeof(_ll_bc_meter[0][0]); j ++ ) {
      char str[16];
      unsigned long x = _ll_bc_meter[i][j];
 
      if ( small ) {
	const char *s;

	if ( x > 1000000 ) {
	  x /= 100000;
	  s = "M";
	} else if ( x > 1000 ) {
	  x /= 100;
	  s = "K";
	} else if ( x > 100 ) {
	  x /= 10;
	  s = "C";
	} else if ( x > 10 ) {
	  x /= 1;
	  s = "D";
	} else {
	  x *= 10;
	  s = " ";
	}

	sprintf(str, "%d.%d%s", (int) x / 10, (int)x % 10, s);
      } else {
	sprintf(str, "%lu", (unsigned long) x);
      }
      fprintf(file, "%6s%s", str, fs);
    }
    fprintf(file, "\n");
  }
  fprintf(file, "\n");

}
#endif


ll_define_primitive(object, llCdump_meter, _0(), _1(no_side_effect,"#t"))
{
#if ll_BC_METER
  FILE *fp;
  const char *fn = "/tmp/llbcmeter";
  if ( (fp = fopen(fn, "w")) ) {
    _ll_bc_meter_dump(fp, 0);
    fclose(fp);
  } else {
    fprintf(stderr, "BC: METER cannot write to '%s'\n", fn);
  }
#else
  fprintf(stderr, "BC: METER: not enabled: try \"ll_BC_METER=1\" in config.mak\n");
#endif
}
ll_define_primitive_end


/*******************************************************************/

static void _ll_make_bmethod(ll_v consts);
static void _ll_make_bmethod_env(ll_v consts);

static int bcCdebugClines;

ll_v _ll_debug_expr_, *_ll_debug_expr = &_ll_debug_expr_;

ll_define_primitive(object, _bmethod_apply, __0(args), _0())
{
  ll_bc_t *pc;
  ll_v exports = ll_undef;
  ll_v *locals = 0;
  char *SLOTS = ll_UNBOX_ref(ll_SELF);
  short bcCdebug = ll_BC_RUN_DEBUG;
  ll_v debug_expr = ll_f;
  ll_v debug_expr_last = ll_undef;
#if ll_USE_LCACHE
  ll_lcache *lcache = 0;
#endif
#if ll_BC_METER
  int bcode_last, bcode;
#endif
#ifndef ll_BC_DEBUG_EXPR
#define ll_BC_DEBUG_EXPR 1
#endif
#if ll_BC_DEBUG_EXPR
  ll_v *prev_debug_expr = _ll_debug_expr;
  _ll_debug_expr = &debug_expr;
#endif

  /* bcCdebug = 1; */

  SLOTS += ll_UNBOX_fixnum(ll_AR_TYPE_OFFSET);

#define METH ll_THIS_ISA(method, ll_AR_METH)
#define BMETH ll_THIS_ISA(byte_code_method, ll_AR_METH)
#define LCACHE BMETH->_lcache_vector
#define ENV ll_THIS_ISA(vector, METH->_env)->_array
#define EXPORTS ll_THIS_ISA(vector, exports)->_array

  if ( _ll_bc_debug_at_start ) {
    bcCdebug = _ll_bc_debug_at_start;
  }

  // restart:
  pc = CODE;
#if ll_BC_METER
  bcode = 0;
#endif

#define SET_4(X) (*(pc ++) = (X), *(pc ++) = (X) >> 8, *(pc ++) = (X) >> 16, *(pc ++) = (X) >> 24)

 next_isn:
#define break goto next_isn

    /*****************************************************************/
    /* debug support */

  _ll_ASSERT_STACKS();

#if 0
  if ( *pc == ll_bc_slot ) {
    _ll_bc_debug = 1;
  }
#endif

  if ( _ll_bc_debug ) {
    bcCdebug = _ll_bc_debug;
    _ll_bc_debug = 0;
  }

  if ( bcCdebug ) {
    if ( pc == CODE ) {
      ll_format(ll_f, "\n  BC: method ~W type-offset ~W\n", 2, ll_AR_METH, ll_AR_TYPE_OFFSET);
    }
    if ( bcCdebugClines == 20 || *pc == 0 ) {
      if ( prompt("BC: RUN: ") ) {
	bcCdebug = _ll_bc_debug_at_start;
      }
      bcCdebugClines = 0;
    }
    bcCdebugClines ++;

    if ( bcCdebug > 1 ) {
      ll_format(ll_f, "\n  BC: method ~W type-offset ~W\n", 2, ll_AR_METH, ll_AR_TYPE_OFFSET);
    }

    if ( ll_NE(debug_expr_last, ll_undef) ) {
      ll_format(ll_undef, "    | ~S\n", 1, debug_expr_last);
      debug_expr_last = ll_undef;
    }
    
    {
      const ll_bc_t *pc_tmp = pc;
      _ll_bc_dump(&pc_tmp, 0, ll_AR_METH, 1);
    }
  }

#if ll_BC_METER
  bcode_last = bcode;
  bcode = GET_1();
  _ll_bc_meter[bcode_last + 1][bcode] ++;
  _ll_bc_meter[0][bcode] ++;
  _ll_bc_meter[0][0] ++;
#if 0
  if ( _ll_bc_meter[0][0] % 4000 == 0 ) {
    /* _ll_bc_meter_dump(stderr, 1); */
    prompt("BC: METER");
  }
#endif
  switch ( bcode ) {
#else
  switch ( GET_1() ) {
#endif

    /*****************************************************************/
    /* NO-OP */

  case ll_bc(nop, 0, 0, 0):
    break;

    /*****************************************************************/
    /* Debugging support */

  case ll_bc(_bcCdebug, 0, 1, 0):
    if ( ! _ll_bc_debug_at_start ) {
      bcCdebug ++;
    }
    _ll_PUSH(ll_unspec);
    break;

  case ll_bc(_bcCdebug_off, 0, 1, 0):
    if ( -- bcCdebug < 0 )
      bcCdebug = 0;
    _ll_PUSH(ll_unspec);
    break;

  case ll_bc(_bcCdebug_expr, 0, 0, 1):
#if ll_BC_DEBUG_EXPR
    debug_expr =
#endif
      _ll_PEEK();
    _ll_PEEK() = ll_unspec;
#if ll_BC_DEBUG_EXPR
    debug_expr_last = debug_expr;
#endif
    break;

    /*****************************************************************/
    /* non-tail-recursive return, rtn value is on the stack */

  case ll_bc(rtn, 0, -1, 1):
    {
      ll_v x = _ll_POP();
#if ll_BC_DEBUG_EXPR
      _ll_debug_expr = prev_debug_expr;
#endif
      ll_return(x);
    }
    break;

    /*****************************************************************/
    /* stack mgmt */

  case ll_bc(probe, 1, 0, 0):
    {
      long x = GET_4();
      _ll_VS_PROBE(x);
    }
    break;

  case ll_bc(locals,1,ARG(0),0):
    {
      int n = GET_4();
      _ll_VS_PROBE(n);
      (void) _ll_PUSHN(n);
      locals = (ll_v*) _ll_val_sp;
    }
    break;

  case ll_bc(pop, 0, -1, 1): /* v | */
    (void) _ll_POP();
    break;


    /*****************************************************************/
    /* arg count checking */

  case ll_bc(nargs, 1, 0, 0): /* */
    {
      int nargs = GET_4();
      if ( ll_ARGC != nargs ) {
	_ll_argc_error();
      }
    }
    break;

    /*****************************************************************/
    /* restarg count checking/construction  */

  case ll_bc(nargs_rest, 1, 1, ARG(0)): /* ... arg[nargs] ... arg[argc] | (args[nargs] ...) */
    {
      int nargs = GET_4();
      if ( ll_ARGC < nargs ) {
	_ll_argc_error();
      }
      _ll_PUSH(ll_listv(ll_ARGC - nargs, ll_ARGV + nargs));
    }
    break;

  case ll_bc(nargs_rest_, 1, 0, ARG(0)): /* | */
    {
      int nargs = GET_4();
      if ( ll_ARGC < nargs ) {
	_ll_argc_error();
      }
    }
    break;

  case ll_bc(list, 1 , 1 - ARG(0), ARG(0)): /* l1 l2 ... ln | <list> */
    {
      int len = GET_4();
      ll_v list;
      list = ll_listv(len, (ll_v*) _ll_val_sp);
      _ll_POPN(len);
      _ll_PUSH(list);
    }
    break;

    /*****************************************************************/
    /* jump relative if false */

  case ll_bc(jf, 1, -1, 1):
    {
      long dist = GET_4();
      if ( ! ll_unbox_boolean(_ll_POP()) ) {
	pc += dist;
      }
    }
    break;

    /*****************************************************************/
    /* jump relative */

  case ll_bc(jmp, 1, 0, 0):
    {
      long dist = GET_4();
      pc += dist;
    }
    break;
 
    /*****************************************************************/
    /* constant */

  case ll_bc(const, 1, 1, 0): /* | const[i] */
    _ll_PUSH(CONSTS[GET_4()]);
    break;

    /*****************************************************************/
    /* locative */

  case ll_bc(contents, 0, 0, 1):     /* locative | *contents */
    _ll_PEEK() = *ll_UNBOX_locative(_ll_PEEK());
    break;

  case ll_bc(set_contents, 0, -1, 2): /* locative value | #<unspec> */
    {
      ll_v *loc = ll_UNBOX_locative(_ll_POP());
      *loc = _ll_PEEK();
      ll_write_barrier(loc);
      _ll_PEEK() = ll_unspec;
    }
    break;

    /*****************************************************************/
    /* exported env */

  case ll_bc(make_exports, 1, -ARG(0), ARG(0)): /* loc1 .. locn | */
    {
      size_t size = GET_4();
      ll_assert(bc, ll_EQ(exports,ll_undef));
      exports = ll_make_copy_vector((ll_v*) _ll_val_sp, size);
      (void) _ll_POPN(size);
    }
    break;

  case ll_bc(exports, 0, 1, 0): /* | <exports-vector> */
    ll_assert(bc, ll_NE(exports, ll_undef));
    _ll_PUSH(exports);
    break;

  case ll_bc(export, 1, 1, 0): /* | exports[n] */
    _ll_PUSH(ll_make_locative(&EXPORTS[GET_4()]));
    break;

    /*****************************************************************/
    /* Create a method closure */

  case ll_bc(make_method, 0, -1, 2): /* <code> <props> | <method> */
    _ll_make_bmethod(_CONSTS);
    break;

  case ll_bc(make_method_env, 0, -2, 3): /* <code> <props> <env> | <method> */
    _ll_make_bmethod_env(_CONSTS);
    break;

  case ll_bc(lambda, 0, 1, 1): /* <method> | <operation> */
    {
      ll_v meth = _ll_POP();
      ll_v op = ll_call(ll_o(make), _1(ll_type(operation)));
      ll_call(ll_o(_add_method), _3(ll_type(object), op, meth));
      _ll_PUSH(op);
    }
    break;

    /*****************************************************************/
    /* environment */

  case ll_bc(env, 1, 1, 0): /* | &env[i] */
    _ll_PUSH(ll_make_locative(&ENV[GET_4()]));
    break;

    /*****************************************************************/
    /* local */

  case ll_bc(local, 1, 1, 0): /* | &locals[i] */
    _ll_PUSH(ll_make_locative(&locals[GET_4()]));
    break;

    /*****************************************************************/
    /* arg */

  case ll_bc(arg, 1, 1, 0): /* | &ll_ARGV[i] */
    _ll_PUSH(ll_make_locative(&ll_ARGV[GET_4()]));
    break;

    /*****************************************************************/
    /* slot */

  case ll_bc(slot, 1, 1, 0):
    {
      int index = _ll_slot_offset(ll_AR_TYPE, CONSTS[GET_4()]);
      /* LOCK(); */
      pc -= 4;
      *(pc - 1) = ll_bc_slot_;
      SET_4(index);
      /* UNLOCK(); */
      _ll_PUSH(ll_make_locative(&SLOTS[index]));
    }
    break;

  case ll_bc(slot_, 1, 1, 0):
    _ll_PUSH(ll_make_locative(SLOTS + GET_4()));
    break;

    /*****************************************************************/
    /* global */
    
#define GLOBAL_BINDING(ISN)			  \
    ({						  \
      ll_v loc = _ll_global_binding(GET_4(), CONSTS);	\
      /* LOCK(); */					\
      /* glo(g) -> glo_(%make-locative g) */		\
      *(pc - 5) = ll_bc_##ISN##_;			\
      loc;						\
    })

#define GLOBAL_BINDING_()			\
    ({						\
      int index = GET_4();			\
      ll_V(CONSTS[1])[index];			\
    })
    
    /* Returns the global variable's binding. */
  case ll_bc(glo_binding, 1, 1, 0):
    {
      ll_v loc = GLOBAL_BINDING(glo_binding);
      _ll_PUSH(loc);
    }
    break;

    /* Returns the global variable's binding. */
  case ll_bc(glo_binding_, 1, 1, 0):
    {
      _ll_PUSH(GLOBAL_BINDING_());
    }
    break;
    
    /* Returns global value. */
  case ll_bc(glo, 1, 1, 0): /* | value */
    {
      ll_v binding = GLOBAL_BINDING(glo);
      _ll_PUSH(__ll_binding_value(binding));
    }
    break;
    
    /* Returns global value. */
  case ll_bc(glo_, 1, 1, 0): /* | value */
    {
      ll_v binding = GLOBAL_BINDING_();
      _ll_PUSH(__ll_binding_value(binding));
    }
    break;
    
  case ll_bc(glo_set, 1, 0, 0): /* value | #u */
    {
      ll_v binding = GLOBAL_BINDING(glo_set);
      _ll_set_binding_valueE(binding, _ll_PEEK());
      _ll_PEEK() = ll_unspec;
    }
    break;

  case ll_bc(glo_set_, 1, 0, 0): /* value | #u */
    {
      ll_v binding = GLOBAL_BINDING_();
      _ll_set_binding_valueE(binding, _ll_PEEK());
      _ll_PEEK() = ll_unspec;
    }
    break;

    /* Returns global value's locative. */
  case ll_bc(glo_loc, 1, 1, 0): /* | locative */
    {
      ll_v x = GLOBAL_BINDING(glo_loc);
      x = _ll_locative_binding_value(x);
      _ll_PUSH(x);
    }
    break;
    
    /* Returns global value's locative. */
  case ll_bc(glo_loc_, 1, 1, 0): /* | locative */
    {
      ll_v x = GLOBAL_BINDING_();
      x = _ll_locative_binding_value(x);
      _ll_PUSH(x);
    }
    break;

    /*****************************************************************/
    /* call */

  case ll_bc(csite_i, 1, 0, 0):
    {
      int i = GET_4();
#if ll_USE_LCACHE
      lcache = LCACHE ? &(LCACHE->_v[i]) : 0;
      if ( lcache ) {
	ll_assert_msg(bc, LCACHE->_s > i, ("LCACHE->_s > i: LCACHE=%p, _s=%d, i=%d", (void*) LCACHE, (int) LCACHE->_s, (int) i));
      }
      /*
	fprintf(stderr, "  lcache %p\n", (void*) lcache);
      */
#else
      fprintf(stderr, "warning: csite_i %d not supported\n", (int) i);
#endif
    }
    break;

  case ll_bc(call, 1, - ARG(0), ARG(0) + 1): /* op args ... | rtn */
    {
      ll_v op = _ll_POP();
      int n = GET_4();
      _ll_callv_w_cache(op, n, lcache);
#if ll_BC_DEBUG_EXPR
      _ll_debug_expr = &debug_expr;
#endif
    }
    break;

  case ll_bc(call_tail, 1, - ARG(0), ARG(0) + 1): /* op args ... | rtn */
    {
      ll_v op = _ll_POP();
      int n = GET_4();
#if ll_BC_DEBUG_EXPR
      *(_ll_debug_expr = prev_debug_expr) = debug_expr;
#endif
#if ll_USE_LCACHE
      if ( bcCdebug ) {
	fprintf(stderr, "lcache = %p, op = %s %s\n", lcache, ll_po(ll_TYPE(op)), ll_po(op));
      }
#endif
      _ll_call_tailv_w_cache(op, n, lcache);
    }
    break;

  case ll_bc(call_super, 1, -1 - ARG(0), ARG(0) + 1): /* super op args ... | rtn */
    {
      ll_v super = _ll_POP();
      ll_v op = _ll_PEEK();
      int n = GET_4();
      _ll_PEEK() = ll_SELF;
      _ll_call_superv(op, super, n);
#if ll_BC_DEBUG_EXPR
      _ll_debug_expr = &debug_expr;
#endif
    }
    break;

  case ll_bc(call_super_tail, 1, -1 - ARG(0), ARG(0) + 1): /* super op args ... | rtn */
    {
      ll_v super = _ll_POP();
      ll_v op = _ll_PEEK();
      int n = GET_4();
      _ll_PEEK() = ll_SELF;
#if ll_BC_DEBUG_EXPR
      *(_ll_debug_expr = prev_debug_expr) = debug_expr;
#endif
      _ll_call_super_tailv(op, super, n);
    }
    break;

  default:
    fprintf(stderr, "\nll: unexpected bytecode %d\n", pc[-1]);
    ll_call(ll_o(_dump), _1(ll_AR_METH));
    _ll_error(ll_fe(bytecode), 1, ll_s(bytecode), ll_make_fixnum(pc[-1]));
  }
}
ll_define_primitive_end

#undef break
#undef METH


/********************************************************************/

static __inline
void _ll_make_bmethod_lcache_vector(ll_v _bcm)
{
#if ll_USE_LCACHE
  ll_v lcache_size = ll_call(ll_o(property), _2(_bcm, ll_s(call_site_count)));
  ll_tsa_byte_code_method *bcm = ll_THIS_ISA(byte_code_method, _bcm);

  bcm->_lcache_vector = 
    ll_unbox_boolean(lcache_size) ? 
    _ll_new_lcache_vector(ll_unbox_fixnum(ll_cdr(lcache_size))) :
    0;
#endif
}


ll_define_primitive(byte_code_method, initialize, __3(self,code,consts,rest), _0())
{
  ll_tsa_method *x;

  x = ll_THIS_ISA(method, ll_SELF);
  x->_func = _ll_prim_func_name(object,_bmethod_apply);
  x->_formals = ll_f;
  x->_properties = ll_nil;
  x->_code = ll_ARG_1;
  x->_consts = ll_ARG_2;
  if ( ll_ARGC >= 4 ) {
    x->_properties = ll_ARG_3;
  }
  if ( ll_ARGC >= 5 ) {
    x->_env = ll_ARG_4;
  }

  _ll_make_bmethod_lcache_vector(ll_SELF);

  if ( ll_BC_DUMP ) {
    ll_call(ll_o(_dump), _1(ll_SELF));
  }

  ll_return(ll_SELF);
}
ll_define_primitive_end


/********************************************************************/


static void _ll_make_bmethod(ll_v consts)
{
  /* stack:  <code> <properties> | <bmethod> */

  ll_v x = _ll_allocate_type(ll_type(byte_code_method));
  ll_tsa_method *m = ll_THIS_ISA(method, x);
  m->_func = _ll_prim_func_name(object,_bmethod_apply);
  m->_formals = ll_f;
  m->_consts = consts;
  m->_code = _ll_POP();
  m->_properties = _ll_PEEK();

  _ll_make_bmethod_lcache_vector(x);

  if ( ll_BC_DUMP ) {
    ll_call(ll_o(_dump), _1(x));
  }

  _ll_PEEK() = x;
}


static void _ll_make_bmethod_env(ll_v consts)
{
  /* stack: <code> <properties> <env> | <bmethod> */

  ll_v x = _ll_allocate_type(ll_type(byte_code_method));
  ll_tsa_method *m = ll_THIS_ISA(method,x);
  m->_func = _ll_prim_func_name(object,_bmethod_apply);
  m->_formals = ll_s(args);
  m->_consts = consts;
  m->_code = _ll_POP();
  m->_properties = _ll_POP();
  m->_env = _ll_PEEK();

  _ll_make_bmethod_lcache_vector(x);

  if ( ll_BC_DUMP ) {
    ll_call(ll_o(_dump), _1(x));
  }

  _ll_PEEK() = x;
}


/********************************************************************/
