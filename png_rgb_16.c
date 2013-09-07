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

#ifdef WITH_LIBPNG

int
export_png_rgb_16(image *i, output *output)
{
	const char *fn;
	int shouldclose;
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 n, x, y, pi;
	pixelref src;
	colour rgb;
	png_bytep pp;
	uint16_t *bp;
	FILE *fp;
	pixel_converter_fn converter;
	
	converter = convert_pixels(i->format, PF_RGB, 16);
	if(!converter)
	{
		fprintf(stderr, "cannot locate a suitable format converter for RGB-16\n");
		return -1;
	}
	fn = output_filename(output, i, &shouldclose);
	fp = fopen(fn, "wb");
	if(!fp)
	{
		return -1;
	}
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr)
	{
		return -1;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr)
	{
		return -1;
	}
	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, i->width, i->height, 16, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);
	
	pp = (png_bytep) malloc(i->width * 3 * sizeof(uint16_t));
	bp = (uint16_t *) pp;
	src.pixel[0] = i->pixels[0];
	src.pixel[1] = i->pixels[1];
	src.pixel[2] = i->pixels[2];
	for(y = 0, pi = 0; y < i->height; y++)
	{
		for(n = 0, x = 0; x < i->width; x++, pi++)
		{
			converter(src, &rgb);
			src.pixel[0]++;
			src.pixel[1]++;
			src.pixel[2]++;
			bp[n] = htons(rgb.p.rgb.r);
			n++;
			bp[n] = htons(rgb.p.rgb.g);
			n++;
			bp[n] = htons(rgb.p.rgb.b);
			n++;
		}
		png_write_row(png_ptr, pp);
	}
	png_write_end(png_ptr, NULL);
	free(pp);
	fclose(fp);
	png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	png_destroy_write_struct(&png_ptr, NULL);
	return 0;
}

#endif /*!WITH_LIBPNG*/
