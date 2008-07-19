# $Id: tool.mak,v 1.7 2000/03/21 08:21:48 stephensk Exp $

######################################################################
# Defaults

TOOL_SUFFIX:=$(exe)

_TOOL_DIR:=$(TOOL_DIR)

ifneq "$(TOOL_TEST)" ""
TOOL_DIR:=$(TEST_DIR)
endif

TOOL:=$(TOOL_DIR)/$(TOOL_NAME)$(TOOL_SUFFIX)

TOOL_DIR:=$(_TOOL_DIR)

ifeq "$(strip $(TOOL_MAIN))" ""
TOOL_MAIN:=$(TOOL_NAME).c
endif

ifeq "$(strip $(TOOL_O_FILES))" ""
TOOL_O_FILES:=$(TOOL_MAIN)
endif
TOOL_O_FILES_$(TOOL):=$(TOOL_O_FILES)

ifeq "$(strip $(TOOL_LIBS))" ""
TOOL_LIBS:=$(LIBS)
endif
TOOL_LIBS_$(TOOL):=$(TOOL_LIBS)

ifeq "$(strip $(TOOL_LIB_FLAGS))" ""
TOOL_LIB_FLAGS:=$(LIB_FLAGS)
endif
TOOL_LIB_FLAGS_$(TOOL):=$(TOOL_LIB_FLAGS)

######################################################################
# Target

$(TOOL) : $(TOOL_O_FILES_$(TOOL)) $(LOCAL_LIBS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o '$@' $(TOOL_O_FILES_$(@)) $(TOOL_LIB_FLAGS_$(@)) $(foreach l,$(TOOL_LIBS_$(@)),-l'$(l)')

#	$(CP) $@ $(notdir $@)

GARBAGE:=$(GARBAGE) $(notdir $(TOOL)) 

######################################################################
# Aliases

.PHONY : $(TOOL_NAME) $(notdir $(TOOL))
$(TOOL_NAME) $(notdir $(TOOL)) : $(TOOL)

######################################################################
# Export the target

TOOL_PRODUCTS:=$(TOOL_PRODUCTS) $(TOOL)
PRODUCTS:=$(PRODUCTS) $(TOOL)

tools :: $(TOOL_PRODUCTS)
	@$(MSG) Made $@: $(TOOL_PRODUCTS)

.PHONY : tools

######################################################################
# Reset vars

TOOL_NAME:=
TOOL_MAIN:=
TOOL_O_FILES:=
TOOL_LIBS:=
TOOL_LIB_FLAGS:=
TOOL_TEST:=

######################################################################
# EOF
