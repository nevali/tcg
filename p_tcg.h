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

# include "tiffio.h"

# define PACKED                         __attribute__((packed))

# define MAX_PLANES                     4

# define PF_RGB                         0
# define PF_RGBA                        1
# define PF_YCBCR                       2

typedef uint16_t pixel;
typedef struct pixelref_s pixelref;
typedef struct colour_s colour;
typedef struct image_s image;
typedef struct format_s format;
typedef struct generator_s generator;
typedef int (*image_export_fn)(image *img, const char *fn);
typedef int (*generator_fn)(image *img, uint32_t frame);

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
	union pixel_u
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
	int format;
	int planes;
	uint32_t width;
	uint32_t height;
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
};

extern colour sub_black, super_white;
extern colour black, white, grey50, grey20;
extern colour red100, green100, blue100, cyan100, magenta100, yellow100;
extern colour red75, green75, blue75, cyan75, magenta75, yellow75;

int colourmap_init(void);

image *image_create(int format, uint32_t width, uint32_t height);
int image_pixel(image *i, uint32_t x, uint32_t y, pixelref *ref);

int image_clear(image *i, colour *col);
int image_draw_hline(image *i, uint32_t x, uint32_t y, uint32_t w, colour *c);
int image_draw_fillrect(image *i, uint32_t x, uint32_t y, uint32_t w, uint32_t h, colour *c);
int image_draw_bars(image *i, uint32_t x, uint32_t y, uint32_t w, uint32_t h, colour **bars, size_t nbars);

int image_export_ycc444_16_planar(image *i, const char *pathname);
int image_export_ycc444_8_planar(image *i, const char *pathname);

# ifdef WITH_LIBTIFF
int image_export_tiff_ycc444_16(image *i, const char *pathname);
int image_export_tiff_ycc444_8(image *i, const char *pathname);
int image_export_tiff_y16(image *i, const char *pathname);
# endif

int generate_ebu100(image *i, uint32_t frame);

#endif /*!P_TCG_H_*/
