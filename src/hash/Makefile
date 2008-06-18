# $Id: Makefile,v 1.5 1999/04/22 05:15:07 stephensk Exp $

C_FILES = \
	charP_int_Table.c \
	charP_voidP_Table.c \
	int_voidP_Table.c \
        voidP_voidP_Table.c \
	generic_Table.c

CFLAGS = -I.. -g

O_FILES = $(C_FILES:.c=.o)

LIB = libhash.a

$(LIB) : $(O_FILES)
	-rm -f $@
	ar r $@ $(O_FILES)

$(O_FILES) : hash.def hash.c hash.h

clean : 
	rm -f $(LIB) $(O_FILES)

