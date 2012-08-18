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

/* EBU Tech 3325 - Test pattern 2: Black and super-white */

int
generate_ebu3325_2(image *i, uint32_t frame)
{
	uint32_t ofx, ofy, size;	
	
	(void) frame;

	/* Patches are squares 13.35% of the height */   
	size = i->height / 7.5;
	/* The offset is 0.4 of the width or height relative to the centre */
	ofx = i->width * 0.4;
	ofy = i->height * 0.4;
	/* Fill with 50% grey */
	image_clear(i, &grey50);
	/* Place the centre white square */
	image_draw_fillrect(i, (i->width - size) / 2, (i->height - size) / 2, size, size, &super_white);
	/* Place the top square */
	image_draw_fillrect(i, (i->width - size) / 2, (i->height - size) / 2 - ofy, size, size, &black);
	/* Place the bottom square */
	image_draw_fillrect(i, (i->width - size) / 2, (i->height - size) / 2 + ofy, size, size, &black);
	/* Place the left square */
	image_draw_fillrect(i, (i->width - size) / 2 - ofx, (i->height - size) / 2, size, size, &black);
	/* Place the right square */
	image_draw_fillrect(i, (i->width - size) / 2 + ofx, (i->height - size) / 2, size, size, &black);

	return 0;
}

	
