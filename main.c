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
#define DEFAULT_GENERATOR               "ebu100"

static const char *progname;

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

static generator generators[] = {
	{ "ebu100", generate_ebu100, "EBU 100% colour bars", PF_YCBCR },
	{ "ebu75", generate_ebu75, "EBU 75% colour bars", PF_YCBCR },
	{ "ebu3325-1", generate_ebu3325_1, "EBU Tech 3325: Test pattern 1", PF_YCBCR },
	{ "ebu3325-2", generate_ebu3325_2, "EBU Tech 3325: Test pattern 2", PF_YCBCR },
	{ "ebu3325-5", generate_ebu3325_5, "EBU Tech 3325: Test patterns 5-1...5-15", PF_YCBCR },
	{ "ebu3325-5r", generate_ebu3325_5_red, "EBU Tech 3325: Test pattern 5-red", PF_YCBCR },
	{ "ebu3325-5g", generate_ebu3325_5_green, "EBU Tech 3325: Test pattern 5-green", PF_YCBCR },
	{ "ebu3325-5b", generate_ebu3325_5_blue, "EBU Tech 3325: Test pattern 5-blue", PF_YCBCR },
	{ "blacklevel", generate_blacklevel, "Black/sub-black checkerboard", PF_YCBCR },
	{ "whitelevel", generate_whitelevel, "White/super-white checkerboard", PF_YCBCR },
	{ NULL, NULL, NULL, 0 }
};

static void
usage(void)
{
	size_t c;

	printf("Usage: %s [OPTIONS] [FORMAT:]OUTFILE [[FORMAT:]OUTFILE] ...\n\n", progname);
	printf("OPTIONS is one or more of:\n");
	printf("   -h                 Print this usage message\n");
	printf("   -s [WIDTHx]HEIGHT  Specify frame size\n");
	printf("   -t TYPE            Generate a TYPE pattern\n");
	printf("   -f INDEX           Specify frame index (for multi-frame patterns)\n");
	printf("   -T WIDE[xTALL]     Split the image into WIDE x TALL tiles (default for both is 1)\n");
	printf("\n");
	printf("FORMAT is one of:\n\n");
	printf("%-16s %-40s %6s %6s %6s\n", "NAME", "DESCRIPTION", "PLANES", "DEPTH", "PLANAR");
	for(c = 0; formats[c].name; c++)
	{
		printf("%-16s %-40s %6d %6d    %c\n", formats[c].name,
			   formats[c].description, formats[c].planes, formats[c].depth,
			   formats[c].planar ? 'Y' : 'N');
	}
	printf("\n");
	printf("TYPE is one of:\n\n");
	printf("%-16s %-40s\n", "NAME", "DESCRIPTION");
	for(c = 0; generators[c].name; c++)
	{
		printf("%-16s %-40s\n", generators[c].name, generators[c].description);
	}
	printf("\n");
	printf("The default format is '%s', and the default pattern type is '%s'.\n",
		   DEFAULT_FORMAT, DEFAULT_GENERATOR);
}

static int
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

static int
parsetiles(char *tiles, uint32_t *wide, uint32_t *tall)
{
	char *t;
	long l;

	t = NULL;
	l = strtol(tiles, &t, 10);
	if(l < 1 || l > 64)
	{
		fprintf(stderr, "%s: cannot parse tile size '%s'\n", progname, tiles);
		return -1;
	}
	*wide = l;
	*tall = 1;
	if(t && *t)
	{
		if(*t != 'x')
		{
			fprintf(stderr, "%s: cannot parse tile size '%s'\n", progname, tiles);
			return -1;
		}
		t++;
		if(*t)
		{
			l = strtol(t, &t, 10);
			if(l < 1 || l > 64)
			{
				fprintf(stderr, "%s: cannot parse tile size '%s'\n", progname, tiles);
				return -1;
			}
			*tall = l;
		}
	}
	return 0;
}

