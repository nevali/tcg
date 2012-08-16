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
main(int argc, char **argv)
{
	image *i;
	FILE *f;

	(void) argc;
	(void) argv;

	colourmap_init();
	i = image_create(PF_YCBCR, 1920, 1080);
	if(!i)
	{
		fprintf(stderr, "%s: failed to allocate image\n", argv[0]);
		exit(255);
	}
	image_clear(i, &black);

	testcard_ebu100(i);

	f = fopen("test.yuv16", "wb");
	image_export_ycc444_16_planar(i, f);
	fclose(f);

	f = fopen("test.yuv8", "wb");
	image_export_ycc444_8_planar(i, f);
	fclose(f);

	image_export_tiff_ycc444_16(i, "test.ycc444-16.tiff");
	image_export_tiff_ycc444_8(i, "test.ycc444-8.tiff");
	return 0;
}
