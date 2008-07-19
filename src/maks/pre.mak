# $Id: pre.mak,v 1.10 2001/03/17 20:20:04 stephens Exp $

include $(MAKS)/tools.mak

#########################################################################
# Variable query
ifdef v
_v : 
	@$(foreach f,$(v),$(ECHO) "$(f)='$($(f))'"; )
.PHONY : _v
endif

#########################################################################
# Variable make

ifdef vm
_vm : 
endif

#########################################################################
# Default targets

all :
.PHONY : all clean

#########################################################################
# Build Options

ifndef DEBUG
DEBUG=YES
endif
export DEBUG
ifndef OPTIMIZE
OPTIMIZE=NO
endif
export OPTIMIZE

#########################################################################
# Disable RCS and SCCS get

% : %,v

% : RCS/%,v

% : s.%

% : SCCS/s.%

%.mak :

%.use :

Makefile :


#########################################################################
# Config

CWD=$(shell pwd)
export CWD

ifeq "$(strip $(MAKE_ARCH))" ""
MAKE_ARCH:=$(shell uname)
export MAKE_ARCH
endif

ifeq "$(strip $(DEV_ROOT))" ""
DEV_ROOT:=$(HOME)
export DEV_ROOT
endif
#DEV_ROOT:=$(strip $(shell cd "$(DEV_ROOT)" && pwd))

#########################################################################
# Directories

# libraries go in LIB_DIR
ifeq "$(strip $(LIB_DIR))" ""
LIB_DIR:=$(DEV_ROOT)/lib/$(MAKE_ARCH)
export LIB_DIR
endif
OUTPUT_DIRS+= $(LIB_DIR)

# executables go in TOOL_DIR
ifeq "$(strip $(TOOL_DIR))" ""
TOOL_DIR:=$(DEV_ROOT)/bin/$(MAKE_ARCH)
export TOOL_DIR
endif
OUTPUT_DIRS+= $(TOOL_DIR)

ifeq "$(strip $(GEN_FILE_ROOT))" ""
GEN_FILE_ROOT:=mak_gen
GARBAGE_VERY_DIRS:=$(GARBAGE_VERY_DIRS) $(GEN_FILE_ROOT)
endif

# test executables go in TEST_DIR
ifeq "$(strip $(TEST_DIR))" ""
TEST_DIR:=$(GEN_FILE_ROOT)/$(MAKE_ARCH)/t
export TEST_DIR
endif
OUTPUT_DIRS+= $(TEST_DIR)

# object files go in O_FILE_DIR
ifeq "$(strip $(O_FILE_DIR))" ""
O_FILE_DIR:=$(GEN_FILE_ROOT)/$(MAKE_ARCH)/o
export O_FILE_DIR
endif
#VPATH:=$(VPATH) $(O_FILE_DIR)#

# Java class files go in JAVA_CLASS_FILE_DIR
ifeq "$(strip $(JAVA_CLASS_FILE_DIR))" ""
JAVA_CLASS_FILE_DIR:=$(GEN_FILE_ROOT)/jvm
export JAVA_CLASS_FILE_DIR
endif

ifeq "$(DEBUG)" "YES"
JAVACFLAGS+=-g
CFLAGS+= $(DEBUG_FLAGS)
CXXFLAGS+= $(DEBUG_FLAGS)
O_FILE_DIR:=$(strip $(O_FILE_DIR)d)
endif
ifeq "$(OPTIMIZE)" "YES"
JAVACFLAGS+=-O
CFLAGS+= $(OPTIMIZE_FLAGS)
CXXFLAGS+= $(OPTIMIZE_FLAGS)
O_FILE_DIR:=$(strip $(O_FILE_DIR)o)
endif

GENERATED_DIRS+= $(O_FILE_DIR)
#

# yacc, lex and other tools put there output in GEN_FILE_DIR

ifeq "$(strip $(GEN_FILE_DIR))" ""
GEN_FILE_DIR:=$(MAKE_ARCH)/g
GEN_FILE_DIR:=$(O_FILE_DIR)
export GEN_FILE_DIR
endif
GENERATED_DIRS+= $(GEN_FILE_DIR)

#EARLY_TARGETS+= $(DIRS)

# Expect to be using some libs generated by us
LIB_DIRS+= $(LIB_DIR)

#########################################################################
# Suffixes

exe=
o=.o
c=.c
cc=.cc
y=.y
yy=.yy
h=.h
hh=.h
java=.java
class=.class
swig=.swig

#########################################################################
# Default CC options

OPTIMIZE_FLAGS+= -O3
DEBUG_FLAGS+= -g
CFLAGS+= -Wall

#########################################################################
# Target-specifics
include $(MAKS)/os/$(MAKE_ARCH).mak

#########################################################################
# BUILD_VARS

