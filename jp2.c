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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "p_tcg.h"

#ifdef WITH_LIBJASPER

int
export_jp2(image *i, output *output)
{
	jas_image_cmptparm_t component_info[4];
	jas_image_t *jp2_image;
	jas_matrix_t *pixels[4];
	jas_stream_t *jp2_stream;
	pixelref ref;
	const char *fn;
	char *options;
	int n, format, shouldclose;
	size_t x, y;

	if(!i)
	{
		if(output->d.jp2)
		{
			jas_stream_close(output->d.jp2);
		}
		return 0;
	}
	fn = output_filename(output, i, &shouldclose);
	if(!output->d.jp2)
	{
		output->d.jp2 = jas_stream_fopen(fn, "w+b");
		if(!output->d.jp2)
		{
			return -1;
		}
	}
	jp2_stream = output->d.jp2;
	for(n = 0; n < i->planes; n++)
	{
		component_info[n].tlx = 0;
		component_info[n].tly = 0;
		component_info[n].hstep = 1;
		component_info[n].vstep = 1;
		component_info[n].width = i->width;
		component_info[n].height = i->height;
		component_info[n].prec = sizeof(pixel) * 8;
		component_info[n].sgnd = 0;
	}
	jp2_image = jas_image_create(i->planes, component_info, JAS_CLRSPC_UNKNOWN);
	if(!jp2_image)
	{
		return -1;
	}
	switch(i->format)
	{
	case PF_YCBCR:
		jas_image_setclrspc(jp2_image,JAS_CLRSPC_SYCBCR);
		jas_image_setcmpttype(jp2_image, 0, JAS_IMAGE_CT_YCBCR_Y);
		jas_image_setcmpttype(jp2_image, 1, JAS_IMAGE_CT_YCBCR_CB);
		jas_image_setcmpttype(jp2_image, 2, JAS_IMAGE_CT_YCBCR_CR);
		if(i->planes == 4)
		{
			jas_image_setcmpttype(jp2_image, 3, JAS_IMAGE_CT_OPACITY);
		}
		break;
	}
	for(n = 0; n < i->planes; n++)
	{
		pixels[n] = jas_matrix_create(1, i->width);
	}
	image_pixel(i, 0, 0, &ref);
	for(y = 0; y < i->height; y++)
	{
		for(x = 0; x < i->width; x++)
		{
			for(n = 0; n < i->planes; n++)
			{
				jas_matrix_setv(pixels[n], x, *(ref.pixel[n]));
				ref.pixel[n]++;
			}
		}
		for(n = 0; n < i->planes; n++)
		{
			jas_image_writecmpt(jp2_image, n, 0, y, i->width, 1, pixels[n]);
		}
	}
	options = "mode=int nomct";
	format = jas_image_strtofmt("jp2");
	jas_image_encode(jp2_image, jp2_stream, format, options);
	for(n = 0; n < i->planes; n++)
	{
		jas_matrix_destroy(pixels[n]);
	}
	jas_image_destroy(jp2_image);
	jas_stream_flush(jp2_stream);
	if(shouldclose)
	{
		jas_stream_close(jp2_stream);
		output->d.jp2 = NULL;
	}
	return 0;
}

#endif /*!WITH_LIBJASPER*/
