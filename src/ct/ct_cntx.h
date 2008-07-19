#ifndef _ct_ct_cntx_h
#define _ct_ct_cntx_h

#include "ct.h"

#if 0
#include "hash/charP_int_Table.h"
#include "hash/int_voidP_Table.h"
#endif

struct ct_tdef;
typedef struct ct_tdef ct_tdef;

struct ct_cntx {
  int inited;

  /* Counters for new type definitions */
  ct_t struct_id;
  ct_t union_id;
  ct_t enum_id;
  ct_t array_id;
  ct_t func_id;

  /* The current named lookup scope */
  char *scope;

  /* This table maps ct_t_name() to ct_t */
  void *name_2_ct_t;
  /* charP_int_Table *name_2_ct_t; */

  /* This table maps ct_t to ct_tdef* */
  void *ct_t_2_ct_tdef;
  /* int_voidP_Table *ct_t_2_ct_tdef; */

  /* This table caches ct_t to ct_tdef* */
  struct ct_t_tdef_cache {
    ct_t t;
    ct_tdef *d;
  } cache[101];
};

typedef struct ct_cntx ct_cntx;

extern ct_cntx *_ct_cntx;

#define _ct_t_struct_id  _ct_cntx->struct_id
#define _ct_t_union_id   _ct_cntx->union_id
#define _ct_t_enum_id    _ct_cntx->enum_id
#define _ct_t_array_id   _ct_cntx->array_id
#define _ct_t_func_id    _ct_cntx->func_id
#define _ct_t_scope      _ct_cntx->scope

#endif
