#ifndef _ct_ct_h
#define _ct_ct_h

#ifndef __rcs_id__
#ifndef __rcs_id_ct_ct_h__
#define __rcs_id_ct_ct_h__
static const char __rcs_id_ct_ct_h[] = "$Id: ct.h,v 1.7 1999/05/07 12:38:34 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

/* $Id: ct.h,v 1.7 1999/05/07 12:38:34 stephensk Exp $ */

#include <stddef.h> /* size_t */

enum {
#ifndef ct_BITS_PER_CHAR
   ct_BITS_PER_CHAR = 8,
#endif
   ct_BITS_PER_INT = ct_BITS_PER_CHAR * sizeof(int),

   _ct_LAST
};

#define ct_NO_SIZE ((size_t) -1L)

/********************************************************************/

#define ct_OFFSETOF(T,X) ((size_t)&(((T *)0)->X))
#define ct_ALIGNOF(T) (ct_OFFSETOF(struct { char __pad[23]; T __X; double __pad2[2]; }, __X)-22)

  /*
#define alignof(T) ct_ALIGNOF(T)
  */

#ifndef ct_alignof
#ifdef __GNUC__
#define ct_alignof(T) __alignof(T)
#else
#error "Need a definition of ct_alignof(TYPE)"
#endif
#endif

#ifndef ct_ALIGNOF_PTR
#define ct_ALIGNOF_PTR sizeof(void*)
#endif

/* standard types */

typedef enum ct_t {
#include "ct/ct_t.def"
  ct_t_LAST
} ct_t;

/* type queries */


/* type constructors */

/* scoping */
void ct_t_set_scope_name(const char *name);
const char *ct_t_scope_name();
#if 0
void ct_t_push_scope_name();
void ct_t_pop_scope_name();
#endif

/* get a type by name */
ct_t ct_t_named(const char *name);

/* a pointer to t type */
ct_t ct_t_ptr(ct_t t);
#define _ct_t_ptr(X)((X)+ct_t_P)
#define ct_t_ptr(X)_ct_t_ptr(X)

/* a bitfield type */
ct_t ct_t_bitfield(ct_t t, int bits);
#define _ct_t_bitfield(T,B)(ct_t_BITFIELD+(B)*2+(((T)-ct_t_char)&1))
#define ct_t_bitfield(T,B)_ct_t_bitfield(T,B)

#define _ct_t_bitfield_type(T) ((((T)-ct_t_BITFIELD)&1)+ct_t_int)
#define ct_t_bitfield_type(T)_ct_t_bitfield_type(T)

#define _ct_t_bitfield_size(T) (((T)-ct_t_BITFIELD)/2)
#define ct_t_bitfield_size(T)_ct_t_bitfield_size(T)

/* an array of type t of length size */
ct_t ct_t_array(ct_t t, size_t size);

/* an enumeration */
ct_t ct_t_enumv(const char *name, int n, const char **names, const long *values);

/* a struct or union definition */
ct_t ct_t_struct(int unionQ, const char *name, ...);
ct_t ct_t_struct_endv(ct_t t, int n, const char **names, const ct_t *types);
ct_t ct_t_struct_end(ct_t t, ...);

/* a function type */
ct_t ct_t_funcv(ct_t rtn, int nargs, const ct_t *types);

/* a typedef */
ct_t ct_t_typedef(ct_t t, const char *name);

/* type info */
const char *ct_t_name(ct_t t);
const char *ct_t_scope(ct_t t);
const char *ct_t_declaration(ct_t t);
const char *ct_t_definition(ct_t t);

/* type size and alignment */
size_t   ct_t_sizeof(ct_t t);
size_t   ct_t_sizeof_bits(ct_t t);
size_t   ct_t_alignof(ct_t t);
size_t   ct_t_alignof_bits(ct_t t);

/* subelement access */
size_t  ct_t_n_elements(ct_t t);
int  ct_t_name_index(ct_t t, const char *name);
const char *ct_t_element_name(ct_t t, int i);
ct_t     ct_t_element_type(ct_t t, int i);
size_t   ct_t_element_offset(ct_t t, int i); /* in bits */

/* mem mgmt */

void *ct_malloc(size_t t);
void ct_free(void *ptr);
void *ct_realloc(void *ptr, size_t s);

/* user opaque data */

int ct_t_get_user_id();
void **ct_t_user(ct_t t);

/* Contexts */
void *ct_t_cntx();
void *ct_t_cntx_set(void *cntx);
void *ct_t_cntx_new();
void ct_t_cntx_free();

#endif
