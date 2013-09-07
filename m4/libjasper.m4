AC_DEFUN([TCG_CHECK_LIBJASPER],[

AC_ARG_WITH([libjasper],
[AS_HELP_STRING([--with-libjasper=PATH],[Specify installation prefix of libjasper])],
[], [with_libjasper=auto])

old_CPPFLAGS="$CPPFLAGS"
old_LDFLAGS="$LDFLAGS"
old_LIBS="$LIBS"

AC_SUBST([LIBJASPER_CPPFLAGS])
AC_SUBST([LIBJASPER_LDFLAGS])
AC_SUBST([LIBJASPER_LIBS])

case "$with_libjasper" in
	yes|no|auto)
		;;
	*)
		LIBJASPER_CPPFLAGS="-I$with_libjasper/include"
		CPPFLAGS="$CPPFLAGS $LIBJASPER_CPPFLAGS"
		LIBJASPER_LDFLAGS="-L$with_libjasper/lib"
		LDFLAGS="$LDFLAGS $LIBJASPER_LDFLAGS"
		;;
esac

if ! test x"$with_libjasper" = x"no" ; then
   have_libjasper=no
   have_libjasper_header=no
   AC_CHECK_HEADER([jasper/jasper.h],[have_libjasper_header=yes],[
		if ! test x"$with_libjasper" = x"auto" ; then
		   AC_MSG_ERROR([cannot find the libjasper header, jasperio.h. You can specify an installation prefix to libjasper with --with-libjasper=PREFIX])
		fi])
   if test x"$have_libjasper_header" = x"yes" ; then
   	  AC_CHECK_LIB([jasper],[jas_image_encode],[LIBJASPER_LIBS="-ljasper"],[
		AC_MSG_ERROR([cannot find the libjasper library. You can specify an installation prefix to libjasper with --with-libjasper=PREFIX])])
      have_libjasper=yes
   fi
   if test x"$have_libjasper" = x"yes" ; then
	   AC_DEFINE_UNQUOTED([WITH_LIBJASPER],[1],[Define if libjasper is available])
   fi
fi

CPPFLAGS="$old_CPPFLAGS"
LDFLAGS="$old_LDFLAGS"
LIBS="$old_LIBS"
])
