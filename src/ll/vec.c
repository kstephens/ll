#ifndef __rcs_id__
#ifndef __rcs_id_ll_vec_c__
#define __rcs_id_ll_vec_c__
static const char __rcs_id_ll_vec_c[] = "$Id: vec.c,v 1.28 2008/05/28 11:48:49 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "call_int.h"

#ifndef _ll_VEC
#error Must define _ll_VEC
#endif
#ifndef _ll_VEC_ELEM_TYPE
#error Must define _ll_VEC_ELEM_TYPE
#endif

#ifndef PASTE2
#define _PASTE2(X,Y)X##Y
#define PASTE2(X,Y)_PASTE2(X,Y)
#endif

#ifndef PASTE3
#define _PASTE3(X,Y,Z)X##Y##Z
#define PASTE3(X,Y,Z)_PASTE3(X,Y,Z)
#endif

#define _ll_VEC_MUTABLE PASTE2(mutable_,_ll_VEC)
#define _ll_VEC_SUPER PASTE2(super_,_ll_VEC)

#define __ll_VEC_NAME(VEC,NAME)ll_##NAME##_##VEC
#define _ll_VEC_NAME(VEC,NAME)__ll_VEC_NAME(VEC,NAME)
#ifndef _ll_VEC_TERM
#define _ll_VEC_TERM 0
#endif
#ifndef _ll_VEC_BOX_VALUE
#define _ll_VEC_BOX_VALUE(X) (X)
#endif
#ifndef _ll_VEC_UNBOX_VALUE
#define _ll_VEC_UNBOX_VALUE(X) (X)
#endif
#ifndef _ll_VEC_TERM_VALUE
#define _ll_VEC_TERM_VALUE 0
#endif
#ifndef _ll_VEC_UNDEF_VALUE
#define _ll_VEC_UNDEF_VALUE 0
#endif
#ifndef _ll_VEC_ELEM_EQUAL
#define _ll_VEC_ELEM_EQUAL(X,Y)((X)==(Y))
#endif
#ifndef _ll_VEC_ELEM_LOCATIVE
#define _ll_VEC_ELEM_LOCATIVE 0
#endif


/************************************************************************/
/* type queries */


ll_define_primitive(_ll_VEC, immutable_type, _1(_ll_VEC), _1(no_side_effect,"#t"))
{
  ll_return(ll_type(_ll_VEC));
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC, mutable_type, _1(_ll_VEC), _1(no_side_effect,"#t"))
{
  ll_return(ll_type(_ll_VEC_MUTABLE));
}
ll_define_primitive_end


/************************************************************************/
/* Immutable vec */


