#ifndef _ll_ll_h

#define _ll_ll_h

/*************************************************************************/
/* config */

#include "ll/config.h"

/* Sanity checks. */

#if ll_MULTIPLICTY && ll_GLOBAL_VARS
#error "Cannot use ll_MULTIPLICY != 0 and ll_GLOBAL_VARS != 0"
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

#include "gmp.h" /* mpz_*: see bignum */

#include "ccont/ccont.h" /* CCont, call/cc support. */

/*************************************************************************/
/* basic utils */

#ifndef ll_STRINGTIZE
#define _ll_STRINGTIZE(X)#X
#define ll_STRINGTIZE(X)_ll_STRINGTIZE(X)
#endif

#ifndef ll_OFFSET_OF
#define ll_OFFSET_OF(T,E)((size_t)(&((T*)(0))->E))
#endif

#ifndef ll_ALIGN_OF
#define ll_ALIGN_OF(T,E) __alignof__(typeof(((T*)(0))->E))
#endif

/*************************************************************************/
/* mem mgmt */

extern void *(*_ll_malloc)(size_t);
extern void *(*_ll_realloc)(void*, size_t);
extern void  (*_ll_free)(void*);

extern void *ll_malloc(size_t);
extern void *ll_realloc(void *, size_t);
extern void  ll_free(void *);

#ifndef ll_USE_GC
#define ll_USE_GC 1
#endif

#if ll_USE_GC

#ifndef ll_USE_GC_BOEHM
#define ll_USE_GC_BOEHM 1
#endif

#ifndef ll_USE_GC_TREADMILL
#define ll_USE_GC_TREADMILL 0
#endif

#if ll_USE_GC_BOEHM
#include "gc.h"
#endif

#if ll_USE_GC_TREADMILL
#include "tredmill/tm.h"
#define ll_write_barrier_ptr(X)tm_write_barrier(X)
#define ll_write_barrier_ptr_pure(X)tm_write_barrier_pure(X)
#define ll_write_root(X)tm_write_root(X)
#endif

#else /* ! ll_USE_GC */

#endif /* ll_USE_GC */


/***********************************************************************/
/* Write barrier support for treadmill allocator. */

#ifndef ll_write_barrier_ptr
#define ll_write_barrier_ptr(X) ((void)0)
#endif

#ifndef ll_write_barrier_ptr_pure
#define ll_write_barrier_ptr_pure(X) ((void)0)
#endif

#ifndef ll_write_barrier
#define ll_write_barrier(X)ll_write_barrier_ptr(ll_UNBOX_ref(X))
#endif

#ifndef ll_write_barrier_pure
#define ll_write_barrier_pure(X)ll_write_barrier(X)
#endif

#ifndef ll_write_barrier_SELF
#define ll_write_barrier_SELF()ll_write_barrier_pure(ll_SELF)
#endif

#ifndef ll_write_root
#define ll_write_root(X)((void)0)
#endif

/***********************************************************************/
/* Set field macros for write barrier support. */

#ifndef ll_setf
#define ll_setf(X,F,V) ((X)->F = (V), ll_write_barrier(X))
#endif

#ifndef ll_setf_THIS
#define ll_setf_THIS(F,V) (ll_THIS->F = (V), ll_write_barrier_SELF())
#endif

/***********************************************************************/
/* ll_v */

#include "ll/value.h"


/***********************************************************************/
/* eq?, equal? */


#ifndef ll_eqQ
#define ll_eqQ(X,Y)ll_EQ((X),(Y))
#endif
int ll_eqvQ(ll_v x, ll_v y);
int ll_equalQ(ll_v x, ll_v y);

/***********************************************************************/
/* undefined, unspecified, uninitialized */

#define ll_UNDEF ll_undef
#define ll_UNSPEC ll_unspec

/*************************************************************************/
/* error handling */

ll_v _ll_error(ll_v name, int nargs, ...);

#define _ll_e_NAME(NAME,SUPER)NAME##_##SUPER
#define ll_e_NAME(NAME,SUPER)_ll_e_NAME(NAME,SUPER)

#ifndef ll_e
#define ll_e(NAME,SUPER) ll_type(ll_e_NAME(NAME,SUPER))
#endif

