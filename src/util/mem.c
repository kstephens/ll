/*
** Unrolled memcpy
*/
void memcpy_fast(char *_dst, const char *_src, unsigned int _len)
{
  if ( _len == 0 )
    return;

#define MEMSET_4() *(long*)_dst = *(long*)_src; _dst += sizeof(long); _src += sizeof(long)
#define MEMSET_1() *(_dst ++) = *(_src ++)
  /* Align to 4 dword */
  switch ( ((unsigned int) _dst) & 3 ) {
    case 1: if ( _len -- <= 0 ) return; MEMSET_1();
    case 2: if ( _len -- <= 0 ) return; MEMSET_1();
    case 3: if ( _len -- <= 0 ) return; MEMSET_1();
  }
  /* Do blocks of 64 bytes */
  while ( _len >= 64 ) {
    MEMSET_4(); MEMSET_4(); MEMSET_4(); MEMSET_4();
    MEMSET_4(); MEMSET_4(); MEMSET_4(); MEMSET_4();
    MEMSET_4(); MEMSET_4(); MEMSET_4(); MEMSET_4();
    MEMSET_4(); MEMSET_4(); MEMSET_4(); MEMSET_4();
    _len -= 64;
  }
  /* Do remaining dwords */
  switch ( _len / 4 ) {
    case 16: MEMSET_4();
    case 15: MEMSET_4();
    case 14: MEMSET_4();
    case 13: MEMSET_4();
    case 12: MEMSET_4();
    case 11: MEMSET_4();
    case 10: MEMSET_4();
    case  9: MEMSET_4();
    case  8: MEMSET_4();
    case  7: MEMSET_4();
    case  6: MEMSET_4();
    case  5: MEMSET_4();
    case  4: MEMSET_4();
    case  3: MEMSET_4();
    case  2: MEMSET_4();
    case  1: MEMSET_4();
    case  0: break;
  }
  /* Do remaining bytes */
  switch ( _len & 3 ) {
    case  3: MEMSET_1();
    case  2: MEMSET_1();
    case  1: MEMSET_1();
    case  0: break;
  }
#undef MEMSET_4
#undef MEMSET_1
}


/*
** Unrolled memset
*/
void memset_fast(unsigned char *_dst, int val, unsigned int _len)
{
  unsigned long _val;

  if ( _len == 0 )
    return;

  _val = val;
  _val |= _val << 8 | _val << 16 | _val << 24;

#define MEMSET_4() (* ((unsigned long*) _dst)) = _val; _dst += sizeof(unsigned long)
#define MEMSET_1() *(_dst ++) = _val;
  /* Align to 4 dword */
  switch ( ((unsigned int) _dst) & 3 ) {
    case 1: if ( _len -- <= 0 ) return; MEMSET_1();
    case 2: if ( _len -- <= 0 ) return; MEMSET_1();
    case 3: if ( _len -- <= 0 ) return; MEMSET_1();
  }
  /* Do blocks of 64 bytes */
  while ( _len >= 64 ) {
    MEMSET_4(); MEMSET_4(); MEMSET_4(); MEMSET_4();
    MEMSET_4(); MEMSET_4(); MEMSET_4(); MEMSET_4();
    MEMSET_4(); MEMSET_4(); MEMSET_4(); MEMSET_4();
    MEMSET_4(); MEMSET_4(); MEMSET_4(); MEMSET_4();
    _len -= 64;
  }
  /* Do remaining dwords */
  switch ( _len / 4 ) { 
    case 16: MEMSET_4();
    case 15: MEMSET_4();
    case 14: MEMSET_4();
    case 13: MEMSET_4();
    case 12: MEMSET_4();
    case 11: MEMSET_4();
    case 10: MEMSET_4();
    case  9: MEMSET_4();
    case  8: MEMSET_4();
    case  7: MEMSET_4();
    case  6: MEMSET_4();
    case  5: MEMSET_4();
    case  4: MEMSET_4();
    case  3: MEMSET_4();
    case  2: MEMSET_4();
    case  1: MEMSET_4();
    case  0: break;
  }
  /* Do remaining bytes */
  switch ( _len & 3 ) { 
    case  3: MEMSET_1();
    case  2: MEMSET_1();
    case  1: MEMSET_1();
    case  0: break;
  }
#undef MEMSET_4  
#undef MEMSET_1
}
