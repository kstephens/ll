#include "util/path.h"
#include <string.h>
#include <ctype.h>

const char *path_sep = "/";
const char *path_cwd = ".";

const char *path_file(const char *p)
{
  const char *s;

  s = strrchr(p, path_sep[0]);
  if ( s ) {
    s ++;
  } else { 
    s = p;
  }

  return s;
}

const char *path_dir(const char *p, char *buf)
{
  const char *s;

  s = strrchr(p, path_sep[0]);

  if ( s ) {
    const char *r = p;
    char *t = buf;

    while ( r < s ) {
      *(t ++) = *(r ++);
    }
    *t = '\0';
  } else {
    strcpy(buf, path_cwd);
  }

  return buf;
}


const char *path_absolute(const char *p, char *buf)
{
  if ( p[0] == path_sep[0] ) {
    return p;
  } else {
    strcpy(buf, path_cwd);
    if ( ! strrchr(buf, path_sep[0]) ) {
      strcat(buf, path_sep);
    }
    strcat(buf, p);
    return buf;
  }
}

const char *path_suffix(const char *p)
{
  const char *s;

  s = strrchr(path_file(p), '.');

  return s;
}

const char *path_no_suffix(const char *p, char *buf)
{
  const char *s;

  s = path_suffix(p);
  if ( s ) {
    strncpy(buf, p, s - p);
    buf[s - p] = '\0';
    return buf;
  } else {
    return p;
  }
}

char *path_simplify(const char *p, char *buf)
{
  const char *s;
  char *t;

  t = buf;
  s = p;
  while ( *s ) {
    /* replace "//" with "/" */
    if ( s[0] == path_sep[0] && s[1] == path_sep[0] ) {
      s ++;
    } else
      /* replace "./" with "" */
    if ( (s == p || s[-1] == path_sep[0]) && s[0] == '.' && s[1] == path_sep[0] ) {
      s += 2;
    } else {
      *(t ++) = *(s ++);
    }
  }
  *t = '\0';

  /* replace "/<str>/../" where <str> != ".." with "" */
  t = buf;
  s = buf;
  while ( *s ) {
    if ( s[0] == path_sep[0] && s[1] == '.' && s[2] == '.' && s[3] == path_sep[0] ) {
      if ( s - buf >= 3 && ! ( s[-3] == path_sep[0] && s[-2] == '.' && s[-1] == '.' ) ) {
	t = (char*) s - 1;
	s += 3;
	while ( t > buf && *t != path_sep[0] ) {
	  t --;
	}
      } else {
	*(t ++) = *(s ++);
	*(t ++) = *(s ++);
      }
    }

    *(t ++) = *(s ++);
  }

  *t = '\0';

  return buf;
}

const char *path_2_dos(const char *s, char *buf)
{
  char *p = buf;

  if ( s[0] == path_sep[0] && s[1] == path_sep[0] && isalpha(s[2]) && s[3] == path_sep[0] ) {
    *(p ++) = toupper(s[2]);
    *(p ++) = ':';
    *(p ++) = '\\';
    s += 4;
  }

  while ( *s ) {
    *(p ++) = *s == path_sep[0] ? '\\' : toupper(*s);
    s ++;
  }
  *p = '\0';

  return buf;
}

