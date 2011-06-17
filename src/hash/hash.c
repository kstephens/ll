#include <stddef.h> /* size_t */
#include <stdlib.h> /* malloc(), realloc(), free() */
#include <assert.h>
#include <stdio.h>

#include "hash.def"

#if HASH_TABLE_SIZE_TO_PRIME
#define EXTERN static
#include "util/prime.c"
#undef EXTERN
#endif

#ifndef HASH_DEBUG_MALLOC
#define HASH_DEBUG_MALLOC 0
#endif

#if HASH_DEBUG_MALLOC
#define malloc(X) ({ void *__ptr = malloc(X); fprintf(stderr, "  malloc(%lu /* %s */) => %p %s:%d\n", (unsigned long) (X), #X, __ptr, __FUNCTION__, __LINE__); __ptr; })
#define free(X)   ({ fprintf(stderr, "  free(%p /* %s */) %s:%d\n", (X), #X, __FUNCTION__, __LINE__); free(X); })
#endif

#ifndef HASH_MALLOC
#define HASH_MALLOC(X) malloc(X)
#endif

#ifndef HASH_FREE
#define HASH_FREE(X) free(X)
#endif

#ifndef HASH_WRITE_BARRIER
#define HASH_WRITE_BARRIER(X)
#endif

#if HASH_KEEP_HASH
#define HASH_ENTRY_HASH(e) (e)->_hash
#else
#define HASH_ENTRY_HASH(e) HASH_KEY_HASH((e)->_key)
#endif


HASH_EXTERN void HASH(TableStats) (HASH(Table) *ht, FILE *fp)
{
  int i;
  int ne = 0;
  int tne = 0; /* total no. of entries */
  int tnhc = 0; /* total no. of hash collisions */
  int tbc = 0; /* total no. of bucket collisions */
  int mbc = 0; /* max no of bucket collisions */
  int biu = 0; /* buckets in use */

#define _HASH_STRINGTIZE(X)#X
#define HASH_STRINGTIZE(X)_HASH_STRINGTIZE(X)

  fprintf(fp, "HASH_STATS %s %p {\n", HASH_STRINGTIZE(HASH(Table)), ht);

  for ( i = 0; i < HASH_TABLE_SIZE(ht); i ++ ) {
    HASH(TableEntry) *e;

    ne = 0;
    for ( e = ht->_entries[i]; e; e = e->_next ) {
      ne ++;
      tne ++;
    }

    if ( ne > 0 ) {
      biu ++;
    }

    if ( ne > 1 ) {
      if ( mbc < ne ) {
	mbc = ne;
      }
      tbc += ne - 1;
    }

    if ( ne > 1 ) {
      HASH_VAL *hashes = malloc(sizeof(hashes[0]) * ne);
      unsigned int *counts = malloc(sizeof(counts[0]) * ne);
      int nhashes = 0;
      int hi;
      
      memset(hashes, 0, sizeof(hashes[0]) * ne);
      memset(counts, 0, sizeof(counts[0]) * ne);
      
      fprintf(fp, "  %5d: %4d: ", i, ne);
      
      for ( e = ht->_entries[i]; e; e = e->_next ) {
	HASH_VAL h = HASH_ENTRY_HASH(e);
	for ( hi = 0; hi < nhashes; hi ++ ) {
	  if ( hashes[hi] == h ) {
	    counts[hi] ++;
	    goto found_hash;
	  }
	}
	nhashes ++;
	assert(nhashes <= ne);
	hashes[hi] = h;
      found_hash:
	counts[hi] ++;
      }
      
      fprintf(fp, "  %3d: { ", nhashes);
      for ( hi = 0; hi < nhashes; hi ++ ) {
	fprintf(stderr, "0x%08lx %4d ", (unsigned long) hashes[hi], (int) counts[hi]); 
	tnhc += counts[hi] - 1;
      }
      fprintf(fp, "}\n");

      free(hashes);
      free(counts);
    }
  }

  fprintf(fp, "}\n");
  fprintf(fp, "total buckets: %d\n", HASH_TABLE_SIZE(ht));
  fprintf(fp, "total buckets in use: %d\n", biu);
  fprintf(fp, "total nentries: %d\n", tne);
  fprintf(fp, "total hash collisions: %d\n", tnhc);
  fprintf(fp, "total bucket collisions: %d\n", tbc);
#if HASH_TABLE_COLLISIONS
  fprintf(fp, "_collisions: %d\n", ht->_collisions);
#endif

  fprintf(fp, "max bucket collisions: %d\n", mbc);
}

