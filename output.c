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

static format formats[] = {
	{ "ycc444p16be", export_ycc444_16_planar, "Raw 4:4:4 YCbCr 16-bpc big-endian", 3, 16, 1 },
	{ "ycc444p8", export_ycc444_8_planar, "Raw 4:4:4 YCbCr 8-bpc", 3, 8, 1 },
#ifdef WITH_LIBTIFF
	{ "tiff-ycc444p16", export_tiff_ycc444_16, "4:4:4 YCbCr 16-bpc TIFF", 3, 16, 1 },
	{ "tiff-ycc444p8", export_tiff_ycc444_8, "4:4:4 YCbCr 8-bpc TIFF", 3, 8, 1 },
	{ "tiff-y16", export_tiff_y16, "Greyscale (luma) 16-bpc TIFF", 3, 8, 1 },
#endif
	{ NULL, NULL, NULL, 0, 0, 0 }
};

/* Create an array of output structures from a set of arguments */
output *
output_parse(int argc, char **argv)
{
	output *p;
	int c, d;
	char *t;
	format *def;

	p = (output *) calloc(argc + 1, sizeof(output));
	if(!p)
	{
		return NULL;
	}
	/* The default format is that matching DEFAULT_FORMAT, or the first in
	 * the list if all else fails.
	 */
	def = NULL;
	for(d = 0; formats[d].name; d++)
	{
		if(!d)
		{
			def = &(formats[d]);
		}
		if(!strcmp(formats[d].name, DEFAULT_FORMAT))
		{
			def = &(formats[d]);
			break;
		}   
	}
	/* For each argument, the pattern is [FORMAT:]PATH
	 * If FORMAT is not specified, the default format is used. If FORMAT is
	 * specified, but is not supported, the 'format' member of the output
	 * structure is set to NULL and no output will be written.
	 */
	for(c = 0; c < argc; c++)
	{
		t = strchr(argv[c], ':');
		if(t)
		{
			*t = 0;
			t++;
			p[c].pattern = t;
			for(d = 0; formats[d].name; d++)
			{
				if(!strcmp(formats[d].name, argv[c]))
				{
					p[c].format = &(formats[d]);
					break;
				}
			}
			if(!formats[d].name)
			{
				fprintf(stderr, "%s: unsupported format '%s'\n", progname, argv[c]);
			}
		}
		else
		{
			p[c].pattern = argv[c];
			p[c].format = def;
		}
		
	}
	return p;
}

/* Destroy an array of output structures */
int
output_destroy(output *outputs)
{
	size_t c;

	for(c = 0; outputs[c].pattern; c++)
	{
		if(outputs[c].format)
		{
			/* Close any file handles */
			outputs[c].format->fn(NULL, &(outputs[c]));
		}
	}
	free(outputs);
	return 0;
}

/* Store a frame */
int
output_store(image *i, output *outputs)
{
	int e;
	size_t c;
	char *t;

	e = 0;
	for(c = 0; outputs[c].pattern; c++)
	{
		if(!outputs[c].format)
		{
			e |= 2;
			continue;
		}
		if(outputs[c].format->fn(i, &(outputs[c])))
		{
			fprintf(stderr, "%s: %s: %s\n", progname, t, strerror(errno));
			e |= 1;
		}
	}
	return e;
}

/* Return the list of output formats */
format *
output_formats(void)
{
	return formats;
}

