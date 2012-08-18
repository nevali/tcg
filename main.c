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

#define DEFAULT_FORMAT                  "ycc444p16be"

static const char *progname;

static format formats[] = {
	{ "ycc444p16be", image_export_ycc444_16_planar, "Raw 4:4:4 YCbCr 16-bpc big-endian", 3, 16, 1 },
	{ "ycc444p8", image_export_ycc444_8_planar, "Raw 4:4:4 YCbCr 8-bpc", 3, 8, 1 },
#ifdef WITH_LIBTIFF
	{ "tiff-ycc444p16", image_export_tiff_ycc444_16, "4:4:4 YCbCr 16-bpc TIFF", 3, 16, 1 },
	{ "tiff-ycc444p8", image_export_tiff_ycc444_8, "4:4:4 YCbCr 8-bpc TIFF", 3, 8, 1 },
#endif
	{ NULL, NULL, 0, 0, 0 }
};

static void
usage(void)
{
	size_t c;

	printf("Usage: %s [OPTIONS] [FORMAT:]OUTFILE [[FORMAT:]OUTFILE] ...\n\n", progname);
	printf("OPTIONS is one or more of:\n");
	printf("   -h                 Print this usage message\n");
	printf("   -s [WIDTHx]HEIGHT  Specify frame size\n");
	printf("\n");
	printf("FORMAT is one of:\n\n");
	printf("%-16s %-40s %6s %6s %6s\n", "NAME", "DESCRIPTION", "PLANES", "DEPTH", "PLANAR");
	for(c = 0; formats[c].name; c++)
	{
		printf("%-16s %-40s %6d %6d    %c\n", formats[c].name,
			   formats[c].description, formats[c].planes, formats[c].depth,
			   formats[c].planar ? 'Y' : 'N');
	}
}

int
parsesize(char *size, int *width, int *height)
{
	char *t;

	t = NULL;
	*height = strtol(size, &t, 10);
	if(t && *t)
	{
		if(*t == 'x')
		{
			t++;
			*width = *height;
			*height = strtol(t, &t, 10);
		}
		else
		{
			fprintf(stderr, "%s: '%s': cannot parse frame size\n", progname, size);
			return -1;
		}
	}
	else
	{
		*width = (*height * 16) / 9;
	}
	if(*width < 320 || *height < 180)
	{
		fprintf(stderr, "%s: '%s' is smaller than minimum 320x180\n", progname, size);
		return -1;
	}
	return 0;
}

int
main(int argc, char **argv)
{
	size_t c;
	image *i;
	FILE *f;
	int e, ch, width, height;
	char *t, *format;

	progname = argv[0];
	width = 1920;
	height = 1080;
	while((ch = getopt(argc, argv, "hs:t:")) != -1)
	{
		switch(ch)
		{
		case 'h':
			usage();
			return 0;
		case 's':
			if(parsesize(optarg, &width, &height))
			{
				return 1;
			}
			break;
		case '?':
			usage();
			return 1;
		}
	}
	if(optind == argc)
	{
		usage();
		return 1;
	}
	colourmap_init();
	i = image_create(PF_YCBCR, width, height);
	if(!i)
	{
		fprintf(stderr, "%s: failed to allocate image\n", argv[0]);
		exit(255);
	}
	image_clear(i, &black);

	testcard_ebu100(i);

	e = 0;
	for(; optind < argc; optind++)
	{
		t = strchr(argv[optind], ':');
		if(t)
		{
			format = argv[optind];
			*t = 0;
			t++;
		}
		else
		{
			format = DEFAULT_FORMAT;
			t = argv[optind];
		}
		for(c = 0; formats[c].name; c++)
		{
			if(!strcmp(formats[c].name, format))
			{
				if(formats[c].fn(i, t))
				{
					fprintf(stderr, "%s: %s: %s\n", progname, t, strerror(errno));
					e = 1;
				}
				break;
			}			
		}
		if(!formats[c].name)
		{
			fprintf(stderr, "%s: unsupported format '%s'\n", progname, format);
			e = 1;
		}
	}

	return e;
}
