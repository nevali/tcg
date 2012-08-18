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

colour sub_black, super_white;
colour black, white, grey50, grey20;
colour red100, green100, blue100, cyan100, magenta100, yellow100;
colour red75, green75, blue75, cyan75, magenta75, yellow75;
colour ebu_grey1, ebu_grey2, ebu_grey3, ebu_grey4, ebu_grey5, ebu_grey6,
	ebu_grey7, ebu_grey8, ebu_grey9, ebu_grey10, ebu_grey11, ebu_grey12,
	ebu_grey13, ebu_grey14, ebu_grey15, ebu_grey16, ebu_grey17, ebu_grey18,
	ebu_grey19;
colour ebu_red, ebu_green, ebu_blue;
colour ebu1, ebu2, ebu3, ebu4, ebu5, ebu6, ebu7, ebu8, ebu9, ebu10,
	ebu11, ebu12, ebu13, ebu14, ebu15;

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

	{ &grey50,       0x7eeb, 0x8000, 0x8000 },
	{ &grey20,       0x3c32, 0x8000, 0x8000 },

	{ &red100,       0x4082, 0x665a, 0xf000 },
	{ &green100,     0xacac, 0x29a6, 0x19f2 },
	{ &blue100,      0x1fca, 0xf000, 0x75b8 },
	{ &cyan100,      0xbc76, 0x99a6, 0x1000 },
	{ &magenta100,   0x504c, 0xd65a, 0xe613 },
	{ &yellow100,    0xdd2e, 0x1000, 0x8a4e },

	{ &red75,        0x3486, 0x6cb0, 0xd499 },
	{ &green75,      0x85f7, 0x3efc, 0x3329 },
	{ &blue75,       0x1be4, 0xd454, 0x7842 },
	{ &cyan75,       0x91da, 0x9350, 0x2b6b },
	{ &magenta75,    0x406a, 0xc104, 0xccdb },
	{ &yellow75,     0xaa7d, 0x2bac, 0x87c2 },

	/* From EBU Tech 3325, shifted left 6 bits */
	{ &ebu_red,      0x3e80, 0x6640, 0xf000 },
	{ &ebu_green,    0xacc0, 0x29c0, 0x1a40 },
	{ &ebu_blue,     0x1fc0, 0xf000, 0x75c0 },

	{ &ebu_grey1,    0x1000, 0x8000, 0x8000 },
	{ &ebu_grey2,    0x1580, 0x8000, 0x8000 },
	{ &ebu_grey3,    0x2280, 0x8000, 0x8000 },
	{ &ebu_grey4,    0x2f80, 0x8000, 0x8000 },
	{ &ebu_grey5,    0x3c80, 0x8000, 0x8000 },
	{ &ebu_grey6,    0x4980, 0x8000, 0x8000 },
	{ &ebu_grey7,    0x5680, 0x8000, 0x8000 },
	{ &ebu_grey8,    0x6380, 0x8000, 0x8000 },
	{ &ebu_grey9,    0x7080, 0x8000, 0x8000 },
	{ &ebu_grey10,   0x7d80, 0x8000, 0x8000 },
	{ &ebu_grey11,   0x8a80, 0x8000, 0x8000 },
	{ &ebu_grey12,   0x9780, 0x8000, 0x8000 },
	{ &ebu_grey13,   0xa480, 0x8000, 0x8000 },
	{ &ebu_grey14,   0xb180, 0x8000, 0x8000 },
	{ &ebu_grey15,   0xbe80, 0x8000, 0x8000 },
	{ &ebu_grey16,   0xcb80, 0x8000, 0x8000 },
	{ &ebu_grey17,   0xd880, 0x8000, 0x8000 },
	{ &ebu_grey18,   0xe580, 0x8000, 0x8000 },
	{ &ebu_grey19,   0xeb00, 0x8000, 0x8000 },
	
	/* EBU 1: Dark Skin */
	{ &ebu1,         0x54f0, 0x7580, 0x9080 },
	/* EBU 2: Light Skin */
	{ &ebu2,         0x9f00, 0x7240, 0x95c0 },
	/* EBU 3: Light Greyish Red */
	{ &ebu3,         0x9180, 0x7780, 0x9400 },
	/* EBU 4: Light Yellow Green */
	{ &ebu4,         0x9040, 0x5500, 0x7800 },
	/* EBU 5: Light Bluish Green */
	{ &ebu5,         0x90c0, 0x8800, 0x66c0 },
	/* EBU 6: Light Violet */
	{ &ebu6,         0x9280, 0x9540, 0x87c0 },
	/* EBU 7: Foliage */
	{ &ebu7,         0x6c40, 0x6ec0, 0x79c0 },
	/* EBU 8: Medium Red */
	{ &ebu8,         0x7300, 0x7440, 0xafc0 },
	/* EBU 9: Medium Green */
	{ &ebu9,         0xa480, 0x5f00, 0x5c80 },
	/* EBU 10: Medium Blue */
	{ &ebu10,        0x7580, 0x9fc0, 0x7500 },
	/* EBU 11: Dark Red */
	{ &ebu11,        0x4fc0, 0x7a80, 0x9a00 },
	/* EBU 12: Dark Green */
	{ &ebu12,        0x79c0, 0x6980, 0x6300 },
	/* EBU 13: Dark Blue */
	{ &ebu13,        0x5040, 0x9a40, 0x7ac0 },
	/* EBU 14: Medium Yellow Red */
	{ &ebu14,        0xa3c0, 0x5740, 0xa840 },
	/* EBU 15: Medium Purple */
	{ &ebu15,        0x7b80, 0x9640, 0x9440 },

	{ NULL,          0,      0,      0      }
};

colour *ebu_greys[] = {
	&ebu_grey1, &ebu_grey2, &ebu_grey3, &ebu_grey4,
	&ebu_grey5, &ebu_grey6, &ebu_grey7, &ebu_grey8,
	&ebu_grey9, &ebu_grey10, &ebu_grey11, &ebu_grey12,
	&ebu_grey13, &ebu_grey14, &ebu_grey15, &ebu_grey16,
	&ebu_grey17, &ebu_grey18, &ebu_grey19, NULL
};

colour *ebu_shades[] = {
	&ebu1, &ebu2, &ebu3, &ebu4, &ebu5, &ebu6, &ebu7, &ebu8,
	&ebu9, &ebu10, &ebu11, &ebu12, &ebu13, &ebu14, &ebu15,
	NULL
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