ifndef BUILD_TARGET
export BUILD_TARGET:=$(MAKE_ARCH)
endif
BUILD_VARS+= BUILD_TARGET

ifndef BUILD_DATE
export BUILD_DATE:=$(shell date)
endif
BUILD_VARS+= BUILD_DATE

ifndef BUILD_HOST
export BUILD_HOST:=$(shell hostname)
endif
BUILD_VARS+= BUILD_HOST

ifndef BUILD_USER
export BUILD_USER:=$(USER)
endif
BUILD_VARS+= BUILD_USER

ifndef BUILD_ROOT
export BUILD_ROOT:=$(CWD)
endif
BUILD_VARS+= BUILD_ROOT

_DEFINES:= $(foreach v,$(BUILD_VARS),-D'$v="$(${v})"' )
DEFINE_FLAGS:=$(DEFINE_FLAGS) $(_DEFINES)

#########################################################################
# BUILD_VARS

build_vars_h = $(GEN_FILE_DIR)/build_vars.h

build_vars_h : $(dir $(build_vars_h))
	$(ECHO) '/* DO NOT MODIFY Generated by maks/pre.mak. */' > $(build_vars_h)
	$(foreach v,$(BUILD_VARS),$(ECHO) '#define $(v) "$($(v))"' >> $(build_vars_h); )
	@$(MSG) Made $@

EARLY_TARGETS+= build_vars_h
GARBAGE+= $(build_vars_h)

#########################################################################
# Defines

DEFINE_FLAGS+= $(foreach f,$(DEFINES), -D'$(f)')

#########################################################################
# Includes

INCLS+= $(O_FILE_DIR)

INCL_FLAGS+= $(foreach f,$(INCLS), -I'$(f)')

CPPFLAGS+= $(DEFINE_FLAGS) $(INCL_FLAGS) 

#########################################################################
# Libs

LIB_FLAGS+= $(foreach f,$(LIB_DIRS), -L'$(f)')
LIB_FLAGS+= $(foreach f,$(LIBS), -l'$(f)')

#########################################################################
# version

#########################################################################
# yacc

Y_FILES_ALL+= $(Y_FILES) $(Y_FILES_GEN)
_Y_FILES_ALL=$(notdir $(Y_FILES_ALL))
Y_C_FILES_GEN+= $(_Y_FILES_ALL:%$(y)=$(GEN_FILE_DIR)/$(notdir %)$(c))
C_FILES_GEN+= $(Y_C_FILES_GEN)

#########################################################################
# swig

SWIG_WRAPPER=tcl8#
SWIG_WRAPPER_tcl8=-namespace -module $(notdir $*)
SWIG_FILES_ALL+= $(SWIG_FILES) $(SWIG_FILES_GEN)
_SWIG_FILES_ALL=$(notdir $(SWIG_FILES_ALL))
SWIG_C_FILES_GEN+= \
$(_SWIG_FILES_ALL:%$(swig)=$(GEN_FILE_DIR)/$(notdir %)_swig$(c)) 
SWIG_WRAPPER_C_FILES_GEN+= \
$(foreach w,$(SWIG_WRAPPER),$(_SWIG_FILES_ALL:%$(swig)=$(GEN_FILE_DIR)/$(notdir %)_swig_$(w)$(c)))
C_FILES_GEN+= $(SWIG_C_FILES_GEN) $(SWIG_WRAPPER_C_FILES_GEN)

#########################################################################
# C

C_FILES_ALL+= $(C_FILES)# $(C_FILES_GEN)
_C_FILES_ALL=$(notdir $(C_FILES_ALL))
C_O_FILES_GEN+= $(_C_FILES_ALL:%$(c)=$(O_FILE_DIR)/$(notdir %)$(o))
C_GEN_O_FILES_GEN+= $(C_FILES_GEN:%$(c)=%$(o))
O_FILES_GEN+= $(C_O_FILES_GEN) $(C_GEN_O_FILES_GEN)

#########################################################################
# C++

CC_FILES_ALL+= $(CC_FILES) $(CC_FILES_GEN)
_CC_FILES_ALL=$(notdir $(CC_FILES_ALL))
CC_O_FILES_GEN+= $(_CC_FILES_ALL:%$(cc)=$(O_FILE_DIR)/$(notdir %)$(o))
O_FILES_GEN+= $(CC_O_FILES_GEN) 

#########################################################################
# Java

JAVA_FILES_ALL+= $(JAVA_FILES) $(JAVA_FILES_GEN)
_JAVA_FILES_ALL=$(notdir $(JAVA_FILES_ALL))
JAVA_CLASS_FILES_GEN+= $(_JAVA_FILES_ALL:%$(java)=$(CLASS_FILE_DIR)/$(notdir %)$(class))

#########################################################################
# Object files

O_FILES+= $(O_FILES_GEN)

#########################################################################
# Use

-include Makefile.use