static int
generateframe(image *i, generator *g, uint32_t width, uint32_t height, uint32_t frame, uint32_t xtiles, uint32_t ytiles, uint32_t *xsize, uint32_t *ysize)
{
	uint32_t xspace, yspace, yc, xc, x, y;

	image_viewport_reset(i);
	image_clear(i, &black);
	if(!*xsize)
	{
		*xsize = (uint32_t) round((double) width / (double) xtiles);
		*ysize = (uint32_t) round((double) height / (double) ytiles);
	}
	yspace = *ysize;
	y = 0;
	for(yc = 0; yc < ytiles; yc++)
	{
		if(yc == ytiles - 1)
		{
			yspace = height - y;
		}
		x = 0;
		xspace = *xsize;
		for(xc = 0; xc < xtiles; xc++)
		{
			if(xc == xtiles - 1)
			{
				xspace = width - x;
			}
			image_viewport(i, x, y, xspace, yspace);
			if(g->fn(i, frame))
			{
				return -1;
			}
			x += xspace;
		}
		y += yspace;
	}
	return 0;
}

int
main(int argc, char **argv)
{
	size_t c;
	unsigned long frame;
	image *i;
	int e, ch, width, height;
	char *t, *format, *pattern;
	uint32_t xtiles, ytiles, xsize, ysize;

	progname = argv[0];
	width = 1920;
	height = 1080;
	xtiles = 1;
	ytiles = 1;
	xsize = 0;
	ysize = 0;
	pattern = NULL;
	while((ch = getopt(argc, argv, "hs:t:f:T:")) != -1)
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
		case 'T':
			if(parsetiles(optarg, &xtiles, &ytiles))
			{
				return 1;
			}
			break;
		case 'f':
			t = NULL;
			frame = strtoul(optarg, &t, 10);
			if(t && *t)
			{
				fprintf(stderr, "%s: invalid frame number '%s'\n", progname, optarg);
				return 1;
			}
			break;
		case 't':
			if(pattern)
			{
				fprintf(stderr, "%s: pattern type may only be specified once\n", progname);
				return 1;
			}
			for(c = 0; generators[c].name; c++)
			{
				if(!strcmp(generators[c].name, optarg))
				{
					pattern = optarg;
					break;
				}
			}
			if(!generators[c].name)
			{
				fprintf(stderr, "%s: unsupported pattern type '%s'\n", progname, optarg);
				fprintf(stderr, "See '%s -h' for a list of supported pattern types\n", progname);
				return 1;
			}
			break;
		case '?':
			fprintf(stderr, "See '%s -h' for usage information\n", progname);
			return 1;
		}
	}
	if(!pattern)
	{
		pattern = DEFAULT_GENERATOR;
	}
	if(optind == argc)
	{
		usage();
		return 1;
	}
	colourmap_init();
	for(c = 0; generators[c].name; c++)
	{
		if(!strcmp(generators[c].name, pattern))
		{
			break;
		}
	}
	if(!generators[c].name)
	{
		fprintf(stderr, "%s: internal error: pattern type '%s' not found\n", progname, pattern);
		return 1;
	}
	i = image_create(generators[c].pixelformat, width, height);
	if(!i)
	{
		fprintf(stderr, "%s: failed to allocate image\n", argv[0]);
		return 1;
	}
	if(generateframe(i, &(generators[c]), width, height, frame, xtiles, ytiles, &xsize, &ysize))
	{
		fprintf(stderr, "%s: %s: %s\n", progname, pattern, strerror(errno));
		return 1;
	}
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
					e |= 1;
				}
				break;
			}			
		}
		if(!formats[c].name)
		{
			fprintf(stderr, "%s: unsupported format '%s'\n", progname, format);
			e |= 2;
		}
	}
	if((e & 2))
	{
		fprintf(stderr, "See '%s -h' for a list of supported formats\n", progname);
	}	
	return e ? 1 : 0;
}
