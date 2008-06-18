#ifndef __rcs_id__
#ifndef __rcs_id_ll_lcache_c__
#define __rcs_id_ll_lcache_c__
static const char __rcs_id_ll_lcache_c[] = "$Id: lcache.c,v 1.12 2008/05/26 19:13:36 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "lcache.h"
#include <string.h> /* memset() */


#if 0
static ll_lcache *_ll_lcache_list = 0;
static size_t _ll_lcache_count = 0;
#else
#define _ll_lcache_list  ll_runtime(lcache_list)
#define _ll_lcache_count ll_runtime(lcache_count)
#endif

ll_lcache_stat _ll_lcache_stat_global;

#define SE(N) ll_cons(ll_s(N), ll_make_fixnum(s->_##N##_n))

#define ll_lcache_stat_ELEMS	     \
  SE(lookup),			     \
    SE(probe),			     \
    SE(op_changed),		     \
    SE(hit),			     \
    SE(overflow),		     \
    SE(reorder),		     \
    SE(miss)

ll_define_primitive(object, _lcache_stats, _0(), _0())
{
  ll_lcache_stat copy = _ll_lcache_stat_global; 
  ll_lcache_stat *s = &copy;

  ll_return(
	    ll_listn(14,
		     ll_cons(ll_s(context), ll_s(global)),
		     ll_lcache_stat_ELEMS,
		     ll_cons(ll_s(cache_count), ll_make_fixnum(_ll_lcache_count)),
		     ll_cons(ll_s(op_lookup), ll_g(_lookup_op_cache_lookup)),
		     /* 10 */
		     ll_cons(ll_s(op_lambda), ll_g(_lookup_op_cache_lambda)),
		     ll_cons(ll_s(op_hit),    ll_g(_lookup_op_cache_hit)),
		     ll_cons(ll_s(op_miss),   ll_g(_lookup_op_cache_miss)),
		     ll_cons(ll_s(op_change), ll_g(_lookup_op_cache_change)),
		     ll_undef
		     )
	    );
}
ll_define_primitive_end


ll_v _ll_lcache_to_object(ll_lcache *c)
{
  ll_lcache_stat *s = &(c->_stat);
  ll_v x;
  ll_v slots = ll_nil, *slotsp = &slots;

  {
    struct ll_lcache_elem *p;
    struct ll_lcache_elem *e = ll_lcache_end(c);  
    
    for ( p = c->_elems; p < e; ++ p ) {
      ll_v e_obj;

      if ( ! e->_hits ) {
	continue;
      }

#define E(n) ll_cons(ll_s(n), e->_##n)

      e_obj =
	ll_listn(7,
		 ll_cons(ll_s(hits), ll_make_fixnum(e->_hits >> 2)),
		 E(op),
		 E(version),
		 E(isa),
		 E(meth),
		 E(type),
		 E(off),
		 ll_undef
		 );

      ll_format(ll_undef, " e = ~S\n", 1, e_obj);

      slotsp = &ll_CDR(*slotsp = ll_cons(e_obj, ll_nil));
    }
#undef E
  }


  x = 
    ll_listn(3,
	     ll_cons(ll_s(context), 
		     ll_cons(ll_make_string(c->_file ? c->_file : "?", -1), 
			     ll_make_fixnum(c->_line))
		     ),
	     ll_listn(7,
		      ll_lcache_stat_ELEMS
		      ),
	     slots,
	     ll_undef
	     );

  return x;
}


ll_define_primitive(object, _lcache_all, _0(), _0())
{
  ll_v l = ll_nil;
  ll_lcache *c;
  
  for ( c = _ll_lcache_list; c; c = c->_next ) {
    ll_v x = _ll_lcache_to_object(c);
    ll_format(ll_undef, "~S\n", 1, x);
  }

  ll_return(l);
}
ll_define_primitive_end


void _ll_lookup_w_cache(ll_lcache *c)
{
  if ( c ) {
  struct ll_lcache_elem *p = c->_elems;
  struct ll_lcache_elem *e = ll_lcache_end(c);
  ll_v isa = ll_RCVR_TYPE;
  ll_v op;

  _ll_lookup_op_check();

  op = ll_AR_OP;
  ll_assert_ref(op);

#define STAT_INCR(N) \
  ++ _ll_lcache_stat_global.N; \
  ++ c->_stat.N

  STAT_INCR(_lookup_n);

  while ( p < e ) {
    STAT_INCR(_probe_n);

    if ( ll_EQ(p->_isa, isa) && ll_EQ(p->_op, op) ) {
      /* Has op been assocated with any new types or methods? */
      if ( ll_NE(p->_version, ll_THIS_ISA(operation, op)->_version) ) {
	STAT_INCR(_op_changed_n);
        goto update_cache;
      }
      
      STAT_INCR(_hit_n);

      /* Update hit count. */
      /* Trim cache hits? */
      if ( (++ p->_hits) == 0 ) {
	struct ll_lcache_elem *x = c->_elems;

	STAT_INCR(_overflow_n);

	-- p->_hits;
        while ( x < e ) {
	  (x ++)->_hits >>= 2;
	}
      }

      /* Unload cache. */
      ll_AR_METH = p->_meth;
      ll_AR_TYPE = p->_type;
      ll_AR_TYPE_OFFSET = p->_off;

      /* Keep cache sorted by decreasing hits. */
      while ( p > c->_elems && p->_hits > (p - 1)->_hits ) {
      	struct ll_lcache_elem temp = *(p - 1);

	STAT_INCR(_reorder_n);

	*(p - 1) = *p;
	*p = temp;
	-- p;
      }
      
      return;
    } else if ( p->_op == 0 || ll_EQ(p->_op, ll_undef) ) {
      /* An empty cache slot was found, use it. */
fill_cache:
      p->_op = op;
      p->_isa = isa;
      p->_hits = 1;

update_cache:
      p->_version = ll_THIS_ISA(operation, ll_AR_OP)->_version;

      STAT_INCR(_miss_n);
      
      /* Do real lookup. */
      _ll_lookup();
      
      /* Fill cache entry. */
      p->_meth = ll_AR_METH;
      p->_type = ll_AR_TYPE;
      p->_off  = ll_AR_TYPE_OFFSET;

      /* Mark cache entry in-use. */
      if ( ! c->_flags ) {
	c->_flags |= 1;

	/* Only add inline C code. */
	if ( c->_file && c->_line ) {
	  /* Add to lcache list. */
	  ++ _ll_lcache_count;
	  c->_next = _ll_lcache_list;
	  _ll_lcache_list = c;
	}
      }

      return;
    }
    
    /* Try next slot. */
    ++ p;
  }
  
  /* Reuse last cache slot. */
  -- p;
  goto fill_cache;

#undef STAT_INCR
  } else {
    _ll_lookup();
  }
}



ll_lcache_vector *_ll_new_lcache_vector(size_t s)
{
  ll_lcache_vector *cv;

  cv = ll_malloc(sizeof(*cv));
  cv->_s = s;
  cv->_v = ll_malloc(sizeof(cv->_v[0]) * s);
  memset(cv->_v, 0, sizeof(cv->_v[0]) * s);

  return cv;
}


ll_INIT(lcache, 5, "lcache")
{
  assert(_ll_lcache_list == 0);
  _ll_lcache_list = 0;
  assert(_ll_lcache_count == 0);
  _ll_lcache_count = 0;

  return 0;
}

