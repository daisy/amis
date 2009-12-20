from distutils.core import setup, Extension
import sys
import os

# Set these variable identical to your configure options.
WITH_QT= not not os.getenv("QTDIR")
WITH_FFMPEG=True
WITH_SDL=True

DEFS =  [
    ]
    
EXTRA_LINK_ARGS=[]
ldflags=os.getenv("LDFLAGS")
##if ldflags:
##    EXTRA_LINK_ARGS=ldflags.split()
##if sys.platform == 'darwin':
##    EXTRA_LINK_ARGS += ['-framework', 'CoreFoundation', '-framework', 'Cocoa']

LIBRARIES=['AmisCore', 'ambulant']
##if WITH_QT:
##    DEFS.append(('WITH_QT', '1'))
##    LIBRARIES.append('ambulant_qt')
##if WITH_FFMPEG:
##    DEFS.append(('WITH_FFMPEG', '1'))
##    LIBRARIES.append('ambulant_ffmpeg')
##if WITH_SDL:
##    DEFS.append(('WITH_SDL', '1'))
##    LIBRARIES.append('ambulant_sdl')

LIBDIRS=[]
if sys.platform == 'darwin':
    LIBRARIES += ['intl']
    LIBDIRS += ['/usr/local/lib']

INCLUDEDIRS=['../AmisCore/include']
# Hack to cater for both Marisa's and Jack's setup:
if os.path.exists('../../ambulant'):
    # Jack
    INCLUDEDIRS+=['../../ambulant/third_party_packages/xerces-unix/src']
    INCLUDEDIRS+=['../../ambulant/include']
    LIBDIRS+=['../../ambulant/build-schil/src/libambulant/.libs']
else:
    Marisa
    INCLUDEDIRS+=['../../../../xerces-c-src_2_7_0/src']
    INCLUDEDIRS+=['../../../../ambulant/include']
    # XXXX Need to set correct LIBDIRS for ambulant here too

setup(name='amiscore',
      version='0.1',
      ext_modules=[
        Extension('amiscore',
            ['amiscoremodule.cpp'],
            libraries=LIBRARIES,
            extra_link_args=EXTRA_LINK_ARGS,
            library_dirs=LIBDIRS,
            include_dirs=INCLUDEDIRS,
            define_macros=DEFS
        )
      ]
)
