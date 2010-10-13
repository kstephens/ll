/* Copyright (c) 1998, 1999 Kurt A. Stephens and Ion, Inc. */

#include "enum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int C_enum_str_to_value(const C_enum *table, const char *name, size_t name_len, long *result)
{
  int i;

  if ( ! name )
    return 0;

  if ( name_len == (size_t) -1 )
    name_len = strlen(name);
  
  for ( i = 0; table[i].name; i ++ ) {
    if ( strncmp(table[i].name, name, name_len) == 0 && table[i].name[name_len] == '\0' ) {
      *result = table[i].value;
      return 1;
    }
  }

  return 0;
}

int C_enum_str_to_or_value(const C_enum *table, const char *name, size_t name_len, long *resultp)
{
  long result = 0;
  const char *s;
  const char *e;

  if ( ! name )
    return 0;

  if ( name_len == (size_t) -1 )
    name_len = strlen(name);

  s = name;
  e = name + name_len;
  while ( s < e ) {
    long eresult = 0;
    const char *t;
    
    /* Scan for '|' or end of s */
    for ( t = s; t != e; ++ t ) {
      if ( *t == '|' ) {
	break;
      }
    }

    if ( C_enum_str_to_value(table, s, t - s, &eresult) ) {
      result |= eresult;
    } else {
      return 0;
    }

    s = t + 1;
  }
  
  *resultp = result;
  return 1;  
}


const char * C_enum_value_to_str(const C_enum *table, long value)
{
  int i;
  
  for ( i = 0; table[i].name; ++ i ) {
    if ( table[i].value == value ) {
      return table[i].name;
    }
  }

  return 0;
}


const char * C_enum_or_value_to_str(const C_enum *table, long value, char *buf, size_t buf_len)
{
  if ( value == 0 ) {
    /* There might be an enum of 0! */
    return C_enum_value_to_str(table, value);
  } else {
    int i;       /* table[i] */
    char *bp;    /* Current buf write position. */
    int new_buf; /* Was the buf allocated by us? */

    /* Calculate buf_len if not specified. */
    if ( ! buf_len ) {
      buf_len = table[i].name ? strlen(table[i].name) + 1 : 16;
    }

    /* Allocate buffer if not specified. */
    if ( ! buf ) {
      new_buf = 1;
      buf = malloc(buf_len);
    } else {
      new_buf = 0;
    }

    /* Initialize buffer and buffer pointer. */
    buf[0] = '\0';
    bp = buf;

    /* While value still has bits: */
    while ( value ) {
      for ( i = 0; table[i].name; ++ i ) {
	/* If value has enum value's bits? */
	if ( (value & table[i].value) == table[i].value ) {
	  size_t len = (bp - buf);
	  size_t name_len = strlen(table[i].name);
	  size_t new_len = len + name_len + 2;

	  /* Resize buffer if possible. */
	  if ( new_len > buf_len ) {
	    if ( new_buf ) {
	      buf = realloc(buf, new_len);
	      bp = buf + len;
	    } else {
	      /* Buffer to small! */
	      return 0;
	    }
	  }

	  /* Append trailing '|'. */
	  if ( buf[0] ) {
	    *(bp ++) = '|';
	  }
	  
	  /* Append table[i].name. */
	  {
	    const char *s = table[i].name;
	    while ( *s ) {
	      *(bp ++) = *(s ++);
	    }
	  }

	  /* Remove enum value bits from value. */
	  value &= ~table[i].value;

	  /* Try more values. */
	  goto next_value;
	}

	/* Try next enum value. */
      }

      /* An enum value for values bits was not found. */
      return 0; /* ERROR */

    next_value:
      /* Try more values. */
      (void) 0;
    }

    *bp = '\0';
    return buf;
  }
}

