#ifndef _ll_LCACHE_H
#define _ll_LCACHE_H

/* $Id: lcache.h,v 1.5 2008/05/26 12:21:38 stephens Exp $ */

#include "value.h"

/*
** Lookup cache
*/

enum {
#ifndef ll_lcache_SIZE
  ll_lcache_SIZE = 4
#endif
};

typedef struct ll_lcache_stat {
  size_t _lookup_n;
  size_t _probe_n;
  size_t _op_changed_n;
  size_t _hit_n;
  size_t _overflow_n;
  size_t _reorder_n;
  size_t _miss_n;
} ll_lcache_stat;

typedef struct ll_lcache {
  struct ll_lcache_elem {
    ll_v _op;        /* The <operation> searched for. */
    ll_v _version;   /* The <operation>'s version when the entry was created. */
    ll_v _isa;       /* The type that was searched for. */
    ll_v _meth;      /* The method found. */ 
    ll_v _type;      /* The type the method was found in. */
    ll_v _off;       /* Offset of the implementing type in the searched type. */
    unsigned long _hits;  /* The number of times this cache slot was hit. */
  } _elems[ll_lcache_SIZE];
  const char *_file;
  int _line;
  int _flags;
  ll_lcache_stat _stat;
  struct ll_lcache *_next;
} ll_lcache;

#define ll_lcache_end(L) ((L)->_elems + ll_lcache_SIZE)

void _ll_lookup_w_cache(ll_lcache *c);

typedef struct ll_lcache_vector {
  ll_lcache *_v;
  size_t _s;
} ll_lcache_vector;

ll_lcache_vector *_ll_new_cache_vector(size_t s);

#endif
