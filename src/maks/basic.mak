# $Id: basic.mak,v 1.9 2001/04/04 09:09:10 stephens Exp $

Makefile=Makefile
Makefile.use=Makefile.use

##########################################################################
# Suffix translation rules

# SWIG

SWIG.swig=swig

$(SWIG_C_FILES_GEN) : $(GEN_FILE_DIR)/$(notdir %)_swig$(c) : %$(swig)
	$(foreach w,$(SWIG_WRAPPER),$(SWIG.swig) -$w $(SWIG_WRAPPER_$w) -o $(GEN_FILE_DIR)/$(notdir $*)_swig_$(w)$(c) $<;)
	touch $(GEN_FILE_DIR)/$(notdir $*)_swig$(c)

# C
%$(c):%$(o)

$(C_O_FILES_GEN) : $(O_FILE_DIR)/$(notdir %)$(o) : %$(c)
	$(COMPILE.c) $< -o $(O_FILE_DIR)/$(notdir $*)$(o)

$(C_GEN_O_FILES_GEN) : $(O_FILE_DIR)/%$(o) : $(GEN_FILE_DIR)/%$(c)
	$(COMPILE.c) $< -o $(O_FILE_DIR)/$*$(o)


# C++
%$(cc):%$(o)

$(CC_O_FILES_GEN) : $(O_FILE_DIR)/$(notdir %)$(o) : %$(cc)
	$(COMPILE.cc) $< -o $(O_FILE_DIR)/$(notdir $*)$(o)

# yacc
%$(y):%$(o)
%$(y):%$(c)

ifeq "$(strip $(MACHTYPE))" "i586-pc-cygwin32"
YACC+= -S '$(CYGFS)/share/bison.simple'
else
YACC=bison --yacc
endif

$(Y_C_FILES_GEN) : $(GEN_FILE_DIR)/$(notdir %)$(c) : %.y
	$(CP) $*$(y) $(GEN_FILE_DIR)/$(notdir $*)$(y)
	cd $(GEN_FILE_DIR) && $(YACC) -d $(notdir $*)$(y)
	$(MV) $(GEN_FILE_DIR)/y.tab.c $(GEN_FILE_DIR)/$(notdir $*)$(c)
	$(MV) $(GEN_FILE_DIR)/y.tab.h $(GEN_FILE_DIR)/$(notdir $*).yh

# Java
ifneq "$(strip $(JAVA_FILES_ALL))" ""

ifeq "$(strip $(JAVA_TIMESTAMP))" ""
JAVA_TIMESTAMP=$(JAVA_CLASS_FILE_DIR)/../.jc
endif

$(JAVA_TIMESTAMP) : $(JAVA_FILES_ALL)
	'$(JAVAC)' $(JAVACFLAGS) -d $(JAVA_CLASS_FILE_DIR) $(JAVA_FILES_ALL)
	$(TOUCH) $@

PRODUCTS+=$(JAVA_TIMESTAMP)
GARBAGE_DIRS+=$(JAVA_CLASS_FILE_DIR)
endif

#DIRS += $(GEN_FILE_DIR)

##########################################################################

EARLY_TARGETS+= USE_ALL DIRS $(Y_C_FILES_GEN) DEPS

##########################################################################

all : $(EARLY_TARGETS) $(PRODUCTS) $(TARGETS)
	@$(MSG) Made $@

##########################################################################
# Autodependencies

# Header deps
#$(O_FILES_GEN) : $(H_FILES)

DEP_FILE_DIR:=$(O_FILE_DIR)

make_dep = $(ECHO) "Making include deps for $<"; $(SHELL) -ec 'o="$(notdir $*)$(o)"; od="$(O_FILE_DIR)/$$o"; d="$(DEP_FILE_DIR)/$(notdir $*).d"; $(CC) -MM $(CPPFLAGS) $< | sed -e "s@$$o[ ]*:@$$od : $(Makefile) $(Makefile.use) @g" > $$d; [ -s $$d ] || rm -f $$d'

C_D_FILES += $(_C_FILES_ALL:%.c=$(DEP_FILE_DIR)/$(notdir %).d)
$(C_D_FILES) : $(DEP_FILE_DIR)/$(notdir %).d : %.c
	$(make_dep)

CC_D_FILES += $(_CC_FILES_ALL:%.cc=$(DEP_FILE_DIR)/$(notdir %).d)
$(CC_D_FILES) : $(DEP_FILE_DIR)/$(notdir %).d : %.cc
	@$(make_dep)

DEPS+= $(C_D_FILES) $(CC_D_FILES)
# Disable deps
DEPS:=

DEPS : DIRS $(DEPS)

.PHONY : DEPS

GARBAGE+= $(DEPS) 

GENERATED_DIRS+= $(DEP_FILE_DIR)
GARBAGE_DIRS+= $(DEP_FILE_DIR)

ifeq "$(strip $(__DISABLE_DEPS__))" ""
EARLY_TARGETS+= DEPS

ifneq "$(strip $(DEPS))" ""
$(DEPS) : $(DEP_FILE_DIR)
-include $(DEPS)
endif

endif

##########################################################################
# Use

USE_ALL :
	@for f in $(USE_ALL) . ; \
	do \
	   [ "$$f" = . ] && break; \
	   $(ECHO) "Using '$$f'" ;\
	   $(MAKE) -C "$$f" > /dev/null ; \
	done

.PHONY : USE_ALL

##########################################################################
# Directories

DIRS+= $(GENERATED_DIRS) $(OUTPUT_DIRS)
DIRS:=$(sort $(DIRS))
$(DIRS) :	
	$(MKDIRS) $@

DIRS : $(DIRS)

.PHONY : DIRS

##########################################################################
# Clean

GARBAGE += $(O_FILES_GEN)
GARBAGE += $(C_FILES_GEN)
GARBAGE += $(PRODUCTS)
GARBAGE += core *.core
#GARBAGE += *.o

clean :
	rm -f $(filter-out .,$(sort $(GARBAGE)))
	rm -rf $(filer-out .,$(sort $(GARBAGE_DIRS)))
	@$(MSG) Made $@

cleanproducts : 
	rm -f $(PRODUCTS)

veryclean : clean
	rm -rf $(filter-out .,$(sort $(GENERATED_DIRS) $(GARBAGE_VERY_DIRS)))
	@$(MSG) Made $@

archiveclean : veryclean
	@$(MSG) Made $@

.PHONY : clean veryclean archiveclean cleanproducts

##########################################################################
# Archive

archive : tgz

tgz : archiveclean
	pwd=`pwd`;dname=`basename $$pwd`;cd ..; $(TAR) -cvf - "$$dname" | $(GZIP) > "$$dname.tgz"

.PHONY : archive tgz


#########################################################################
# Variable make

ifdef vm
_vm : $(foreach f,$(vm), $($(f)))
	@$(MSG) Made $(vm): $<
.PHONY : _vm
endif

##########################################################################
# EOF
