#
# This automake file is quite a bit of a hack, mainly because (a) automake
# and Python distutils aren't very friendly to each other and (b) distutils
# does not like to be run outside of the source directory.
#
# (a) is solved by pretty much ignoring automake and using the all-am and related
# targets to depend on targets that build and install through distutils.
# (b) is solved by running setup.py in the source directory. This should also
# work for building for multiple platforms, because distutils has its own
# platform structure under its build directory (but it will not work for building
# with multiple pythons, etc).
#

amiscorelibdir=$(top_builddir)/AmisCore/.libs

EXTRA_DIST=setup.py \
    bgenCxxSupport.py genobjects.py \
    amiscorescan.py amiscoresupport.py \
    amiscoremodule.h amiscoremodule.cpp
    
# Various flags for optional features


# Get all the extra bits together
EXTRA_CFLAGS=$(XERCES_INCLUDES)

# These targets are magic for automake

all-am: build-pyamiscore

check-am: check-pyamiscore

install-exec-hook: install-pyamiscore

uninstall-local: clean-pyamiscore

# This target re-creates the cpp files. Use only when you know what you
# are doing.

regen: regenerate-pyamiscore
   
BGENADDITIONS=$(srcdir)/bgenCxxSupport.py
BGENOUTPUT=$(srcdir)/amiscoremodule.cpp
BGENTEMP=$(srcdir)/amiscoregen.py
BGENINPUT=$(srcdir)/amiscorescan.py $(srcdir)/amiscoresupport.py
GENOBJOUTPUT=$(srcdir)/amiscoreobjgen.py $(srcdir)/amiscoreincludegen.py $(srcdir)/amiscoremodule.h

# Set these variables to change the way the tests are run
DEBUGGER=
#DEBUGGER=gdb --args
PYTHONARGS=
#PYTHONARGS=-i
PYTHONENV=
#PYTHONENV=DYLD_INSERT_LIBRARIES=/usr/lib/libgmalloc.dylib

BUILDLIBDIR=`echo ./build/lib.*`
	
build-pyamiscore: $(srcdir)/setup.py $(BGENOUTPUT) $(srcdir)/amiscoremodule.h
	( a=`cd $(amiscorelibdir); pwd` ; \
	  cd $(srcdir) ; \
	  LDFLAGS="-L$$a $(XERCES_LIBS)" CFLAGS="$(EXTRA_CFLAGS)" $(PYTHON) setup.py build ; \
	)

check-pyamiscore:
	( a=`cd $(amiscorelibdir); pwd` ; \
	  cd $(srcdir) ; \
	  LD_LIBRARY_PATH="$$a:$(XERCES_PREFIX)/lib" DYLD_LIBRARY_PATH="$$a:$(XERCES_PREFIX)/lib" $(PYTHONENV) PYTHONPATH=$(BUILDLIBDIR) $(DEBUGGER) $(PYTHON) $(PYTHONARGS) -c "import amiscore" ; \
	)

install-pyamiscore:
	cd $(srcdir) ; $(PYTHON) setup.py install
	
mostlyclean-am:
	-cd $(srcdir) ; $(PYTHON) setup.py clean
	-rm -r $(srcdir)/build

# Targets to regenerate amiscoremodule.cpp

regenerate-pyamiscore:
	cd $(srcdir) ; $(PYTHON) genobjects.py
	cd $(srcdir) ; $(PYTHON) amiscorescan.py
	