#ifndef HASH_COLLISION_STATS
#define HASH_COLLISION_STATS 0
#endif

#if HASH_COLLISION_STATS
static int hcs = 0;
#define HASH_CHECK_COLLISION_STATS(ht) if ( hcs ++ > HASH_COLLISION_STATS ) { hcs = 0; HASH(TableStats) (ht, stderr); }
#else
#define HASH_CHECK_COLLISION_STATS(ht)
#endif

HASH_EXTERN HASH(TableEntry) ** HASH(TableSearch) ( HASH(Table) *ht, HASH_KEY _key, HASH_VAL _hash )
{
#if HASH_CACHE
  if ( ht->_cache && (*ht->_cache) &&
#if HASH_KEEP_HASH
       _hash == (*ht->_cache)->_hash && 
#endif
       (HASH_KEY_EQUAL(_key, (*ht->_cache)->_key)) ) {
    return ht->_cache;
  }
#endif

  {
  unsigned int i = ((unsigned int) _hash) % HASH_TABLE_SIZE(ht);
  HASH(TableEntry) **e = &(ht->_entries[i]);

  while ( *e ) {
    if ( 
#if HASH_KEEP_HASH
	_hash == (*e)->_hash && 
#endif
	(HASH_KEY_EQUAL(_key, (*e)->_key)) ) {
#if HASH_SEARCH_MOVE_TO_FRONT != 0
      if ( HASH_SEARCH_MOVE_TO_FRONT ) {
	HASH(TableEntry) *ee = *e;
	*e = ee->_next;
	HASH_WRITE_BARRIER(e);
	e = &(ht->_entries[i]);
	ee->_next = *e;
	HASH_WRITE_BARRIER(ee);
	*e = ee;
	HASH_WRITE_BARRIER(ht->entries);
      }
#endif
      return e;
    }
    e = &((*e)->_next);
  }
  
  return 0;
  }
}

HASH_EXTERN void HASH(TableAddEntry) ( HASH(Table) *ht, HASH(TableEntry) *e )
{
  unsigned int i = ((unsigned int) HASH_ENTRY_HASH(e)) % HASH_TABLE_SIZE(ht);
#if HASH_CACHE
  ht->_cache = 0;
#endif
#if HASH_TABLE_COLLISIONS != 0
  if ( ht->_entries[i] )
    ht->_collisions ++;
#endif
  e->_next = ht->_entries[i];
  HASH_WRITE_BARRIER(e);
  ht->_entries[i] = e;
  HASH_WRITE_BARRIER(ht->_entries);
#if HASH_TABLE_NENTRIES != 0
  ht->_nentries ++;
#endif
  HASH_CHECK_COLLISION_STATS(ht);
}

HASH_EXTERN void HASH(TableInit) ( HASH(Table) *ht, int capacity )
{
#ifdef HASH_TABLE_INIT
  HASH_TABLE_INIT(ht);
#undef HASH_TABLE_INIT
#endif

  HASH_TABLE_CAPACITY_ADJUST(capacity);

  if ( capacity < 1 )
    capacity = 1;

#if HASH_CACHE
  ht->_cache = 0;
#endif

#if HASH_TABLE_FIXED_SIZE == 0
  ht->_entriesLen = capacity;
  ht->_entries = HASH_MALLOC(sizeof(ht->_entries[0]) * HASH_TABLE_SIZE(ht));
#endif

  memset(ht->_entries, 0, sizeof(ht->_entries[0]) * HASH_TABLE_SIZE(ht));
  HASH_WRITE_BARRIER(ht->_entries);

#if HASH_TABLE_NENTRIES != 0
  ht->_nentries = 0;
#endif

#if HASH_TABLE_COLLISIONS != 0
  ht->_collisions = 0;
#endif
}

HASH_EXTERN size_t HASH(TableSize) ( HASH(Table) *ht )
{
  return HASH_TABLE_SIZE(ht);
}

HASH_EXTERN size_t HASH(TableNEntries) ( HASH(Table) *ht )
{
#if HASH_TABLE_NENTRIES != 0
  return ht->_nentries;
#else
  int i;
  size_t tne = 0;

  for ( i = 0; i < HASH_TABLE_SIZE(ht); i ++ ) {
    HASH(TableEntry) *e;

    for ( e = ht->_entries[i]; e; e = e->_next ) {
      ++ tne;
    }
  }

  return tne;
#endif
}

