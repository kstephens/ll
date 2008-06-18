#ifndef _ll_prim_h
#define _ll_prim_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_prim_h__
#define __rcs_id_ll_prim_h__
static const char __rcs_id_ll_prim_h[] = "$Id: prim.h,v 1.17 2008/05/26 06:35:03 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll/value.h" /* ll_v */
#include "ll/global_index.h" /* ll_gi */
#if 0
#include "ll/stack.h" /* ll_VS_POP() */
#endif
#include "ll/prim_type.h" /* ll_func() */

/***********************************************************************/

typedef struct ll_primitive {
  /*@ A primitive descriptor used to initialize a <primitive> object. */
  ll_func _func;
  /*@ The low-level <primitive> method hander. */
  const char *_type;
  /*@ The type name for this primitive. */
  const char *_op;
  /*@ The operation name for this primitive. */
  const char *_formals[7];
  /*@ A list of formal parameter names for this primitive. */
  short _restarg;
  /*@ If non-zero the primitive takes rest args. */
  const char *_props[6];
  /*@ The name of any properties. */
  const char *_props_values[6];
  /*@ The values of any properties. */
  ll_v _prim;
  /*@ The reference to the initialized <primitive> object. */
} ll_primitive;

#define ll_prim(TYPE,NAME) _ll_p_##TYPE##_##NAME

#define ll_declare_primitive(TYPE,NAME) \
extern ll_primitive ll_prim(TYPE,NAME)

extern ll_v _ll_argc_error();

#ifndef ll_define_primitive
/*
OLD macros
#define ll_define_primitive(TYPE,NAME,MINNARGS,ARGS,OPTS)_ll_define_primitive(TYPE,NAME,MINNARGS,ARGS,OPTS)
*/
#define ll_define_primitive(TYPE,NAME,ARGS,OPTS)_ll_define_primitive(TYPE,NAME,_ll_FR_MINNARGS(ARGS),ARGS,OPTS)

#define _ll_FR_MINNARGS_0()0
#define _ll_FR_MINNARGS_1(_1)1
#define _ll_FR_MINNARGS_2(_1,_2)2
#define _ll_FR_MINNARGS_3(_1,_2,_3)3
#define _ll_FR_MINNARGS_4(_1,_2,_3,_4)4
#define _ll_FR_MINNARGS_5(_1,_2,_3,_4,_5)5
#define _ll_FR_MINNARGS_6(_1,_2,_3,_4,_5,_6)6

#define _ll_FR_MINNARGS__0(_R)0
#define _ll_FR_MINNARGS__1(_1,_R)1
#define _ll_FR_MINNARGS__2(_1,_2,_R)2
#define _ll_FR_MINNARGS__3(_1,_2,_3,_R)3
#define _ll_FR_MINNARGS__4(_1,_2,_3,_4,_R)4
#define _ll_FR_MINNARGS__5(_1,_2,_3,_4,_5,_R)5
#define _ll_FR_MINNARGS__6(_1,_2,_3,_4,_5,_6,_R)6

#define _ll_FR_MINNARGS(X)_ll_FR_MINNARGS##X

#define _ll_FR_MAXNARGS_0()0
#define _ll_FR_MAXNARGS_1(_1)1
#define _ll_FR_MAXNARGS_2(_1,_2)2
#define _ll_FR_MAXNARGS_3(_1,_2,_3)3
#define _ll_FR_MAXNARGS_4(_1,_2,_3,_4)4
#define _ll_FR_MAXNARGS_5(_1,_2,_3,_4,_5)5
#define _ll_FR_MAXNARGS_6(_1,_2,_3,_4,_5,_6)6

#define _ll_FR_MAXNARGS__0(_R)-1
#define _ll_FR_MAXNARGS__1(_1,_R)-1
#define _ll_FR_MAXNARGS__2(_1,_2,_R)-1
#define _ll_FR_MAXNARGS__3(_1,_2,_3,_R)-1
#define _ll_FR_MAXNARGS__4(_1,_2,_3,_4,_R)-1
#define _ll_FR_MAXNARGS__5(_1,_2,_3,_4,_5,_R)-1
#define _ll_FR_MAXNARGS__6(_1,_2,_3,_4,_5,_6,_R)-1

