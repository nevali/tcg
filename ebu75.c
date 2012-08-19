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
generate_ebu75(image *i)
{
	uint32_t h;	
	colour *bars[7];

	h = i->vpheight / 10;
	
	bars[0] = &white;
	bars[1] = &yellow75;
	bars[2] = &cyan75;
	bars[3] = &green75;
	bars[4] = &magenta75;
	bars[5] = &red75;
	bars[6] = &blue75;
	if(image_draw_bars(i, 0, 0, i->vpwidth, i->vpheight - h, bars, 7))
	{
		return -1;
	}
	bars[0] = &blue75;
	bars[1] = &black;
	bars[2] = &magenta75;
	bars[3] = &black;
	bars[4] = &cyan75;
	bars[5] = &black;
	bars[6] = &white;
	if(image_draw_bars(i, 0, i->vpheight - h, i->vpwidth, h, bars, 7))
	{
		return -1;
	}
	return 0;
}

	
