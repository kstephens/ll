MAKS=../maks

INCLS += ..

H_FILES = \
	path.h \
	ConfigInfo.h \
	enum.h \
	setenv.h \
	sig.h \
	signals.h \
	errlist.h \
	prime.h \
	lockfile.h \
	rc4.h \
	ssprintf.h \
	outbuf.h \
	file.h \
	host.h \
	port.h

C_FILES = \
	path.c \
	ConfigInfo.c \
	enum.c \
	setenv.c \
	sig.c \
	errlist.c \
	prime.c \
	lockfile.c \
	rc4.c \
	ssprintf.c \
	outbuf.c \
	file.c \
	host.c \
	port.c

#################################################################

include $(MAKS)/pre.mak

#################################################################
# Create

LIB_NAME:=ionutil
include $(MAKS)/lib.mak

#################################################################
# Basic
include $(MAKS)/basic.mak

#################################################################

signals.h : sigs.pl
	perl sigs.pl < /usr/include/sys/signal.h > $@

sig.o : signals.h

#################################################################

errlist.c : errlist.pl
	perl errlist.pl < /usr/include/sys/errno.h > $@

#################################################################
