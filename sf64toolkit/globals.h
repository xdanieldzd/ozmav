#define APPTITLE		"Star Fox 64 Toolkit"
#define VERSION			"v1"

#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480

// ----------------------------------------

enum { true = 1, false = 0 };

union { unsigned long ul; float f; } u;

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

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include <curses.h>

#include "misaka.h"

#include "main.h"
#include "n64sums.h"
#include "sf64.h"
#include "oz.h"
#include "draw.h"
#include "camera.h"
#include "sf64_viewer.h"
#include "f3dex.h"

// ----------------------------------------

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

#define ArraySize(x)	(sizeof((x)) / sizeof((x)[0]))

#define GetPaddingSize(Filesize, Factor) \
	(((Filesize / Factor) + 1) * Factor) - Filesize

// ----------------------------------------

typedef struct {
	unsigned int ID;			// always "MIO0"
	unsigned int OutputSize;	// decompressed data size
	unsigned int CompLoc;		// compressed data loc
	unsigned int RawLoc;		// uncompressed data loc
} MIO0Header;

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

struct __Program {
	bool IsRunning;
	bool Key[256];

	bool IsROMLoaded;
	bool IsROMCompressed;
	bool IsViewerEnabled;

	unsigned int FragCachePosition;
	unsigned int TextureCachePosition;

	int MousePosX, MousePosY;
	int MouseCenterX, MouseCenterY;
	bool MouseButtonDown;

	char Title[256];
	char AppPath[MAX_PATH];

	int HandleOptions;
	int HandleAbout;

	unsigned int DMATableOffset;
	int DMAFileCount;
};

struct __OpenGL {
	char * ExtensionList;
	char ExtSupported[256];
	char ExtUnsupported[256];
	bool IsExtUnsupported;
	bool Ext_MultiTexture;
	bool Ext_TexMirroredRepeat;
	bool Ext_FragmentProgram;
};

struct __ROM {
	char Filepath[MAX_PATH];
	char Filename[256];
	/*
	 time_t and clock_t shall be integer or real-floating types.
	 http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/types.h.html#tag_13_67
	*/
	time_t LastModified;
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

struct __RAM {
	bool IsSet;
	unsigned int Size;
	unsigned char * Data;
};

struct __DMA {
	unsigned int VStart;
	unsigned int PStart;
	unsigned int PEnd;
	unsigned int CompFlag;
};

struct __Vertex {
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

struct __Palette {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

struct __Texture {
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

struct __RGBA {
	float R;
	float G;
	float B;
	float A;
};

struct __FillColor {
	float R;
	float G;
	float B;
	float A;
	float Z;
	float DZ;
};

struct __PrimColor {
	float R;
	float G;
	float B;
	float A;
	float L;
	unsigned short M;
};

struct __FragmentCache {
	unsigned int Combiner0;
	unsigned int Combiner1;
	GLuint ProgramID;
};

struct __TextureCache {
	unsigned int Offset;
	unsigned int RealWidth;
	unsigned int RealHeight;
	GLuint TextureID;
};

struct __Gfx {
	int DLStack[16];
	int DLStackPos;

	GLuint GLListCount;

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

	struct __RGBA BlendColor;
	struct __RGBA EnvColor;
	struct __RGBA FogColor;
	struct __FillColor FillColor;
	struct __PrimColor PrimColor;

	bool IsMultiTexture;
	int CurrentTexture;

	GLuint GLTextureID[2048];
	int GLTextureCount;
};

struct __Camera {
	float AngleX, AngleY;
	float X, Y, Z;
	float LX, LY, LZ;
};

struct __Viewer {
	int LevelID;
	int LevelFile;
	int ObjCount;
};

struct __Object {
	unsigned int LvlPos;
	short X;
	short Y;
	short Z;
	short XRot;
	short YRot;
	short ZRot;
	unsigned short ID;
	unsigned short Unk;

	unsigned int DListOffset;
};

// ----------------------------------------

typedef struct {
	char TitleText[256];
	char GameID[5];
	int Version;
	unsigned int CRC1;
	unsigned int CRC2;
	unsigned int DMATableOffset;
} __GameVer;

// ----------------------------------------

extern struct __Program Program;
extern struct __OpenGL OpenGL;
extern struct __ROM ROM;
extern struct __RAM RAM[64];
extern struct __DMA DMA[256];
extern struct __Vertex Vertex[32];
extern struct __Palette Palette[256];
extern struct __Texture Texture[2];
extern struct __Gfx Gfx;
extern struct __FragmentCache FragmentCache[256];
extern struct __TextureCache TextureCache[2048];
extern struct __Camera Camera;
extern struct __Viewer Viewer;
extern struct __Object Object[2048];

extern __GameVer ThisGame;

extern struct stat romstat;

// ----------------------------------------
