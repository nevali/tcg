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

const char *progname;

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
	format *formats;

	printf("Usage: %s [OPTIONS] [FORMAT:]OUTFILE [[FORMAT:]OUTFILE] ...\n\n", progname);
	printf("OPTIONS is one or more of:\n");
	printf("   -h                 Print this usage message\n");
	printf("   -s [WIDTHx]HEIGHT  Specify frame size\n");
	printf("   -t TYPE            Generate a TYPE pattern\n");
	printf("   -f START[-END]     Specify frame index or range (for multi-frame patterns)\n");
	printf("   -T WIDE[xTALL]     Split the image into WIDE x TALL tiles\n");
	printf("                      (default for both is 1)\n");
	printf("\n");
	printf("FORMAT is one of:\n\n");
	printf("%-16s %-40s %6s %6s %6s\n", "NAME", "DESCRIPTION", "PLANES", "DEPTH", "PLANAR");
	formats = output_formats();
	if(formats)
	{
		for(c = 0; formats[c].name; c++)
		{
			printf("%-16s %-40s %6d %6d    %c\n", formats[c].name,
				   formats[c].description, formats[c].planes, formats[c].depth,
			   formats[c].planar ? 'Y' : 'N');
		}
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
parseframeseq(char *seq, unsigned long *firstframe, unsigned long *lastframe)
{
	char *t;
	unsigned long l;

	t = NULL;
	l = strtoul(seq, &t, 10);
	*firstframe = l;
	*lastframe = l;
	if(t && *t)
	{
		if(*t != '-')
		{
			fprintf(stderr, "%s: cannot parse frame sequence '%s'\n", progname, seq);
			return -1;
		}
		t++;
		if(*t)
		{
			l = strtoul(t, &t, 10);
			*lastframe = l;
		}
	}
	return 0;
}

static int
generateframe(image *i, generator *g, uint32_t width, uint32_t height, uint32_t xtiles, uint32_t ytiles, uint32_t *xsize, uint32_t *ysize)
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
			if(g->fn(i))
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
	unsigned long firstframe, lastframe, frame;
	image *i;
	int e, ch, width, height, r;
	char *pattern;
	uint32_t xtiles, ytiles, xsize, ysize;
	output *outputs;

	progname = argv[0];
	width = 1920;
	height = 1080;
	xtiles = 1;
	ytiles = 1;
	xsize = 0;
	ysize = 0;
	firstframe = 0;
	lastframe = 0;
	e = 0;
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
			if(parseframeseq(optarg, &firstframe, &lastframe))
			{
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
	outputs = output_parse(argc - optind, &(argv[optind]));
	if(!outputs)
	{
		fprintf(stderr, "%s: %s\n", progname, strerror(errno));
		return 1;
	}
	/* Attempt to locate the pattern generator */
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
	colourmap_init();
	i = image_create(generators[c].pixelformat, width, height);
	if(!i)
	{
		fprintf(stderr, "%s: failed to allocate image\n", argv[0]);
		return 1;
	}
	/* Generate each of the frames */
	for(frame = firstframe; frame <= lastframe; frame++)
	{
		i->frame = frame;
		if(generateframe(i, &(generators[c]), width, height, xtiles, ytiles, &xsize, &ysize))
		{
			fprintf(stderr, "%s: %s: %s\n", progname, pattern, strerror(errno));
			return 1;
		}
		r = output_store(i, outputs);
		if(r < 0)
		{
			return 1;
		}
		e |= r;
	}
	/* Clean up */
	if((e & 2))
	{
		fprintf(stderr, "See '%s -h' for a list of supported formats\n", progname);
	}	
	return e ? 1 : 0;
}
