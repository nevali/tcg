AC_DEFUN([TCG_CHECK_LIBJPEG],[

AC_ARG_WITH([libjpeg],
[AS_HELP_STRING([--with-libjpeg=PATH],[Specify installation prefix of libjpeg])],
[], [with_libjpeg=auto])

old_CPPFLAGS="$CPPFLAGS"
old_LDFLAGS="$LDFLAGS"
old_LIBS="$LIBS"

AC_SUBST([LIBJPEG_CPPFLAGS])
AC_SUBST([LIBJPEG_LDFLAGS])
AC_SUBST([LIBJPEG_LIBS])

case "$with_libjpeg" in
	yes|no|auto)
		;;
	*)
		LIBJPEG_CPPFLAGS="-I$with_libjpeg/include"
		CPPFLAGS="$CPPFLAGS $LIBJPEG_CPPFLAGS"
		LIBJPEG_LDFLAGS="-L$with_libjpeg/lib"
		LDFLAGS="$LDFLAGS $LIBJPEG_LDFLAGS"
		;;
esac

if ! test x"$with_libjpeg" = x"no" ; then
   have_libjpeg=no
   have_libjpeg_header=no
   AC_CHECK_HEADER([jpeglib.h],[have_libjpeg_header=yes],[
		if ! test x"$with_libjpeg" = x"auto" ; then
		   AC_MSG_ERROR([cannot find the libjpeg header, jpegio.h. You can specify an installation prefix to libjpeg with --with-libjpeg=PREFIX])
		fi])
   if test x"$have_libjpeg_header" = x"yes" ; then
   	  AC_CHECK_LIB([jpeg],[jpeg_mem_init],[LIBJPEG_LIBS="-ljpeg"],[
		AC_MSG_ERROR([cannot find the libjpeg library. You can specify an installation prefix to libjpeg with --with-libjpeg=PREFIX])])
	  have_libjpeg=yes
   fi
   if test x"$have_libjpeg" = x"yes" ; then
      AC_DEFINE_UNQUOTED([WITH_LIBJPEG],[1],[Define if libjpeg is available])
   fi
fi

CPPFLAGS="$old_CPPFLAGS"
LDFLAGS="$old_LDFLAGS"
LIBS="$old_LIBS"
])
