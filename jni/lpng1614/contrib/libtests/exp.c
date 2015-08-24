// Adapted from makepng.c distributed with libpng

#define _ISOC99_SOURCE /* for strtoull */

#include <stddef.h> /* for offsetof */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>

#if defined(HAVE_CONFIG_H) && !defined(PNG_NO_CONFIG_H)
#  include <config.h>
#endif

/* Define the following to use this test against your installed libpng, rather
 * than the one being built here:
 */
#ifdef PNG_FREESTANDING_TESTS
#  include <png.h>
#else
#  include "../../png.h"
#endif
/*
static void set_value(png_bytep row, size_t rowbytes, png_uint_32 x, png_uint_32 value) {
	int bit_depth=8;
	x *= 8;
	png_uint_32 offset = x >> 3;
	if(value>255) value=255;

	if (offset < rowbytes && (bit_depth < 16 || offset+1 < rowbytes)) {
		row += offset;
		*row = value;
	} else {
		fprintf(stderr, "makepng: row buffer overflow (internal error)\n");
		exit(1);
	}
}
*/

static void set_RGBA(png_bytep row,size_t rowbytes,png_uint_32 x,unsigned char r,unsigned char g,unsigned char b,unsigned char a) {
/*	x *= 8; 
	png_uint_32 offset = x >> 3;*/
	if (x < rowbytes) {
		row += x;
		*row = r;
		*(row+1) = g;
		*(row+2) = b;
		*(row+3) = a;
	} else {
		fprintf(stderr, "makepng: row buffer overflow (internal error)\n");
		exit(1);
	}
}

static void generate_row(png_bytep row, size_t rowbytes, unsigned int y, int color_type, int bit_depth) {
	png_uint_32 x;
	for (x=0; x<500; ++x) {
		set_RGBA(row,rowbytes,4*x,255,x>>1,255,y>>1);
/*		set_value(row, rowbytes, 4*x+0, 255);
		set_value(row, rowbytes, 4*x+1, y>>1);
		set_value(row, rowbytes, 4*x+2, y>>1);
		set_value(row, rowbytes, 4*x+3, y>>1);*/
	}
}


static void PNGCBAPI makepng_warning(png_structp png_ptr, png_const_charp message) {
   const char **ep = png_get_error_ptr(png_ptr);
   const char *name;

   if (ep != NULL && *ep != NULL)
      name = *ep;

   else
      name = "makepng";

  fprintf(stderr, "%s: warning: %s\n", name, message);
}

static void PNGCBAPI makepng_error(png_structp png_ptr, png_const_charp message) {
   makepng_warning(png_ptr, message);
   png_longjmp(png_ptr, 1);
}

static int /* 0 on success, else an error code */
write_png(const char **name, FILE *fp, int color_type, int bit_depth,
   volatile png_fixed_point gamma,
   unsigned int filters, unsigned int *colors)
{
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, name, makepng_error, makepng_warning);
	volatile png_infop info_ptr = NULL;
	volatile png_bytep row = NULL;

	if (png_ptr == NULL) {
		fprintf(stderr, "makepng: OOM allocating write structure\n");
		return 1;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_structp nv_ptr = png_ptr;
		png_infop nv_info = info_ptr;

		png_ptr = NULL;
		info_ptr = NULL;
		png_destroy_write_struct(&nv_ptr, &nv_info);
		if (row != NULL) free(row);
		return 1;
	}

	/* Allow benign errors so that we can write PNGs with errors */
	png_set_benign_errors(png_ptr, 1/*allowed*/);
	png_init_io(png_ptr, fp);

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) png_error(png_ptr, "OOM allocating info structure");

	unsigned int size = 500;
	unsigned int i;

	png_set_IHDR(png_ptr, info_ptr, size, size, bit_depth, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	if (gamma == PNG_DEFAULT_sRGB) png_set_sRGB(png_ptr, info_ptr, PNG_sRGB_INTENT_ABSOLUTE);

	/* Write the file header. */
	png_write_info(png_ptr, info_ptr);

	/* Restrict the filters */
	png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, filters);

	int passes = png_set_interlace_handling(png_ptr);
	int pass;
	png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	row = malloc(rowbytes);

	if (row == NULL) png_error(png_ptr, "OOM allocating row buffer");

	for (pass = 0; pass < passes; ++pass) {
		unsigned int y;
		for (y=0; y<size; ++y) {
			generate_row(row, rowbytes, y, color_type, bit_depth);
			png_write_row(png_ptr, row);
		}
	}

	/* Finish writing the file. */
	png_write_end(png_ptr, info_ptr);


	png_structp nv_ptr = png_ptr;
	png_infop nv_info = info_ptr;

	png_ptr = NULL;
	info_ptr = NULL;
	png_destroy_write_struct(&nv_ptr, &nv_info);

	free(row);
	return 0;
}



int main(int argc, char **argv) {
	FILE *fp = stdout;
	const char *file_name = NULL;
	unsigned int colors[5];
	unsigned int filters = PNG_ALL_FILTERS;
	int color_type,bit_depth;
	png_fixed_point gamma;
 
	memset(colors, 0, sizeof colors);

	gamma = PNG_DEFAULT_sRGB;
	color_type = PNG_COLOR_TYPE_RGB_ALPHA;
	bit_depth=8;
	//filters &= ~(PNG_FILTER_NONE | PNG_FILTER_AVG);
	filters = PNG_FILTER_NONE;
//	fp = fopen("lixo.png", "wb");
	int ret = write_png(&file_name, fp, color_type, bit_depth, gamma, filters, colors);
	if (ret != 0 && file_name != NULL) remove(file_name);
	return ret;
}

