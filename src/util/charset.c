#include <stdio.h>
#include <string.h> /* strchr() */
#include <ctype.h> /* isdigit(), isxdigit(), isprint(), iscntrl() */
#include "charset.h"

#define PUTC(x) (*(s ++) = (x))
#define GETC() (*s ? *(s ++) : -1)
#define PEEKC() (*s ? *s : -1)
#define UNGETC(c) (*(-- s))

#define MATCHC(X) ({ if ( PEEKC() != (X) ) goto SYNTAX_ERROR; GETC(); })
#define PEEKCMATCHC(X) (PEEKC() == (X) ? (GETC(), 1) : 0)

static const char escape_chars[] = "n\nr\rt\tf\fb\ba\av\ve\033";

int getc_escaped(char **_s)
{
  unsigned char *s = (unsigned char *)*_s;
  int c;
  
  c = GETC();
  if ( c == '\\' ) {
    const char *r;
    c = GETC();
    if ( (r = strchr(escape_chars, c)) && ((r - escape_chars) % 2 == 0) ) {
      c = r[1];
    } else
    if ( '0' <= c && c <= '7' ) { /* \012 -> \n */
      int n = c;
      int ndigits = 0;
      c = 0;
      do {
        c <<= 3;
	c += n - '0';
	ndigits ++;
      } while ( ndigits < 3 && (n = PEEKC(), '0' <= n && n <= '7' && GETC()) );
    } else
    if ( tolower(c) == 'x' ) { /* \xff -> 0xff */
      int n;
      int ndigits = 0;
      c = 0;
      while ( ndigits < 2 && (n = PEEKC(), isxdigit(n) && GETC()) ) {
        c <<= 4;
	c += isdigit(n)           ? n - '0' :
	     'a' <= n && n <= 'f' ? n - 'a' + 10 :
	     	                    n - 'A' + 10;
	ndigits ++;
      }
      if ( ! ndigits ) {
        goto SYNTAX_ERROR;
      }
    } else
    if ( tolower(c) == 'c' ) { /* \cC -> Control-C */
      c = GETC();
      c &= 0x1F; /* leave bottom 5 bits */
    }
    if ( c < 0 || c >= (UCHAR_MAX+1) ) {
      return -2;
    }
  }
  
  *_s = (char*) s;
  return c;
SYNTAX_ERROR:
  *_s = (char*) s;
  return -2;
}
#define GETC_ESCAPED() ({ int c = getc_escaped((char **)&s); if ( c < 0 ) goto SYNTAX_ERROR; c; })

int putc_escaped(char **_s, int i)
{
  char *s = *_s;
  char *r;
  
  if ( isprint(i) && isascii(i) && ! iscntrl(i) ) {
    PUTC(i);
  } else
  if ( (r = strchr(escape_chars, i)) && ((r - escape_chars) % 2 == 1) ) {
    PUTC('\\');
    PUTC(r[-1]);
  } else {
    PUTC('\\');
    PUTC(((i >> 6) & 0x7) + '0');
    PUTC(((i >> 3) & 0x7) + '0');
    PUTC(((i >> 0) & 0x7) + '0');
  }
  
  *_s = (char*) s;
  return 0;
}

int charset_compile(charset *cs, char **_s)
{
  unsigned char *s = (unsigned char*) *_s;
  short invert = 0;
  int c;
  
  charset_CLEAR(*cs);
  
  MATCHC('[');
  invert = PEEKCMATCHC('^');
  while ( (c = PEEKC()) >= 0 && c != ']' ) {
    c = GETC_ESCAPED();
    charset_SET(cs,c);
    if ( PEEKCMATCHC('-') ) {
      int oc = GETC_ESCAPED();
      if ( c > oc ) {
	int t;
      
	t = c;
	c = oc;
	oc = t;
      }
      while ( c <= oc ) {
	charset_SET(cs,c);
	c ++;
      }
    }
  }
  MATCHC(']');
  
  if ( invert ) {
    charset_INVERT(cs);
  }
  
  *_s = (char*) s;
  return 0;
  
  SYNTAX_ERROR:
  *_s = (char*) s;
  return -2;
}

int charset_print(charset *cs, char **_s)
{
  char *s = *_s;
  int i, j;
  int invert;
  
  PUTC('[');
  if ( (invert = charset_MATCH(cs,(UCHAR_MAX+1))) ) {
    PUTC('^');
    charset_INVERT(cs); /* Temporarly invert */
  }
  for ( i = 0; i < (UCHAR_MAX+1); i ++ ) {
    if ( charset_MATCH(cs,i) ) {
      if ( i == '^' || i == '-' ) {
	PUTC('\\');
	PUTC(i);
      } else {
        putc_escaped(&s, i);
      }
      for ( j = i + 1; j < (UCHAR_MAX+1); j ++ ) {
        if ( ! charset_MATCH(cs,j) )
	  break;
      }
      if ( j - i > 3 ) {
 	i = j - 1;
        PUTC('-');
	putc_escaped(&s, i);
      }
    } 
  }
  if ( invert ) {
    charset_INVERT(cs); /* revert */
  }
  PUTC(']');
  
  *_s = (char*) s;
  return 0;
}

#undef GETC
#undef PEEKC
#undef UNGETC

#undef MATCHC
#undef PEEKCMATCHC
#undef GETC_ESCAPED

#ifdef TEST
int main(int argc, char **argv)
{
  int i;
  
  /* printf("sizeof(charset) = %d\n", (int) sizeof(charset)); */
  for ( i = 1; i < argc; i ++ ) {
    char *t;
    
    if ( strcmp(argv[i], "-e") == 0 ) {
      i ++;
      for ( t = argv[i]; *t; ) {
	int c;
	unsigned char *s = t;
	c = getc_escaped(&t);
	
	printf("%s -> %d\n", s, c);
      }
    } else
    if ( strcmp(argv[i], "-c") == 0 ) {
      i ++;
      for ( t = argv[i]; *t; ) {
	unsigned char *s = t;
	charset cs;
	char buf[1024], *b = buf;
	
	if ( charset_compile(&cs,&t) < 0 ) {
	  printf("syntax error at %s in %s\n", t, s);
	} else {
	  charset_print(&cs,&b);
	}
	
	printf("%s -> %s\n", s, buf);
      }
    }
    
  }
  
  return 0;
}
#endif
