#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([HPCMetaMorpheus], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_MACRO_DIR(m4)
m4_include([config/compiler_checks.m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_MAKE_SET

SUBCONFIGURE_FLAGS="CXX=$CXX "

AC_MSG_CHECKING([if want developer-level debugging flags])
AC_ARG_ENABLE(debug,
  AC_HELP_STRING([Enable debug flags for compilation (default=yes)]))
if test "$enable_debug" = yes ; then
   SUBCONFIGURE_FLAGS="$SUBCONFIGURE_FLAGS --enable-debug"
   AC_MSG_RESULT([yes])
   CXXFLAGS="-g -O0"
   CFLAGS="-g -O0"
elif test "$enable_debug" = no ; then   
   SUBCONFIGURE_FLAGS="$SUBCONFIGURE_FLAGS --disable-debug"
   AC_MSG_RESULT([no]) 
   CXXFLAGS="-O3"
   CFLAGS="-O3"
else
   AC_MSG_RESULT([not set (implicit yes)]) 
   CXXFLAGS="-g -O0"
   CFLAGS="-g -O0"
fi 


AC_LANG_PUSH([C++])
GET_CXX_COMPILER_VENDOR(compiler_vendor)
if test "$compiler_vendor" = "gnu" ; then
  CXXFLAGS="$CXXFLAGS -Wno-delete-non-virtual-dtor -std=gnu++17"
elif test "$compiler_vendor" = "intel" ; then
  CXXFLAGS="$CXXFLAGS -std=c++17"
fi

PROG_CXX_CXX17_HELPER([],[have_cxx17_support])
if test "x$have_cxx17_support" = xno ; then
   AC_MSG_WARN([HPCMetaMorpheus requires a C++ 2017 (or newer) compiler.])
   AC_MSG_ERROR([Aborting.])
fi

# Checks for libraries.

dnl ==================================================
AC_MSG_CHECKING(for HPCMetaMorpheus main directory)
AC_ARG_WITH(metamorpheus_dir,
 [  --with-metamorpheus-dir=dir     Main HPCMetaMorpheus directory (default=$PWD)],
     mmorpheus_dir="${withval}", mmorpheus_dir="")
         if test "x$mmorpheus_dir" = "x" ; then
                mmorpheus_dir=$PWD
         fi
     AC_MSG_RESULT(${mmorpheus_dir})
     AC_SUBST(mmorpheus_dir)
dnl ==================================================

AC_CHECK_HEADER([boost/math/special_functions/gamma.hpp],
[],
[AC_MSG_ERROR(Couldn't find boost math library)])
AC_CHECK_HEADER_STDBOOL

AC_LANG_POP([C++])
AC_LANG_PUSH([C])
# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([fesetround floor pow sqrt])

# Checks for header files.
AC_CHECK_HEADER([zlib.h],
[],
[AC_MSG_ERROR(Couldn't find zlib.h)])

AC_CHECK_HEADER([math.h],
[],
[AC_MSG_ERROR(Couldn't find math.h)])

AC_LANG_POP([C])
AC_LANG_PUSH([C++])
dnl ==================================================
AC_MSG_CHECKING(for xsdcxx)
AC_ARG_WITH(xsdcxx_dir,
 [  --with-xsdcxx-dir=dir          xsdcxx directory (default=/usr/include)],
      xsdcxx_dir="${withval}", xsdcxx_dir="")
      if test "x$xsdcxx_dir" = "x" ; then
                  xsdcxx_dir=/usr/include
      else
         SUBCONFIGURE_FLAGS="$SUBCONFIGURE_FLAGS --with-xsdcxx-dir=${withval}"
      fi
      AC_MSG_RESULT(${xsdcxx_dir})

     save_CXXFLAGS="$CXXFLAGS"
     save_CPPFLAGS="$CPPFLAGS"
     CXXFLAGS="$save_CXXFLAGS -I$xsdcxx_dir"
     CPPFLAGS="$save_CPPFLAGS -I$xsdcxx_dir"
     AC_CHECK_HEADER([xsd/cxx/config.hxx],
                     [],
                     [AC_MSG_ERROR(Couldn't find xsd/cxx/config.hxx)])

     CXXFLAGS="$save_CXXFLAGS"
     CPPFLAGS="$save_CPPFLAGS"
     AC_SUBST(xsdcxx_dir)
dnl ==================================================

AX_XERCESC

dnl ==================================================
AC_MSG_CHECKING(for libxml2)
AC_ARG_WITH(libxml2_dir,
 [  --with-libxml2-dir=dir          libxml2 directory (default=/usr/include/libxml2/)],
     libxml2_dir="${withval}", libxml2_dir="")
         if test "x$libxml2_dir" = "x" ; then
                libxml2_dir=/usr/include/libxml2
         else
            SUBCONFIGURE_FLAGS="$SUBCONFIGURE_FLAGS --with-libxml2-dir=${withval}"
         fi
     AC_MSG_RESULT(${libxml2_dir})

     save_CXXFLAGS="$CXXFLAGS"
     save_CPPFLAGS="$CPPFLAGS"
     CXXFLAGS="$save_CXXFLAGS -I$libxml2_dir" 
     CPPFLAGS="$save_CPPFLAGS -I$libxml2_dir" 
     AC_CHECK_HEADER([libxml/xmlreader.h],
     [],
     [AC_MSG_ERROR(Couldn't find libxml/xmlreader.h)])
     CXXFLAGS="$save_CXXFLAGS"
     CPPFLAGS="$save_CPPFLAGS"
     AC_SUBST(libxml2_dir)
dnl ==================================================

echo " "
AC_MSG_NOTICE(creating final output files)

#AC_CONFIG_FILES([Makefile.defs])
AC_SUBST(CXX)
AC_SUBST(CC)
AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)
AC_OUTPUT(Makefile.defs)

dnl ==================================================
dnl configure mzlib subdirectory

echo " " 
mzlib_dir="$mmorpheus_dir/mzlib"
mmorpheus_lib_dir="$mmorpheus_dir/lib"
AC_MSG_NOTICE(running configure in mzlib directory in $mzlib_dir )
SUBCONFIGURE_FLAGS="$SUBCONFIGURE_FLAGS --with-mzlib-dir=$mzlib_dir --with-mzlib-lib-dir=$mmorpheus_lib_dir"
eval cd ./mzlib && ./configure $SUBCONFIGURE_FLAGS && cd -

dnl done configuring mzlib
dnl ==================================================
