#ifndef _util_errlist_h
#define _util_errlist_h

#include <sys/errno.h>

#ifndef Linux
#ifndef NeXT
extern const char * const sys_errlist[];
extern int sys_nerr;
#endif
#endif

#endif
