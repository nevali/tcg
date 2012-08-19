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
generate_ebu100(image *i)
{
	colour *bars[8];

	bars[0] = &white;
	bars[1] = &yellow100;
	bars[2] = &cyan100;
	bars[3] = &green100;
	bars[4] = &magenta100;
	bars[5] = &red100;
	bars[6] = &blue100;
	bars[7] = &black;
	return image_draw_bars(i, 0, 0, i->vpwidth, i->vpheight, bars, 8);
}

	
