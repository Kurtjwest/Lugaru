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

#ifndef _SETTINGS_HPP_
#define _SETTINGS_HPP_

struct Settings
{
	float usermousesensitivity = 0.f;
	bool ismotionblur = false;
	bool floatjump = false;
	bool mousejump = false;
	bool ambientsound = false;
	int bloodtoggle = 0;
	bool autoslomo = false;
	bool foliage = false;
	bool musictoggle = false;
	bool trilinear = false;
	bool decalstoggle = false;
	bool invertmouse = false;
	float gamespeed = 0.f;
	float oldgamespeed = 0.f;
	bool damageeffects = false;
	bool texttoggle = false;
	bool devtools = false;
	bool showpoints = false;
	bool showdamagebar = false;
	bool alwaysblur = false;
	bool immediate = false;
	bool velocityblur = false;
	float volume = 0.f;
	int detail = 0;
	int kContextWidth = 0;
	int kContextHeight = 0;
	float screenwidth = 0.f;
	float screenheight = 0.f;
	bool fullscreen = false;
	float minscreenwidth = 0.f;
	float minscreenheight = 0.f;
	float maxscreenwidth = 0.f;
	float maxscreenheight = 0.f;
};

// TODO Get rid of gLobals
extern float usermousesensitivity;
extern bool ismotionblur;
extern bool floatjump;
extern bool mousejump;
extern bool ambientsound;
extern int bloodtoggle;
extern bool autoslomo;
extern bool foliage;
extern bool musictoggle;
extern bool trilinear;
extern bool decalstoggle;
extern bool invertmouse;
extern float gamespeed;
extern float oldgamespeed;
extern bool damageeffects;
extern bool texttoggle;
extern bool devtools;
extern bool showpoints;
extern bool showdamagebar;
extern bool alwaysblur;
extern bool immediate;
extern bool velocityblur;
extern float volume;
extern int detail;
extern int kContextWidth;
extern int kContextHeight;
extern float screenwidth, screenheight;
extern bool fullscreen;
extern float minscreenwidth, minscreenheight;
extern float maxscreenwidth, maxscreenheight;

Settings DefaultSettings();
void SaveSettings();
bool LoadSettings();

#endif
