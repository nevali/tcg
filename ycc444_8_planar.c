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
image_export_ycc444_8_planar(image *i, FILE *f)
{
	char *row;
	int d;
	size_t n, c, y, x, nbytes;
	
	nbytes = i->width;
	row = malloc(nbytes);
	if(!row)
	{
		return -1;
	}
	for(d = 0; d < i->planes; d++)
	{
		n = 0;
		for(y = 0; y < i->height; y++)
		{
			c = 0;
			for(x = 0; x < i->width; x++)
			{
				row[c] = (i->pixels[d][n] & 0xff00) >> 8;
				c++;
				n++;
			}
			fwrite(row, nbytes, 1, f);
		}
	}
	free(row);
	return 0;
}
