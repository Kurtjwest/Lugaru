#pragma once

class ImageRec;

bool load_png(const char* fname, ImageRec& tex);
bool save_screenshot_png(const char* file_name);