#define ll_re(NAME)ll_e(NAME,recoverable_error)
#define ll_ee(NAME)ll_e(NAME,error)
#define ll_fe(NAME)ll_e(NAME,fatal_error)

/* basic error constructors */
ll_v _ll_range_error(ll_v name, ll_v value, long low, long high);
ll_v _ll_rangecheck(ll_v name, ll_v *value, long low, long high);
ll_v _ll_typecheck(ll_v type, ll_v *value);
ll_v _ll_readonly_variable(ll_v var);
ll_v _ll_undefined_variable(ll_v var);

void _ll_abort(const char *file, int lineno, const char *msg);
#define ll_abort(msg) _ll_abort(__FILE__, __LINE__, msg) 

/*************************************************************************/
/* types */

#include "ll/char.h"

#include "type.h"


/***********************************************************************/
/* The runtime */

#include "ll/char.h" /* ll_tsa_char_table forward decl. */

#include "ll/runtime.h"

#include "ll/global.h"


/*************************************************************************/
/* call */

#include "call.h"


/*************************************************************************/
/* environment */

#if ll_GLOBAL_VARS
extern int _ll_env_bindings_n;
extern ll_v *_ll_env_bindings;
extern ll_v _ll_env;
#else
#define _ll_env_bindings_n ll_runtime(env_bindings_n)
#define _ll_env_bindings   ll_runtime(env_bindings)
#define _ll_env            ll_runtime(env)  
#endif

#ifndef ll_gb
/* Returns global binding by name. */
#define ll_gb(N) _ll_env_bindings[ll_gi(N)]
#endif
#define _ll_gv(B) (*ll_UNBOX_locative(ll_THIS_ISA(binding, (B))->_locative))
#define _ll_gr(N) _ll_gv(ll_gb(N))

/************************************************************************/
/* operation references */

#ifndef ll_define_operation
#define ll_define_operation(NAME,TYPE)
#endif

#ifndef ll_o
#define ll_o(X)ll_g(X)
#endif

/************************************************************************/
/* primitives */

#include "prim.h"

/*************************************************************************/
/* macros */

#define ll_mn(NAME)_mC##NAME

#ifndef ll_define_macro
#define _ll_define_macro(TYPE,NAME,ARGS)ll_define_primitive(TYPE,ll_mn(NAME),ARGS,_0())
#define ll_define_macro(TYPE,NAME,ARGS)_ll_define_macro(TYPE,NAME,ARGS)
#define ll_define_macro_end ll_define_primitive_end
#endif

/*************************************************************************/
/* initialization */

#include "ll/init.h"

ll_v _ll_validate_ref(ll_v x);
void _ll_add_validator(const char *name, void *func);

/*************************************************************************/
/* type */

ll_v _ll_allocate_type(ll_v type);
ll_v _ll_make_type(ll_v metatype, ll_v supers, ll_v slots);
void _ll_add_method(ll_v type, ll_v op, ll_v meth);
void _ll_remove_method(ll_v type, ll_v op);

/* Returns the ll type object <N>. */
#ifndef ll_type
#if ll_TYPES_STATIC
#define _ll_type(N) ll_BOX_ref(&ll_runtime(t_##N))
#else
#define _ll_type(N) ll_g(LT##N##GT)
#endif
#define ll_type(N)_ll_type(N)
#endif


/*************************************************************************/
/* operation */

ll_v _ll_make_operation();
ll_v _ll_make_settable_operation();
ll_v _ll_make_locatable_operation();


/*************************************************************************/
/* call => lookup/apply */

ll_v __ll_lookup(ll_v type, ll_v op, ll_v super);
void _ll_print_trace(ll_v super);

/*************************************************************************/
/* methods */

#include "ll/bcode.h"


/*************************************************************************/
/* pair */

ll_v ll_cons(ll_v car, ll_v cdr);
ll_v ll_append(ll_v x, ll_v y);
ll_v ll_immutable_cons(ll_v car, ll_v cdr);

int ll_pairQ(ll_v x);

ll_v ll_car(ll_v x);
void ll_set_carE(ll_v x, ll_v v);
ll_v ll_locative_car(ll_v x);
#define ll_CAR(X) ll_SLOTS(X)[1]

ll_v ll_cdr(ll_v x);
void ll_set_cdrE(ll_v x, ll_v v);
ll_v ll_locative_cdr(ll_v x);
#define ll_CDR(X) ll_SLOTS(X)[2]

