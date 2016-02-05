## Process this file with automake to produce Makefile.in

## Created by Anjuta


uidir = $(pkgdatadir)/ui
ui_DATA = uniband_monitor.ui
confdir = $(pkgdatadir)/conf
conf_DATA = uniband-monitor.conf

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DPACKAGE_BIN_DIR=\""$(bindir)"\" \
	$(UNIBAND_MONITOR_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = suid \
	uniband_monitor

suid_SOURCES =  \
	suid.c

uniband_monitor_SOURCES = \
	main.c \
	callbacks.c \
	engine.c \
	read_options.c \
	card_handler.c \
	main.h \
	threads.c \
	signaldetails.c

uniband_monitor_LDFLAGS = \
	-Wl,--export-dynamic

uniband_monitor_LDADD = $(UNIBAND_MONITOR_LIBS)


EXTRA_DIST = $(ui_DATA) \
	$(conf_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(confdir)
	-rm -r $(pkgdatadir)
	
install-exec-hook:
	chmod u+s $(bindir)/suid