#define _ll_FR_MAXNARGS(X)_ll_FR_MAXNARGS##X

#define _ll_FR_NAMES_0(){0},0
#define _ll_FR_NAMES_1(_1){#_1},0
#define _ll_FR_NAMES_2(_1,_2){#_1,#_2},0
#define _ll_FR_NAMES_3(_1,_2,_3){#_1,#_2,#_3},0
#define _ll_FR_NAMES_4(_1,_2,_3,_4){#_1,#_2,#_3,#_4},0
#define _ll_FR_NAMES_5(_1,_2,_3,_4,_5){#_1,#_2,#_3,#_4,#_5},0
#define _ll_FR_NAMES_6(_1,_2,_3,_4,_5,_6){#_1,#_2,#_3,#_4,#_5,#_6},0

#define _ll_FR_NAMES__0(_R){#_R},1
#define _ll_FR_NAMES__1(_1,_R){#_1,#_R},1
#define _ll_FR_NAMES__2(_1,_2,_R){#_1,#_2,#_R},1
#define _ll_FR_NAMES__3(_1,_2,_3,_R){#_1,#_2,#_3,#_R},1
#define _ll_FR_NAMES__4(_1,_2,_3,_4,_R){#_1,#_2,#_3,#_4,#_R},1
#define _ll_FR_NAMES__5(_1,_2,_3,_4,_5,_R){#_1,#_2,#_3,#_4,#_5,#_R},1
#define _ll_FR_NAMES__6(_1,_2,_3,_4,_5,_6,_R){#_1,#_2,#_3,#_4,#_5,#_6,#_R},1
#define _ll_FR_NAMES(X)_ll_FR_NAMES##X

/* Property list expansion macros. */
#define _ll_p_PROP_0(){}
#define _ll_p_PROP_1(_1,_1v){#_1},{_1v}
#define _ll_p_PROP_2(_1,_1v,_2,_2v){#_1,#_2},{_1v,_2v}
#define _ll_p_PROP_3(_1,_1v,_2,_2v,_3,_3v){#_1,#_2,#_3},{_1v,_2v,_3v}
#define _ll_p_PROP_4(_1,_1v,_2,_2v,_3,_3v,_4,_4v){#_1,#_2,#_3,#_4},{_1v,_2v,_3v,_4v}
#define _ll_p_PROP_5(_1,_1v,_2,_2v,_3,_3v,_4,_4v,_5,_5v){#_1,#_2,#_3,#_4,#_5},{_1v,_2v,_3v,_4v,_5v}
#define _ll_p_PROP_6(_1,_1v,_2,_2v,_3,_3v,_4,_4v,_5,_5v,_6,_6v){#_1,#_2,#_3,#_4,#_5,#_6},{_1v,_2v,_3v,_4v,_5v,_6v}
#define _ll_p_PROP(X)_ll_p_PROP##X

/* C symbol for primitive handler function. */
#define _ll_prim_func_name(TYPE,NAME)_ll_pf_##NAME##_C_##TYPE

/* Defines the beginning of a primitive method. */
#define _ll_define_primitive(TYPE,NAME,MINNARGS,ARGS,PROPS) \
  /* Forward declaration to the primitive handler function. */ \
static ll_func_DECL(_ll_prim_func_name(TYPE,NAME)); \
  /* Defines the primitive initializer. */ \
ll_primitive ll_prim(TYPE,NAME) = { \
  /* Pointer to the primitive's handler function. */ \
  &_ll_prim_func_name(TYPE,NAME), \
  /* The type name. */ \
  #TYPE, \
  /* The operation name. */ \
  #NAME, \
  /* The formals list and minimum argument count. */ \
  _ll_FR_NAMES(ARGS), \
  /* The properties list. */ \
  _ll_p_PROP(PROPS) \
}; \
  /* Defines the beginning of the sub-primitive handler function. */ \
