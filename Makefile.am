EXTRA_DIST = README

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = tcg

tcg_CPPFLAGS = $(WARNING_CPPFLAGS) $(LIBTIFF_CPPFLAGS) $(LIBJASPER_CPPFLAGS) $(LIBJPEG_CPPFLAGS) $(LIBPNG_CPPFLAGS) $(AM_CPPFLAGS)
tcg_LDFLAGS = $(LIBTIFF_LDFLAGS) $(LIBJASPER_LDFLAGS) $(LIBJASPER_LDFLAGS) $(LIBPNG_LDFLAGS) $(AM_LDFLAGS)
tcg_LDADD = $(LIBTIFF_LIBS) $(LIBJASPER_LIBS) $(LIBJPEG_LIBS) $(LIBPNG_LIBS) $(AM_LDADD)

tcg_SOURCES = main.c image.c draw.c output.c \
	colourmap.c convert.c \
	\
	ebu100.c ebu75.c ebu3325_1.c ebu3325_2.c ebu3325_5.c \
	blacklevel.c whitelevel.c \
	phase.c \
	\
	ycc444_16_planar.c ycc444_8_planar.c \
	tiff_ycc444_16.c tiff_ycc444_8.c tiff_y16.c \
	jp2.c \
	png_rgb_16.c
