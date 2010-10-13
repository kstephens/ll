# $Id: Makefile,v 1.4 2008/01/01 10:55:20 stephens Exp $

GC_VERSION=7.2alpha4#
GC=gc-$(GC_VERSION)#

MAKE_GC=$(MAKE) -C $(GC)
all : gc

clean distclean:	
	-[ -f $(GC)/Makefile ] && $(MAKE_GC) $@
	if [ -d $(GC) ]; then cd $(GC) && find . -name '.deps' -o -name '.libs' | xargs rm -rf; fi
	if [ -d $(GC) ]; then cd $(GC) && find . -name '*.o' -o -name '*.lo' -o -name '*.Plo' | xargs rm -rf; fi

veryclean : distclean
	rm -rf $(GC)/

gc : $(GC)/.libs/libgc.a

$(GC)/.libs/libgc.a : $(GC).tar.gz
	if [ ! -d $(GC) ]; then tar -zxvf $^; fi
	cd $(GC) && if [ ! -f Makefile ]; then ./configure --disable-threads; fi
	cd $(GC) && $(MAKE)



