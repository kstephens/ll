#include <string.h>
#include <stdlib.h>

#include "setenv.h"

extern char **environ;

void setenv(const char *name, const char *value)
{
  size_t name_len, value_len;
  int i;
  char *e, *v;
  static int realloc_environ = 0;

  if ( ! (name && *name && value && *value) )
    return;

  name_len = strlen(name);
  value_len = strlen(value);

  for ( i = 0; (e = environ[i]); i ++ ) {
    if ( strncmp(e, name, name_len) == 0 && e[name_len] == '=' ) {
      v = e + name_len + 1;
      if ( strlen(v) <= value_len ) {
	strcpy(v, value);
	return;
      } else {
	break;
      }
    }
  }

  if ( ! e ) {
    if ( realloc_environ ) {
      environ = realloc(environ, sizeof(environ[0]) * (i + 2));
    } else {
      char **new_environ = malloc(sizeof(environ[0]) * (i + 2));
      memcpy(new_environ, environ, sizeof(environ[0]) * i);
      environ = new_environ;
      realloc_environ ++;
    }
    environ[i + 1] = 0;
  }

  /* This might leak mem */
  e = malloc(name_len + value_len + 2);
  strcpy(e, name);
  strcpy(e + name_len, "=");
  strcpy(e + name_len + 1, value);
  environ[i] = e;  
}
