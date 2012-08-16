tiffdir = /opt/ports

CFLAGS = -W -Wall -O0 -g -I$(tiffdir)/include
LDFLAGS = -g -L$(tiffdir)/lib
LIBS = -ltiff -lm

OUT = tcg
OBJ = image.o main.o colourmap.o draw.o \
	ebu100.o \
	ycc444_16_planar.o \
	ycc444_8_planar.o \
	tiff_ycc444_16.o \
	tiff_ycc444_8.o


####

all: $(OUT)

clean:
	rm -f $(OUT) $(OBJ)

####

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) -o $(OUT) $(OBJ) $(LIBS)
