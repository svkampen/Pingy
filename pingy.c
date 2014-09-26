#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h> // eww, I know, but oh well.
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#define PNG_DEBUG 3
#define _cb(X) abs((X)*(X)*(X))
#include <png.h>

typedef uint8_t (*color_func)(int, int);
int width;
int height;
int cur_rand;

int write_png(char *filename, int width, int height,
		color_func r, color_func g, color_func b) {
	FILE *fp;
	int code = 0;

	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep row;

	fp = fopen(filename, "wb");
	if (!fp) {
		fprintf(stderr, "Could not open file %s for writing.\n", filename);
		return 1;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fprintf(stderr, "Could not allocate write struct.\n");
		code = 1;
		goto finalize;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct.\n");
		code = 1;
		goto finalize;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation.\n");
		code = 1;
		goto finalize;
	}

	png_init_io(png_ptr, fp);

	png_set_IHDR(png_ptr, info_ptr, width, height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, info_ptr);

	/*
	 * You made it! After roughly 200 years of setup, we're at the
	 * interesting part, the writing.
	 * A row is 3 bytes * the width of the image
	 * Let's set the RGB values for each pixel in the row and
	 * write the row!
	 *
	 * After this, we're basically done.
	 */
	row = (png_bytep) malloc(3 * width);
	int x, y;

	for (y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x) {
			cur_rand = rand();
			png_bytep pixel = &row[x*3];
			pixel[0] = r(x, y);
			pixel[1] = g(x, y);
			pixel[2] = b(x, y);
		}
		png_write_row(png_ptr, row);
	}

	png_write_end(png_ptr, NULL);

finalize:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);

	return code;
}

uint8_t wrap(int n, int max) {
	while (n < 0) {
		n = max - n;
	}

	while (n > max) {
		n -= max;
	}

	return n;
}

uint8_t r_base(int x, int y) {
	float s = 2*(.6/(y + 90));
	float out = ((width+x) * s);
	out += (y * s);
	out = (int)out % 2;

	out += ((2*width - x) * s);
	out += (y * s);
	out = (int)out % 2;

	return out * 255;
}

uint8_t r(int x, int y) {
	int out = 0;;
	if (r_base(x, y)) {
		out += ((float)cur_rand/INT_MAX)*100;
		out += (((y ^ x) % 3) * 255) << 4;
	}
	return out;
}

uint8_t g(int x, int y) {
	return r(x, y);
}

uint8_t b(int x, int y) {
	return r(x, y);
	int out = 0;
	if (!r_base(x, y)) {
		out += (((y ^ x) % 3) * (cur_rand % 255)) << 4;
	} else {
		out += ((float)cur_rand/INT_MAX)*100;
	}
	return out;
}

int main(int argc, char *argv[]) {
	char *filename = argv[1];
	width = 1024;
	height = 1024;

	if (argc == 4) {
		width = atoi(argv[2]);
		height = atoi(argv[3]);
	}
		
	write_png(filename, width, height, r, g, b);
	srand(time(NULL));
}
