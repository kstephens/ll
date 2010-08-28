#ifndef BITSET_H
#define BITSET_H

#include <stddef.h>

/* Bitset ops. */
typedef unsigned int bitset_t;

#ifndef bitset_BITS_PER_BYTE
#define bitset_BITS_PER_BYTE 8
#endif

#define bitset_ELEM_BSIZE (sizeof(bitset_t) * bitset_BITS_PER_BYTE)

#define bitset_ELEM_LEN(S) (S / bitset_ELEM_BSIZE)

#define bitset_MASK (bitset_ELEM_BSIZE-1)

#define bitset_SHIFT (sizeof(bitset_t) == 4 ? 5 : 4)

#define bitset_BIT(i) (1<<((i)&bitset_MASK))

#define bitset_OFF(i) ((i)>>bitset_SHIFT)

#define bitset_get(bs, i) (bs[bitset_OFF(i)] & bitset_BIT(i))

#define bitset_set(bs, i) (bs[bitset_OFF(i)] |= bitset_BIT(i))

#define bitset_clr(bs, i) (bs[bitset_OFF(i)] &= ~bitset_BIT(i))

static __inline
size_t bitset_index_of_previous_one(bitset_t *bs, size_t size, size_t offset, size_t min_offset)
{
  while ( offset > min_offset ) {
    /* Word has at least one bit set? */
    if ( bs[bitset_OFF(offset)] ) {
      /* Scan back until set bit is found. */
      do {
	if ( bitset_get(bs, offset) ) {
	  return offset;
	}
	-- offset;
      } while ( offset > min_offset );
    } else {
      /* Move to end of previous word. */
      size_t dec = offset % bitset_ELEM_BSIZE;
      offset -= dec + 1;
    }
  }

  /* Not found. */
  return (size_t) -1L;
}

#endif
