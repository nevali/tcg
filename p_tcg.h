/*
 * Copyright 2012 Mo McRoberts.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef P_TCG_H_
# define P_TCG_H_                       1

# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>
# include <unistd.h>
# include <errno.h>
# include <string.h>
# include <math.h>
# include <ctype.h>

# ifdef WITH_LIBTIFF
#  include "tiffio.h"
# endif

# ifdef WITH_LIBJASPER
/* Work around developers not knowing how autoconf works */
#  define JAS_CONFIG_H
#  include "jasper/jasper.h"
# endif

# ifdef WITH_LIBPNG
#  include "png.h"
# endif

# define PACKED                         __attribute__((packed))

# define MAX_PLANES                     4

# define PF_UNKNOWN                     -1
# define PF_RGB                         0
# define PF_RGBA                        1
# define PF_YCBCR                       2

/* Nominal range for Y:
 *
 * 8-bit:            0x10 (16)   ...   0xEB (235)
 * 10-bit:          0x040 (64)   ...  0x3AC (940)
 * 16-bit:         0x1000 (4096) ... 0xEB00 (60160) 
 *
 * Nominal range for Cb & Cr:
 *
 * 8-bit:           0x10 (16)    ...   0xF0 (240)
 * 10-bit:         0x040 (64)    ...  0x3C0 (960)
 * 16-bit:        0x1000 (4096)  ... 0xF000 (61440)
 *
 * Achromatic points for Cb & Cr:
 *
 * 8-bit:           0x80 (128)
 * 10-bit:         0x200 (512)
 * 16-bit:        0x8000 (32768)
 */

# define SUB_Y16                       0x0000
# define LOW_Y16                       0x1000
# define MID_Y16                       0x6d80
# define HIGH_Y16                      0xeb00
# define SUPER_Y16                     0xffff
# define RANGE_Y16                     (HIGH_Y16 - LOW_Y16)
# define STRETCH_Y16(y)                (((double) y - LOW_Y16) / RANGE_Y16 * SUPER_Y16)

# define SUB_CBCR16                    0x0000
# define LOW_CBCR16                    0x1000
# define MID_CBCR16                    0x7000
# define HIGH_CBCR16                   0xf000
# define SUPER_CBCR16                  0xffff
# define RANGE_CBCR16                  (HIGH_CBCR16 - LOW_CBCR16)
# define STRETCH_CBCR16(c)             (((double) c - LOW_CBCR16) / RANGE_CBCR16 * SUPER_CBCR16)

# define ABSMID_YCBCR16                0x8000

/* Colour conversion constants */
# define YCBCR_RGB_JFIF_K1             1.402
# define YCBCR_RGB_JFIF_K2             0.344
# define YCBCR_RGB_JFIF_K3             0.714
# define YCBCR_RGB_JFIF_K4             1.772

# define DEFAULT_FORMAT                 "ycc444p16be"
# define DEFAULT_GENERATOR              "ebu100"

typedef uint16_t pixel;
typedef struct pixelref_s pixelref;
typedef struct colour_s colour;
typedef struct image_s image;
typedef struct format_s format;
typedef struct generator_s generator;
typedef struct output_s output;
typedef struct converter_s converter;
typedef int (*image_export_fn)(image *img, output *out);
typedef int (*generator_fn)(image *img);
typedef int (*pixel_converter_fn)(const pixelref src, colour *dest);

struct ycbcr_s
{
	pixel y;
	pixel cb;
	pixel cr;
} PACKED;

struct rgb_s
{
	uint16_t r;
	uint16_t g;
	uint16_t b;
} PACKED;

struct colour_s
{
	int format;	
	union
	{
		pixel values[MAX_PLANES];
		struct ycbcr_s ycbcr;
		struct rgb_s rgb;
	} p;
};

struct pixelref_s
{
	pixel *pixel[MAX_PLANES];
};

struct image_s
{
	unsigned long frame;
	int format;
	int planes;
	uint32_t width;
	uint32_t height;
	int viewport;
	uint32_t vpx;
	uint32_t vpy;
	uint32_t vpwidth;
	uint32_t vpheight;
	pixel *pixels[MAX_PLANES];
};

