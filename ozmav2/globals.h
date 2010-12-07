#define APPTITLE		"OZMAV2"
#define VERSION			"v1.0"

#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480

// ----------------------------------------

enum { true = 1, false = 0 };

//union { unsigned long ul; float f; } u;

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
#include "zelda.h"
#include "camera.h"
#include "mips-eval.h"
#include "mips.h"
#include "zactors.h"

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

#define FIXED2FLOATRECIP1	0.5f
#define FIXED2FLOATRECIP2	0.25f
#define FIXED2FLOATRECIP3	0.125f
#define FIXED2FLOATRECIP4	0.0625f
#define FIXED2FLOATRECIP5	0.03125f
#define FIXED2FLOATRECIP6	0.015625f
#define FIXED2FLOATRECIP7	0.0078125f
#define FIXED2FLOATRECIP8	0.00390625f
#define FIXED2FLOATRECIP9	0.001953125f
#define FIXED2FLOATRECIP10	0.0009765625f
#define FIXED2FLOATRECIP11	0.00048828125f
#define FIXED2FLOATRECIP12	0.00024414063f
#define FIXED2FLOATRECIP13	0.00012207031f
#define FIXED2FLOATRECIP14	6.1035156e-05f
#define FIXED2FLOATRECIP15	3.0517578e-05f
#define FIXED2FLOATRECIP16	1.5258789e-05f

#define _FIXED2FLOAT( v, b ) \
	((float)v * FIXED2FLOATRECIP##b)

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

	int MouseMode;
	int MousePosX, MousePosY;
	int MouseCenterX, MouseCenterY;
	bool MouseButtonLDown, MouseButtonRDown;

	__Vect3D SceneCoords;

	char Title[256];
	char WndTitle[256];
	char AppPath[MAX_PATH];
	int HandleOptions;
	int HandleAbout;
	int HandleLoadScene;
	int HandleModifyActor;

	GLuint AxisMarker;
	GLuint CubeDL;
};

struct __zOptions {
	unsigned int SceneNo;
	int MapToRender;
	int DebugLevel;
	int CreateLog;
	int DumpModel;
	int EnableTextures;
	int EnableCombiner;
	int EnableActors;
	int SelectedActor;
	int SelectedActorMap;
};

struct __zROM {
	char FilePath[MAX_PATH];
	char Filename[MAX_PATH];
	char _Pad0;
	bool IsROMLoaded;
	unsigned int Size;
	char Title[20];
	char _Pad1;
	char GameID[4];
	char _Pad2;
	int Version;
	char _Pad3;
	unsigned int CRC1;
	unsigned int CRC2;
	unsigned char * Data;
};

struct __zGame {
	int GameType;					// 0 = OoT, 1 = MM
	unsigned int DMATableOffset;
	int CodeFileNo;
	int CodeRAMOffset;
	unsigned int SceneTableOffset;
	unsigned int ObjectTableOffset;
	unsigned int ActorTableOffset;
	int SceneCount;
	char TitleText[256];
	bool IsCompressed;
	bool HasFilenames;
	unsigned int FilenameTableOffset;

	DMA DMATable;
	DMA Bootfile;

	unsigned char * CodeBuffer;
	int ObjectCount;
	int ActorCount;
};

struct __zHeader {
	unsigned char ActorCount;
	unsigned int ActorOffset;
	unsigned char LinkCount;
	unsigned int LinkOffset;
	unsigned int ColDataOffset;
	unsigned char MapCount;
	unsigned int MapOffset;
	unsigned int MeshHeaderOffset;
	unsigned char ObjCount;
	unsigned int ObjOffset;
	unsigned char DoorCount;
	unsigned int DoorOffset;
	unsigned char EnvSetCount;
	unsigned int EnvSetOffset;
	unsigned int MapTime;
	unsigned char TimeFlow;
	unsigned char Skybox;
	unsigned char TimeOfDay;
	unsigned char EchoLevel;
	unsigned char GameplayObj;
	unsigned int AltHeaderOffset;

	unsigned int AltHeaders[64];
	unsigned int AltHeaderCount;

	unsigned int FileStart;
	unsigned int FileEnd;
	unsigned int FileSize;
};

struct __zCamera {
	float AngleX, AngleY;
	float X, Y, Z;
	float LX, LY, LZ;
	float RotX, RotY;
};

struct __zGfx {
	unsigned int DLOffset[256][512];
	int DLCount[256];
	GLuint GLLists[256];
};

struct __zObject {
	bool IsSet;
	unsigned int StartOffset;
	unsigned int EndOffset;
	int Size;
	unsigned char * Data;
	char * Name;
};

struct __zActor {
	bool IsSet;
	unsigned int PStart;
	unsigned int PEnd;
	unsigned int VStart;
	unsigned int VEnd;
	unsigned int ProfileVStart;
	unsigned int NameRStart;
	unsigned int NameCStart;
	char Name[128];
	unsigned short Object;
	int Size;
	unsigned char * Data;
	float Scale;
	unsigned int BoneSetup;
	unsigned int Animation;
	unsigned int DisplayList;
};

struct __zMapActor {
	unsigned short Number;
	__Vect3D Pos;
	__Vect3D Rot;
	unsigned short Var;

	GLuint GLDList;
};

struct __zDoor {
	unsigned char RoomFront;
	unsigned char FadeFront;
	unsigned char RoomRear;
	unsigned char FadeRear;
	unsigned short Number;
	__Vect3D Pos;
	__Vect3D Rot;
	unsigned short Var;

	GLuint GLDList;
};

// ----------------------------------------

extern struct __zProgram zProgram;
extern struct __zOptions zOptions;

extern struct __zROM zROM;
extern struct __zGame zGame;
extern struct __zHeader zSHeader[256];
extern struct __zHeader zMHeader[256][256];

extern struct __zGfx zGfx;

extern struct __zObject zObject[1024];
extern struct __zActor zActor[1024];
extern struct __zMapActor zLink[256];
extern struct __zMapActor zMapActor[256][256];
extern struct __zDoor zDoor[256];

extern struct __zCamera zCamera;

// ----------------------------------------

extern int DoMainKbdInput();
extern void GetFilePath(char * FullPath, char * Target);
extern void GetFileName(char * FullPath, char * Target);
extern inline void dbgprintf(int Level, int Type, char * Format, ...);
