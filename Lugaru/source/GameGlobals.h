#pragma once

#include <string>

#include "Environment/Skybox.hpp"
#include "Graphic/Texture.hpp"
#include "Graphic/Text.hpp"
#include "Graphic/Models.hpp"

namespace Game
{
	extern Texture terraintexture;
	extern Texture terraintexture2;
	extern Texture loadscreentexture;
	extern Texture Maparrowtexture;
	extern Texture Mapboxtexture;
	extern Texture Mapcircletexture;
	extern Texture cursortexture;
	extern GLuint screentexture;
	extern GLuint screentexture2;
	extern Texture Mainmenuitems[10];

	extern int selected;
	extern int keyselect;

	extern int newdetail;
	extern int newscreenwidth;
	extern int newscreenheight;

	extern bool gameon;
	extern float deltah, deltav;
	extern int mousecoordh, mousecoordv;
	extern int oldmousecoordh, oldmousecoordv;
	extern float yaw, pitch;
	extern SkyBox* skybox;
	extern bool cameramode;
	extern bool firstLoadDone;

	extern float leveltime;
	extern float wonleveltime;
	extern float loadtime;

	extern Model hawk;
	extern Vector3 hawkcoords;
	extern Vector3 realhawkcoords;
	extern Texture hawktexture;
	extern float hawkyaw;
	extern float hawkcalldelay;

	extern Model eye;
	extern Model iris;
	extern Model cornea;

	extern bool stealthloading;
	extern int loading;

	extern int musictype;

	extern Vector3 mapcenter;
	extern float mapradius;

	extern Text* text;
	extern Text* textmono;
	extern float fps;

	extern bool editorenabled;
	extern int editortype;
	extern float editorsize;
	extern float editoryaw;
	extern float editorpitch;

	extern int tryquit;

	extern Vector3 pathpoint[30];
	extern int numpathpoints;
	extern int numpathpointconnect[30];
	extern int pathpointconnect[30][30];
	extern int pathpointselected;

	extern int endgame;
	extern bool scoreadded;
	extern int numchallengelevels;

	extern bool console;
	extern std::string consoletext[15];
	extern float consoleblinkdelay;
	extern bool consoleblink;
	extern unsigned consoleselected;

	extern int oldenvironment;
	extern int targetlevel;
	extern float changedelay;

	extern bool waiting;

	extern unsigned short crouchkey, jumpkey, forwardkey, backkey, leftkey, rightkey, drawkey, throwkey, attackkey;
	extern unsigned short consolekey;
}