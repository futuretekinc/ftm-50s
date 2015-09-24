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
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir install DESTDIR=/home/xtra/work/ftm-50s/initrd/apps/target; \
	done

