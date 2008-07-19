#include "ct.h"

#include "ct_sym.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "hash/charP_voidP_Table.h"
#include "hash/voidP_voidP_Table.h"

/*************************************************************************/

void ct_sym_free(ct_sym *s)
{
  if ( s->buf ) {
    ct_free(s->buf);
    s->buf = 0;
  }
  ct_free(s);
}

/*************************************************************************/

static
unsigned long scan_hex(char **_s)
{
  char *s = *_s;
  unsigned long x = 0;

  if ( ! isxdigit(*s) ) {
    return 0;
  }

  while ( isxdigit(*s) ) {
    x *= 16;
    if ( isdigit(*s) ) {
      x += *s - '0';
    } else {
      x += (isupper(*s) ? *s - 'A' : *s - 'a') + 10;
    }
    s ++;
  }

  while ( isspace(*s) )
    s ++;
  
  *_s = s;

  return x;
}

static
char * scan_token(char **_s)
{
  char *s = *_s;
  char *t;

  t = s;

  while ( ! isspace(*s) )
    s ++;
  
  *s = '\0';
 
  while ( isspace(*s) )
    s ++;

  *_s = s;

  return t;
}


/*********************************************************************/

static charP_voidP_Table *get_charP_table()
{
  static charP_voidP_Table *table;
  if ( ! table ) {
    table = ct_malloc(sizeof(*table));
    memset(table, 0, sizeof(*table));
    charP_voidP_TableInit(table, 1000); /* Expect at least 1000 syms */
  }
  return table;
}

static voidP_voidP_Table *get_voidP_table()
{
  static voidP_voidP_Table *table;
  if ( ! table ) {
    table = ct_malloc(sizeof(*table));
    memset(table, 0, sizeof(*table));
    voidP_voidP_TableInit(table, 1000); /* Expect at least 1000 syms */
  }
  return table;
}

static ct_sym* add_sym(ct_sym *sym)
{
  ct_sym **s;

  /* add to symtable */
  if ( (s = (ct_sym**) charP_voidP_TableGet(get_charP_table(), sym->name)) ) {
    (*s)->addr = sym->addr;
    voidP_voidP_TableRemove(get_voidP_table(), sym->addr);
    voidP_voidP_TablePush(get_voidP_table(), sym->addr, *s);
    ct_sym_free(sym);
    sym = *s;
  } else {
    charP_voidP_TablePush(get_charP_table(), sym->name, sym);
    voidP_voidP_TablePush(get_voidP_table(), sym->addr, sym);
  }
  return sym;
}

ct_sym *ct_sym_for_name(const char *name)
{
  ct_sym **sym;

  sym = (ct_sym **) charP_voidP_TableGet(get_charP_table(), name);

  return sym ? *sym : 0;
}

ct_sym *ct_sym_for_address(void *addr)
{
  ct_sym **sym;

  sym = (ct_sym **) voidP_voidP_TableGet(get_voidP_table(), addr);

  return sym ? *sym : 0;
}


/*********************************************************************/

static
int ct_load_syms_from_nm(const char *file)
{
  FILE *fp;

  if ( ! (fp = fopen(file, "r")) ) {
    return -1;
  }

  {
    char buf[128];
    ct_sym *sym = 0;

    while ( fgets(buf, sizeof(buf), fp) ) {
      char *s = buf;

      if ( ! sym )
	sym = ct_malloc(sizeof(*sym));
    
      memset(sym, 0, sizeof(*sym));

      /* Parse addr */
      sym->addr = (void*) scan_hex(&s);

      /* Parse type */
      switch ( *s ) {
      case 'T':
      case 't':
	sym->is_data = 0;
	break;

      case 'D':
      case 'd':
	sym->is_data = 1;
	break;

      default:
	continue;
	break;
      }

      s ++;
      while ( isspace(*s) )
	s ++;
    
      /* Parse name */
      sym->buf = strcpy(ct_malloc(strlen(s) + 1), s);
      s = sym->buf;

      sym->name = scan_token(&s);
      if ( ! sym->name ) {
      name_error:
	ct_free(sym->buf);
	sym->buf = 0;
	continue;
      }

      /* Validate name */
      {
	const char *t = sym->name;

	while ( *t ) {
	  if ( ! (isalnum(*t) || *t == '_') )
	    goto name_error;
      
	  t ++;
	}
      }

      add_sym(sym);
      sym = 0; /* We stored it */

    }

    if ( sym ) {
      ct_sym_free(sym);
    }
  }

  fclose(fp);

  return 0;
}

/*********************************************************************/