static ll_func_DECL(_ll_prim_func_name(TYPE, NAME)) \
{ \
  const char *ll_PRIM_TYPE_NAME = (ll_PRIM_TYPE_NAME = #TYPE); \
  const char *ll_PRIM_OP_NAME = (ll_PRIM_OP_NAME = #NAME); \
  ll_v *_ll_ARGV = (_ll_ARGV = (ll_v *) ll_ARGV); \
  ll_message *_ll_AR = (_ll_AR = (ll_message *) _ll_ar_sp); \
  /* The return value. */ \
  ll_v __ll_return_val; \
  /* Get C-pointer to offset of TYPE block in receiver. */ \
  ll_tsa_##TYPE *ll_THIS = \
    (ll_THIS = (void*) (((char*) ll_UNBOX_ref(_ll_ARGV[0])) - (sizeof(ll_tsa_##TYPE) - sizeof(ll_ts_##TYPE)) + ll_UNBOX_fixnum(_ll_AR->_type_offset) )); \
  /* Check argument count. */ \
  if ( (MINNARGS > 0 && ll_ARGC < MINNARGS) || (_ll_FR_MAXNARGS(ARGS) >= 0 && ll_ARGC > _ll_FR_MAXNARGS(ARGS)) ) { ll_return(_ll_argc_error()); } \
{
#endif

extern ll_v _ll_debug_at_rtn(ll_v *rtnval);

#ifndef _ll_DB_AT_RTN
#define _ll_DB_AT_RTN \
  /* Should debugger be invoked? */ \
  if ( ll_unbox_boolean(ll_AR_DB_AT_RTN) ) { \
    __ll_return_val = _ll_debug_at_rtn(&__ll_return_val); \
  }
#endif

#define ll_return(X)do{__ll_return_val = (X); goto __ll_return; }while(0)

/* Ends the primitive handler function. */
#define ll_define_primitive_end \
} \
  /* Default return value to unspecified. */ \
  __ll_return_val = ll_unspec; \
  /* Place for ll_return() to jump to. */ \
  __ll_return: \
  /* Check if debugger should be envoked now. */ \
  _ll_DB_AT_RTN; \
  /* Pop arguments off stack. */ \
  /* Check if value stack buffer needs to be popped. */ \
  if ( _ll_val_stack_buffer != ll_AR_ARGV_SB ) \
    _ll_val_stack_buffer_switch(ll_AR_ARGV_SB); \
  _ll_VS_POP(ll_ARGV + ll_ARGC); \
  /* Allocate space for return value. */ \
  _ll_VS_PROBE(1); \
  /* Push return value. */ \
  _ll_PUSH(__ll_return_val); \
  /* Return to caller, without tail-call (0). */ \
  return 0; \
}


/*************************************************************************/
/* primitives */


#define ll_p(TYPE,NAME)ll_prim(TYPE,NAME)._prim

ll_declare_primitive(object,_identity);
#define _ll_p_identity ll_p(object,_identity)

ll_declare_primitive(object,_true);
#define _ll_p_true ll_p(object,_true)

ll_declare_primitive(object,_false);
#define _ll_p_false ll_p(object,_false)

ll_declare_primitive(object,_method_not_found);
#define _ll_p_method_not_found ll_p(object,_method_not_found)

ll_declare_primitive(object,_bad_typecheck);
#define _ll_p_typecheck_error ll_p(object,_bad_typecheck)

/************************************************************************/

ll_v _ll_make_primitive(ll_func prim, const char *name, ll_v formals, ll_v props);

/************************************************************************/
/* Primitive method inits */


typedef struct _ll_prim_init {
  _ll_gi type;
  _ll_gi op;
  ll_primitive *prim;
} _ll_prim_init;


extern _ll_prim_init _ll_prim_inits[];


#endif
