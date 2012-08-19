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

#ifdef WITH_LIBTIFF

int
export_tiff_ycc444_8(image *i, output *output)
{
	int d, shouldclose;
	uint8_t *row;
	size_t n, y, c, x, nbytes;
	TIFF *tif;
	const char *fn;

	if(!i)
	{
		if(output->d.tiff)
		{
			TIFFClose(output->d.tiff);
		}
		return 0;
	}
	nbytes = i->width;
	row = malloc(nbytes);
	if(!row)
	{
		return -1;
	}

	fn = output_filename(output, i, &shouldclose);
	if(!output->d.tiff)
	{
		output->d.tiff = TIFFOpen(fn, "w");
	}
	tif = output->d.tiff;

	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, i->width);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, i->height);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_SEPARATE);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, i->planes);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 0, NULL);
	TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_YCBCR);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
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
				TIFFError(output->pattern, "Failed to write scanline %lu/%d\n", (unsigned long) y, d);
				TIFFClose(tif);
				return -1;
			}
		}
	}
	TIFFWriteDirectory(tif);
	if(shouldclose)
	{
		TIFFClose(tif);
		output->d.tiff = NULL;
	}
	return 0;
}

#endif /*WITH_LIBTIFF*/
