/* $Id: port.h,v 1.3 2000/01/04 06:34:47 stephensk Exp $ */

#ifndef _util_port_h
#define _util_port_h

#ifdef __cplusplus
extern "C" {
#endif

int port_by_name(const char *port, const char *proto);

#ifdef __cplusplus
}
#endif

#endif
