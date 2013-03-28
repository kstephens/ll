C_FILES = ../charP_int_Table.c

CFLAGS = -I.. -I../.. -g -Wall

O_FILES = $(C_FILES:.c=.o)

EXE=.exe

all : test$(EXE)

test$(EXE) : test.c $(O_FILES)
	$(CC) $(CFLAGS) -o $@ test.c $(O_FILES)

$(O_FILES) : 
	$(MAKE) -C ..

test-run : test$(EXE)
	./test$(EXE)

test-debug : test$(EXE)
	gdb --args test$(EXE)

clean:
	rm -f *.o test$(EXE)

