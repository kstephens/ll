# $Id: Makefile,v 1.77 2008/05/27 04:06:41 stephens Exp $

################################################################
# Configuration

CWD:=$(shell /bin/pwd)#
PATH:=$(CWD):$(CWD)/sbin:$(PATH)#
export PATH

CONFIG_MAK=config.mak
include $(CONFIG_MAK)
MAKEFILES=Makefile $(CONFIG_MAK)

################################################################

PERL = perl
MAKE_DEFS = sh ./defs.sh - CPP "$(CPP)" - CPPFLAGS "$(CPPFLAGS)"
LLCC=llcc - od $(DEF_DIR) 
LLCC_COMPILE=$(LLCC) - CPP '$(CPP)' - CPPFLAGS '$(CPPFLAGS)'
LLCC_COLLECT=$(LLCC) --collect

################################################################

BUILD_ROOT:=$(CWD)#

DEF_DIR=lib/ll#

C_DEBUG=-g3
#C_DEBUG+=-fverbose-asm 
C_PROFILE=-pg
#C_PROFILE=--coverage
C_PROFILE=
C_OPTIMIZE=-O3
#C_OPTIMIZE=-O2
#C_OPTIMIZE=
C_WARNINGS=-Wall -Werror
CC = gcc
export CC
CPPFLAGS = -I.. -I$(dir $(DEF_DIR)) -Isrc/include# -I/usr/include 
CFLAGS = $(C_OPTIMIZE) $(C_DEBUG) $(C_PROFILE) $(C_WARNINGS) $(CPPFLAGS)
CPP = $(CC) -E

C_FILES = \
  runtime.c \
  init.c \
  assert.c \
  mem.c \
  error.c \
  toplevel.c \
  debugger.c \
  debug.c \
  stack.c \
  lookup.c \
  lcache.c \
  call.c \
  ar.c \
  eq.c \
  object.c \
  type.c \
  props.c \
  locative.c \
  constant.c \
  list.c \
  cons.c \
  cadr.c \
  nil.c \
  undef.c \
  bool.c \
  number.c \
  rational.c \
  integer.c \
  fixnum.c \
  flonum.c \
  ratnum.c \
  bignum.c \
  char.c \
  string.c \
  symbol.c \
  vector.c \
  op.c \
  meth.c \
  prim.c \
  catch.c \
  ccont.c \
  callcc.c \
  env.c \
  binding.c \
  port.c \
  write.c \
  format.c \
  objdump.c \
  read.c \
  named.c \
  eval.c \
  macro.c \
  syntax.c \
  trace.c \
  posix.c \
  bcompile.c \
  bcompile1.c \
  bcompile2.c \
  cfold.c \
  bmethod.c \
  load.c \
  fluid.c \
  singleton.c \
  map.c \
  doc.c \
  sig.c

ifneq "$(strip $(HAS_READLINE))" "0"
C_FILES += readline.c
LIBS += -lreadline
LIBS += -lncurses
endif

ifneq "$(strip $(HAS_HISTORY))" "0"
LIBS += -lhistory
endif

LIBS += -lgmp
LIBS += -lm

EARLY_H = \
	../util/signals.h \
	$(DEF_DIR)/config.h \
	$(DEF_DIR)/floatcfg.h \

DEFS = \
	$(DEF_DIR)/errors.h \
	$(DEF_DIR)/prims.h \
	$(DEF_DIR)/macros.h \
	$(DEF_DIR)/ops.h \
	$(DEF_DIR)/globals.h \
	$(DEF_DIR)/symbols.h \
	$(DEF_DIR)/bcs.h \
	$(DEF_DIR)/debugs.h \
	$(DEF_DIR)/inits.h \
	$(DEF_DIR)/types.h


H_FILES = \
	ll.h \
	value.h \
	call.h \
	call_int.h \
	assert.h \
	init.h \
	type_defs.h \
	type.h \
	assert.h \
	sig.h \
	symbol1.h \
	op1.h \
	global.h \
	global1.h \
	cops.h \
	lcache.h \
	bcode.h \
	bcompile.h \
	cadr.h

####################################################################
# garbage collector configuration
#

ifeq "$(strip $(ll_USE_GC))" "0"
GC=.
GC_BEGIN=
GC_END=
GC_LIB=

else

GC=../gc
GC_BEGIN = $(GC)/gc_begin.c
GC_END = $(GC)/gc.c $(GC)/gc_end.c
GC_LIB =

ifneq "$(strip $(ll_USE_GC_BOEHM))" "0"
GC=../gc_boehm/gc-7.0
CPPFLAGS+= -I$(GC)/include
GC_BEGIN =
GC_END =
GC_LIB = -L$(GC) -L$(GC)/.libs -lgc
GC_TARGET=gc_boehm
GC_DEPS=$(GC)/libgc.la
endif

