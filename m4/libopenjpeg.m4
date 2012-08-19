AC_DEFUN([TCG_CHECK_LIBOPENJPEG],[
AC_REQUIRE([PKG_PROG_PKG_CONFIG])dnl
AC_ARG_VAR([JP2_CFLAGS],[C compiler flags for libopenjpeg, overriding pkg-config])dnl
AC_ARG_VAR([JP2_LIBS],[linker flags for libopenjpeg, overriding pkg-config])dnl

AC_MSG_CHECKING([for libopenjpeg])

_PKG_CONFIG([JP2_CFLAGS],[cflags],[libopenjpeg])
_PKG_CONFIG([JP2_LIBS],[libs],[libopenjpeg])

if test x"$pkg_failed" = x"yes" ; then
   AC_MSG_RESULT([no])
else
   AC_MSG_RESULT([yes])
   JP2_CFLAGS=$pkg_cv_JP2_CFLAGS
   JP2_LIBS=$pkg_cv_JP2_LIBS
   AC_DEFINE_UNQUOTED([WITH_JP2],[1],[Define if libopenjpeg is present])
fi
AC_SUBST([JP2_CFLAGS])
AC_SUBST([JP2_LIBS])
])
