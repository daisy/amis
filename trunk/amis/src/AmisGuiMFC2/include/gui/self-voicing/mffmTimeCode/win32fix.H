/*
AMIS: Adaptive Multimedia Information System
Software for playing DAISY books
Homepage: http://amis.sf.net

Copyright (C) 2004-2007  DAISY for All Project

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* Copyright 2001 Matt Flax <flatmax@ieee.org>
   This file is part of MFFM Time Scale Modification for Audio.

   MFFM Time Scale Modification for Audio is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   MFFM Time Scale Modification for Audio is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You have received a copy of the GNU General Public License
   along with MFFM Time Scale Modification for Audio
 */
#ifndef WIN32FIX_H_
#define WIN32FIX_H_

// DLLs in Windows should use the standard calling convention
#ifndef ADR_CALL
  #if defined(WIN32) || defined(_WIN32)
    #define ADR_CALL __stdcall
  #else
    #define ADR_CALL
  #endif
#endif

// Export functions from the DLL
#ifndef ADR_DECL
#  if defined(WIN32) || defined(_WIN32)
#    ifdef LIBFFT_EXPORTS
#      define ADR_DECL __declspec(dllexport)
#    else
#      define ADR_DECL __declspec(dllimport)
#    endif
#  else
#    define ADR_DECL
#  endif
#endif

#define ADR_FUNCTION(ret) extern "C" ADR_DECL ret ADR_CALL

#ifndef round
#define round(a) roundD(a)
#endif

#ifndef bzero
#define bzero(a,c) memset(a,0,c)
#endif

#ifndef remainder
#define remainder(a,b) fmod(a,b)
#endif

#ifndef MAXDOUBLE
//#include <limits>
//#define MAXDOUBLE (numeric_limits<double>::max())
//#define MAXDOUBLE DBL_MAX
#define MAXDOUBLE 1e308
#endif

#ifndef MAXINT
//#include <limits>
//#define MAXINT (numeric_limits<int>::max())
#define MAXINT INT_MAX
#endif

#ifndef cerr
#include <iostream>
using namespace std;
//#define cerr std:cout
#endif
#endif
