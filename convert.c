/*
 * Copyright 2013 Mo McRoberts.
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "p_tcg.h"

static int convert_pixel_null(const pixelref src, colour *rgb);
static int convert_pixel_8(const pixelref src, colour *rgb);
static int convert_pixel_ycbcr_rgb16_jfif(const pixelref src, colour *rgb);
static int convert_pixel_ycbcr_rgb8_jfif(const pixelref src, colour *rgb);
static int convert_pixel_ycbcr_rgb16_jfifnom(const pixelref src, colour *rgb);
static int convert_pixel_ycbcr_rgb8_jfifnom(const pixelref src, colour *rgb);

static const char *converter_name = NULL;

static converter converters[] = 
{
	{ "jfif", "Convert YCbCr to RGB using JFIF coefficients (full range)", 1, convert_pixel_ycbcr_rgb16_jfif, PF_RGB, 16 },
	{ "jfif", NULL, 0, convert_pixel_ycbcr_rgb8_jfif, PF_RGB, 8 },
	{ "jfif-nom", "Convert YCbCr to RGB using JFIF coefficients (nominal range)", 0, convert_pixel_ycbcr_rgb16_jfifnom, PF_RGB, 16 },
	{ "jfif-nom", NULL, 0, convert_pixel_ycbcr_rgb8_jfifnom, PF_RGB, 8 },
	{ NULL, NULL, 0, NULL, 0, 0 }
};

/* Obtain a pixel converter */
pixel_converter_fn
convert_pixels(int informat, int outformat, int outdepth)
{
	size_t c;
	
	if(informat == outformat)
	{
		if(outdepth == 16)
		{
			return convert_pixel_null;
		}
		if(outdepth == 8)
		{
			return convert_pixel_8;
		}
		return NULL;
	}
	for(c = 0; converters[c].name; c++)
	{
		if(converters[c].pixel_fn && converters[c].format == outformat && converters[c].depth)
		{
			if(converter_name && strcmp(converters[c].name, converter_name))
			{
				continue;
			}
			return converters[c].pixel_fn;
		}
	}
	return NULL;
}

/* Return the list of converters */
converter *
convert_list(void)
{
	return converters;
}

/* Attempt to set the converter to the specified named implementation */
int
convert_set(const char *name)
{
	size_t c;

	for(c = 0; converters[c].name; c++)
	{
		if(!strcmp(converters[c].name, name))
		{
			converter_name = name;
			return 0;
		}
	}
	return -1;
}


/* Simply copy the 16-bit YCbCr pixel values to the destination */
static int
convert_pixel_null(const pixelref src, colour *dest)
{
	dest->format = PF_UNKNOWN;
	dest->p.values[0] = *src.pixel[0];
	dest->p.values[1] = *src.pixel[1];
	dest->p.values[2] = *src.pixel[2];
	return 0;
}

/* Copy the 16-bit YCbCr pixel values to the destination, discarding the low 8 bits */
static int
convert_pixel_8(const pixelref src, colour *dest)
{
	dest->format = PF_UNKNOWN;
	dest->p.values[0] = *src.pixel[0] >> 8;
	dest->p.values[1] = *src.pixel[1] >> 8;
	dest->p.values[2] = *src.pixel[2] >> 8;
	return 0;
}

#define CLIP16(v) \
	v = (v < 0 ? 0 : ((v > UINT16_MAX) ? UINT16_MAX : v))
#define CLIP_RGB16(r, g, b) \
	CLIP16(r); \
	CLIP16(g); \
	CLIP16(b);

#define CONVERT_YCBCR_RGB(y, cb, cr, r, g, b, k1, k2, k3, k4, mid) \
	r = (y) + ( (k1) * ( (cr) - (mid) ) ); \
	g = (y) - ( (k2) * ( (cb) - (mid) ) ) - ( (k3) * ( (cr) - (mid) ) ); \
	b = (y) + ( (k4) * ( (cb1) - (mid) ) );

