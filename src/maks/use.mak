# $Id: use.mak,v 1.4 2001/03/21 02:04:14 stephens Exp $
#
# Use a package.
#
# USE:=<package-directory>
# include $(MAKS)/use.mak
#
# A package's Makefile.use contains the names of libraries
# and header directories needed to 'USE' the package.
#

#USE:=$(strip $(shell cd $(USE) && /bin/pwd)) # Canonicalize the name
error USE=$(USE)
ifeq "$(findstring $(USE),$(USE_ALL))" "" # Include it once
USE_ALL:=$(USE_ALL) $(USE) # Keep track of directories we USEd
-include $(USE)/Makefile.use
endif

