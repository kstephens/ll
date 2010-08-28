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