HASH_EXTERN size_t HASH(TableCollisions) ( HASH(Table) *ht )
{
#if HASH_TABLE_COLLISIONS != 0
  return ht->_collisions;
#else
  return (size_t) -1L;
#endif
}

HASH_EXTERN void HASH(TableResize) ( HASH(Table) *ht, int capacity )
{
#if HASH_TABLE_FIXED_SIZE == 0
  unsigned int i;

  /* Remember the old entries */
  HASH(TableEntry) **_entries = ht->_entries;
  unsigned int _entriesLen = HASH_TABLE_SIZE(ht);
  
  HASH_TABLE_CAPACITY_ADJUST(capacity);

  if ( capacity < 1 )
    capacity = 1;

  if ( capacity == ht->_entriesLen )
    return;

  /* Create a new entries table. */
  HASH(TableInit) ( ht, capacity );
  
  /* Scan through old entries. */
  for ( i = 0; i < _entriesLen; i ++ ) {
    HASH(TableEntry) *e, *e_next;
    
    /* Reverse the list because Add puts them at the front of the list */
    e = _entries[i];
    _entries[i] = 0;
    HASH_WRITE_BARRIER(_entries);
    for ( ; e; e = e_next ) {
      e_next = e->_next;
      e->_next = _entries[i];
      HASH_WRITE_BARRIER(e);
      _entries[i] = e;
      HASH_WRITE_BARRIER(_entries);
    }

    /* Add them to the new table. */
    for ( e = _entries[i]; e; e = e_next ) {
      e_next = e->_next;
      HASH(TableAddEntry) (ht, e);
    }
  }

  /* Free old table. */
  HASH_FREE((void*) _entries);

#if 0 
  /* Show some stats. */
  HASH(TableStats) (ht, stderr);
#endif
#endif
}


HASH_EXTERN void HASH(TableClear) ( HASH(Table) *ht )
{
  unsigned int i;

#if HASH_CACHE
  ht->_cache = 0;
#endif

  for ( i = 0; i < HASH_TABLE_SIZE(ht); i ++ ) {
    HASH(TableEntry) *e = ht->_entries[i], *e_next;

    ht->_entries[i] = 0;
    for ( ; e; e = e_next ) {
      e_next = e->_next;

      /* free the key */
      HASH_KEY_FREE((void*) e->_key);

      /* free the data */
#ifdef HASH_DATA_DECL
      HASH_DATA_FREE((void*) e->_data);
#endif

      /* free the entry */
      HASH_FREE((void*) e);
    }
  }

#if HASH_TABLE_NENTRIES != 0
  ht->_nentries = 0;
#endif

#if HASH_TABLE_COLLISIONS != 0
  ht->_collisions = 0;
#endif

}

HASH_EXTERN void HASH(TableDestroy) ( HASH(Table) *ht )
{
  HASH(TableClear)(ht);

  /* Delete the table */
#if HASH_TABLE_FIXED_SIZE == 0
  HASH_FREE((void*) ht->_entries);
  ht->_entries = 0;
  HASH_WRITE_BARRIER(ht);
  ht->_entriesLen = 0;
#endif

#ifdef HASH_TABLE_DESTROY
  HASH_TABLE_DESTROY(ht);
#undef HASH_TABLE_DESTROY

#endif
}

HASH_EXTERN void HASH(TablePush_) ( HASH(Table) *ht, HASH_KEY _key HASH_DATA_ARG , HASH_VAL _hash)
{
  HASH(TableEntry) *e;
  
  e = HASH_MALLOC(sizeof(*e));

#if HASH_KEEP_HASH
  e->_hash = _hash;
#endif

  HASH_KEY_SET(e->_key, _key);
  HASH_WRITE_BARRIER(e);

#ifdef HASH_DATA_DECL
  HASH_DATA_SET(e->_data, _data);
  HASH_WRITE_BARRIER(e);
#endif

  HASH(TableAddEntry) (ht, e);

  HASH_TABLE_AUTO_RESIZE(ht, 1);
}

HASH_EXTERN void HASH(TablePush) ( HASH(Table) *ht, HASH_KEY _key HASH_DATA_ARG)
{
  HASH(TablePush_) (ht, _key HASH_DATA_ARG_VAR ,(HASH_KEY_HASH(_key)));
}

