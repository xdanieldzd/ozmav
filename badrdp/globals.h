#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <GL/gl.h>
#include <GL/glext.h>

#ifdef WIN32
#include <windows.h>
#else
#include <GL/glx.h>
#include <X11/X.h>
#endif

// ----------------------------------------

typedef unsigned char bool;
enum { true = 1, false = 0 };

// ----------------------------------------

#include "badrdp.h"

#include "dlparse.h"
#include "rdp.h"
#include "f3d.h"
#include "f3dex.h"
#include "f3dex2.h"

// ----------------------------------------

#define CACHE_TEXTURES		2048
#define CACHE_FRAGMENT		512

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

// ----------------------------------------

struct __System {
	unsigned int FragCachePosition;
	unsigned int TextureCachePosition;
};

struct __Matrix {
	float Model[4][4];
	float Proj[4][4];
	float ModelStack[32][4][4];
	int ModelStackSize;
	int ModelIndex;
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
	unsigned int zCombiner0;
	unsigned int zCombiner1;
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

	GLuint GLTextureID[CACHE_TEXTURES];
	int GLTextureCount;
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

extern struct __System System;
extern struct __Matrix Matrix;
extern struct __Gfx Gfx;
extern struct __Palette Palette[256];
extern struct __Vertex Vertex[32];
extern struct __Texture Texture[2];
extern struct __FragmentCache FragmentCache[CACHE_FRAGMENT];
extern struct __TextureCache TextureCache[CACHE_TEXTURES];
extern struct __OpenGL OpenGL;
