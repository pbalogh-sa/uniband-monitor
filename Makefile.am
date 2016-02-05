## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po

uniband_monitordocdir = ${prefix}/doc/uniband_monitor
uniband_monitordoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = $(uniband_monitordoc_DATA) \
	$(INTLTOOL_FILES)


DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(uniband_monitordocdir)
