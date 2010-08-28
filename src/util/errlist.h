#ifndef _util_errlist_h
#define _util_errlist_h

#include <sys/errno.h>

/* See gcc -dD -E -o - - < /dev/null */
#ifdef __APPLE_CC__
#include <stdio.h> /* really? */
#else
#ifndef Linux
#ifndef NeXT
extern const char * const sys_errlist[];
extern int sys_nerr;
#endif
#endif
#endif

#endif
