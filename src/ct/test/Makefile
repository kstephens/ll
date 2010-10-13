C_FILES = test.c cthc_test.c

CFLAGS = -I../.. -g

O_FILES = $(C_FILES:.c=.o)

EXE=.exe

DEV_ROOT=$(HOME)#

LIBS = ../libct.a ../../hash/libhash.a $(DEV_ROOT)/lib/$(MAKE_ARCH)/libionutil.a

all : LIBS test$(EXE) 

LIBS : 
	make -C ../../hash
	make -C ../../util
	make -C ..

# pickup cthc.pl.
PATH:=..:$(PATH)#
export PATH

$(OFILES) : ../ct.h ../ct_sym.h

test$(EXE) : $(O_FILES) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $(O_FILES) $(LIBS)

run_test : LIBS test$(EXE)
	./test$(EXE)

debug_test : LIBS test$(EXE)
	gdb test$(EXE)

clean veryclean :
	rm -f test$(EXE) $(O_FILES) *.ct *.nm *core

