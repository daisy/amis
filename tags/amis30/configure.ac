AC_INIT([Amis], [0.1], [Marisa and Jack])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LIBTOOL

dnl Testing for Xerces
dnl ======================================================================
test_xerces=false
AC_MSG_CHECKING(for xerces XML handling library)

AC_ARG_WITH(xerces,
	[  --with-xerces		Turn on/off use of Xerces XML library],
	[
    case "${withval}" in    
     yes) test_xerces=true;;
     no)  test_xerces=false;;
     *)   test_xerces=true
          XERCES_PREFIX=$withval
          ;;
    esac
])
dnl test for slurped version  first
dnl
if test "x$test_xerces" = "xtrue"; then
	if test "x$XERCES_PREFIX" = "x";	
	then
          XERCES_PREFIX=../../../xerces-c-src_2_7_0
	fi
fi	
if test "x$test_xerces" = "xtrue";
then if test -f $XERCES_PREFIX/include/xercesc/parsers/SAXParser.hpp;
    then
        XERCES_PREFIX=`cd $XERCES_PREFIX; pwd`
	    XERCES_LIBS="-L$XERCES_PREFIX/lib -lxerces-c"
	    XERCES_INCLUDES="-I$XERCES_PREFIX/include"
	    AC_DEFINE(WITH_XERCES, 1, [Define to enable xerces XML parser])
    else 
        AC_MSG_ERROR([xerces not found in $XERCES_PREFIX])
        test_xerces=false
        unset XERCES_PREFIX
        unset XERCES_LIBS
        unset XERCES_INCLUDES
     fi
else
    unset XERCES_PREFIX
    unset XERCES_LIBS
    unset XERCES_INCLUDES
fi
AC_SUBST(XERCES_PREFIX)
AC_SUBST(XERCES_LIBS)
AC_SUBST(XERCES_INCLUDES)
AM_CONDITIONAL(WITH_XERCES, test x$test_xerces = xtrue)
AC_MSG_RESULT($test_xerces)

dnl Testing for Python
dnl ======================================================================
AC_MSG_CHECKING(whether to build Python interface)
AC_ARG_WITH(python,
    [  --with-python		Build Python interface to Amis],
    [case "${withval}" in
        yes)
            PYTHON=`which python`
            with_python=true
            ;;
        no)
            PYTHON=
            with_python=false
            ;;
        *)
            PYTHON=`which ${withval}`
            with_python=true
            ;;
    esac], [
        with_python=false
    ])
AM_CONDITIONAL(WITH_PYTHON, test x$with_python = xtrue)
AC_SUBST(PYTHON)
AC_MSG_RESULT($with_python - $PYTHON)

dnl Testing for Ambulant
dnl =======================================================================
AC_MSG_CHECKING(where Ambulant sources are)
AC_ARG_WITH(ambulant,
	[  --with-ambulant		Tell us where Ambulant sources are],
	[
    case "${withval}" in    
     yes) test_ambulant=true;;
     no)  test_ambulant=false;;
     *)   test_ambulant=true
          AMBULANT_PREFIX=$withval
          ;;
    esac
])
dnl test for slurped version first
dnl
if test "x$test_ambulant" = "xtrue"; then
	if test "x$AMBULANT_PREFIX" = "x";	
	then
          AMBULANT_PREFIX=../../../ambulant
	fi
fi	
if test "x$test_ambulant" = "xtrue";
then if test -f $AMBULANT_PREFIX/include/ambulant/net/url.h;
    then
        AMBULANT_PREFIX=`cd $AMBULANT_PREFIX; pwd`
	    AMBULANT_INCLUDES="-I$AMBULANT_PREFIX/include"
	    AC_DEFINE(WITH_AMBULANT, 1, [Define to enable Ambulant])
    else 
        AC_MSG_ERROR([Ambulant not found in $AMBULANT_PREFIX])
        test_ambulant=false
        unset AMBULANT_PREFIX
        unset AMBULANT_INCLUDES
     fi
else
    unset AMBULANT_PREFIX
    unset AMBULANT_INCLUDES
fi
AC_SUBST(AMBULANT_PREFIX)
AC_SUBST(AMBULANT_INCLUDES)
AM_CONDITIONAL(WITH_AMBULANT, test x$test_ambulant = xtrue)
AC_MSG_RESULT($test_ambulant)

AC_CONFIG_FILES([
	Makefile
	AmisCore/Makefile
	AmisCore/testAmisCore/Makefile
	PyAmisCore/Makefile])
AC_OUTPUT
