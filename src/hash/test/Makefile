C_FILES = ../charP_int_Table.c

CFLAGS = -I.. -I../.. -g

O_FILES = $(C_FILES:.c=.o)

EXE=.exe

test$(EXE) : test.c $(O_FILES)
	$(CC) $(CFLAGS) -o $@ test.c $(O_FILES)

$(O_FILES) : 
	$(MAKE) -C ..

run_test : test$(EXE)
	test$(EXE)

debug_test : test$(EXE)
	gdb test$(EXE)
