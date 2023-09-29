#include "LoadPng.h"

#include <libpng16/png.h>
#include <string>
#include <iostream>

using std::cerr;
using std::endl;


extern int kContextWidth;
extern int kContextHeight;

#include "Utils/ImageIO.hpp"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define Polygon WinPolygon
#include <windows.h>
#undef Polygon
#include <GL/gl.h>
#else
#include "Graphic/gamegl.hpp"
#endif

/* stolen from public domain example.c code in libpng distribution. */
bool load_png(const char* file_name, ImageRec& tex)
{
	bool hasalpha = false;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	bool retval = false;
	png_byte** row_pointers = NULL;
	errno = 0;
	FILE* fp = fopen(file_name, "rb");

	if (fp == NULL) {
		perror((std::string("Couldn't open file ") + file_name).c_str());
		return false;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		goto png_done;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		goto png_done;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		goto png_done;
	}

	png_init_io(png_ptr, fp);
	png_read_png(png_ptr, info_ptr,
		PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING,
		NULL);
	png_get_IHDR(png_ptr, info_ptr, &width, &height,
		&bit_depth, &color_type, &interlace_type, NULL, NULL);

	if (bit_depth != 8) { // transform SHOULD handle this...
		goto png_done;
	}

	if (color_type & PNG_COLOR_MASK_PALETTE) { // !!! FIXME?
		goto png_done;
	}

	if ((color_type & PNG_COLOR_MASK_COLOR) == 0) { // !!! FIXME?
		goto png_done;
	}

	hasalpha = ((color_type & PNG_COLOR_MASK_ALPHA) != 0);
	row_pointers = png_get_rows(png_ptr, info_ptr);
	if (!row_pointers) {
		goto png_done;
	}

	if (!hasalpha) {
		png_byte* dst = tex.data;
		for (int i = height - 1; i >= 0; i--) {
			png_byte* src = row_pointers[i];
			for (unsigned j = 0; j < width; j++) {
				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];
				dst[3] = 0xFF;
				src += 3;
				dst += 4;
			}
		}
	}

	else {
		png_byte* dst = tex.data;
		int pitch = width * 4;
		for (int i = height - 1; i >= 0; i--, dst += pitch) {
			memcpy(dst, row_pointers[i], pitch);
		}
	}

	tex.sizeX = width;
	tex.sizeY = height;
	tex.bpp = 32;
	retval = true;

png_done:
	if (!retval) {
		cerr << "There was a problem loading " << file_name << endl;
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	if (fp) {
		fclose(fp);
	}
	return (retval);
}

bool save_screenshot_png(const char* file_name)
{
	FILE* fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	bool retval = false;

	errno = 0;
	fp = fopen(file_name, "wb");
	if (fp == NULL) {
		perror((std::string("Couldn't open file ") + file_name).c_str());
		return false;
	}

	png_bytep* row_pointers = new png_bytep[kContextHeight];
	png_bytep screenshot = new png_byte[kContextWidth * kContextHeight * 3];
	if ((!screenshot) || (!row_pointers)) {
		goto save_png_done;
	}

	glGetError();
	glReadPixels(0, 0, kContextWidth, kContextHeight,
		GL_RGB, GL_UNSIGNED_BYTE, screenshot);
	if (glGetError() != GL_NO_ERROR) {
		goto save_png_done;
	}

	for (int i = 0; i < kContextHeight; i++) {
		row_pointers[i] = screenshot + ((kContextWidth * ((kContextHeight - 1) - i)) * 3);
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		goto save_png_done;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		goto save_png_done;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		goto save_png_done;
	}

	png_init_io(png_ptr, fp);

	if (setjmp(png_jmpbuf(png_ptr))) {
		goto save_png_done;
	}

	png_set_IHDR(png_ptr, info_ptr, kContextWidth, kContextHeight,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	if (setjmp(png_jmpbuf(png_ptr))) {
		goto save_png_done;
	}

	png_write_image(png_ptr, row_pointers);

	if (setjmp(png_jmpbuf(png_ptr))) {
		goto save_png_done;
	}

	png_write_end(png_ptr, NULL);
	retval = true;

save_png_done:
	png_destroy_write_struct(&png_ptr, &info_ptr);
	delete[] screenshot;
	delete[] row_pointers;
	if (fp) {
		fclose(fp);
	}
	if (!retval) {
		unlink(file_name);
	}
	return retval;
}
