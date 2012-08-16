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
image_export_tiff_ycc444_8(image *i, const char *pathname)
{
	int d;
	uint8_t *row;
	size_t n, y, c, x, nbytes;
	TIFF *tif;

	nbytes = i->width;
	row = malloc(nbytes);
	if(!row)
	{
		return -1;
	}
	
	tif = TIFFOpen(pathname, "w");
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, i->width);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, i->height);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_SEPARATE);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, i->planes);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 0, NULL);
	TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_YCBCR);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
/*	TIFFSetField(tif, TIFFTAG_YCBCRCOEFFICIENTS,  */
	TIFFSetField(tif, TIFFTAG_YCBCRSUBSAMPLING, 1, 1, NULL);
	
	TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1);

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
			if(TIFFWriteScanline(tif, (tdata_t) row, y, (tsample_t) d) == -1)
			{
				TIFFError(pathname, "Failed to write scanline %lu/%d\n", (unsigned long) y, d);
				TIFFClose(tif);
				return -1;
			}
		}
	}

	TIFFClose(tif);
	return 0;
}
