#
# Usage:
#
# LIB_NAME:=<library-name>
# include $(MAKS)/lib.mak
#
# Adds targets to create a library named LIB_NAME from LIB_O_FILES.
# LIB_O_FILES defaults to O_FILES.
#

##############################################################
# Defaults

LIB_PREFIX:=lib#
LIB_SUFFIX:=.a#

LIB:=$(LIB_DIR)/$(LIB_PREFIX)$(LIB_NAME)$(LIB_SUFFIX)#

ifeq "$(strip $(LIB_O_FILES))" ""
LIB_O_FILES:=$(O_FILES)#
endif

ifeq "$(strip $(LIB_H_FILES))" ""
LIB_H_FILES:=$(H_FILES)#
endif

ifeq "$(strip $(LIB_LIB_FLAGS))" ""
LIB_LIB_FLAGS:=$(LIB_FLAGS)
endif

ifeq "$(strip $(LIB_LIBS))" ""
LIB_LIBS:=$(LIBS)
endif

##############################################################
# Target

ifneq "$(strip $(LIB_DYNAMIC))" ""


LIB_DLL_SUFFIX=.dll#
LIB_DLL:=$(LIB_DIR)/$(LIB_NAME)$(LIB_DLL_SUFFIX)#
LIB_DLL_DEF:=$(LIB_DIR)/$(LIB_NAME).def#


ifneq "$(strip $(SWIG2DEF))" ""
LIB_SWIG2DEF:=perl $(MAKS)/../bin/swig2def.pl $(SWIG2DEF) > $(LIB_DLL_DEF)#
LIB_DEF:=--def $(LIB_DLL_DEF)#
else
LIB_SWIG2DEF:=true#
LIB_DEF:=--output-def $(LIB_DLL_DEF)#
endif


$(LIB_DLL) $(LIB) : $(LIB_O_FILES)
	$(LIB_SWIG2DEF)
	dllwrap --verbose --output-lib $(LIB) $(LIB_DEF) --dllname $(LIB_DLL) $(LIB_O_FILES) $(LIB_LIB_FLAGS) $(foreach l,$(LIB_LIBS),-l'$(l)')
else

LIB_DLL:=#

$(LIB) : $(LIB_O_FILES)
	-$(RM) $@
	$(AR) cr $@ $(LIB_O_FILES)

endif

$(LIB_DLL) $(LIB) : LIB:=$(LIB)
$(LIB_DLL) $(LIB) : LIB_O_FILES:=$(LIB_O_FILES)
$(LIB_DLL) $(LIB) : LIB_DLL:=$(LIB_DLL)
$(LIB_DLL) $(LIB) : LIB_DLL_DEF:=$(LIB_DLL_DEF)
$(LIB_DLL) $(LIB) : LIB_SWIG2DEF:=$(LIB_SWIG2DEF)
$(LIB_DLL) $(LIB) : LIB_DEF:=$(LIB_DEF)
$(LIB_DLL) $(LIB) : LIB_LIB_FLAGS:=$(LIB_LIB_FLAGS)
$(LIB_DLL) $(LIB) : LIB_LIBS:=$(LIB_LIBS)


##############################################################
# Alias

.PHONY : $(LIB_NAME)
$(LIB_NAME) : $(LIB)

##############################################################
# Exports

LOCAL_LIBS:=$(LOCAL_LIBS) $(LIB)#
LIB_PRODUCTS:=$(LIB_PRODUCTS) $(LIB) $(LIB_DLL)#
PRODUCTS:=$(PRODUCTS) $(LIB_H_FILES) $(LIB) $(LIB_DLL)#

libs :: $(LIB_PRODUCTS)
	@$(MSG) Made $@: $(LIB_PRODUCTS)

.PHONY : libs

##############################################################
# Reset vars

LIB:=#
LIB_NAME:=#
LIB_O_FILES:=#
LIB_DLL:=#
SWIG2DEF:=#

##############################################################
# EOF