#define ASSIGN_RGB16(colour, red, green, blue) \
	(colour).format = PF_RGB; \
	(colour).p.rgb.r = (red); \
	(colour).p.rgb.g = (green); \
	(colour).p.rgb.b = (blue);

#define ASSIGN_RGB8(colour, red, green, blue) \
	(colour).format = PF_RGB; \
	(colour).p.rgb.r = (pixel)(red) >> 8; \
	(colour).p.rgb.g = (pixel)(green) >> 8; \
	(colour).p.rgb.b = (pixel)(blue) >> 8;

/* Convert 16-bit YCbCr pixel values to 16-bit RGB colours using the JPEG values */
static int
convert_pixel_ycbcr_rgb16_jfif(const pixelref src, colour *dest)
{
	double y1, cb1, cr1, r1, g1, b1;
	
	y1 = *(src.pixel[0]);
	cb1 = *(src.pixel[1]);
	cr1 = *(src.pixel[2]);

	CONVERT_YCBCR_RGB(y1, cb1, cr1, r1, g1, b1, YCBCR_RGB_JFIF_K1, YCBCR_RGB_JFIF_K2, YCBCR_RGB_JFIF_K3, YCBCR_RGB_JFIF_K4, ABSMID_YCBCR16);	
	CLIP_RGB16(r1, g1, b1);
	ASSIGN_RGB16(*dest, r1, g1, b1);
	return 0;
}

/* Convert 16-bit YCbCr pixel values to 8-bit RGB colours using the JPEG values */
static int
convert_pixel_ycbcr_rgb8_jfif(const pixelref src, colour *dest)
{
	double y1, cb1, cr1, r1, g1, b1;
	
	y1 = *(src.pixel[0]);
	cb1 = *(src.pixel[1]);
	cr1 = *(src.pixel[2]);

	CONVERT_YCBCR_RGB(y1, cb1, cr1, r1, g1, b1, YCBCR_RGB_JFIF_K1, YCBCR_RGB_JFIF_K2, YCBCR_RGB_JFIF_K3, YCBCR_RGB_JFIF_K4, ABSMID_YCBCR16);	
	CLIP_RGB16(r1, g1, b1);
	ASSIGN_RGB8(*dest, r1, g1, b1);
	return 0;
}

/* Convert 16-bit YCbCr pixel values to 16-bit RGB colours using the JPEG values in nominal range */
static int
convert_pixel_ycbcr_rgb16_jfifnom(const pixelref src, colour *dest)
{
	double y1, cb1, cr1, r1, g1, b1;
	
	y1 = STRETCH_Y16(*(src.pixel[0]));
	cb1 = STRETCH_CBCR16(*(src.pixel[1]));
	cr1 = STRETCH_CBCR16(*(src.pixel[2]));
	
	CONVERT_YCBCR_RGB(y1, cb1, cr1, r1, g1, b1, YCBCR_RGB_JFIF_K1, YCBCR_RGB_JFIF_K2, YCBCR_RGB_JFIF_K3, YCBCR_RGB_JFIF_K4, ABSMID_YCBCR16);	
	CLIP_RGB16(r1, g1, b1);
	ASSIGN_RGB16(*dest, r1, g1, b1);
	return 0;
}

/* Convert 16-bit YCbCr pixel values to 8-bit RGB colours using the JPEG values */
static int
convert_pixel_ycbcr_rgb8_jfifnom(const pixelref src, colour *dest)
{
	double y1, cb1, cr1, r1, g1, b1;
	
	y1 = STRETCH_Y16(*(src.pixel[0]));
	cb1 = STRETCH_CBCR16(*(src.pixel[1]));
	cr1 = STRETCH_CBCR16(*(src.pixel[2]));

	CONVERT_YCBCR_RGB(y1, cb1, cr1, r1, g1, b1, YCBCR_RGB_JFIF_K1, YCBCR_RGB_JFIF_K2, YCBCR_RGB_JFIF_K3, YCBCR_RGB_JFIF_K4, ABSMID_YCBCR16);	
	CLIP_RGB16(r1, g1, b1);
	ASSIGN_RGB8(*dest, r1, g1, b1);
	return 0;
}
