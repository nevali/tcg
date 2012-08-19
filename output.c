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

static const char *
parsepattern(output *p, image *i)
{
	char format[64];
	char *buf, *dest, *f;
	size_t c, len;
	int top;
	int prev, width, zeropad, neg;

	len = 0;
	prev = 0;
	dest = p->filename;
	top = -1;
	for(c = 0; p->pattern[c]; c++)
	{
		top = -1;
		if(prev == '%')
		{
			top = c - 1;
			if(p->pattern[c] == '%')
			{
				if(i)
				{
					*dest = '%';
					dest++;
				}
				prev = 0;
				continue;
			}
			neg = 0;
			if(p->pattern[c] == '-')
			{
				neg = 1;
				c++;
				if(!p->pattern[c])
				{
					break;
				}
			}
			zeropad =0;
			while(p->pattern[c] == '0')
			{
				zeropad = 1;
				c++;
			}
			if(!p->pattern[c])
			{
				break;
			}
			width = 0;
			while(isdigit(p->pattern[c]))
			{
				width = (width * 10) + (p->pattern[c] - '0');
				c++;
			}
			if(!p->pattern[c])
			{
				break;
			}
			switch(p->pattern[c])
			{
			case 'd':
				p->ispattern = 1;
				if(i)
				{
					f = format;
					*f = '%';
					f++;
					if(width)
					{
						if(neg)
						{
							*f = '-';
							f++;
						}
						if(zeropad)
						{
							*f = '0';
							f++;
						}
						f += sprintf(f, "%d", width);
					}
					*f = 'l';
					f++;
					*f = 'u';
					f++;
					*f = 0;
					dest += sprintf(dest, format, (unsigned long) i->frame);
				}
				else
				{
					len += width + 32;
				}
				break;
			default:
				if(i)
				{
					/* Just write the characters verbatim */
					for(; (size_t) top <= c; top++)
					{
						*dest = p->pattern[top];
						dest++;
					}
				}
				else
				{
					fprintf(stderr, "%s: warning: unexpected type '%c' in format string '%s'\n", progname, p->pattern[c], p->pattern);
				}
			}
			top = -1;
			prev = 0;
			continue;
		}
		if(i && p->pattern[c] != '%')
		{
			*dest = p->pattern[c];
			dest++;
		}
		prev = p->pattern[c];
	}
	if(top != -1)
	{
		if(i)
		{
			for(; (size_t) top <= c; top++)
			{
				*dest = p->pattern[top];
				dest++;
			}
		}
		else
		{
			fprintf(stderr, "%s: warning: unexpected end of format '%s' in '%s'\n", progname, &(p->pattern[top]), p->pattern);			
		}
	}	
	if(i)
	{
		*dest = 0;
		return p->filename;
	}
	if(p->ispattern)
	{
		buf = malloc(len + c + 1);
		if(!buf)
		{
			return NULL;
		}
		p->filename = buf;
		return p->filename;
	}
	return p->pattern;
}

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
		if(!parsepattern(&(p[c]), NULL))
		{
			return NULL;
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
		free(outputs[c].filename);
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

/* Return the output filename for a given frame */
const char *
output_filename(output *o, image *i, int *shouldclose)
{
	if(o->ispattern)
	{
		/* Open a new file based upon the pattern */
		*shouldclose = 1;
		return parsepattern(o, i);
	}
	/* Write to the existing file */
	*shouldclose = 0;
	return o->pattern;
}

/* Return an open stdio file for a given frame */
FILE *
output_file(output *o, image *i, int *shouldclose)
{
	const char *fn;

	if(o->ispattern)
	{
		/* Open a new file based upon the pattern */
		*shouldclose = 1;
		fn = parsepattern(o, i);
		if(!fn)
		{
			return NULL;
		}
		return fopen(fn, "w");
	}
	*shouldclose = 0;
	if(!o->d.f)
	{
		o->d.f = fopen(o->pattern, "w");
	}
	return o->d.f;
}

