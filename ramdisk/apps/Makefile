CROSS_COMPILE   =   armv5-linux-
CC              = $(CROSS_COMPILE)gcc
TARGETDIR       = target
SUBDIRS         = busybox-1.21.0 ppp-2.4.5 iptables-1.4.19.1

subdirs: 
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

distclean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir distclean; \
	done

install: 
	cd iptables-1.4.19.1 && $(MAKE) install DESTDIR=$(CURDIR)/$(TARGETDIR)
    $(shell mkdir -p $(TARGETDIR)/bin)
    $(shell mkdir -p $(TARGETDIR)/usr/sbin)
    $(shell cp busybox-1.21.0/busybox       $(TARGETDIR)/bin/)
    $(shell cp ppp-2.4.5/pppd/pppd          $(TARGETDIR)/usr/sbin/)
    $(shell cp ppp-2.4.5/pppdump/pppdump    $(TARGETDIR)/usr/sbin/)
    $(shell cp ppp-2.4.5/pppstats/pppstats  $(TARGETDIR)/usr/sbin/)
    $(shell cp ppp-2.4.5/chat/chat          $(TARGETDIR)/usr/sbin/)
	#cd busybox-1.21.0 && $(MAKE) install CONFIG_PREFIX=$(CURDIR)/$(TARGETDIR)

