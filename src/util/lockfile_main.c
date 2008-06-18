/* $Id: lockfile_main.c,v 1.1 1999/06/09 05:43:31 stephensk Exp $ */
#include "lockfile.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  int errors = 0;
  int i;

  lockfile_progname = argv[0];
  lockfile_unlock_at_exit = 0;

  switch ( argv[1] && argv[1][0] == '-' ? argv[1][1] : 0 ) {
  case 'l': case 'a':
    for ( i = 2; i < argc; i ++ ) {
      if ( lockfile_acquire(argv[i], 60, 10) ) {
	errors ++;
      }
    }
    break;

  case 'u': case 'r':
    for ( i = 2; i < argc; i ++ ) {
      if ( unlink(argv[i]) ) {
	fprintf(stderr, "%s: cannot unlink: '%s'\n", argv[0], argv[i]); 
	errors ++;
      }
    }
    break;

  default:
    fprintf(stderr, "%s: bad option: '%s'\n", argv[0], argv[1]);
    errors ++;
    /* FALL THROUGH */

  case 'h': case '?':
    fprintf(stderr, "%s: USAGE: %s [-lock|-unlock] files...\n", argv[0], argv[0]);
    break;
  }

  return errors;
}