#include "ll/cadr.h"

/*************************************************************************/
/* nil */

#if ll_GLOBAL_VARS
#define ll_nil ll_g(nil)
#else
#define ll_nil ll_BOX_ref(&ll_runtime(_nil))
#endif

#define ll_nullQ(X) ll_EQ((X), ll_nil)

/*************************************************************************/
/* list */

#define ll_LIST_LOOP(L,X) do{ ll_v _##X = (L); while ( ! ll_nullQ(_##X) ) { ll_v X = ll_car(_##X); _##X = ll_cdr(_##X)
#define ll_LIST_LOOP_REF(L,X) do{ ll_v _##X = (L); while ( ! ll_nullQ(_##X) ) { ll_v* X = &ll_CAR(_##X); _##X = ll_cdr(_##X)
#define ll_LIST_LOOP_END }}while(0)

size_t _ll_list_length(ll_v l);

ll_v ll_listn(int n, ...);
ll_v ll_listv(int n, const ll_v *v);

void _ll_assq_set(ll_v *list, ll_v key, ll_v value);
ll_v _ll_assq_to_front(ll_v *list, ll_v key);
ll_v _ll_assq_delete(ll_v *x, ll_v key);

ll_v ll_assq(ll_v obj, ll_v list);
ll_v ll_memq(ll_v obj, ll_v list);

ll_v ll_quote(ll_v x);

/*************************************************************************/
/* boolean */

#if ll_GLOBAL_VARS
#define ll_t ll_g(__t)
#define ll_f ll_g(__f)
#else
#define ll_t ll_BOX_ref(&ll_runtime(_t))
#define ll_f ll_BOX_ref(&ll_runtime(_f))
#endif

#define ll_make_boolean(X)  ((X) ? ll_t : ll_f)
#define ll_unbox_boolean(X) ll_NE((X), ll_f)

/*************************************************************************/
/* undefined, unspecified, and end-of-stream */

#define ll_undef ll_g(__undef)
#define ll_unspec ll_g(__unspec)
#define ll_eos ll_g(__eos)


/*************************************************************************/
/* char */

#include "ll/char.h"


/*************************************************************************/
/* string */

#define _ll_VEC string
#define _ll_VEC_ELEM_TYPE char
#include "vec.h"

#define ll_STRING_LOOP_REF(L,X)do{ char * X = ll_ptr_string(L); size_t _l##X = ll_len_string(L); while ( _l##X -- ) { 
#define ll_STRING_LOOP(L,X)ll_STRING_LOOP_REF(L,_##X) char X = *(_##X ++)
#define ll_STRING_LOOP_END }}while(0)

const char *ll_cstr_string(ll_v x);

/*************************************************************************/
/* vector */

#define _ll_VEC vector
#define _ll_VEC_ELEM_TYPE ll_v
#include "vec.h"

#define ll_VECTOR_LOOP_REF_FROM(L,X,I)do{ ll_v * X = ll_ptr_vector(L) + (I); size_t _l##X = ll_len_vector(L) - (I); while ( _l##X -- ) { 
#define ll_VECTOR_LOOP_REF(L,X)ll_VECTOR_LOOP_REF_FROM(L,X,0)
#define ll_VECTOR_LOOP(L,X)ll_VECTOR_LOOP_REF(L,_##X) ll_v X = *(_##X ++)
#define ll_VECTOR_LOOP_FROM(L,X,I)ll_VECTOR_LOOP_REF_FROM(L,_##X,I) ll_v X = *(_##X ++)
#define ll_VECTOR_LOOP_END }}while(0)

/*************************************************************************/
/* symbol */

#include "ll/symbol.h"


/*************************************************************************/
/* number */

ll_v ll_make_integer(ll_v_word i);
ll_v ll_make_integer_d(double d);
#define ll_make_fixnum ll_make_integer

int ll_negativeQ(ll_v x);
int ll_zeroQ(ll_v x);
int ll_positiveQ(ll_v x);
int ll_oneQ(ll_v x);
ll_v ll_abs(ll_v x);
ll_v ll_modulo(ll_v x, ll_v y);

ll_v ll_coerce_flonum(ll_v X);

