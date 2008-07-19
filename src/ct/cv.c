
#include "cv.h"

#include "bitfield.h"

#include <stdlib.h>
#include <string.h> /* memcpy() */

const ct_v ct_v_ZERO;

int ct_v_eqQ(const ct_v *a, const ct_v *b)
{
#define X ((const int*) a)
#define Y ((const int*) b)

  switch ( sizeof(*a) / sizeof(int) ) {
  default:
    abort();
    break;
  case 4:
    if ( X[3] != Y[3] ) return 0;
  case 3:
    if ( X[2] != Y[2] ) return 0;
  case 2:
    if ( X[1] != Y[1] ) return 0;
  case 1:
    if ( X[0] != Y[0] ) return 0;    
  }

#undef X
#undef Y

  return 1;

}

int ct_v_coerce(ct_v *x, const ct_v *y)
{
  ct_t t = x->_t;

  if ( ct_t_BITFIELDQ(t) ) {
    t = ct_t_bitfield_type(t);
  }

  *x = ct_v_ZERO;
  x->_t = t;

  if ( x->_t == y->_t ) {
    *x = *y;
    return 0;
  }

  switch ( t ) {
  case ct_t_void:
    return 0;

  case ct_t_char:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._char = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_uchar:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._uchar = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_short:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._short = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_ushort:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._ushort = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_int:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._int = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_uint:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._uint = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_long:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._long = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_ulong:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._ulong = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_longlong:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._longlong = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_ulonglong:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._ulonglong = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_float:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._float = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_double:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._double = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_longdouble:
    switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: x->_u._longdouble = y->_u._##N; return 0;
#define CT_NO_VOID
#include "ct.def"
    default: break;
    }
    break;

  case ct_t_voidP:
    if ( ct_t_PTRQ(x->_t) || ct_t_ARRAYQ(x->_t) || ct_t_FUNCQ(x->_t) ) {
      x->_u._voidP = y->_u._voidP;
      return 0;
    } else {
      switch ( x->_t ) {
#define CT(N,T) case ct_t_##N: return x->_u._##N;
#define CT_NO_VOID
#include "ct.def"
      default: break;
      }
    }

  default: break;
  }
  
  return -1;
}

int ct_v_box(ct_v *dst, void *src)
{
  ct_t t = dst->_t;

  *dst = ct_v_ZERO;
  dst->_t = t;

  if ( ! src ) 
    return -1;

  switch ( t) {
  case ct_t_void: return 0;
#define CT_NO_VOID
#define CT(N,T) dst->_u._##N = *(T*) src; return 0;
#include "ct.def"
  case ct_t_voidP:
    dst->_u._voidP = *(void***) src; return 0;

  default: break;
  }

  if ( ct_t_PTRQ(t) ) {
    dst->_u._voidP = * (void**) src;
    return 0;
  } 
  else if ( ct_t_ARRAYQ(t) || ct_t_STRUCTQ(t) || ct_t_UNIONQ(t) ) {
    dst->_u._voidP = src;
    return 0;
  } 
  else if ( ct_t_FUNCQ(t) ) {
    dst->_t = ct_t_ptr(t);
    dst->_u._voidP = src;
    return 0;
  } else if ( ct_t_ENUMQ(t) ) {
    dst->_u._enum = * (ct_t*) src;
    return 0;
  }

  return -1;
}

int ct_v_unbox(void *dst, const ct_v *src)
{
  if ( ! dst ) 
    return -1;

  switch ( src->_t ) {
    case ct_t_void: return 0;
#define CT_NO_VOID
#define CT(N,T) *(T*) dst = src->_u._##N; return 0;
CT(voidP,void*)
#include "ct.def"
  default: break;
  }

  if ( ct_t_PTRQ(src->_t) ) {
    * (void**) dst = src->_u._voidP; 
  } else if ( ct_t_ARRAYQ(src->_t) || ct_t_STRUCTQ(src->_t) || ct_t_UNIONQ(src->_t) ) {
    memcpy(dst, src->_u._voidP, ct_t_sizeof(src->_t));
  } else if ( ct_t_FUNCQ(src->_t) ) {
    * (void**) dst = src->_u._voidP;
  } else if ( ct_t_ENUMQ(src->_t) ) {
    * (ct_t*) dst = src->_u._enum;;
  } else {
    return -1;
  }

  return 0;
}


/* result = src->i */
/* result = src[i] */
int ct_v_get_element(ct_v *result, const ct_v *src, int i)
{
  ct_t st = src->_t;
  char *p = src->_u._voidP;
  ct_t et = ct_t_element_type(st, i);
  size_t o = ct_t_element_offset(st, i);

  *result = ct_v_ZERO;

  if ( ct_t_STRUCTQ(st) || ct_t_UNIONQ(st) ) {
    if ( ct_t_BITFIELDQ(et) ) {
      size_t ii = o / (sizeof(int) * 8);
      int    io = o & (sizeof(int) * 8 - 1);

      result->_t = ct_t_bitfield_type(et);
      return (_ct_bf_getter[io][ct_t_bitfield_size(et)][result->_t - ct_t_int])(((int*) p) + ii, &result->_u._uint);
    }
    else if ( ct_t_STRUCTQ(et) || ct_t_UNIONQ(et) ) {
      result->_t = et;
      result->_u._voidP = p + (o / ct_BITS_PER_CHAR);
      return 0;
    } else {
      result->_t = et;
      return ct_v_box(result, p + (o / ct_BITS_PER_CHAR));
    }
  } else if ( ct_t_PTRQ(st) || ct_t_ARRAYQ(st) ) {
    result->_t = et;
    return ct_v_box(result, p + (o / ct_BITS_PER_CHAR));
  }

  return -1;
}

/* dst->i = value */
/* dst[i] = value */
int ct_v_set_element(ct_v *dst, int i, const ct_v *value)
{
  ct_t dt = dst->_t;
  ct_t et = ct_t_element_type(dt, i);
  size_t o = ct_t_element_offset(dt, i);
  char *p = dst->_u._voidP;
  ct_v t;

  /* coerce */
  t._t = et;
  if ( ct_v_coerce(&t, value) )
    return -1;

  value = &t;

  /* Clear value */
  *dst = ct_v_ZERO;
  dst->_t = et;

  if ( ct_t_STRUCTQ(dt) || ct_t_UNIONQ(dt) ) {
    if ( ct_t_BITFIELDQ(et) ) {
      size_t ii = o / (sizeof(int) * 8);
      int    io = o & (sizeof(int) * 8 - 1);
      ct_t bt = ct_t_bitfield_type(et);

      return (_ct_bf_setter[io][ct_t_bitfield_size(et)][bt - ct_t_int])(((int*) p) + ii, &value->_u._uint);
    }
    else if ( ct_t_STRUCTQ(et) || ct_t_UNIONQ(et) ) {
      memcpy(p + (o / ct_BITS_PER_CHAR), value->_u._voidP, ct_t_sizeof(et));
      return 0;
    } else {
      return ct_v_unbox(p + (o / ct_BITS_PER_CHAR), value); 
    }
  } else if ( ct_t_PTRQ(dt) || ct_t_ARRAYQ(dt) ) {
    return ct_v_unbox(p + (o / ct_BITS_PER_CHAR), value);
  }

  return 0;
}


