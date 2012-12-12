## Source directory

h_sources = 
cc_sources = BaseTestMain.cpp

AM_CPPFLAGS = -I@top_srcdir@ -I@top_srcdir@/tests
AM_LDFLAGS = -L@top_srcdir@/wifip2p/.libs -lwifip2p

check_PROGRAMS = basetest
basetest_CXXFLAGS = ${AM_CPPFLAGS} ${CPPUNIT_CFLAGS} -Wall
basetest_LDFLAGS = ${AM_LDFLAGS} ${CPPUNIT_LIBS}
basetest_SOURCES = $(h_sources) $(cc_sources)

TESTS = basetest
