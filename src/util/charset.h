#ifndef _charset_h

#define _charset_h

#ifndef BITS_PER_CHAR
#define BITS_PER_CHAR 8
#endif
#ifndef UCHAR_MAX
#define UCHAR_MAX 255
#endif

typedef unsigned char charset[(UCHAR_MAX+1+1+BITS_PER_CHAR-1)/BITS_PER_CHAR];
#define charset_CLEAR(cs) bzero(cs, sizeof(charset))
#define charset_SET(cs,x) ((*(cs))[(x)/BITS_PER_CHAR]|=(1<<((x)%BITS_PER_CHAR)))
#define charset_MATCH(cs,x) ((*(cs))[(x)/BITS_PER_CHAR]&(1<<((x)%BITS_PER_CHAR)))
#define charset_INVERT(cs) do { \
  int i; \
  for ( i = 0; i < sizeof(charset); i ++ ) (*(cs))[i] ^= ~ 0; \
} while(0)

int getc_escaped(char **_s);
int putc_escaped(char **_s, int i);

int charset_compile(charset *cs, char **_s);
int charset_print(charset *cs, char **_s);

#endif
