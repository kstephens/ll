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
