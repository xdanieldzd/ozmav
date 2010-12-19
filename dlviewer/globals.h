#define APPTITLE		"DLViewer"
#define VERSION			"v0.0"

#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480

// ----------------------------------------

enum { true = 1, false = 0 };

typedef struct {
	short X, Y, Z;
} __Vect3D;

// ----------------------------------------

#ifdef WIN32
#include "__win32.h"
#else
#include "__linux.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include <png.h>
#include <curses.h>

#include "misaka.h"
#include "badrdp.h"

#include "oz.h"
#include "hud.h"
#include "hud_menu.h"
#include "draw.h"
#include "mouse.h"
#include "camera.h"
#include "dlist.h"
#include "confunc.h"

// ----------------------------------------

#define Read16(Buffer, Offset) \
	(Buffer[Offset] << 8) | Buffer[(Offset) + 1]

#define Read32(Buffer, Offset) \
	(Buffer[Offset] << 24) | (Buffer[(Offset) + 1] << 16) | (Buffer[(Offset) + 2] << 8) | Buffer[(Offset) + 3]

#define Write16(Buffer, Offset, Value) \
	Buffer[Offset] = (Value & 0xFF00) >> 8; \
	Buffer[Offset + 1] = (Value & 0x00FF);

#define Write32(Buffer, Offset, Value) \
	Buffer[Offset] = (Value & 0xFF000000) >> 24; \
	Buffer[Offset + 1] = (Value & 0x00FF0000) >> 16; \
	Buffer[Offset + 2] = (Value & 0x0000FF00) >> 8; \
	Buffer[Offset + 3] = (Value & 0x000000FF);

#define Write64(Buffer, Offset, Value1, Value2) \
	Buffer[Offset] = (Value1 & 0xFF000000) >> 24; \
	Buffer[Offset + 1] = (Value1 & 0x00FF0000) >> 16; \
	Buffer[Offset + 2] = (Value1 & 0x0000FF00) >> 8; \
	Buffer[Offset + 3] = (Value1 & 0x000000FF); \
	Buffer[Offset + 4] = (Value2 & 0xFF000000) >> 24; \
	Buffer[Offset + 5] = (Value2 & 0x00FF0000) >> 16; \
	Buffer[Offset + 6] = (Value2 & 0x0000FF00) >> 8; \
	Buffer[Offset + 7] = (Value2 & 0x000000FF);

#define _SHIFTL( v, s, w )	\
    (((unsigned long)v & ((0x01 << w) - 1)) << s)
#define _SHIFTR( v, s, w )	\
    (((unsigned long)v >> s) & ((0x01 << w) - 1))

#define ArraySize(x)	(sizeof((x)) / sizeof((x)[0]))

#ifndef min
#define min(a, b)				((a) < (b) ? (a) : (b))
#endif

#define GetPaddingSize(Filesize, Factor) \
	(((Filesize / Factor) + 1) * Factor) - Filesize

// ----------------------------------------

struct __zProgram {
	bool IsRunning;
	bool Key[256];

	int WindowWidth, WindowHeight;
	int HandleAbout;

	float LastTime;
	int Frames;
	int LastFPS;

	int MousePosX, MousePosY;
	int MouseCenterX, MouseCenterY;
	bool MouseButtonLDown, MouseButtonRDown;

	__Vect3D SceneCoords;

	char Title[256];
	char WndTitle[256];
	char AppPath[MAX_PATH];

	GLuint GLAxisMarker;
	GLuint GLGrid;

	float ScaleFactor;
	unsigned int UCode;

	GLuint DListGL[2048];
	unsigned int DListAddr[2048];
	int DListCount;
	int DListSel;
};

struct __zOptions {
	int DebugLevel;
	bool EnableHUD;
	bool EnableGrid;
};

struct __zCamera {
	float AngleX, AngleY;
	float X, Y, Z;
	float LX, LY, LZ;
	float CamSpeed;
};

// ----------------------------------------

extern struct __zProgram zProgram;
extern struct __zOptions zOptions;

extern struct __zCamera zCamera;

// ----------------------------------------

extern char * UCodeNames[];

extern int DoMainKbdInput();
extern float ScaleRange(float in, float oldMin, float oldMax, float newMin, float newMax);
extern void GetFilePath(char * FullPath, char * Target);
extern void GetFileName(char * FullPath, char * Target);
extern inline void dbgprintf(int Level, int Type, char * Format, ...);
