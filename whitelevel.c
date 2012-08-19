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

/* White and super-white 4x4 checkerboard on a white field */

int
generate_whitelevel(image *i, uint32_t frame)
{
	uint32_t size, midx, midy, y, x, n;
	
	(void) frame;

	/* Patches are squares 13.35% of the height */   
	size = i->height / 7.5;
	/* Find the centre */
	midx = i->width / 2;
	midy = i->height / 2;
	/* Fill with black */
	if(image_clear(i, &white))
	{
		return -1;
	}
	n = 1;
	for(y = midy - size * 2; y < midy + size * 2; y += size)
	{
		for(x = midx - size * 2; x < midx + size * 2; x += size)
		{
			if(n % 2)
			{
				if(image_draw_fillrect(i, x, y, size, size, &super_white))
				{
					return -1;
				}
			}
			n++;		
		}
		n++;
	}
	return 0;
}

	
