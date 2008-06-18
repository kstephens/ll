#include "rc4.h"

/* $Id: rc4.c,v 1.6 2006/08/31 06:01:56 stephens Exp $ */

RC4_EXPORT int rc4_init(rc4_state *s, const char *_key, size_t keylen)
{
  int i;
  int temp1, temp2, temp3, temp4;
  int i1, i2;
  const unsigned char *key = (const unsigned char *) _key; 
   
  if ( keylen == 0 ) {
    key = (const unsigned char *) "\0";
    keylen = 1;
  }

  for ( i = 0; i < 256; i ++ )
    s->_state[i] = i;
  s->_x = s->_y = 0;
  
  i1 = i2 = 0;
  for ( i = 0; i < 256; i ++ ) {
    int ch = key[i1];
    temp4 = (ch + s->_state[i] + i2) & 255;
    temp1 = s->_state[temp4];
    temp2 = s->_state[i];
    temp3 = (i1 + 1) % keylen;

    s->_state[i] = temp1;
    s->_state[i2] = temp2;
    i1 = temp3;
    i2 = temp4;
  }
  
  return 0;
}


RC4_EXPORT int rc4_crypt(rc4_state *s, char *_data, size_t datalen)
{
  int i;
  unsigned char *data = (unsigned char *) _data;
  
  for ( i = 0; i < datalen; i ++ ) {
    int temp1, temp2;
    
    s->_x ++; /* SAME AS: s->_x = (s->_x + 1) & 255; */
    s->_y += s->_state[s->_x]; /* SAME AS: s->_y = (s->_y + s->_state[s->_x]) & 255; */
    
    temp1 = s->_state[s->_x];    
    s->_state[s->_x] = temp2 = s->_state[s->_y];
    s->_state[s->_y] = temp1;
    
    data[i] ^= s->_state[(temp1 + temp2) & 255];
  }
  
  return 0;
}