ifneq "$(strip $(ll_USE_GC_TREADMILL))" "0"
GC=../tredmill
GC_LIB = -L$(DEV_ROOT)/lib/$(MAKE_ARCH) -ltredmill
GC_BEGIN =
GC_END =
endif

endif

GC_DEPS += $(GC_BEGIN) $(GC_END)

#LIBS+=$(GC_LIB)

####################################################################

DEF_SOURCES = $(C_FILES)
DEF_SOURCES_DEPS =
DEF_FILES = 

MAKE_SAVE_I=YES
MAKE_SAVE_I=

%.o : %.c 
	if test '$*.c' -nt '$*.c.llcc/_' -o ! -e '$*.c.llcc/_'; then make '$*.c.llcc/_' $(COLLECT); fi
ifneq "$(MAKE_SAVE_I)" ""
	$(CPP) $(CPPFLAGS) -CC $*.c | sed -e 's/^#.*//' -e 's/;/;@/g' | tr '@' '\012' > $*.i
	$(CC) $(CFLAGS) -Wa,-adlh -c -S -o $*.s $*.i >$*.sc
	$(CC) $(CFLAGS) -Wa,-adlh -c -o $*.o $*.s >$*.oc
else
	$(CC) $(CFLAGS) -Wa,-adlh -c -o $@ $*.c >$*.oc || ( rm -rf '$*.c.llcc'; exit 1 )
	make '$*.c.llcc/_' $(COLLECT)
endif


%.i : %.c 
	if test '$*.c' -nt '$*.c.llcc/_' -o ! -e '$*.c.llcc/_'; then make '$*.c.llcc/_' $(COLLECT); fi
	$(CPP) $(CPPFLAGS) $*.c | sed -e 's/;/;@/g' | tr '@' '\012' > $@

%.s : %.c 
	if test '$*.c' -nt '$*.c.llcc/_' -o ! -e '$*.c.llcc/_'; then make '$*.c.llcc/_' $(COLLECT); fi
	$(CPP) $(CPPFLAGS) $*.c | sed -e 's/;/;@/g' | tr '@' '\012' > $*.i
	$(CC) $(CFLAGS) -c -S -o $*.s $*.i

O_FILES = $(C_FILES:.c=.o) 

########################################################################

ifeq "$(strip $(shell uname))" "Linux"
EXE=
else
EXE=.exe
endif

########################################################################

all :

ifneq "$(strip $(v))" ""
v :
	@$(foreach x,$(v),echo $(x)='$($(x))';)
endif

########################################################################

ll_COLLECT_FILES = \
	types.c \
	globals.c \
        debugs.c \
	prims.c \
        macros.c \
	ops.c \
	symbols.c \
	inits.c

ll_COLLECT_FILES_LLCC=$(ll_COLLECT_FILES:%=%.llcc/_)#

LLCC_FILES_BASE = $(C_FILES:=.llcc/_) $(H_FILES:=.llcc/_) 
LLCC_FILES = $(LLCC_FILES_BASE) $(ll_COLLECT_FILES_LLCC)

########################################################################

PRODUCTS = \
  llt$(EXE) \
  libll.a

all : $(EARLY_H) .lldefs $(LLCC_FILES) $(PRODUCTS)

LLT:=$(CWD)/llt$(EXE)#

libll.a : .lldefs $(ll_COLLECT_FILES_LLCC) $(O_FILES) $(DEFS) $(GC_DEPS)
	ar cr $@ $(O_FILES) 
	ranlib $@
#	$(LLCC) --collect

llt$(EXE) : .lldefs llt.c $(DEFS) $(ll_COLLECT_FILES) libll.a 
	$(CC) $(CFLAGS) -o $@ llt.c $(ll_COLLECT_FILES) -L . -l ll $(LIBS) $(GC_LIB)
	-cp -p $@ $@-save$(EXE)
	-cp -p $@ $@.strip$(EXE)
	-strip $@.strip$(EXE)


########################################################################

$(O_FILES) : .lldefs ll.h $(DEFS) $(DEF_DIR)/config.h type_defs.h runtime_type.h
read.o : lispread.c
ccont.o : ccont.c ../ccont/ccont.c
string.o : vec.c
vector.o : vec.c
fixnum.o : num.c
flonum.o : num.c
symbol.o : ../hash/hash.c ../hash/hash.def ../hash/hash.h
bcompile.o : $(DEF_DIR)/bcs.h
bmethod.o : $(DEF_DIR)/bcs.h
runtime.o : $(DEF_DIR)/types.h runtime_type.h runtime.h

#######################################################################
# EARLY_H
#

../util/signals.h :
	cd ../util && $(MAKE) signals.h