static
char *fstr(FILE *fp)
{
  char buf[1024];
  char *s = buf;
  int c;

  while ( isspace(c = fgetc(fp)) )
    ;

  if ( c == EOF )
    return 0;

  if ( c == '"' ) {
    while ( (c = fgetc(fp)) != EOF && c != '"' ) {
      *(s ++) = c;
    }
    if ( c != EOF ) {
      while ( isspace(c = fgetc(fp)) )
	;
      c = ungetc(c, fp);
    }

  } else {
    *(s ++) = c;
    while ( (c = fgetc(fp)) != EOF && ! isspace(c) ) {
      *(s ++) = c;
    }
    if ( c != EOF ) {
      while ( isspace(c) )
	c = fgetc(fp);
      ungetc(c, fp);
    }
  }
  *s = '\0';

  s = strcpy(ct_malloc(strlen(buf) + 1), buf);

  return s;
}

int ct_load_types_from_ct(const char *name)
{
  FILE *fp;

  if ( ! (fp = fopen(name, "r")) )
    return -1;

  {
    int i, j, k;
    double v;
    unsigned long d;
    char *file;
    int ntypes;
    struct type {
      int id;
      char type;
      char *name;
      int subtype;
      unsigned long size;
      int elems_n;
      int *elems_id;
      char **elems_name;
      long *elems_value;
      unsigned long *elems_offset;
      unsigned long *elems_size;
      int completed;
      ct_t ct;
    } *tobjs;
    int ntypedefs;
    int nsymbols;

#if 0
#define P(F,X) fprintf(stderr, "%s = " F "\n", #X, (X))
#else
#define P(F,X) (void)(X)
#endif

    assert(fscanf(fp, "V %lg ", &v) == 1);
    P("%lg", v);

    assert(fscanf(fp, "D %lu ", &d) == 1);
    P("%lu", d);

    fscanf(fp, "F ");
    file = fstr(fp);
    P("%s", file);

    /* Read type table */
    assert(fscanf(fp, "T %d ", &ntypes) == 1);
    P("%d", ntypes);

    tobjs = ct_malloc(sizeof(*tobjs) * (ntypes + 1));
    memset(tobjs, 0, (sizeof(*tobjs) * (ntypes + 1)));

    for ( i = 0; i < ntypes && ! feof(fp); i ++ ) {
      struct type *t;
      int id;
      char type;

      id = type = 0;
      assert(fscanf(fp, "%d %c ", &id, &type) == 2);
      t = &tobjs[id];
      assert(t->id == 0);
      assert(id < ntypes);
      t->id = id;
      t->type = type;

      switch ( t->type ) {
      case 'i':
	t->name = fstr(fp);
	break;

      case 'p':
	assert(fscanf(fp, "%d ", &t->subtype) == 1);
	assert(t->subtype < ntypes);
	break;

      case 'a':
      case 'b':
	assert(fscanf(fp, "%d %lu ", &t->subtype, &t->size) == 2);
	assert(t->subtype < ntypes);
	break;

      case 's':
      case 'u':
	assert(fscanf(fp, "%d ", &t->elems_n) == 1);
	t->name = fstr(fp);
	assert(fscanf(fp, "%lu ", &t->size) == 1);
	t->elems_id = ct_malloc(sizeof(t->elems_id[0]) * t->elems_n);
	t->elems_name = ct_malloc(sizeof(t->elems_name[0]) * t->elems_n);
	t->elems_offset = ct_malloc(sizeof(t->elems_offset[0]) * t->elems_n);
	t->elems_size = ct_malloc(sizeof(t->elems_size[0]) * t->elems_n);
	for ( j = 0; j < t->elems_n; j ++ ) {
	  assert(fscanf(fp, "%d ", &t->elems_id[j]) == 1);
	  assert(t->elems_id[j] < ntypes);

	  t->elems_name[j]= fstr(fp);

	  assert(fscanf(fp, "%lu %lu ", &t->elems_offset[j], &t->elems_size[j]) == 2);
	}
	break;

      case 'e':
	assert(fscanf(fp, "%d ", &t->elems_n) == 1);
	t->name = fstr(fp);
	t->elems_value = ct_malloc(sizeof(t->elems_value[0]) * t->elems_n);
	t->elems_name = ct_malloc(sizeof(t->elems_name[0]) * t->elems_n);
	for ( j = 0; j < t->elems_n; j ++ ) {
	  assert(fscanf(fp, "%ld ", &t->elems_value[j]) == 1);
	  t->elems_name[j] = fstr(fp);
	}
	break;

      case 'f':
	assert(fscanf(fp, "%d %d ", &t->elems_n, &t->subtype) == 2);
	t->elems_id = ct_malloc(sizeof(t->elems_id[0]) * t->elems_n);
	t->elems_name = ct_malloc(sizeof(t->elems_name[0]) * t->elems_n);
	for ( j = 0; j < t->elems_n; j ++ ) {
	  assert(fscanf(fp, "%d ", &t->elems_id[j]) == 1);
	  assert(t->elems_id[j] < ntypes);

	  t->elems_name[j] = fstr(fp);
	}
	break;

      default:
	abort();
      }

      assert(t->subtype < ntypes);
      if ( t->elems_id ) {
	for ( j = 0; j < t->elems_n; j ++ ) {
	  assert(t->elems_id[j] < ntypes);
	}
      }

#if 0
      fprintf(stderr, "{ %d, %c, %s, %d, %d }\n", t->id, t->type, t->name, t->subtype, t->elems_n);
#endif
    }

    /* Create struct/unions upfront */
    for ( i = 0; i < ntypes; i ++ ) {
      struct type *t = &tobjs[i];

      switch ( t->type ) {
      case 's':
	t->ct = ct_t_struct(0, t->name, 0);
	assert(ct_t_STRUCTQ(t->ct));
	break;

      case 'u':
	t->ct = ct_t_struct(1, t->name, 0);
	assert(ct_t_UNIONQ(t->ct));
	break;
      }
    }

    /* Create other types */
    do {
      k = -1;

      for ( i = 0; i < ntypes; i ++ ) {
	struct type *t = &tobjs[i];
	ct_t *elems_ct = 0;

	if ( ! t->ct ) {
	  switch ( t->type ) {
	  case 'i':
	    if ( strncmp(t->name, "complex ", 8) != 0 ) {
	      t->ct = ct_t_named(t->name);
	      assert(t->ct);
	    }
	    break;
	    
	  case 'p':
	    if ( ! tobjs[t->subtype].ct ) {
	      k = t->subtype;
	      goto try_again;
	    }
	    t->ct = ct_t_ptr(tobjs[t->subtype].ct);
	    assert(ct_t_PTRQ(t->ct));
	    break;
	    
	  case 'a':
	    if ( ! tobjs[t->subtype].ct ) {
	      k = t->subtype;
	      goto try_again;
	    }
	    t->ct = ct_t_array(tobjs[t->subtype].ct, t->size);
	    assert(ct_t_ARRAYQ(t->ct));
	    break;
	    
	  case 'b':
	    if ( ! tobjs[t->subtype].ct ) {
	      k = t->subtype;
	      goto try_again;
	    }
	    t->ct = ct_t_bitfield(tobjs[t->subtype].ct, t->size);
	    assert(ct_t_BITFIELDQ(t->ct));
	    break;
	    
	  case 'e':
	    t->ct = ct_t_enumv(t->name, t->elems_n, (const char **) t->elems_name, t->elems_value);
	    assert(ct_t_ENUMQ(t->ct));
	    break;
	  }
	}
	
	/* Finish struct/union/func types */

	/* Create a ct_t* array from t->elems_id */
	switch ( t->type ) {
	case 'f':
	  if ( t->ct )
	    goto try_again;
	  /* FALL THROUGH */

	case 's':
	case 'u':
	  elems_ct = ct_malloc(sizeof(elems_ct[0]) * t->elems_n);
	  for ( j = 0; j < t->elems_n; j ++ ) {
	    if ( ! tobjs[t->elems_id[j]].ct ) {
	      k = t->elems_id[j];
	      goto try_again;
	    }
	    elems_ct[j] = tobjs[t->elems_id[j]].ct;
	  }
	  break;
	}

	switch ( t->type ) {
	case 's':
	  if ( ! t->completed ) {
	    t->completed ++;
	    t->ct = ct_t_struct_endv(t->ct, t->elems_n, (const char **) t->elems_name, elems_ct);
	    assert(ct_t_STRUCTQ(t->ct));
	  }
	  break;
	  
	case 'u':
	  if ( ! t->completed ) {
	    t->completed ++;
	    t->ct = ct_t_struct_endv(t->ct, t->elems_n, (const char **) t->elems_name, elems_ct);
	    assert(ct_t_UNIONQ(t->ct));
	  }
	  break;
	  
	case 'f':
	  if ( ! tobjs[t->subtype].ct ) {
	    k = t->subtype;
	    goto try_again;
	    break;
	  }
	  
	  t->completed ++;
	  t->ct = ct_t_funcv(tobjs[t->subtype].ct, t->elems_n, elems_ct);
	  assert(ct_t_FUNCQ(t->ct));
	  break;
	}
	
      try_again:
	if ( elems_ct ) {
	  ct_free(elems_ct);
	  elems_ct = 0;
	}

#if 0
	if ( k != -1 ) {
	  struct type *t = &tobjs[k];
	  fprintf(stderr, "%d needs completion: ", k);
	  fprintf(stderr, "{ %d, %c, %s, %d, %d }\n", t->id, t->type, t->name, t->subtype, t->elems_n);
	}
#endif
      }

    } while ( k != -1 );

    /* Verify size and offset computation */
    for ( i = 0; i < ntypes; i ++ ) {
      struct type *t = &tobjs[i];
      
      if ( t->ct && t->completed ) {
	if ( t->size ) 
	  assert(ct_t_sizeof(t->ct) == t->size);

	if ( t->elems_offset ) {
	  assert(ct_t_n_elements(t->ct) == t->elems_n);
	  for ( j = 0; j < t->elems_n; j ++ ) {
	    assert(ct_t_element_offset(t->ct, j) == t->elems_offset[j]);
	  }
	}
      }
    }

    /* Parse an execute typedefs */
    fscanf(fp, "D %d ", &ntypedefs);
    P("%d", ntypedefs);
    for ( i = 0; i < ntypedefs && ! feof(fp); i ++ ) {
      int id;
      char *name;
      char *file;

      fscanf(fp, "%d ", &id);
      name = fstr(fp);
      file = fstr(fp);

      assert(id < ntypes);
      assert(name && *name);
      ct_t_typedef(tobjs[id].ct, name);
      
      ct_free(name);
      ct_free(file);
    }

    /* Parse and store symbols */
    fscanf(fp, "S %d ", &nsymbols);
    P("%d", nsymbols);
    for ( i = 0; i < nsymbols && ! feof(fp); i ++ ) {
      char type = 0;
      int id = 0;
      char *name = 0;
      unsigned long addr = 0;
      char *file = 0;
      int line = 0;
      ct_sym *sym = 0;

      fscanf(fp, "%c ", &type);
      P("%c", type);
      assert(type == 's' || type == 'e');

      fscanf(fp, "%d ", &id); 
      P("%d", id);
      assert(id < ntypes);

      name = fstr(fp);
      P("%s", name);

      fscanf(fp, "%lx ", &addr);
      P("%lx", addr);

      file = fstr(fp);
      P("%s", file);

      fscanf(fp, "%d ", &line);
      P("%d", line);

      sym = ct_malloc(sizeof(*sym));
      memset(sym, 0, sizeof(*sym));

      sym->name = name;
      sym->addr = (void*) addr;
      sym->type = tobjs[id].ct;
      sym->file = file;
      sym->line = line;

      add_sym(sym);
    }

    fscanf(fp, "E ");
    

    /* Free types */
    for ( i = 0; i < ntypes; i ++ ) {
      struct type *t = &tobjs[i];

      if ( t->elems_value ) {
	ct_free(t->elems_value);
	t->elems_value = 0;
      }
      if ( t->elems_id ) {
	ct_free(t->elems_id);
	t->elems_id = 0;
      }
      if ( t->elems_offset ) {
	ct_free(t->elems_offset);
	t->elems_offset = 0;
      }
      if ( t->elems_size ) {
	ct_free(t->elems_size);
	t->elems_size = 0;
      }

      if ( t->elems_name ) {
	for ( j = 0; j < t->elems_n; j ++ ) {
	  ct_free(t->elems_name[j]);
	}
	ct_free(t->elems_name);
	t->elems_name = 0;
      }
    }

    ct_free(file);
    file = 0;
  }
  
  fclose(fp);

  return 0;
}