#define ll_ISA_ratnum(X) ll_EQ(ll_TYPE(X), ll_type(ratnum))

ll_v ll_make_ratnum_(ll_v n, ll_v d);
ll_v ll_make_ratnum(ll_v n, ll_v d);
ll_v ll_coerce_ratnum(ll_v n);
ll_v ll_numerator(ll_v x);
ll_v ll_denominator(ll_v x);

#define ll_ISA_bignum(X) ll_EQ(ll_TYPE(X), ll_type(bignum))

ll_v ll_make_bignum_(ll_v_word x);
ll_v ll_make_bignum_d(double x);
ll_v ll_coerce_bignum(ll_v x);


#define BOP(N,O) ll_v ll__##N(ll_v x, ll_v y);
#define UOP(N,O) ll_v ll__##N(ll_v x);

#include "cops.h"


/*************************************************************************/
/* fluid bindings: (fluid foo), (set! (fluid foo) nil) */

ll_v ll_bind_fluid(ll_v var, ll_v val);
ll_v ll_fluid(ll_v var);
void ll_set_fluid(ll_v var, ll_v val);
void ll_define_fluid(ll_v name, ll_v value);
void ll_unbind_fluid(ll_v prev_bindings);

#define _ll_fluid_bindings() ll_g(_fluid_bindings)
#define _ll_set_fluid_bindings(X) ll_set_g(_fluid_bindings, X)

#define ll_let_fluid() do { ll_v __fb_save = _ll_fluid_bindings()
#define ll_let_fluid_END() _ll_set_fluid_bindings(__fb_save); } while(0)


/*************************************************************************/
/* catch */

ll_v _ll_make_catch(ll_v type, ll_v *xp);
void _ll_begin_catch(ll_v catch);
ll_v _ll_end_catch(ll_v catch); /* returns the thrown value */

#define ll_CATCH_BEGIN(T,X) \
do { \
  ll_v _ll_catch = ll_undef; \
  ll_v X = _ll_make_catch(T, &_ll_catch); \
  if ( ! setjmp(ll_THIS_ISA(catch,_ll_catch)->__jb)  ) { \
    _ll_begin_catch(_ll_catch)

#define ll_CATCH_VALUE(V) \
    _ll_end_catch(_ll_catch); \
  } else { \
    ll_v V = _ll_end_catch(_ll_catch)

#define ll_CATCH_END \
  } \
}while(0)

/*************************************************************************/
/* Named objects. */

#include "ll/named.h"

/*************************************************************************/
/* formatting */
 
ll_v ll_write_string(ll_v port, const char *v, size_t l);
ll_v ll_format(ll_v port, const char *format, int nargs, ...);

/*************************************************************************/
/* call/cc */
 

/*************************************************************************/
/* errors */

ll_v _ll_error(ll_v type, int nargs, ...);
ll_v _ll_argc_error();
ll_v _ll_range_error(ll_v name, ll_v value, long low, long high);
ll_v _ll_rangecheck(ll_v name, ll_v *value, long low, long high);
ll_v _ll_undefined_variable_error(ll_v name);
ll_v _ll_readonly_variable_error(ll_v name);
ll_v _ll_typecheck_error(ll_v type, ll_v value);
ll_v _ll_typecheck(ll_v type, ll_v *value);
void _ll_additional_error_properties(int n, ...);


/*************************************************************************/
/* binding */

#include "ll/binding.h"


/*************************************************************************/
/* debug support */


#define _ll_DEBUG_SYM(X)SdebugC##X##S

#ifndef _ll_DEBUG
#define _ll_DEBUG(X) ll_unbox_fixnum(ll_g(_ll_DEBUG_SYM(X)))
#endif

#ifndef ll_DEBUG
#define ll_DEBUG(X) _ll_DEBUG(X)
#endif

#define ll_DEBUG_SET(X, Y) ll_set_g(_ll_DEBUG_SYM(X), ll_make_fixnum(Y))

void _ll_DEBUG_PRINT(const char *format, ...);
#define ll_DEBUG_PRINT(X,Y) do { if ( ll_DEBUG(X) ) { _ll_DEBUG_PRINT Y; } } while(0)

/*************************************************************************/
/* top-level */

ll_v ll_eval_string(const char *s, size_t l);


/*************************************************************************/
/* EOF */


#endif
