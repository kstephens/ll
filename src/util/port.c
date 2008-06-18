#ifndef __rcs_id__
#ifndef __rcs_id_util_port_c__
#define __rcs_id_util_port_c__
static const char __rcs_id_util_port_c[] = "$Id: port.c,v 1.2 2000/01/04 06:22:24 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

#include "port.h"
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int port_by_name(const char *port, const char *proto)
{
  if ( isdigit(*port) ) {
    return atoi(port);
  } else {
    struct servent *se;
    char service[64];

    if ( ! (proto && *proto) ) {
      const char *r;
      /* parse "service/protocol"? */
      if ( (r = strchr(port, '/')) ) {
	strncpy(service, port, r - port);
	service[r - port] = 0;
	port = service;

	proto = r + 1;

	/* Default to "tcp" */
	if ( ! *proto ) {
	  proto = "tcp";
	}
      } else {
	proto = "tcp";
      }
    }

    if ( ! (se = getservbyname(port, proto)) )
      return 0;

    return se->s_port;
  }
}