ll_define_primitive(_ll_VEC, initialize, __1(_ll_VEC,length_fill), _0())
{
  /* Check for size */
  if ( ll_ARGC >= 2 ) {
    ll_THIS->_length = ll_unbox_fixnum(ll_ARG_1);
  } else {
    ll_THIS->_length = 0;
  }

  ll_THIS->_array = 0;
  ll_write_barrier_SELF();

  /* (initialize <vector> <l> ?<fill>?) */
  /* 
     ll_call_super(ll_o(initialize), ll_f, _1(ll_SELF));
  */

  /* Allocate _array. */
  ll_call(ll_o(deepen), _1(ll_SELF));

  /* Check for fill */
  if ( ll_ARGC >= 3 ) {
    /*
      fprintf(stderr, "vec::initialize %d %lu %s\n", ll_ARGC, ll_THIS->_length, ll_po(ll_ARG_2));
    */
    {
    size_t i = ll_THIS->_length;
    _ll_VEC_ELEM_TYPE fill = _ll_VEC_UNBOX_VALUE(ll_ARG_2);
    _ll_VEC_ELEM_TYPE *V = ll_THIS->_array;

    while ( i -- ) {
      V[i] = fill;
    }
    ll_write_barrier(V);
    }
  }

  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(fixnum, PASTE2(make_,_ll_VEC), __1(size,fill), _0())
{
  /*
    ll_call(ll_o(debugger), _0());
  */
  /* stack is now : <k> <fill>? */
  if ( ll_ARGC == 1 ) {
    ll_call_tail(ll_o(make), _2(ll_type(_ll_VEC_MUTABLE), ll_ARG_0));
  } else {
    ll_call_tail(ll_o(make), _3(ll_type(_ll_VEC_MUTABLE), ll_ARG_0, ll_ARG_1));
  }
#if 0
  _ll_VS_PROBE(1);
  _ll_PUSH(ll_type(_ll_VEC_MUTABLE));
  /* stack is now : <vector> <k> <fill>? */
  __ll_call_tailv(ll_o(make), ll_ARGC + 1);
#endif
}
ll_define_primitive_end


ll_define_primitive(object, _ll_VEC, __0(elements), _0())
{
  size_t l = ll_ARGC;
  ll_v v = ll_call(ll_o(make), _2(ll_type(_ll_VEC_MUTABLE), ll_make_fixnum(l)));
  _ll_VEC_ELEM_TYPE *V = ll_THIS_ISA(_ll_VEC,v)->_array;

  while ( l -- > 0 ) {
    V[l] = _ll_VEC_UNBOX_VALUE(ll_ARGV[l]);
  }

  ll_return(v);
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC, PASTE2(_ll_VEC,_ref), _2(_ll_VEC, index), _1(no_side_effect,"#t"))
{
  long i;

  _ll_rangecheck(ll_s(index), &ll_ARG_1, 0, ll_THIS->_length - 1);
  i = ll_UNBOX_fixnum(ll_ARG_1);

  ll_return(_ll_VEC_BOX_VALUE(ll_THIS->_array[i]));
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC, PASTE2(_ll_VEC,_length), _1(_ll_VEC), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_fixnum(ll_THIS->_length));
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC, _ptr, _1(_ll_VEC), _0())
{
  ll_return((ll_v) ll_THIS->_array);
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(_ll_VEC, clone, _1(_ll_VEC), _0())
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC, initialize_clone_mutable, _2(_ll_VEC, src), _0())
{
  ll_call_tail(ll_o(deepen), _1(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC_MUTABLE, initialize_clone, _2(_ll_VEC, src), _0())
{
  ll_call_tail(ll_o(deepen), _1(ll_SELF));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(_ll_VEC, make_immutable, _1(_ll_VEC), _0())
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC_MUTABLE, make_immutable, _1(_ll_VEC), _0())
{
  ll_TYPE_ref(ll_SELF) = ll_type(_ll_VEC);
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC, make_mutable, _1(_ll_VEC), _0())
{
  ll_SELF = ll_call_super(ll_o(clone), ll_f, _0());
  ll_TYPE_ref(ll_SELF) = ll_type(_ll_VEC_MUTABLE);
  ll_call_tail(ll_o(deepen), _1(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC_MUTABLE, make_mutable, _1(_ll_VEC), _0())
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(_ll_VEC_MUTABLE, PASTE2(_ll_VEC,_fillE), _2(_ll_VEC, fill), _0())
{
  size_t i = ll_THIS->_ll_VEC_SUPER._length;
  _ll_VEC_ELEM_TYPE *V = ((_ll_VEC_ELEM_TYPE *)(ll_THIS->_ll_VEC_SUPER._array));
  while ( i -- > 0 ) {
    V[i] = _ll_VEC_UNBOX_VALUE(ll_ARG_1);
  }
  ll_write_barrier_pure(V);
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC, PASTE2(_ll_VEC,_append), __1(_ll_VEC, sequences), _0())
{
  ll_SELF = ll_call(ll_o(clone_mutable), _1(ll_SELF));
  __ll_call_tailv(ll_o(PASTE2(_ll_VEC,_appendE)),ll_ARGC);
}
ll_define_primitive_end


ll_define_primitive(object, PASTE2(_ll_VEC,_append), _0(), _0())
{
  ll_call_tail(ll_o(make), _1(ll_type(_ll_VEC)));
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC, reverse, _1(_ll_VEC), _0())
{
  ll_SELF = ll_call(ll_o(clone), _1(ll_SELF));
  ll_TYPE_ref(ll_SELF) = ll_type(_ll_VEC_MUTABLE);
  ll_call_tail(ll_o(reverseE), _1(ll_SELF));
}
ll_define_primitive_end


/* Scheme 4 string->list, vector->list, etc. */
ll_define_primitive(_ll_VEC, PASTE2(_ll_VEC,__list), _1(_ll_VEC), _0())
{
  ll_v list = ll_nil;
  size_t n = ll_THIS->_length;
  const _ll_VEC_ELEM_TYPE *v = ll_THIS->_array + n;

  while ( n -- ) {
    list = ll_cons(_ll_VEC_BOX_VALUE(*(-- v)), list);
  }

  ll_return(list);
}
ll_define_primitive_end


ll_define_primitive(null, PASTE2(list__,_ll_VEC), _1(list), _0())
{
  ll_call_tail(ll_o(make), _2(ll_type(_ll_VEC), ll_BOX_fixnum(0)));
}
ll_define_primitive_end


ll_define_primitive(pair, PASTE2(list__,_ll_VEC), _1(list), _0())
{
  ll_v size = ll_call(ll_o(length), _1(ll_SELF));
  ll_v v = ll_call(ll_o(make), _2(ll_type(_ll_VEC_MUTABLE), size));
  size_t i = 0;

  ll_LIST_LOOP(ll_SELF, e);
  {
    ll_call(ll_o(PASTE2(_ll_VEC,_setE)), _3(v, ll_make_fixnum(i), e));
    ++ i;
  }
  ll_LIST_LOOP_END;

  ll_return(v);
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC, _equalQ, _2(_ll_VEC,_ll_VEC), _1(no_side_effect,"#t"))
{
  size_t l1, l2;
  _ll_VEC_ELEM_TYPE *v1, *v2;

  l1 = ll_THIS->_length;
  /* XXX: typechecking */
  l2 = ll_THIS_ISA(_ll_VEC, ll_ARG_1)->_length;

  if ( l1 != l2 ) {
    ll_return(ll_f);
  }

  v1 = (ll_THIS->_array);
  /* XXX: typechecking */
  v2 = (ll_THIS_ISA(_ll_VEC, ll_ARG_1)->_array);

  while ( l1 && l2 ) {
    if ( ! _ll_VEC_ELEM_EQUAL(*v1, *v2) )
      ll_return(ll_f);
    v1 ++;
    v2 ++;
    l1 --;
    l2 --;
  }
  ll_return(ll_f);
}
ll_define_primitive_end


/****************************************************************************/
/* mutable vector */


#if _ll_VEC_ELEM_LOCATIVE
ll_define_primitive(_ll_VEC_MUTABLE, PASTE3(locative_,_ll_VEC,_ref), _2(_ll_VEC, index), _1(no_side_effect,"#t"))
{
  long i;

  _ll_rangecheck(ll_s(index), &ll_ARG_1, 0, ll_THIS_ISA(_ll_VEC, ll_SELF)->_length - 1);
  i = ll_UNBOX_fixnum(ll_ARG_1);

  ll_return(ll_BOX_locative(&ll_THIS_ISA(_ll_VEC, ll_SELF)->_array[i]));
}
ll_define_primitive_end
#endif


ll_define_primitive(_ll_VEC_MUTABLE, PASTE2(_ll_VEC,_setE), _3(_ll_VEC, index, value), _0())
{
  long i;

  _ll_rangecheck(ll_s(index), &ll_ARG_1, 0, ll_THIS->_ll_VEC_SUPER._length - 1);
  i = ll_UNBOX_fixnum(ll_ARG_1);

  ll_THIS->_ll_VEC_SUPER._array[i] = _ll_VEC_UNBOX_VALUE(ll_ARG_2);
  ll_write_barrier(ll_THIS->_ll_VEC_SUPER._array);
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC, deepen, _1(_ll_VEC), _0())
{
  size_t s;
  _ll_VEC_ELEM_TYPE *v;

  s = ll_THIS->_length;
  v = ll_malloc(sizeof(v[0]) * (s + _ll_VEC_TERM));
  if ( (ll_THIS->_array) ) {
    memcpy(v, (ll_THIS->_array), sizeof(v[0]) * (s + _ll_VEC_TERM));
  } else {
    size_t i;

    for ( i = 0; i < s; i ++ ) {
      v[i] = _ll_VEC_UNDEF_VALUE;
    }
#if _ll_VEC_TERM != 0
    v[i] = _ll_VEC_TERM_VALUE;
#endif
  }
  ll_write_barrier(v);

  ll_THIS->_array = v;
  ll_write_barrier_SELF();

  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC_MUTABLE, set_lengthE, _2(_ll_VEC, length), _0())
{
  size_t s;
  long ns;
  _ll_VEC_ELEM_TYPE *v, *nv;

  s = ll_THIS->_ll_VEC_SUPER._length;
  v = (ll_THIS->_ll_VEC_SUPER._array);
 
  _ll_rangecheck(ll_s(index), &ll_ARG_1, 0, ll_MAX_fixnum);
  ns = ll_UNBOX_fixnum(ll_ARG_1);

  if ( ns > s ) {
    nv = ll_malloc(sizeof(nv[0]) * (ns + _ll_VEC_TERM));

    if ( v ) {
      memcpy(nv, v, sizeof(nv[0]) * ns);
    }

    while ( s < ns ) {
      nv[s ++] = _ll_VEC_UNDEF_VALUE;
    }

#if _ll_VEC_TERM != 0
    nv[ns] = _ll_VEC_TERM_VALUE;
#endif

    ll_write_barrier(nv);

    ll_THIS->_ll_VEC_SUPER._array = nv;
  }

  ll_THIS->_ll_VEC_SUPER._length = ll_ARG_1; 
  ll_write_barrier_SELF();
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC_MUTABLE, append_oneE, _2(_ll_VEC, obj), _0())
{
  size_t s;
  _ll_VEC_ELEM_TYPE *v, *nv;

  s = ll_THIS->_ll_VEC_SUPER._length;
  v = (ll_THIS->_ll_VEC_SUPER._array);

  nv = ll_malloc(sizeof(nv[0]) * (s + 1 + _ll_VEC_TERM));

  if ( v ) {
    memcpy(nv, v, sizeof(nv[0]) * s);
  }

  nv[s] = _ll_VEC_UNBOX_VALUE(ll_ARG_1);
  ll_write_barrier(nv);

#if _ll_VEC_TERM != 0
  nv[s + 1] = _ll_VEC_TERM_VALUE;
#endif
  ll_write_barrier(nv);

  ll_THIS->_ll_VEC_SUPER._array = nv;
  ll_THIS->_ll_VEC_SUPER._length = s + 1;
  ll_write_barrier_SELF();
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC_MUTABLE, PASTE2(_ll_VEC,_appendE), __1(_ll_VEC, sequences), _0())
{
  size_t s;
  _ll_VEC_ELEM_TYPE *v;
  int i;

  /* Compute size of vector after append. */
  s = ll_THIS->_ll_VEC_SUPER._length;
  for ( i = 1; i < ll_ARGC; i ++ ) {
    _ll_typecheck(ll_type(_ll_VEC), &ll_ARGV[i]);
    s += ll_unbox_fixnum(ll_call(ll_o(PASTE2(_ll_VEC,_length)), _1(ll_ARGV[i])));
  }

  /* If not all are null vectors, */
  if ( s != ll_THIS->_ll_VEC_SUPER._length ) {
    /* Allocate new vector. */
    _ll_VEC_ELEM_TYPE *vp = v = ll_malloc(sizeof(v[0]) * (s + _ll_VEC_TERM));

    /* Copy old vector into new vector. */
    if ( ll_THIS->_ll_VEC_SUPER._array ) {
      memcpy(v, (ll_THIS->_ll_VEC_SUPER._array), sizeof(v[0]) * (ll_THIS->_ll_VEC_SUPER._length));
      ll_write_barrier_pure(v);
    }

    /* Skip old vector contents. */
    vp += ll_THIS->_ll_VEC_SUPER._length;
    
    for ( i = 1; i < ll_ARGC; i ++ ) {
      /* Get the next vector's size. */
      size_t es = ll_unbox_fixnum(ll_call(ll_o(PASTE2(_ll_VEC,_length)), _1(ll_ARGV[i])));

      /* Get the next vector's pointer. */
      _ll_VEC_ELEM_TYPE *ev = _ll_VEC_NAME(_ll_VEC,ptr)(ll_ARGV[i]);

      /* Copy next vector's contents into place. */
      memcpy(vp, ev, sizeof(vp[0]) * (es));
      ll_write_barrier_pure(v);

      /* Skip over new vector's contents. */
      vp += es;
    }

    /* Commit the new vector and size change here.
     * This ensures that if an error occurs during
     * append, this vector is unchanged.
     */
    ll_THIS->_ll_VEC_SUPER._array = v;
    ll_THIS->_ll_VEC_SUPER._length = s;
    ll_write_barrier_SELF();
  }

  ll_return(ll_SELF);
}
ll_define_primitive_end


/***************************************************************************/


ll_define_primitive(_ll_VEC_MUTABLE, reverseE, _1(_ll_VEC), _0())
{
  size_t len = ll_THIS->_ll_VEC_SUPER._length;
  _ll_VEC_ELEM_TYPE *v1 = (ll_THIS->_ll_VEC_SUPER._array), *v2;
  v2 = v1 + len - 1;

  while ( v1 < v2 ) {
    _ll_VEC_ELEM_TYPE tmp = *v1;
    *v1 = *v2;
    *v2 = tmp;
    v1 ++;
    v2 --;
  }
  
  ll_return(ll_SELF);
}
ll_define_primitive_end


/***************************************************************************/


ll_define_primitive(_ll_VEC, PASTE2(_sub,_ll_VEC), _3(_ll_VEC, start, end), _1(no_side_effect,"#t"))
{
  long start, end;
  size_t len = ll_THIS->_length;
  _ll_VEC_ELEM_TYPE *v = (ll_THIS->_array);

  /* 0 <= start < len */
  _ll_rangecheck(ll_s(start), &ll_ARG_1, 0, len);
  start = ll_UNBOX_fixnum(ll_ARG_1);
  end = ll_unbox_fixnum(ll_ARG_2);
  if ( end < start ) {
    end = start;
  }

  /* The subvector should be of same mutality */
  {
    ll_v vec = _ll_VEC_NAME(_ll_VEC,make)(v + start, end - start);
    ll_TYPE_ref(vec) = ll_TYPE_ref(ll_SELF);
    ll_return(vec);
  }
}
ll_define_primitive_end


ll_define_primitive(_ll_VEC, PASTE2(sub,_ll_VEC), _3(_ll_VEC, start, end), _1(no_side_effect,"#t"))
{
  ll_SELF = ll_call(ll_o(PASTE2(_sub,_ll_VEC)), _3(ll_SELF, ll_ARG_1, ll_ARG_2));
  ll_TYPE_ref(ll_SELF) = ll_type(_ll_VEC_MUTABLE);
  ll_call_tail(ll_o(deepen), _1(ll_SELF));
}
ll_define_primitive_end


/***************************************************************************/


ll_v _ll_VEC_NAME(_ll_VEC,make)(_ll_VEC_ELEM_TYPE *v, size_t s)
{
  ll_v x = _ll_allocate_type(ll_type(_ll_VEC_MUTABLE));
  _ll_VEC_NAME(_ll_VEC,ts) *self = (void*) &(ll_SLOTS(x)[1]);
  self->_array = v;

#ifdef _ll_VEC_COMPUTE_SIZE
  if ( s == (size_t) -1 ) {
    s = _ll_VEC_COMPUTE_SIZE(v);
  }
#endif

  self->_length = s;

  return x;
}


ll_v _ll_VEC_NAME(_ll_VEC,make_copy)(const _ll_VEC_ELEM_TYPE *v, size_t s)
{
  _ll_VEC_ELEM_TYPE *nv;

#ifdef _ll_VEC_COMPUTE_SIZE
  if ( s == (size_t) -1 ) 
    s = _ll_VEC_COMPUTE_SIZE(v);
#endif

  nv = ll_malloc(sizeof(nv[0]) * (s + _ll_VEC_TERM));

  if ( v ) {
    memcpy(nv, v, sizeof(nv[0]) * s);
  } else {
    size_t i;

    for ( i = 0; i < s; i ++ ) {
      nv[i] = _ll_VEC_UNDEF_VALUE;
    }
  }

#if _ll_VEC_TERM != 0
  nv[s] = _ll_VEC_TERM_VALUE;
#endif

  return _ll_VEC_NAME(_ll_VEC,make)(nv, s);
}


_ll_VEC_ELEM_TYPE *_ll_VEC_NAME(_ll_VEC,ptr)(ll_v x)
{
  _ll_VEC_NAME(_ll_VEC,tsa) *self;
  ll_v type = ll_TYPE(x);

  if ( ll_EQ(type, ll_type(_ll_VEC)) || ll_EQ(type, ll_type(_ll_VEC_MUTABLE)) ) {
    self = ll_THIS_ISA(_ll_VEC, x);
    return (self->_array);
  } else {
    return (void*) ll_call(ll_o(_ptr), _1(x));
  }

  return 0;
}


size_t _ll_VEC_NAME(_ll_VEC,len)(ll_v x)
{
  _ll_VEC_NAME(_ll_VEC,tsa) *self;
  ll_v type = ll_TYPE(x);

  if ( ll_EQ(type, ll_type(_ll_VEC)) || ll_EQ(type, ll_type(_ll_VEC_MUTABLE)) ) {
    self = ll_THIS_ISA(_ll_VEC, x);
    return self->_length;
  } else {
    return ll_unbox_fixnum(ll_call(ll_o(PASTE2(_ll_VEC,_length)), _1(x)));
  }
  
  return 0;
}


#undef _ll_VEC_MUTABLE
#undef _ll_VEC_SUPER
#undef _ll_VEC
#undef _ll_VEC_NAME
#undef __ll_VEC_NAME
#undef _ll_VEC_TERM
#undef _ll_VEC_TERM_VALUE
#ifdef _ll_VEC_COMPUTE_SIZE
#undef _ll_VEC_COMPUTE_SIZE
#endif
#undef _ll_VEC_BOX_VALUE
#undef _ll_VEC_UNBOX_VALUE
#undef _ll_VEC_ELEM_EQUAL
#undef _ll_VEC_ELEM_LOCATIVE
