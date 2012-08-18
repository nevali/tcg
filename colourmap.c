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

colour sub_black;
colour super_white;
colour black;
colour white;
colour red100;
colour green100;
colour blue100;
colour yellow100;
colour cyan100;
colour magenta100;

struct colourmap
{
	colour *col;
	uint16_t red, green, blue;
};

/* Values are FULL RANGE 16-bit YCbCr
 *
 * Nominal range for Y:
 *
 * 8-bit:            0x10 (16)   ...   0xEB (235)
 * 10-bit:          0x040 (64)   ...  0x3AC (940)
 * 16-bit:         0x1000 (4096) ... 0xEB00 (60160) 
 *
 * Nominal range for Cb & Cr:
 *
 * 8-bit:           0x10 (16)    ...   0xF0 (240)
 * 10-bit:         0x040 (64)    ...  0x3C0 (960)
 * 16-bit:        0x1000 (4096)  ... 0xF000 (61440)
 *
 * Achromatic points for Cb & Cr:
 *
 * 8-bit:           0x80 (128)
 * 10-bit:         0x200 (512)
 * 16-bit:        0x8000 (32768)
 */

static struct colourmap maptable[] =
{
	{ &sub_black,    0x0000, 0x8000, 0x8000 },
	{ &super_white,  0xffff, 0x8000, 0x8000 },

	{ &black,        0x1000, 0x8000, 0x8000 },
	{ &white,        0xeb00, 0x8000, 0x8000 },

	{ &red100,       0x4082, 0x665a, 0xf000 },
	{ &green100,     0xacac, 0x29a6, 0x19f2 },
	{ &blue100,      0x1fca, 0xf000, 0x75b8 },
	{ &cyan100,      0xbc76, 0x99a6, 0x1000 },
	{ &magenta100,   0x504c, 0xd65a, 0xe613 },
	{ &yellow100,    0xdd2e, 0x1000, 0x8a4e },
	{ NULL,          0,      0,      0      }
};

int
colourmap_init(void)
{
	size_t n;
	colour *p;

	for(n = 0; maptable[n].col; n++)
	{
		p = maptable[n].col;
		p->format = PF_YCBCR;
		p->p.ycbcr.y = maptable[n].red;
		p->p.ycbcr.cb = maptable[n].green;
		p->p.ycbcr.cr = maptable[n].blue;
	}
	return 0;
}
