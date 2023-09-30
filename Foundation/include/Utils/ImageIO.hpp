#pragma once

/*
Copyright (C) 2003, 2010 - Wolfire Games
Copyright (C) 2010-2017 - Lugaru contributors (see AUTHORS file)

This file is part of Lugaru.

Lugaru is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Lugaru is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lugaru.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include "Utils/Callbacks.h"

/**> DATA STRUCTURES <**/
class ImageRec
{
public:
	uint8_t* data = nullptr; // Image Data (Up To 32 Bits)
	unsigned int bpp = 0;    // Image Color Depth In Bits Per Pixel.
	unsigned int sizeX = 0;
	unsigned int sizeY = 0;

	ImageRec();
	~ImageRec();

private:
	/* Make sure this class cannot be copied to avoid memory problems */
	ImageRec(ImageRec const&);
};

bool load_image(const char* fname, ImageRec& tex, ProgressCallback callback);
bool save_screenshot(const char* fname);