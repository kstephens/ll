#ifndef __rcs_id__
#ifndef __rcs_id_ct_ct_c__
#define __rcs_id_ct_ct_c__
static const char __rcs_id_ct_ct_c[] = "$Id: ct.c,v 1.9 1999/10/13 17:13:06 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

#include "ct_cntx.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include "util/enum.h"
#include "util/ssprintf.h"

#include "hash/charP_int_Table.h"
#include "hash/int_voidP_Table.h"

#ifndef ct_DEBUG
#define ct_DEBUG 0
#endif

/********************************************************************/

/* static */
ct_cntx __ct_cntx, *_ct_cntx = &__ct_cntx;

void *ct_t_cntx()
{
  return _ct_cntx;
}

void *ct_t_cntx_set(void *cntx)
{
  void *t = _ct_cntx;
  _ct_cntx = cntx;
  return t;
}

void *ct_t_cntx_new()
{
  ct_cntx *c;

  c = ct_malloc(sizeof(*c));
  memset(c, 0, sizeof(*c));
  return ct_t_cntx_set(c);
}

void ct_t_cntx_free()
{
  ct_cntx *c = _ct_cntx;
  ct_free(c);
}


/********************************************************************/

C_enum C_enum_ct_t[] = {
#define ct_t_D(N) { "ct_t_" #N, ct_t_##N },
#define ct_t_DV(N,V) { "ct_t_" #N, ct_t_##N },
#include "ct/ct_t.def"
  { 0, 0 }
};

/********************************************************************/

static void _ct_init();

struct ct_tdef {
  ct_t _t;

  char *_name;
  char *_decl;
  char *_def;
  char *_scope;

  size_t _sizeof;
  size_t _sizeof_bits;
  size_t _alignof;
  size_t _alignof_bits;

  size_t _nelements;
  char **_names;
  long *_elements;
  size_t *_offsets; /* in bits */

  ct_t _subt; /* for ptr/array; func uses it as the return type */

  int_voidP_Table *_array; /* maps array size to a ct_tdef */

  int _completed; /* was ct_t_struct_end called? */

#ifndef ct_N_USER
#define ct_N_USER 4
#endif
  void *_user[ct_N_USER];

#if 0 /* for testing */
  void *_dummy[7][23];
#endif
};


static
int ct_tdef_equal(const ct_tdef *a, const ct_tdef *b)
{
  size_t i;

  /* struct/union */
  if ( (ct_t_STRUCTQ(a->_t) && ct_t_STRUCTQ(b->_t)) || (ct_t_UNIONQ(a->_t) && ct_t_UNIONQ(b->_t))) {
    if ( a->_nelements != b->_nelements )
      return 0;
    for ( i = 0; i < a->_nelements; i ++ ) {
      if ( a->_elements[i] != b->_elements[i] )
	return 0;
      if ( strcmp(a->_names[i], b->_names[i]) )
	return 0;
    }

    return ! 0;
    /* enum */
  } else if ( ct_t_ENUMQ(a->_t) && ct_t_ENUMQ(b->_t) ) {
    if ( a->_nelements != b->_nelements )
      return 0;
    for ( i = 0; i < a->_nelements; i ++ ) {
      if ( a->_elements[i] != b->_elements[i] )
	return 0;
      if ( strcmp(a->_names[i], b->_names[i]) )
	return 0;
    }
    
    return ! 0;
    /* func */
  } else if ( ct_t_FUNCQ(a->_t) && ct_t_FUNCQ(b->_t) ) {
    if ( a->_nelements != b->_nelements )
      return 0;

    for ( i = 0; i < a->_nelements; i ++ ) {
      if ( a->_elements[i] != b->_elements[i] )
	return 0;
    }
    
    return ! 0;
    
    /* array */
  } else if ( ct_t_ARRAYQ(a->_t) && ct_t_ARRAYQ(b->_t) ) {
    return a->_nelements == b->_nelements && a->_subt == b->_subt;
  }

  return a->_t == b->_t;
}

/********************************************************************
 * Memory Mgmt
 */

void *(*_ct_malloc)(size_t) = malloc;
void (*_ct_free)(void*) = free;
void *(*_ct_realloc)(void*,size_t) = realloc;

void *ct_malloc(size_t s)
{
  return (*_ct_malloc)(s);
}
void ct_free(void *p)
{
  (*_ct_free)(p);
}
void *ct_realloc(void *p, size_t s)
{
  return (*_ct_realloc)(p, s);
}

#define strunique(X) strcpy(ct_malloc(strlen(X)+1), (X))


/********************************************************************/

void ct_error_default(const char *msg)
{
  fprintf(stderr, "ct: %s\n", msg);
  abort();
}

void (*_ct_error)(const char *msg) = ct_error_default;

void ct_error(const char *format, ...)
{
  char buf[1024];

  va_list vap;

  va_start(vap,format);
  vsprintf(buf, format, vap);
  va_end(vap);

  (*_ct_error)(buf);
}

