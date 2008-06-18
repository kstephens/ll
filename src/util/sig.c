#include "sig.h"
#include <string.h>
#include <ctype.h> /* isdigit() */
#include <stdlib.h> /* atoi */

static sigdef sigdefs[] = {
#define signal_def(N,S,D) { #N, S, D },
#include "signals.h"
  { 0, -1, 0 }
};

sigdef *sigdef_begin() 
{
  return sigdefs;
}

sigdef *sigdef_end()
{
  return sigdefs + sizeof(sigdefs)/sizeof(sigdefs[0]) - 1;
}


sigdef *sigdef_by_name(const char *name)
{
  int i;

  if ( ! name )
    return 0;

  if ( strncasecmp(name, "SIG", 3) == 0 ) {
    name += 3;
  }
  
  if ( isdigit(*name) ) {
    int sig = atoi(name);
    for ( i = 0; sigdefs[i].name; i ++ ) {
      if ( sigdefs[i].sig == sig )
	return &sigdefs[i];
    }
  } else {
    for ( i = 0; sigdefs[i].name; i ++ ) {
      if ( strcasecmp(sigdefs[i].name, name) == 0 )
	return &sigdefs[i];
    }
  }
  
  return 0;
}


sigdef *sigdef_by_sig(int sig)
{
  int i;

  for ( i = 0; sigdefs[i].name; i ++ ) {
    if ( sigdefs[i].sig == sig )
      return &sigdefs[i];
  }

  return 0;
}

sigdef *sigdef_max_sig(void)
{
  int i;
  sigdef *m = &sigdefs[0];

  for ( i = 1; sigdefs[i].name; i ++ ) {
    if ( sigdefs[i].sig > m->sig )
      m = &sigdefs[i];
  }

  return m;
}

sigdef *sigdef_min_sig(void)
{
  int i;
  sigdef *m = &sigdefs[0];

  for ( i = 1; sigdefs[i].name; i ++ ) {
    if ( sigdefs[i].sig < m->sig )
      m = &sigdefs[i];
  }

  return m;
}

