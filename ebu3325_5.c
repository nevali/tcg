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

/* EBU Tech 3325 - Test pattern 5: Single-colour patches on a black field */

int
generate_ebu3325_5_red(image *i, uint32_t frame)
{
	uint32_t size;
	
	(void) frame;

	/* Patches are squares 13.35% of the height */   
	size = i->height / 7.5;
	/* Fill with black */
	if(image_clear(i, &black))
	{
		return -1;
	}
	/* Place the centre square */
	if(image_draw_fillrect(i, (i->width - size) / 2, (i->height - size) / 2, size, size, &ebu_red))
	{
		return -1;
	}
	return 0;
}

int
generate_ebu3325_5_green(image *i, uint32_t frame)
{
	uint32_t size;
	
	(void) frame;

	/* Patches are squares 13.35% of the height */   
	size = i->height / 7.5;
	/* Fill with black */
	if(image_clear(i, &black))
	{
		return -1;
	}
	/* Place the centre square */
	if(image_draw_fillrect(i, (i->width - size) / 2, (i->height - size) / 2, size, size, &ebu_green))
	{
		return -1;
	}
	return 0;
}

int
generate_ebu3325_5_blue(image *i, uint32_t frame)
{
	uint32_t size;
	
	(void) frame;

	/* Patches are squares 13.35% of the height */   
	size = i->height / 7.5;
	/* Fill with black */
	if(image_clear(i, &black))
	{
		return -1;
	}
	/* Place the centre square */
	if(image_draw_fillrect(i, (i->width - size) / 2, (i->height - size) / 2, size, size, &ebu_blue))
	{
		return -1;
	}
	return 0;
}
