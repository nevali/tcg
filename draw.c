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

int
image_clear(image *i, colour *c)
{
	int d;
	uint64_t n, npixels;
	pixel *p;
	
	if(c->format != i->format)
	{
		errno = EINVAL;
		return -1;
	}
	if(i->viewport)
	{
		return image_draw_fillrect(i, 0, 0, i->vpwidth, i->vpheight, c);
	}
	npixels = i->width * i->height;
	for(d = 0; d < i->planes; d++)
	{
		p = i->pixels[d];
		for(n = 0; n < npixels; n++)
		{
			*p = c->p.values[d];
			p++;
		}
	}
	return 0;
}

int
image_draw_hline(image *i, uint32_t x, uint32_t y, uint32_t w, colour *c)
{
	pixelref ref;
	int d;
	uint32_t n;

	if(c->format != i->format)
	{
		errno = EINVAL;
		return -1;
	}	
	if(image_pixel(i, x, y, &ref) < 0)
	{
		return -1;
	}
	if(w > (i->vpwidth - x))
	{
		w = i->vpwidth - x;
	}
	for(d = 0; d < i->planes; d++)
	{
		for(n = 0; n < w; n++)
		{
			ref.pixel[d][n] = c->p.values[d];
		}
	}
	return 0;
}

int
image_draw_fillrect(image *i, uint32_t x, uint32_t y, uint32_t w, uint32_t h, colour *c)
{
	uint32_t n;

	for(n = 0; n < h; n++)
	{
		if(image_draw_hline(i, x, y + n, w, c) < 0)
		{
			return -1;
		}
	}
	return 0;
}

int
image_draw_bars(image *i, uint32_t x, uint32_t y, uint32_t width, uint32_t height, colour **bars, size_t nbars)
{
	uint32_t space, startx, w;
	size_t n;

	space = (uint32_t) round((double) width / (double) nbars);
	startx = x;
	for(n = 0; n < nbars; n++)
	{
		if(n == nbars - 1)
		{
			w = startx + width - x;
		}
		else
		{
			w = space;
		}
		if(bars[n])
		{
			image_draw_fillrect(i, x, y, w, height, bars[n]);
		}
		x += w;
	}
	return 0;
}

/* Copy up to w pixels from src up to (destx,desty)-(destx+w-1,desty) */
int
image_draw_copyline(image *i, pixelref *src, uint32_t destx, uint32_t desty, uint32_t w)
{
	pixelref dest;
	int c;
	
	if(destx > i->width || desty > i->height)
	{
		return 0;
	}
	if(w + destx > i->width)
	{
		w = i->width - destx;
	}
	if(image_pixel(i, destx, desty, &dest))
	{
		return -1;
	}
	for(c = 0; c < i->planes; c++)
	{
		memcpy(dest.pixel[c], src->pixel[c], sizeof(pixel) * w);
	}
	return 0;
}