/********************************************************************/

static
ct_t ct_t_named_1(char *name)
{
  int *tp;
  char *c;

  if ( (c = strrchr(name, '*')) ) {
    ct_t ptr = ct_t_P;

    *c = '\0';
    c --;

    /* remove spaces before '*' and total up ptr depth */
    while ( c >= name ) {
      if ( isspace(*c) ) {
	*c = '\0';
	c --;
      } else if ( *c == '*' ) {
	*c = '\0';
	c --;
	ptr += ct_t_P;
      } else {
	break;
      }
    }
    return ct_t_named_1(name) + ptr;
  }

  /* Try as asked for */
  if ( (tp = charP_int_TableGet(_ct_cntx->name_2_ct_t, name)) ) {
    return *tp;
  }

  /* Try with scope */
  if ( _ct_t_scope &&_ct_t_scope[0] ) {
    char buf[256];

    strcpy(buf, _ct_t_scope);
    strcat(buf, name);
    if ( (tp = charP_int_TableGet(_ct_cntx->name_2_ct_t, name)) ) {
      return *tp;
    }
  }

#if 0
  /* Try without scope */
  {

    if ( (c = strrchr(name, ':')) ) {
      if ( (tp = charP_int_TableGet(_ct_cntx->name_2_ct_t, name)) ) {
	return *tp;
      }
    }
  }
#endif

  return 0;
}

ct_t ct_t_named(const char *name)
{
   char *c;
    
   if ( (c = strrchr(name, '*')) ) {
     char buf[256];
     strcpy(buf, name);
     return ct_t_named_1(buf);
   }
   return ct_t_named_1((char *) name);
}

ct_t ct_t_typedef(ct_t t, const char *name)
{
  char buf[256];

  assert(t);
  _ct_init();

  if ( _ct_t_scope && _ct_t_scope[0] ) {
    strcpy(buf, _ct_t_scope);
    strcat(buf, name);
    name = buf;
  }

  if ( charP_int_TableGet(_ct_cntx->name_2_ct_t, name) ) {
    ct_error("ct_t_typedef: '%s' already named", name);
    return t;
  }

  charP_int_TableAdd(_ct_cntx->name_2_ct_t, name, t);

  return t;
}

static
void _ct_tdef_install(ct_tdef *d)
{
  assert(d->_t);

#if 1
  assert ( int_voidP_TableGet(_ct_cntx->ct_t_2_ct_tdef, d->_t) == 0 );
#endif

  if ( d->_sizeof_bits != ct_NO_SIZE ) {
    d->_completed = 1;
  }

  int_voidP_TableAdd(_ct_cntx->ct_t_2_ct_tdef, d->_t, d);
  if ( d->_name && d->_name[0] ) {
    charP_int_TableAdd(_ct_cntx->name_2_ct_t, d->_name, d->_t);
  } 
}

static
ct_tdef *_ct_tdef_setName(ct_tdef *d, const char *prename, const char *name)
{
  if ( name && name[0] ) {
    d->_name = ct_malloc(strlen(prename) + strlen(name) + 1);
    strcpy(d->_name, prename);
    strcat(d->_name, name);
  } else {
    d->_name = ct_malloc(1);
    d->_name[0] = 0;
  }

  d->_decl = ct_malloc(strlen(d->_name) + strlen(" %s") + 1);
  strcpy(d->_decl, d->_name);
  strcat(d->_decl, " %s");

  return d;
}

static
ct_tdef *ct_tdef_new()
{
  ct_tdef *d = ct_malloc(sizeof(*d));

  memset(d, 0, sizeof(*d));

  d->_sizeof = d->_sizeof_bits =
  d->_alignof = d->_alignof_bits = ct_NO_SIZE;

  return d;
}


static
ct_tdef *ct_t_def(ct_t t)
{
  struct ct_t_tdef_cache *cp;
  ct_tdef *d;
  assert(t);

  /* Use a lookup cache */
  /* This really speeds things up!  */
  cp = _ct_cntx->cache + (((unsigned int) t) % (sizeof(_ct_cntx->cache) / sizeof(_ct_cntx->cache[0]))); 
  if ( cp->t == t && cp->d ) {
    d = cp->d;
  } else {
    ct_tdef **dp;
    
    _ct_init();
    
    dp = (ct_tdef**) int_voidP_TableGet(_ct_cntx->ct_t_2_ct_tdef, t);
    d = dp ? *dp : 0;
    
    cp->t = t;
    cp->d = d;
  }

  /* Create ptr ct_t_def only if needed */
  if ( ! d && ct_t_PTRQ(t) ) {
    ct_t subt = ct_t_PTR_TO(t);
    const char *decl;

    d = ct_tdef_new();
    d->_t = t;
    d->_subt = subt;
    decl = ct_t_declaration(subt);
    d->_name = ssprintf(0, decl, "*");
    d->_decl = ssprintf(0, decl, "*%s");
    d->_sizeof_bits = (d->_sizeof = sizeof(void*)) * ct_BITS_PER_CHAR;
    d->_alignof_bits = (d->_alignof = ct_ALIGNOF_PTR) * ct_BITS_PER_CHAR;

    _ct_tdef_install(d);

    cp->t = t;
    cp->d = d;
  }

  return d;
}


