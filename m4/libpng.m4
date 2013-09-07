AC_DEFUN([TCG_CHECK_LIBPNG],[
AC_REQUIRE([PKG_PROG_PKG_CONFIG])dnl
AC_ARG_VAR([LIBPNG_CPPFLAGS],[C compiler flags for libpng, overriding pkg-config])dnl
AC_ARG_VAR([LIBPNG_LIBS],[linker flags for libpng, overriding pkg-config])dnl

AC_MSG_CHECKING([for libpng])

_PKG_CONFIG([LIBPNG_CPPFLAGS],[cflags],[libpng])
_PKG_CONFIG([LIBPNG_LIBS],[libs],[libpng])

if test x"$pkg_failed" = x"yes" ; then
   AC_MSG_RESULT([no])
else
   AC_MSG_RESULT([yes])
   LIBPNG_CPPFLAGS=$pkg_cv_LIBPNG_CPPFLAGS
   LIBPNG_LIBS=$pkg_cv_LIBPNG_LIBS
   AC_DEFINE_UNQUOTED([WITH_LIBPNG],[1],[Define if libpng is present])
fi
AC_SUBST([LIBPNG_CPPFLAGS])
AC_SUBST([LIBPNG_LIBS])
])
