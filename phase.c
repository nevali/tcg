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

int
generate_phase(image *i)
{
	pixelref line1, line2;
	uint32_t n;
	
	if(image_clear(i, &super_white))
	{
		return -1;
	}
	if(image_pixel(i, 0, 0, &line1) || image_pixel(i, 0, 2, &line2))
	{
		return -1;
	}
	for(n = 0; n < i->width; n++)
	{
		if(n % 2)
		{
			line2.pixel[0][n] = sub_black.p.ycbcr.y;
			line2.pixel[1][n] = sub_black.p.ycbcr.cb;
			line2.pixel[2][n] = sub_black.p.ycbcr.cr;
		}
		else
		{
			line1.pixel[0][n] = sub_black.p.ycbcr.y;
			line1.pixel[1][n] = sub_black.p.ycbcr.cb;
			line1.pixel[2][n] = sub_black.p.ycbcr.cr;
		}
	}
	if(image_draw_copyline(i, &line1, 0, 1, i->width))
	{
		return -1;
	}
	for(n = 3; n < i->height; n++)
	{
		if((n + 1) % 3)
		{
			if(image_draw_copyline(i, &line1, 0, n, i->width))
			{
				return -1;
			}
		}
		else
		{
			if(image_draw_copyline(i, &line2, 0, n, i->width))
			{
				return -1;
			}
		}
	}
	return 0;
}

	