/**********************************************************************/

static void _ct_init()
{
  ct_tdef *t;
  int i, j;

  if ( _ct_cntx->inited )
    return;
  
  _ct_t_struct_id = ct_t_STRUCT;
  _ct_t_union_id = ct_t_UNION;
  _ct_t_enum_id = ct_t_ENUM;
  _ct_t_array_id = ct_t_ARRAY;
  _ct_t_func_id = ct_t_FUNC;

  assert(ct_t_P > ct_t_ARRAY_END);

  _ct_cntx->inited ++;
  
  _ct_cntx->name_2_ct_t = ct_malloc(sizeof(charP_int_Table));
  charP_int_TableInit(_ct_cntx->name_2_ct_t, 64);

  _ct_cntx->ct_t_2_ct_tdef = ct_malloc(sizeof(int_voidP_Table));
  int_voidP_TableInit(_ct_cntx->ct_t_2_ct_tdef, 64);

  /***********************************************************************/
  /* intrinsics */

#define CT(N,T) \
  t = ct_malloc(sizeof(*t)); \
  memset(t, 0, sizeof(*t)); \
  t->_t = ct_t_##N; \
  t->_sizeof_bits = (t->_sizeof_bits = 0); \
  t->_alignof_bits = (t->_alignof_bits = 0); \
  t->_name = (char*) #T; \
  t->_decl = (char*) #T " %s"; \
  t->_def = ""; \
  _ct_tdef_install(t);
  
#include "ct.def"

  /* intrinsic sizeof/alignof */

#define CT(N,T) \
  t = ct_t_def(ct_t_##N); \
  t->_sizeof_bits = (t->_sizeof = sizeof(T)) * ct_BITS_PER_CHAR; \
  t->_alignof_bits = (t->_alignof = ct_alignof(T)) * ct_BITS_PER_CHAR;

#define CT_NO_VOID
#include "ct.def"

  /***********************************************************************/
  /* ellipsis */
									
  t = ct_malloc(sizeof(*t));
  memset(t, 0, sizeof(*t));
  t->_t = ct_t_ELLIPSIS;
  t->_sizeof_bits = (t->_sizeof_bits = 0);
  t->_alignof_bits = (t->_alignof_bits = 0);
  t->_name = t->_decl = (char*) "...";
  t->_def = "";
  _ct_tdef_install(t);
 	      			       
  /***********************************************************************/
  /* ptrs */

#define CT(N,T) \
  t = ct_malloc(sizeof(*t)); \
  memset(t, 0, sizeof(*t)); \
  t->_t = ct_t_##N##P; \
  t->_subt = ct_t_##N; \
  t->_sizeof_bits = (t->_sizeof = sizeof(void*)) * ct_BITS_PER_CHAR; \
  t->_alignof_bits = (t->_alignof = ct_ALIGNOF_PTR) * ct_BITS_PER_CHAR; \
  t->_name = (char*) #T " *"; \
  t->_decl = (char*) #T " *%s"; \
  t->_def = ""; \
  _ct_tdef_install(t);
#include "ct.def"

  /***********************************************************************/
  /* ptr-to-ptrs */

#define CT(N,T) \
  t = ct_malloc(sizeof(*t)); \
  memset(t, 0, sizeof(*t)); \
  t->_t = ct_t_##N##PP; \
  t->_subt = ct_t_##N##P; \
  t->_sizeof_bits = (t->_sizeof = sizeof(void**)) * ct_BITS_PER_CHAR; \
  t->_alignof_bits = (t->_alignof = ct_ALIGNOF_PTR) * ct_BITS_PER_CHAR; \
  t->_name = (char*) #T " **"; \
  t->_decl = (char*) #T " **%s"; \
  t->_def = ""; \
  _ct_tdef_install(t);
#include "ct.def"

  /***********************************************************************/
  /* ptr-to-ptr-to-ptr */

#define CT(N,T) \
  t = ct_malloc(sizeof(*t)); \
  memset(t, 0, sizeof(*t)); \
  t->_t = ct_t_##N##PPP; \
  t->_subt = ct_t_##N##PP; \
  t->_sizeof_bits = (t->_sizeof = sizeof(void***)) * ct_BITS_PER_CHAR; \
  t->_alignof_bits = (t->_alignof = ct_ALIGNOF_PTR) * ct_BITS_PER_CHAR; \
  t->_name = (char*) #T " ***"; \
  t->_decl = (char*) #T " ***%s"; \
  t->_def = ""; \
  _ct_tdef_install(t);
#include "ct.def"

  /***********************************************************************/
  /* Bitfields */

  /* Even ct_t_BITFIELDs are signed */
  /* Odd ct_t_BITFIELDs are unsigned */

  for ( i = ct_t_BITFIELD; i < ct_t_BITFIELD_END; i ++ ) {
    int k = (i - ct_t_BITFIELD) & 1;
    j = (i - ct_t_BITFIELD) / 2;
    
    t = ct_malloc(sizeof(*t));
    memset(t, 0, sizeof(*t));
    t->_t = i;
    t->_subt = k ? ct_t_uint : ct_t_int;
    t->_sizeof = ( t->_sizeof_bits = j ) / ct_BITS_PER_CHAR;
    t->_alignof = ( t->_alignof_bits = 1 ) / ct_BITS_PER_CHAR;
    t->_decl = ssprintf(t->_decl, "%s %%s:%d", k ? "unsigned" : "signed", j);
    t->_name = ssprintf(t->_name, t->_decl, "");
    t->_def = "";
    _ct_tdef_install(t);
  }

  /***********************************************************************/
  /* Common aliases */

  ct_t_typedef(ct_t_char, "signed char");
  ct_t_typedef(ct_t_char, "char signed");

  ct_t_typedef(ct_t_short, "short int");
  ct_t_typedef(ct_t_short, "signed short");
  ct_t_typedef(ct_t_short, "signed short int");
  ct_t_typedef(ct_t_short, "short signed int");
  ct_t_typedef(ct_t_short, "short signed");

  ct_t_typedef(ct_t_ushort, "unsigned short int");
  ct_t_typedef(ct_t_ushort, "short unsigned int");
  ct_t_typedef(ct_t_ushort, "short unsigned");

  ct_t_typedef(ct_t_int, "signed int");
  ct_t_typedef(ct_t_int, "int signed");

  ct_t_typedef(ct_t_long, "long int");
  ct_t_typedef(ct_t_long, "signed long");
  ct_t_typedef(ct_t_long, "signed long int");

  ct_t_typedef(ct_t_ulong, "unsigned long int");
  ct_t_typedef(ct_t_ulong, "long unsigned int");
  ct_t_typedef(ct_t_ulong, "long unsigned");

  ct_t_typedef(ct_t_longlong, "long long int");
  ct_t_typedef(ct_t_longlong, "signed long long");
  ct_t_typedef(ct_t_longlong, "signed long long int");
  ct_t_typedef(ct_t_longlong, "long signed long int");
  ct_t_typedef(ct_t_longlong, "long long signed int");

  ct_t_typedef(ct_t_ulonglong, "unsigned long long int");
  ct_t_typedef(ct_t_ulonglong, "long unsigned long int");
  ct_t_typedef(ct_t_ulonglong, "long unsigned long");
  ct_t_typedef(ct_t_ulonglong, "long long unsigned int");
  ct_t_typedef(ct_t_ulonglong, "long long unsigned");

#ifdef CT_NO_LDOUBLE
#else
  ct_t_typedef(ct_t_longdouble, "double long");
#endif
}

static
ct_tdef *ct_tdef_setElements(ct_tdef *d, int nelements, const long *elements, const char **names)
{
  int i;

  d->_nelements = nelements;
  if ( d->_nelements ) {
    if ( names ) {
      d->_names = ct_malloc(sizeof(d->_names[0]) * d->_nelements);
      memcpy(d->_names, names, sizeof(d->_names[0]) * d->_nelements);
      for ( i = 0; i < d->_nelements; i ++ ) {
	d->_names[i] = strunique(d->_names[i]);
	/* fprintf(stderr, "%p %d '%s'\n", d, i, names[i]); */
      }
    } else {
      d->_names = 0;
    }
   
    if ( elements ) {
      d->_elements = ct_malloc(sizeof(d->_elements[0]) * d->_nelements);
      memcpy(d->_elements, elements, sizeof(d->_elements[0]) * d->_nelements);
    } else {
      d->_elements = 0;
    }
  } else {
    d->_names = 0;
    d->_elements = 0;
  }

  assert(d->_completed == 0);
  d->_completed ++;

  return d;
}

typedef struct ct_va_list {
  va_list v;
} ct_va_list;

static
ct_t ct_t_struct_endvap(ct_t t, ct_va_list _vap)
{
  int nelements = 0;
  ct_t *types;
  char **names;
  ct_va_list vap;

  vap = _vap;
  while ( va_arg(vap.v, ct_t) ) {
    (void) va_arg(vap.v, const char *);
    nelements ++;
  }

  if ( nelements ) {
    types = alloca(sizeof(types[0]) * nelements);
    names = alloca(sizeof(names[0]) * nelements);
    
    {
      int i = 0;
      
      vap = _vap;
      while ( i < nelements ) {
	types[i] = va_arg(vap.v, ct_t);
	names[i] = va_arg(vap.v, char*);
	i ++;
      }
      
      t = ct_t_struct_endv(t, nelements, (const char **) names, types);
    }
  }

  return t;
}

ct_t ct_t_struct(int unionQ, const char *name, ...)
{
  ct_tdef *d;
  ct_t t;

  if ( unionQ == ct_t_STRUCT ) {
    unionQ = 0;
  }

  t = unionQ ? ++ _ct_t_union_id : ++ _ct_t_struct_id;
  if ( unionQ ) {
    assert(ct_t_UNIONQ(t));
  } else {
    assert(ct_t_STRUCTQ(t));
  }
  d = ct_tdef_new();
  d->_t = t;

  _ct_tdef_setName(d, unionQ ? "union " : "struct ", name);

  _ct_tdef_install(d);
  
  /* try va list */
  {
    ct_va_list vap;

    va_start(vap.v, name);
    t = ct_t_struct_endvap(t, vap);
    va_end(vap.v);
  }

  return t;
}

ct_t ct_t_struct_endv(ct_t t, int nelements, const char **names, const ct_t *types )
{
  ct_tdef *d;

  assert(ct_t_STRUCTQ(t) || ct_t_UNIONQ(t));
  d = ct_t_def(t);
  
  assert(sizeof(types[0]) == sizeof(d->_elements[0]));
  ct_tdef_setElements(d, nelements, (const long *) types, names);

  return t;
}

ct_t ct_t_struct_env(ct_t t, ...)
{
  ct_va_list vap;
  ct_tdef *d;

  assert(ct_t_STRUCTQ(t) || ct_t_UNIONQ(t));
  d = ct_t_def(t);
  
  va_start(vap.v, t);
  t = ct_t_struct_endvap(t, vap);
  va_end(vap.v);

  assert(d->_completed == 0);
  d->_completed ++;

  return t;
}

ct_t ct_t_enumv(const char *name, int nelements, const char **names, const long *values)
{
  ct_tdef *d;
  ct_t t;

  t = ++ _ct_t_enum_id;
  assert(ct_t_ENUMQ(t));

  d = ct_tdef_new();
  d->_t = t;

  _ct_tdef_setName(d, "enum ", name);

  assert(sizeof(values[0]) == sizeof(d->_elements[0]));
  ct_tdef_setElements(d, nelements, values, names);

  _ct_tdef_install(d);

  return t;
}

ct_t ct_t_enum(const char *name, ...)
{
  int nelements;
  const char **names;
  long *values;
  va_list vap;
  const char *x;

  nelements = 0;
  va_start(vap, name);
  while ( va_arg(vap, const char *) ) {
    (void) va_arg(vap, long);
    nelements ++;
  }
  va_end(vap);

  names = alloca(sizeof(names[0]) * nelements);
  values = alloca(sizeof(values[0]) * nelements);

  nelements = 0;
  va_start(vap, name);
  while ( (x = va_arg(vap, const char *)) ) {
    names[nelements] = x;
    values[nelements ++] = va_arg(vap, long);
  }
  va_end(vap);

  return ct_t_enumv(name, nelements, names, values);
}

ct_t ct_t_array(ct_t et, size_t length)
{
  ct_tdef *ed, *d;
  ct_t t;

  assert(et);
  t = ++ _ct_t_array_id;
  assert(ct_t_ARRAYQ(t));

  ed = ct_t_def(et);

  /* Look for an existing array type of t in t's def */
  if ( ed->_array ) {
    ct_tdef **ad;

    if ( (ad = (void*) int_voidP_TableGet(ed->_array, length)) ) {
      return (*ad)->_t;
    }
  } else {
    ed->_array = ct_malloc(sizeof(*ed->_array));
    int_voidP_TableInit(ed->_array, 2);
  }

  d = ct_tdef_new();
  d->_t = t;
  d->_subt = et;
  d->_nelements = length;
  d->_completed = 1;

  {
    const char *t_decl = ct_t_declaration(et);
    char buf[32];
    const char *di;

    if ( length == ct_NO_SIZE ) {
      di = "%s[]";
    } else {
      sprintf(buf, "%%s[%lu]", (unsigned long) length);
      di = buf;
    }
    d->_decl = ssprintf(0, t_decl, di);
    d->_name = ssprintf(0, d->_decl, "");
  }
  
  _ct_tdef_install(d);

  /* Remember the new t[] type in t's def */
  int_voidP_TablePush(ed->_array, length, d);
  
  return t;
}

ct_t ct_t_funcv(ct_t rtn, int nargs, const ct_t *types)
{
  ct_tdef *d;
  ct_t t;

  assert(rtn);
  t = ++ _ct_t_func_id;
  assert(ct_t_FUNCQ(t));

  d = ct_tdef_new();

  d->_t = t;
  d->_subt = rtn;
  d->_nelements = nargs;

  ct_tdef_setElements(d, nargs, (long*) types, 0);

  {
    const char *t_decl = ct_t_declaration(rtn);
    char *di = 0;
    int i;

    di = ssprintf(di, "(%%s)(");
    for ( i = 0; i < d->_nelements; ) {
      di = ssprintf(di, "%s", ct_t_name(d->_elements[i]));
      i ++;
      if ( i < d->_nelements ) {
	di = ssprintf(di, ",");
      }
    }
    di = ssprintf(di, ")");

    d->_decl = ssprintf(0, t_decl, di);
    d->_name = ssprintf(0, d->_decl, "");

    ct_free(di);
  }

  _ct_tdef_install(d);

  return t;
}


/************************************************************************/

size_t ct_t_sizeof_bits(ct_t t)
{
  ct_tdef *d;
  
  if ( ct_t_PTRQ(t) )
    return sizeof(void*) * ct_BITS_PER_CHAR;
  
  if ( ct_t_ENUMQ(t) )
    return sizeof(ct_t) * ct_BITS_PER_CHAR;
  
  if ( ct_t_BITFIELDQ(t) )
    return (t - ct_t_BITFIELD) / 2;

  d = ct_t_def(t);
  
  /* Computing sizeof */
  if ( d->_sizeof_bits == ct_NO_SIZE ) {
    int i;
    int bitfields = 0;

    if ( ct_t_STRUCTQ(t) ) {
      d->_offsets = ct_malloc(sizeof(d->_offsets[0]) * d->_nelements);

      if ( ! d->_completed )
	return 0;

      d->_sizeof_bits = 0;
        
      if ( ct_DEBUG ) {
	fprintf(stderr, "ct_t_sizeof_bits(): struct %s {\n", d->_name);
      }

      for ( i = 0; i < d->_nelements; i ++ ) {
#define ALIGNN(X,Ea) { if ( Ea && X % Ea ) X += Ea - (X % Ea); }
#define ALIGN(X,E) { size_t Ea = ct_t_alignof_bits(E); ALIGNN(X,Ea); }

	if ( ct_t_INTRINSICQ(d->_elements[i]) && d->_elements[i] >= ct_t_int ) {
	  ALIGNN(d->_sizeof_bits, ct_t_sizeof_bits(ct_t_int));
	} else {
	  ALIGN(d->_sizeof_bits, d->_elements[i]);
	}

	d->_offsets[i] = d->_sizeof_bits;

	if ( d->_offsets[i] & 7 ) {
	  bitfields ++;
	}

	if ( ct_DEBUG ) {
	  fprintf(stderr, "  ");
	  fprintf(stderr, ct_t_declaration(d->_elements[i]), d->_names[i]);
	  fprintf(stderr, "; /* offset %lu */\n", (unsigned long) d->_offsets[i]);
	}

	d->_sizeof_bits += ct_t_sizeof_bits(d->_elements[i]);
      }

      /* If we have bitfields make sure its at least as wide as an int */
      if ( bitfields ) {
	ALIGNN(d->_sizeof_bits, ct_t_sizeof_bits(ct_t_int));
      }

      /* Make sure first element is still in alignment. */
#ifdef ct_ALIGN_FIRST_STRUCT_ELEMENT
      if ( d->_nelements ) {
	ALIGN(d->_sizeof_bits, d->_elements[0]);
      }
#endif

#undef ALIGN
#undef ALIGNN

      if ( ct_DEBUG ) {
	fprintf(stderr, "}\n\n");
      }


    } else
    if ( ct_t_UNIONQ(t) ) {
      int i;
      size_t s;
      
      if ( ! d->_completed )
	return 0;

      d->_sizeof_bits = 0;
    
      for ( i = 0; i < d->_nelements; i ++ ) {
        s = ct_t_sizeof_bits((ct_t) d->_elements[i]);
        if ( d->_sizeof_bits < s )
	  d->_sizeof_bits = s;
      }

      if ( d->_sizeof_bits ) {
	/* Must be at least as wide as an int  */
	s = ct_t_sizeof_bits(ct_t_int);
	if ( d->_sizeof_bits < s )
	  d->_sizeof_bits = s;

	/* Must align for array */
	s = ct_t_alignof_bits(t);
	if ( d->_sizeof_bits < s )
	  d->_sizeof_bits = s;
      }
    
    } else
    if ( ct_t_ARRAYQ(t) ) {
      if ( d->_nelements == ct_NO_SIZE ) {
	d->_sizeof_bits = 0;
      } else {
	d->_sizeof_bits = ct_t_sizeof_bits(d->_subt) * d->_nelements;
      }
    } else
    if ( ct_t_FUNCQ(t) ) {
      for ( i = 0; i < d->_nelements; i ++ ) {
	if ( d->_elements[i] != ct_t_ELLIPSIS ) {
	  size_t s = ct_t_sizeof_bits((ct_t) d->_elements[i]);
	  
	  /* int argument promotion */
	  if ( s < sizeof(int) * ct_BITS_PER_CHAR )
	    s = sizeof(int) * ct_BITS_PER_CHAR;
	  
	  d->_sizeof_bits += s;
	}
      }
    }
    else if ( ct_t_ENUMQ(t) ) {
      d->_sizeof_bits = sizeof(enum ct_t);
    } else {
      ct_error("ct_struct_end expected struct or union %s\n", d->_name);
    }


    if ( ct_DEBUG ) {
      fprintf(stderr, "ct_t_sizeof_bits(%s /* %s */) = %lu\n", C_enum_value_to_str(C_enum_ct_t, t), d->_name, (unsigned long) d->_sizeof_bits);
    }

  }

  return(d->_sizeof_bits);
}

size_t ct_t_sizeof(ct_t t)
{
  ct_tdef *d;
  
  if ( ct_t_PTRQ(t) )
    return sizeof(void*);

  if ( ct_t_ENUMQ(t) )
    return sizeof(ct_t);

  if ( ct_t_BITFIELDQ(t) )
    return ((t - ct_t_BITFIELD) / 2) / ct_BITS_PER_CHAR;

  d = ct_t_def(t);

  if ( ! d->_completed )
    return 0;

    /* Computing sizeof */
  if ( d->_sizeof == ct_NO_SIZE ) {
    if ( d->_sizeof_bits == ct_NO_SIZE ) {
      ct_t_sizeof_bits(t);
    }
    d->_sizeof = d->_sizeof_bits / ct_BITS_PER_CHAR;
  }

  return d->_sizeof;
}

size_t ct_t_alignof_bits(ct_t t)
{
  ct_tdef *d;

  if ( ct_t_PTRQ(t) )
    return ct_ALIGNOF_PTR * ct_BITS_PER_CHAR;

  if ( ct_t_ENUMQ(t) )
    return ct_ALIGNOF(ct_t) * ct_BITS_PER_CHAR;

  if ( ct_t_BITFIELDQ(t) )
    return 1;

  d = ct_t_def(t);

  if ( d->_alignof_bits == ct_NO_SIZE ) {
    if ( ct_t_STRUCTQ(t) ) {
      if ( ! d->_completed )
	return 1;

      d->_alignof_bits = d->_nelements ? ct_t_alignof_bits(d->_elements[0]) : 0;

    } else if ( ct_t_UNIONQ(t) ) {
      int i;
      
      if ( ! d->_completed )
	return 1;

      d->_alignof_bits = 0;
      
      for ( i = 0; i < d->_nelements; i ++ ) {
	size_t a = ct_t_alignof_bits(d->_elements[i]);
	if ( d->_alignof_bits < a ) {
	  d->_alignof_bits = a;
	}
      }
    } else if ( ct_t_FUNCQ(t) ) {
      /* what does the alignment mean for a function type? */
      d->_alignof_bits = ct_alignof(int) * ct_BITS_PER_CHAR;
    } else if ( ct_t_ARRAYQ(t) ) {
      d->_alignof_bits = ct_t_alignof_bits(d->_subt);
    }
    else if ( ct_t_ENUMQ(t) ) {
      d->_alignof_bits = ct_ALIGNOF(enum ct_t);
    } else {
      ct_error("ct_alignof expected struct or union\n");
    }

    if ( ct_DEBUG ) {
      fprintf(stderr, "ct_t_alignof_bits(%s) = %lu\n", d->_name, (unsigned long) d->_alignof_bits);
    }
  }


  return(d->_alignof_bits);
}

size_t ct_t_alignof(ct_t t)
{
  ct_tdef *d;

  if ( ct_t_PTRQ(t) )
    return ct_ALIGNOF_PTR;

  if ( ct_t_ENUMQ(t) )
    return ct_ALIGNOF(ct_t);

  if ( ct_t_BITFIELDQ(t) )
    return 0;

  d = ct_t_def(t);

  if ( ! d->_completed )
    return 0;

  if ( d->_alignof == ct_NO_SIZE ) {
    if ( d->_alignof_bits == ct_NO_SIZE ) {
      ct_t_alignof_bits(t);
    }
    d->_alignof = d->_alignof_bits / ct_BITS_PER_CHAR;
  }

  return d->_alignof;
}

size_t ct_t_n_elements(ct_t t)
{
#define ct_t_HAS_ELEMENTSQ(t) (ct_t_STRUCT_OR_UNIONQ(t) || ct_t_ARRAYQ(t) || ct_t_FUNCQ(t))

  if ( ct_t_HAS_ELEMENTSQ(t) ) {
    ct_tdef *d = ct_t_def(t);
    return d ? d->_nelements : 0;
  }

  return 0;
}

int ct_t_name_index(ct_t t, const char *name)
{
  if ( ct_t_HAS_ELEMENTSQ(t) ) {
    ct_tdef *d = ct_t_def(t);

    if ( d && d->_names ) {
      int i;

      for ( i = 0; i < d->_nelements; i ++ ) {
	if ( d->_names[i] && d->_names[i][0] == name[0] && strcmp(d->_names[i], name) == 0 ) {
	  return i;
	}
      }
    }
  }
  return -1;
}

const char * ct_t_element_name(ct_t t, int i)
{
  if ( ct_t_HAS_ELEMENTSQ(t) ) {
    ct_tdef *d = ct_t_def(t);

    return d && d->_names ? d->_names[i] : 0;
  }
  return 0;
}

size_t ct_t_element_offset(ct_t t, int i)
{
  if ( ct_t_PTRQ(t) ) {
    return ct_t_sizeof_bits(ct_t_PTR_TO(t)) * i;
  }
  else if ( ct_t_UNIONQ(t) ) {
      return 0;
  }
  else if (  ct_t_HAS_ELEMENTSQ(t) ) {
    ct_tdef *d;
    
    d = ct_t_def(t);
    if ( ct_t_ARRAYQ(t) ) {
      return ct_t_sizeof_bits(d->_subt) * i;
    }

    /* Force _offsets computation */
    ct_t_sizeof_bits(t);
    
    return d && d->_offsets ? d->_offsets[i] : ct_NO_SIZE;
  }

  return ct_NO_SIZE;
}

ct_t ct_t_element_type(ct_t t, int i)
{
  ct_tdef *d;

  if ( ct_t_PTRQ(t) ) {
    return ct_t_PTR_TO(t);
  }
  else if ( ct_t_ARRAYQ(t) ) {
    d = ct_t_def(t);
    return d->_subt;
  }
  else if ( ct_t_STRUCT_OR_UNIONQ(t) || ct_t_FUNCQ(t) ) {
    d = ct_t_def(t);
    if ( 0 <= i && i < d->_nelements ) {
      return d->_elements[i];
    }
  }
  else if ( ct_t_ENUMQ(t) ) {
    return t;
  }

  return 0;
}

/**************************************************************************/
/* source code representation */

const char *ct_t_name(ct_t t)
{
  ct_tdef *d = ct_t_def(t);
  return d ? d->_name : 0;
}

const char *ct_t_declaration(ct_t t)
{
  ct_tdef *d = ct_t_def(t);

  if ( d ) {
    if ( ! d->_decl ) {
      if ( ct_t_STRUCT_OR_UNION_OR_ENUMQ(t) ) {
	if ( d->_name ) {
	  d->_decl = ssprintf(0, "%s %%s", d->_name);
	} else { 
	  d->_decl = ssprintf(0, "%s %%s", ct_t_definition(t));
	}
      }
    }
  }
  
  return d ? d->_decl : 0;
}

const char *ct_t_definition(ct_t t)
{
  ct_tdef *d;

  if ( ct_t_PTRQ(t) || ct_t_ARRAYQ(t) || ct_t_BITFIELDQ(t) || ct_t_FUNCQ(t) )
    return "";

  d = ct_t_def(t);

  if ( d ) {
    if ( ! d->_def ) {
      int i;

      if ( ct_t_STRUCT_OR_UNION_OR_ENUMQ(t) ) {
	char *str = 0;

	str = ssprintf(str, "%s%s { /* size %lu */ \n", 
		 d->_name ? " " : "",
		 d->_name ? d->_name : "",
		 ct_t_sizeof(t));

	for ( i = 0; i < d->_nelements; i ++ ) {
	  if ( ct_t_ENUMQ(t) ) {
	    str = ssprintf(str, "  %s = %ld,\n", d->_names[i], d->_elements[i]);
	  } else {
	    char *decl = ssprintf(0, ct_t_declaration(d->_elements[i]), d->_names[i]);
	    
	    str = ssprintf(str, "  %s; /* offset %lu */\n", decl, d->_offsets ? d->_offsets[i] : 0);
	    
	    ct_free(decl);
	  }
	}

	str = ssprintf(str, "}");

	d->_def = str;
      }

    }
    return d->_def;
  }

  return 0;
}


/*************************************************************************/

static int ct_t_user_id = 0;

int ct_t_get_user_id() 
{
  if ( ct_t_user_id >= ct_N_USER )
    abort();

  return ct_t_user_id ++;
}

void **ct_t_user(ct_t t)
{
  return ct_t_def(t)->_user;
}

/*************************************************************************/

#ifdef ct_t_ptr
#undef ct_t_ptr
#endif
ct_t ct_t_ptr(ct_t t)
{
  return _ct_t_ptr(t);
}


#ifdef ct_t_bitfield
#undef ct_t_bitfield
#endif
ct_t ct_t_bitfield(ct_t t, int bits)
{
  return _ct_t_bitfield(t, bits);
}

/*************************************************************************/

