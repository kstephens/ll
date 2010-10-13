#include "ll/ll.h"

#include "assert.h"

#if 0
#if ll_USE_GC_BOEHM
#include "gc.h"
#endif

#if ll_USE_GC_TREADMILL
#include "tredmill/tm.h"
#endif
#endif

/************************************************************************/

void *(*_ll_malloc)(size_t);
void *(*_ll_realloc)(void*, size_t);
void  (*_ll_free)(void*);

/************************************************************************/

#ifdef ll_malloc
#undef ll_malloc
#endif

/* #define ll_malloc_limit (10 * 1024) */

static ll_v _ll_out_of_memory_error()
{
  return _ll_error(ll_fe(out_of_memory), 0);
}

void *ll_malloc(size_t s)
{
  void *p;
#ifdef ll_malloc_limit
  ll_assert_msg(mem,s < ll_malloc_limit,("possible <type>::size corruption?"));
#endif
  if ( ! (p = _ll_malloc(s)) ) {
    _ll_out_of_memory_error();
  }
  return p;
}

#ifdef ll_realloc
#undef ll_realloc
#endif

void *ll_realloc(void *p, size_t s)
{
#ifdef ll_malloc_limit
  ll_assert_msg(mem,s < ll_malloc_limit,("possible <type>::size corruption?"));
#endif
  if ( ! (p = _ll_realloc(p, s)) ) {
    _ll_out_of_memory_error();
  }
  return p;
}

#ifdef ll_free
#undef ll_free
#endif

void ll_free(void *p)
{
  _ll_free(p);
}

static 
void _ll_free_nothing(void *p)
{
  /* NOTHING */
}


ll_INIT(gc, 0, "GC system")
{
  const char *env;

  _ll_free    = _ll_free_nothing;

  env = getenv("LL_GC");
  if ( ! env ) env = "1";
  if ( atoi(env) ) {

#if ll_USE_GC_BOEHM
    _ll_malloc  = GC_malloc;
    _ll_realloc = GC_realloc;
#define _ll_gcollect() GC_gcollect()
#define _ll_gc_register_finalizer(ptr,data) GC_register_finalizer_ignore_self(ptr, _ll_finalize, data, 0, 0)
#endif

#if ll_USE_GC_TREADMILL
#define _ll_gc_init(ARGC,ARGV,ENVP)tm_init((ARGC),(ARGV),(ENVP))
    _ll_malloc  = tm_alloc;
    _ll_realloc = tm_realloc;
#define _ll_gcollect() tm_gc_full()
#endif

  } else {
    fprintf(stderr, "\nll: GC disabled!\n");
  }

#ifndef _ll_gc_init
#define _ll_gc_init(ARGC,ARGV,ENVP)((void)0)
#endif

#ifndef _ll_gcollect
#define _ll_gcollect()((void)0)
#endif

#ifndef _ll_gc_register_finalizer
#define _ll_gc_register_finalizer(ptr,data) ((void) 0)
#endif

  if ( ! _ll_malloc ) {
    _ll_malloc = malloc;
    _ll_realloc = realloc;
    _ll_free = free;
  }

  _ll_gc_init(_argcp, _argvp, _envp);
  return 0;
}


/************************************************************************/

void _ll_finalize(void *obj, void *data)
{
  if ( ll_DEBUG(finalize) ) {
    ll_format(ll_f, "\n  ll: (%finalize ~S ~S)\n", 2, ll_BOX_ref(obj), ll_BOX_ref(data));
  }
  ll_call(ll_BOX_ref(data), _1(ll_BOX_ref(obj)));
}


ll_define_primitive(object, _register_finalizer, __0(operation), _0())
{
  ll_v op = ll_ARGC > 1 ? ll_ARG_1 : ll_o(_finalize);
  if ( ll_DEBUG(finalize) ) {
    ll_format(ll_f, "\n  ll: (%register_finalizer ~S, ~S)\n", 2, ll_SELF, op);
  }
  if ( ll_ISA_ref(ll_SELF) && ll_ISA_ref(op) ) {
    _ll_gc_register_finalizer(ll_UNBOX_ref(ll_SELF),
			      ll_UNBOX_ref(op));
    ll_return(ll_t);
  } else {
    ll_return(ll_f);
  }
}
ll_define_primitive_end


ll_define_primitive(object, _finalize, _1(obj), _0())
{
  /* NOTHING */
}
ll_define_primitive_end


/************************************************************************/

ll_define_primitive(object, _gcollect, _0(), _0())
{
  _ll_gcollect();
}
ll_define_primitive_end


ll_define_primitive(object, _mem_stats, _0(), _0())
{
  ll_v x = ll_f;

#if ll_USE_GC_BOEHM
  x = 
    ll_listn(4,
#define X(N) ll_cons(ll_s(N), ll_make_fixnum(GC_get_##N()))
	     X(heap_size),
	     X(free_bytes),
	     X(bytes_since_gc),
	     X(total_bytes),
	     ll_undef);
#undef X
#endif

  ll_return(x);

}
ll_define_primitive_end

/************************************************************************/

/*
** Force all other code linked with ll to use our malloc().
*/
#if 0

#ifdef malloc
#undef malloc
#endif

void *malloc(size_t size)
{
  return size ? ll_malloc(size) : 0;
}

#ifdef realloc
#undef realloc
#endif

void *realloc(void *ptr, size_t size)
{
  return ptr ? ll_realloc(ptr, size) : malloc(size);
}


#ifdef free
#undef free
#endif

void free(void *ptr)
{
  if ( ptr )
    ll_free(ptr);
}


#ifdef calloc
#undef calloc
#endif

void *calloc(size_t x, size_t y)
{
  return malloc(x * y);
}

#endif
