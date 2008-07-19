# $Id: Makefile,v 1.5 1999/05/07 12:38:34 stephensk Exp $

C_FILES = \
	ct.c \
	ct_sym.c \
	bitfield.c \
	cv.c

CFLAGS = -I.. -g -Wall -O3

O_FILES = $(C_FILES:.c=.o)

LIB = libct.a

all : $(LIB) # $(DEV_ROOT)/bin/cthc.pl

$(DEV_ROOT)/bin/cthc.pl : cthc.pl
	cp cthc.pl $@

bitfield.o : bitfield.c
	$(CC) $(filter-out,-g,$(CFLAGS)) -O3 -c $*.c -o $@

$(LIB) : $(O_FILES)
	-rm -f $@
	ar r $@ $(O_FILES)

$(O_FILES) : ct.def ct.h Makefile

#EXE = stab.exe
#
#$(EXE) : stab.c
#	gcc -g -Wall -o $@ stab.c -lbfd -lopcodes -liberty

clean veryclean : 
	rm -f $(LIB) $(EXE) $(O_FILES)
	cd test && make $@


