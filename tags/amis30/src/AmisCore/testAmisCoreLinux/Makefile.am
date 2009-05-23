noinst_PROGRAMS = testAmisCore

INCLUDES = -I$(top_srcdir)/AmisCore/include \
    $(XERCES_INCLUDES) \
    $(AMBULANT_INCLUDES)
LDADD = $(top_builddir)/AmisCore/.libs/libAmisCore.a \
	$(XERCES_LIBS) \
	$(AMBULANT_PREFIX)/src/libambulant/.libs/libambulant.0.dylib
testAmisCore_SOURCES = testAmisCoreLinux.cpp

