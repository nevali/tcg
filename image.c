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

image *
image_create(int format, uint32_t width, uint32_t height)
{
	image *i;
	void *p;
	int c;
	size_t nbytes;

	i = (image *) calloc(1, sizeof(image));
	if(!i)
	{
		return NULL;
	}
	i->format = format;
	i->width = width;
	i->height = height;
	switch(i->format)
	{
	case PF_RGB:
	case PF_YCBCR:
		i->planes = 3;
		break;
	case PF_RGBA:
		i->planes = 4;
		break;
	default:
		i->planes = 3;
	}
	nbytes = width * height * sizeof(pixel);
	for(c = 0; c < i->planes; c++)
	{		
		p = malloc(nbytes);
		if(!p)
		{
			free(p);
			for(; c >= 0; c--)
			{
				free(i->pixels[c]);
			}
			free(i);
			return NULL;
		}
		memset(p, 0, nbytes);
		i->pixels[c] = p;
	}
	return i;
}

int
image_pixel(image *i, uint32_t x, uint32_t y, pixelref *ref)
{
	size_t ofs;
	int c;

	memset(ref, 0, sizeof(pixelref));
	if(x >= i->width ||
	   y >= i->height)
	{
		errno = EINVAL;
		return -1;
	}
	ofs = (y * i->width) + x;
	for(c = 0; c < i->planes; c++)
	{
		ref->pixel[c] = &(i->pixels[c][ofs]);
	}
	return 0;
}
