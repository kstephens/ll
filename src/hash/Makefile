C_FILES = \
	charP_int_Table.c \
	charP_voidP_Table.c \
	int_voidP_Table.c \
        voidP_voidP_Table.c \
	generic_Table.c

CFLAGS = -I.. -g -Wall

O_FILES = $(C_FILES:.c=.o)

LIB = libhash.a

all : $(LIB) test-run

$(LIB) : $(O_FILES)
	-rm -f $@
	ar r $@ $(O_FILES)

$(O_FILES) : hash.def hash.c hash.h

test-run :
	$(MAKE) -C test test-run

clean : 
	rm -f $(LIB) $(O_FILES)
	$(MAKE) -C test clean


