#!/bin/sh
# Run Python test program conveniently.
# Linux library path:
cwd=`pwd`
export LD_LIBRARY_PATH="$cwd/../AmisCore/.libs:/Users/jack/packages/xerces-c-src_2_7_0/lib"
# MacOS library path:
export DYLD_LIBRARY_PATH="$cwd/../AmisCore/.libs:/Users/jack/packages/xerces-c-src_2_7_0/lib"
# Python path:
export PYTHONPATH=`echo $cwd/../PyAmisCore/build/lib.*`
# Enable for malloc debugging on MacOSX:
#export DYLD_INSERT_LIBRARIES=/usr/lib/libgmalloc.dylib
# Enable for running under gdb:
#GDB="gdb --args"
rm amiscore.so
ln -s ../PyAmisCore/build/lib.*/amiscore.so
./dist/MacAmis.app/Contents/MacOS/MacAmis $@
