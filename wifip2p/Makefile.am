## Source directory

SUBDIRS = 

noinst_HEADERS = config.h

hostap_h_sources = $(top_srcdir)/hostap/src/common/wpa_ctrl.h
hostap_cc_sources = $(top_srcdir)/hostap/src/common/wpa_ctrl.c

if WIN32
hostap_cc_sources += $(top_srcdir)/hostap/src/utils/os_win32.c
else
hostap_cc_sources += $(top_srcdir)/hostap/src/utils/os_unix.c
endif

h_sources = wifip2p.h SupplicantHandle.h CoreEngine.h WifiP2PInterface.h Timer.h Connection.h Peer.h NetworkIntf.h
cc_sources = SupplicantHandle.cpp CoreEngine.cpp WifiP2PInterface.cpp Timer.cpp Connection.cpp Peer.cpp NetworkIntf.cpp

library_includedir=$(includedir)/$(GENERIC_LIBRARY_NAME)-$(GENERIC_API_VERSION)/$(GENERIC_LIBRARY_NAME)
library_include_HEADERS = $(hostap_h_sources) $(h_sources)

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/hostap/src -I$(top_srcdir)/hostap/src/utils -DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX
AM_LDFLAGS = 

wifip2p_libs = 

lib_LTLIBRARIES= libwifip2p.la
libwifip2p_la_SOURCES= $(hostap_h_sources) $(h_sources) $(hostap_cc_sources) $(cc_sources)
libwifip2p_la_LIBADD= $(wifip2p_libs)
libwifip2p_la_LDFLAGS= -version-info $(GENERIC_LIBRARY_VERSION) -release $(GENERIC_RELEASE)