$(DEF_DIR)/config.h : $(CONFIG_MAK)
	perl -nl config.pl < $(CONFIG_MAK) > $@

$(DEF_DIR)/floatcfg.h : $(DEF_DIR)/config.h value.h floatcfg.c
	$(CC) $(CFLAGS) floatcfg.c -lm -o floatcfg$(EXE)
	./floatcfg -v > $@
	#rm -f floatcfg$(EXE)

########################################################################
# Early DEFS
#

%.llcc/_ : %
	$(LLCC_COMPILE) $*
	touch -f $@

$(ll_COLLECT_FILES_LLCC) : $(LLCC_FILES_BASE)

.lldefs : 
	$(MAKE) .lldefs2

.lldefs2 : $(LLCC_FILES)
	$(LLCC_COMPILE) op1.h
	$(LLCC_COMPILE) global1.h
	$(LLCC_COMPILE) symbol1.h
	$(LLCC) --collect
	touch $@ .lldefs


########################################################################
# Collected DEFS
#

DEFS : $(DEFS) 

COLLECT :
	$(LLCC) --collect
	$(MAKE) DEFS

$(DEFS) : $(CONFIG_MAK) # defs.sh defs.pl llcc

$(DEF_DIR)/errors.h :
	$(LLCC) - def error --collect
types.o : $(DEF_DIR)/errors.h

$(DEF_DIR)/prims.h : 
	$(LLCC) - def prim --collect
prims.o : $(DEF_DIR)/prims.h

$(DEF_DIR)/macros.h :
	$(LLCC) - def macro --collect
macros.o : $(DEF_DIR)/macros.h $(DEF_DIR)/symbols.h

$(DEF_DIR)/ops.h : op1.h.llcc/_ 
	$(LLCC) - def op --collect
ops.o : $(DEF_DIR)/ops.h type_defs.h

$(DEF_DIR)/debugs.h : 
	$(LLCC) - def debug --collect
debugs.o : $(DEF_DIR)/debugs.h $(DEF_DIR)/globals.h

$(DEF_DIR)/globals.h : global1.h.llcc/_
	$(LLCC) - def global --collect
globals.o : $(DEF_DIR)/globals.h

$(DEF_DIR)/symbols.h : symbol1.h.llcc/_
	$(LLCC) - def symbol --collect
symbols.o : $(DEF_DIR)/symbols.h

$(DEF_DIR)/bcs.h : bmethod.c
	$(MAKE_DEFS) - o $@ - f ll_bc - t ll_bc_def bmethod.c
bmethod.o : $(DEF_DIR)/bcs.h

$(DEF_DIR)/inits.h : 
	$(LLCC) - def init --collect
inits.o : $(DEF_DIR)/inits.h $(DEF_DIR)/symbols.h

$(DEF_DIR)/types.h :
	$(LLCC) - def type --collect
#types.o : $(DEF_DIR)/types.h
types.o : $(DEF_DIR)/types.h

env.o : $(DEFS)

########################################################################

ifeq "$(GC_TARGET)" "gc_boehm"

gc_boehm : $(GC)/libgc.la

$(GC)/Makefile : $(GC)/configure
	cd $(dir $@) && ./configure

$(GC)/libgc.la : $(GC)/Makefile
	$(MAKE) -C $(dir $@)

endif

########################################################################

gdb : all
	gdb -x llt.gdb llt

########################################################################

LL_LOAD_PATH:=$(CWD)/lib/ll
export LL_LOAD_PATH

#LL_DEBUG_COMPILE_EMIT=10
#export LL_DEBUG_COMPILE_EMIT

test : # all
	sh ./test_suite.sh

TAK=lib/ll/test/tak.scm

speed_test: all
	sh ./speed_test.sh $(TAK)

speed_test_clean :
	rm -f ./tak.*

########################################################################

TAGS : $(C_FILES) $(H_FILES)
	ctags-exuberant -f $@ -h ".h" $(C_FILES) $(H_FILES)


########################################################################

clean : speed_test_clean
	-mv llt$(EXE) llt-save$(EXE)
	rm -f $(PRODUCTS) TAGS llt$(EXE) llt.strip$(EXE) floatcfg$(EXE) *core *.a *.o *.i *.s *.exe [0-9]*.tmp *~ .#*
	rm -f *.gcda *.gcno *.gcov *.oc

cleandefs :
	rm -f $(DEFS) 

cleanllcc :
	rm -rf $(DEFS:%=%.llcc)

veryclean : clean cleandefs
	rm -f .lldefs .lldefs2 .def .gc
	find . -name '*.llcc' | xargs rm -rf 

distclean : veryclean
	rm -f llt-save$(EXE)

########################################################################
#EOF