struct format_s
{
	const char *name;
	image_export_fn fn;
	const char *description;
	int planes;
	int depth;
	int planar;
};

struct generator_s
{
	const char *name;
	generator_fn fn;
	const char *description;
	int pixelformat;
};

struct converter_s
{
	const char *name;
	const char *description;
	int def;
	pixel_converter_fn pixel_fn;
	int format;
	int depth;
};

struct output_s
{
	format *format;
	const char *pattern;
	char *filename;
	int ispattern;
	/* Format-specific data */
	union
	{
		void *ptr;
		FILE *f;
#ifdef WITH_LIBTIFF
		TIFF *tiff;
#endif
#ifdef WITH_LIBJASPER
		jas_stream_t *jp2;
#endif
	} d;
};

/* The program name as used at the point of invocation */
extern const char *progname;

/* Global colour palette */
extern colour sub_black, super_white;
extern colour black, white, grey50, grey20;
extern colour red100, green100, blue100, cyan100, magenta100, yellow100;
extern colour red75, green75, blue75, cyan75, magenta75, yellow75;
extern colour ebu_grey1, ebu_grey2, ebu_grey3, ebu_grey4, ebu_grey5, ebu_grey6,
	ebu_grey7, ebu_grey8, ebu_grey9, ebu_grey10, ebu_grey11, ebu_grey12,
	ebu_grey13, ebu_grey14, ebu_grey15, ebu_grey16, ebu_grey17, ebu_grey18,
	ebu_grey19;
extern colour ebu_red, ebu_green, ebu_blue;
extern colour ebu1, ebu2, ebu3, ebu4, ebu5, ebu6, ebu7, ebu8, ebu9, ebu10,
	ebu11, ebu12, ebu13, ebu14, ebu15;
extern colour *ebu_greys[], *ebu_shades[];

int colourmap_init(void);

/* Imaging core */
image *image_create(int format, uint32_t width, uint32_t height);
int image_pixel(image *i, uint32_t x, uint32_t y, pixelref *ref);
int image_clear(image *i, colour *col);
int image_viewport(image *i, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
int image_viewport_reset(image *i);

/* Drawing */
int image_draw_hline(image *i, uint32_t x, uint32_t y, uint32_t w, colour *c);
int image_draw_fillrect(image *i, uint32_t x, uint32_t y, uint32_t w, uint32_t h, colour *c);
int image_draw_bars(image *i, uint32_t x, uint32_t y, uint32_t w, uint32_t h, colour **bars, size_t nbars);

/* Output handling */
output *output_parse(int argc, char **argv);
format *output_formats(void);
int output_destroy(output *outputs);
int output_store(image *i, output *outputs);
const char *output_filename(output *o, image *i, int *shouldclose);
FILE *output_file(output *o, image *i, int *shouldclose);

/* Colour conversion */
pixel_converter_fn convert_pixels(int informat, int outformat, int outdepth);
converter *convert_list(void);
int convert_set(const char *name);

/* Export formats */
int export_ycc444_16_planar(image *i, output *out);
int export_ycc444_8_planar(image *i, output *out);
# ifdef WITH_LIBTIFF
int export_tiff_ycc444_16(image *i, output *out);
int export_tiff_ycc444_8(image *i, output *out);
int export_tiff_y16(image *i, output *out);
# endif
# ifdef WITH_LIBJASPER
int export_jp2(image *i, output *out);
# endif
# ifdef WITH_LIBPNG
int export_png_rgb_16(image *i, output *output);
# endif

/* Pattern generators */
int generate_ebu100(image *i);
int generate_ebu75(image *i);
int generate_ebu3325_1(image *i);
int generate_ebu3325_2(image *i);

int generate_ebu3325_5(image *i);
int generate_ebu3325_5_red(image *i);
int generate_ebu3325_5_green(image *i);
int generate_ebu3325_5_blue(image *i);

int generate_blacklevel(image *i);
int generate_whitelevel(image *i);

#endif /*!P_TCG_H_*/
