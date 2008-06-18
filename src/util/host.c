#ifndef __rcs_id__
#ifndef __rcs_id_util_host_c__
#define __rcs_id_util_host_c__
static const char __rcs_id_util_host_c[] = "$Id: host.c,v 1.2 2000/01/04 06:22:24 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

#include "host.h"
#include <string.h>
#include <sys/param.h> /* MAXHOSTNAMELEN */

int hostname_matches_hostent(const char *h, const struct hostent *he)
{
  int i;

  if ( strcmp(h, he->h_name) == 0 )
    return 1;

  for ( i = 0; he->h_aliases[i]; i ++ ) {
    if ( strcmp(h, he->h_aliases[i]) == 0 )
      return 1;
  }

  return 0;
}

int hostname_matches_hostname(const char *h1, const char *h2)
{
  struct hostent *he1, *he2;
  char hostname[MAXHOSTNAMELEN + 1];

  if ( ! (h1 && *h1) ) {
    if ( gethostname(hostname, sizeof(hostname)) )
      return 0;
    h1 = hostname;
  }
  if ( ! (h2 && *h2) ) {
    if ( gethostname(hostname, sizeof(hostname)) )
      return 0;
    h2 = hostname;
  }

  if ( strcmp(h1, h2) == 0 )
    return 1;
 
  if ( ! (he1 = gethostbyname(h1)) )
    return 0;
  if ( ! (he2 = gethostbyname(h2)) )
    return 0;

  return strcmp(he1->h_name, he2->h_name) == 0;
}