/*********************************************************************/

static time_t file_mtime(const char *name)
{
  struct stat s;
  
  if ( stat(name, &s) == 0 ) {
    return s.st_mtime;
  }

  return(~ 0UL);
}

static
int ct_load_syms_for_exe(const char *name)
{
  char buf[1024];

  strcpy(buf, name);
  strcat(buf, ".nm");
  if ( file_mtime(name) > file_mtime(buf) ) {
    char cmd[1024];

    sprintf(cmd, "nm -aln '%s' > '%s'", name, buf); 
    if ( system(cmd) )
      return -1;
  }

  return ct_load_syms_from_nm(buf);
}

static
int ct_load_types_for_exe(const char *name)
{
  char buf[1024];

  strcpy(buf, name);
  strcat(buf, ".ct");
  if ( file_mtime(name) > file_mtime(buf) ) {
    char cmd[1024];

    sprintf(cmd, "perl -S cthc.pl '%s' -o '%s'", name, buf); 
    if ( system(cmd) )
      return -1;
  }

  return ct_load_types_from_ct(buf);
}

int ct_sym_load_for_exe(const char *name)
{
  int result;

  if ( (result = ct_load_types_for_exe(name)) )
    return result;

#if 0
  if ( (result = ct_load_syms_for_exe(name)) )
    return result;
#endif

  return result;
}


