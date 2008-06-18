#ifndef __rcs_id__
#ifndef __rcs_id_ll_mem_c__
#define __rcs_id_ll_mem_c__
static const char __rcs_id_ll_mem_c[] = "$Id: mem.c,v 1.10 2008/05/25 22:36:42 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll/ll.h"

#include "assert.h"

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

ll_INIT(gc, 0, "GC system")
{
  _ll_gc_init(_argcp, _argvp, _envp);
  return 0;
}

/************************************************************************/

ll_define_primitive(object, _gcollect, _0(), _0())
{
  ll_gcollect();
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
#undef X
#endif
	     ll_undef);

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
