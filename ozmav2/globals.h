#define APPTITLE		"OZMAV2"
#define VERSION			"0.1"

#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480

// ----------------------------------------

enum { true = 1, false = 0 };

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

#include "oz.h"
#include "draw.h"
#include "zelda.h"
#include "f3dex2.h"
#include "camera.h"
#include "dump.h"

// ----------------------------------------

#define Read16(Buffer, Offset) \
	(Buffer[Offset] << 8) | Buffer[Offset + 1]

#define Read32(Buffer, Offset) \
	(Buffer[Offset] << 24) | (Buffer[Offset + 1] << 16) | (Buffer[Offset + 2] << 8) | Buffer[Offset + 3]

#define Write32(Buffer, Offset, Value) \
	Buffer[Offset] = (Value & 0xFF000000) >> 24; \
	Buffer[Offset + 1] = (Value & 0x00FF0000) >> 16; \
	Buffer[Offset + 2] = (Value & 0x0000FF00) >> 8; \
	Buffer[Offset + 3] = (Value & 0x000000FF);

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

// ----------------------------------------

#ifdef WIN32
PFNGLMULTITEXCOORD1FARBPROC			glMultiTexCoord1fARB;
PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB;
PFNGLMULTITEXCOORD3FARBPROC			glMultiTexCoord3fARB;
PFNGLMULTITEXCOORD4FARBPROC			glMultiTexCoord4fARB;
PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB;
#endif

PFNGLGENPROGRAMSARBPROC				glGenProgramsARB;
PFNGLBINDPROGRAMARBPROC				glBindProgramARB;
PFNGLDELETEPROGRAMSARBPROC			glDeleteProgramsARB;
PFNGLPROGRAMSTRINGARBPROC			glProgramStringARB;
PFNGLPROGRAMENVPARAMETER4FARBPROC	glProgramEnvParameter4fARB;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC	glProgramLocalParameter4fARB;

// ----------------------------------------

struct __zProgram {
	bool IsRunning;
	bool Key[256];

	unsigned int FragCachePosition;
	unsigned int TextureCachePosition;

	int MousePosX, MousePosY;
	int MouseCenterX, MouseCenterY;
	bool MouseButtonDown;

	unsigned int WavefrontObjVertCount;
	unsigned int WavefrontObjMaterialCnt;
	FILE * FileWavefrontObj;
	FILE * FileWavefrontMtl;

	char Title[256];
	char AppPath[MAX_PATH];
	int HandleOptions;
	int HandleAbout;
	int HandleLoadScene;
};

struct __zOpenGL {
	char * ExtensionList;
	char ExtSupported[256];
	char ExtUnsupported[256];
	bool IsExtUnsupported;
	bool Ext_MultiTexture;
	bool Ext_TexMirroredRepeat;
	bool Ext_FragmentProgram;
};

struct __zOptions {
	unsigned int SceneNo;
	int DebugLevel;
	int CreateLog;
	int DumpModel;
	int EnableTextures;
	int EnableCombiner;
};

struct __zROM {
	bool IsROMLoaded;
	char FilePath[MAX_PATH];
	char Filename[256];
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
};

struct __zRAM {
	bool IsSet;
	unsigned int Size;
	unsigned char * Data;
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

	unsigned char EchoLevel;
};

struct __zVertex {
	short X;
	short Y;
	short Z;
	short S;
	short T;
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

struct __zPalette {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

struct __zTexture {
	unsigned int Offset;
	unsigned int PalOffset;

	unsigned int Format;
	unsigned int Tile;
	unsigned int Width;
	unsigned int RealWidth;
	unsigned int Height;
	unsigned int RealHeight;
	unsigned int ULT, ULS;
	unsigned int LRT, LRS;
	unsigned int LineSize, Palette;
	unsigned int MaskT, MaskS;
	unsigned int ShiftT, ShiftS;
	unsigned int CMT, CMS;
	float ScaleT, ScaleS;
	float ShiftScaleT, ShiftScaleS;
};

struct __zRGBA {
	float R;
	float G;
	float B;
	float A;
};

struct __zFillColor {
	float R;
	float G;
	float B;
	float A;
	float Z;
	float DZ;
};

struct __zPrimColor {
	float R;
	float G;
	float B;
	float A;
	float L;
	unsigned short M;
};

struct __zFragmentCache {
	unsigned int zCombiner0;
	unsigned int zCombiner1;
	GLuint ProgramID;
};

struct __zTextureCache {
	unsigned int Offset;
	unsigned int RealWidth;
	unsigned int RealHeight;
	GLuint TextureID;
};

struct __zCamera {
	float AngleX, AngleY;
	float X, Y, Z;
	float LX, LY, LZ;
};

struct __zGfx {
	int DLStack[16];
	int DLStackPos;

	unsigned int DLOffset[256][512];
	int DLCount[256];
	int DLCountTotal;
	GLuint GLListCount[256];

	unsigned int ChangedModes;
	unsigned int GeometryMode;
	unsigned int OtherModeL;
	unsigned int OtherModeH;
	GLfloat LightAmbient[4];
	GLfloat LightDiffuse[4];
	GLfloat LightSpecular[4];
	GLfloat LightPosition[4];
	unsigned int Store_RDPHalf1, Store_RDPHalf2;
	unsigned int Combiner0, Combiner1;

	struct __zRGBA BlendColor;
	struct __zRGBA EnvColor;
	struct __zRGBA FogColor;
	struct __zFillColor FillColor;
	struct __zPrimColor PrimColor;

	bool IsMultiTexture;
	int CurrentTexture;

	GLuint GLTextureID[512];
	int GLTextureCount;
};

struct __zObject {
	bool IsSet;
	unsigned int StartOffset;
	unsigned int EndOffset;
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
};

struct __zMapActor {
	unsigned short Number;
	short X;
	short Y;
	short Z;
	short RX;
	short RY;
	short RZ;
	unsigned short Var;
};

// ----------------------------------------

extern struct __zProgram zProgram;
extern struct __zOpenGL zOpenGL;
extern struct __zOptions zOptions;

extern struct __zROM zROM;
extern struct __zGame zGame;
extern struct __zRAM zRAM[64];
extern struct __zHeader zSHeader[256];
extern struct __zHeader zMHeader[256][256];

extern struct __zGfx zGfx;
extern struct __zPalette zPalette[256];

extern struct __zVertex zVertex[32];

extern struct __zTexture zTexture[2];

extern struct __zObject zObject[1024];
extern struct __zActor zActor[1024];
extern struct __zMapActor zLink[256];
extern struct __zMapActor zMapActor[256][256];

extern struct __zFragmentCache zFragmentCache[256];
extern struct __zTextureCache zTextureCache[512];

extern struct __zCamera zCamera;

// ----------------------------------------

extern void GetFilePath(unsigned char * FullPath, unsigned char * Target);
extern void GetFileName(unsigned char * FullPath, unsigned char * Target);
extern inline void dbgprintf(int Level, int Type, char * Format, ...);
