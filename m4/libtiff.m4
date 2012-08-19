AC_DEFUN([TCG_CHECK_LIBTIFF],[

AC_ARG_WITH([libtiff],
[AS_HELP_STRING([--with-libtiff=PATH],[Specify installation prefix of libtiff])],
[], [with_libtiff=auto])

old_CPPFLAGS="$CPPFLAGS"
old_LDFLAGS="$LDFLAGS"
old_LIBS="$LIBS"

AC_SUBST([LIBTIFF_CPPFLAGS])
AC_SUBST([LIBTIFF_LDFLAGS])
AC_SUBST([LIBTIFF_LIBS])

case "$with_libtiff" in
	yes|no|auto)
		;;
	*)
		LIBTIFF_CPPFLAGS="-I$with_libtiff/include"
		CPPFLAGS="$CPPFLAGS $LIBTIFF_CPPFLAGS"
		LIBTIFF_LDFLAGS="-L$with_libtiff/lib"
		LDFLAGS="$LDFLAGS $LIBTIFF_LDFLAGS"
		;;
esac

if ! test x"$with_libtiff" = x"no" ; then
   have_libtiff=no
   have_libtiff_header=no
   AC_CHECK_HEADER([tiffio.h],[have_libtiff_header=yes],[
		if ! test x"$with_libtiff" = x"auto" ; then
		   AC_MSG_ERROR([cannot find the libtiff header, tiffio.h. You can specify an installation prefix to libtiff with --with-libtiff=PREFIX])
		fi])
   if test x"$have_libtiff_header" = x"yes" ; then
   	  AC_CHECK_LIB([tiff],[TIFFOpen],[LIBTIFF_LIBS="-ltiff"],[
		AC_MSG_ERROR([cannot find the libtiff library. You can specify an installation prefix to libtiff with --with-libtiff=PREFIX])])
   fi

   AC_DEFINE_UNQUOTED([WITH_LIBTIFF],[1],[Define if libtiff is available])
fi

CPPFLAGS="$old_CPPFLAGS"
LDFLAGS="$old_LDFLAGS"
LIBS="$old_LIBS"
])
