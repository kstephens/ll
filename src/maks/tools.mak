# $Id: tools.mak,v 1.5 1999/09/09 04:59:21 stephensk Exp $
ECHO=echo
MSG_PREFIX=+++
MSG=$(ECHO) $(MSG_PREFIX)
MKDIR=mkdir
MKDIRS=$(MKDIR) -p
CP=cp
CP_RECURSIVE=$(CP) -rp
MV=mv
TAR=tar
GZIP=gzip -9
GUNZIP=gzip -d
