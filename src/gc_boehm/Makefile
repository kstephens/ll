# $Id: Makefile,v 1.4 2008/01/01 10:55:20 stephens Exp $

GC_VERSION=7.0#
GC=gc-$(GC_VERSION)#

MAKE_GC=$(MAKE) -C $(GC)
all : 
	cd $(GC) && ./configure
	$(MAKE_GC) all

clean distclean:	
	-[ -f $(GC)/Makefile ] && $(MAKE_GC) $@
	cd $(GC) && find . -name '.deps' -o -name '.libs' | xargs --verbose rm -rf
	cd $(GC) && find . -name '*.o' -o -name '*.lo' -o -name '*.Plo' | xargs --verbose rm -rf

veryclean : distclean


