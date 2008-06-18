/* $Id: host.h,v 1.3 2000/01/04 06:34:47 stephensk Exp $ */

#ifndef _util_host_h
#define _util_host_h

#include <netdb.h>

#ifdef __cplusplus
extern "C" {
#endif

int hostname_matches_hostent(const char *h, const struct hostent *he);
int hostname_matches_hostname(const char *h1, const char *h2);

#ifdef __cplusplus
}
#endif

#endif