HASH_EXTERN int HASH(TableAdd_) (HASH(Table) *ht, HASH_KEY _key HASH_DATA_ARG ,HASH_VAL _hash)
{
  HASH(TableEntry) **en = HASH(TableSearch) (ht, _key, _hash);
  if ( en ) {
#ifdef HASH_DATA_DECL
    HASH_DATA_SET((*en)->_data, _data);
    HASH_WRITE_BARRIER(*en);
#endif
    return 0;
  } else {
    HASH(TablePush_)(ht, _key HASH_DATA_ARG_VAR , _hash);
    return 1;
  }
}

HASH_EXTERN int HASH(TableAdd) (HASH(Table) *ht, HASH_KEY _key HASH_DATA_ARG)
{
  return HASH(TableAdd_) (ht, _key HASH_DATA_ARG_VAR , (HASH_KEY_HASH(_key)));
}


HASH_EXTERN int HASH(TableIsSet_) (HASH(Table) *ht, HASH_KEY _key, HASH_VAL _hash)
{
  return HASH(TableSearch) (ht, _key, _hash) != 0;
}

HASH_EXTERN int HASH(TableIsSet) (HASH(Table) *ht, HASH_KEY _key)
{
  return HASH(TableIsSet_) (ht, _key, (HASH_KEY_HASH(_key)));
}


HASH_EXTERN HASH_DATA *HASH(TableGet_) (HASH(Table) *ht, HASH_KEY _key, HASH_VAL _hash)
{
  HASH(TableEntry) **en = HASH(TableSearch) (ht, _key, _hash);
  if ( en ) {
#if HASH_CACHE
    /* Look up was successful, remember it in the cache. */
    ht->_cache = en;
#endif

#ifdef HASH_DATA_DECL
    return &(*en)->_data;
#else
    return &(*en)->_key;
#endif
  } else {
    return 0;
  }
}

HASH_EXTERN HASH_DATA *HASH(TableGet) (HASH(Table) *ht, HASH_KEY _key)
{
  return HASH(TableGet_) (ht, _key, (HASH_KEY_HASH(_key)));
}

HASH_EXTERN int HASH(TableRemove_) (HASH(Table) *ht, HASH_KEY _key, HASH_VAL _hash)
{
  HASH(TableEntry) **en = HASH(TableSearch) (ht, _key, _hash);
  if ( en ) {
    HASH(TableEntry) *e = *en;
    *en = e->_next;    
    HASH_WRITE_BARRIER(en);

#if HASH_CACHE
    if ( ht->_cache == en )
      ht->_cache = 0;
#endif

#if HASH_TABLE_NENTRIES != 0
    ht->_nentries --;
#endif

#if HASH_TABLE_COLLISIONS
    if ( e->_next )
      ht->_collisions --;
#endif

    HASH_KEY_FREE((void*) e->_key);

#ifdef HASH_DATA_DECL
    HASH_DATA_FREE((void*) e->_data);
#endif
    HASH_FREE((void*) e);

    HASH_TABLE_AUTO_RESIZE(ht, -1);
    return ! 0;
  } else {
    return 0;
  }
}

HASH_EXTERN int HASH(TableRemove) (HASH(Table) *ht, HASH_KEY _key)
{
  return HASH(TableRemove_) (ht, _key, (HASH_KEY_HASH(_key)));
}

HASH_EXTERN void HASH(TableIteratorInit)(HASH(Table) *ht, HASH(TableIterator) *i)
{
  i->_i = -1;
  i->_e = i->_e_next = 0;
}

HASH_EXTERN int HASH(TableIteratorNext)(HASH(Table) *ht, HASH(TableIterator) *i)
{
  if ( i->_i != -1 && i->_i >= HASH_TABLE_SIZE(ht) )
    return 0;
  
  i->_e = i->_e_next;
  
  while ( ! i->_e ) {
    if ( ++ i->_i >= HASH_TABLE_SIZE(ht) )
      return 0;
    i->_e = ht->_entries[i->_i];
  }
  i->_e_next = i->_e ? i->_e->_next : 0;

  return 1;
}

HASH_EXTERN HASH_KEY *HASH(TableIteratorKey)(HASH(Table) *ht, HASH(TableIterator) *i)
{
  return i->_e ? (HASH_KEY*) &(i->_e->_key) : (HASH_KEY*) 0;
}

HASH_EXTERN HASH_DATA *HASH(TableIteratorData)(HASH(Table) *ht, HASH(TableIterator) *i)
{
#ifdef HASH_DATA_DECL
  return i->_e ? (HASH_DATA *) &(i->_e->_data) : (HASH_DATA *) 0;
#else
  return i->_e ? (HASH_DATA *) &(i->_e->_key) : (HASH_DATA *) 0;
#endif
}


