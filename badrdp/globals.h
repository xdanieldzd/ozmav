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
#include <limits.h>
#define MAX_PATH	PATH_MAX
#endif

#include <png.h>

// ----------------------------------------

typedef unsigned char bool;
enum { true = 1, false = 0 };

union { unsigned long ul; float f; } u;

// ----------------------------------------


// ----------------------------------------

#define CACHE_TEXTURES		4096
#define CACHE_FRAGMENT		1024

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

#ifndef isnan
#define isnan(x) ((x) != (x))
#endif

// ----------------------------------------

typedef struct {
	int DrawWidth, DrawHeight;

	unsigned int FragCachePosition;
	unsigned int TextureCachePosition;

	bool ObjDumpingEnabled;
	char WavefrontObjPath[MAX_PATH];
	FILE * FileWavefrontObj;
	FILE * FileWavefrontMtl;
	unsigned int WavefrontObjVertCount;
	unsigned int WavefrontObjMaterialCnt;

	unsigned char Options;
} __System;

typedef struct {
	float Comb[4][4];
	float Model[4][4];
	float Proj[4][4];
	float ModelStack[32][4][4];
	int ModelStackSize;
	int ModelIndex;

	bool UseMatrixHack;
} __Matrix;

typedef struct {
	short X;
	short Y;
	short Z;
	short W;
	short S;
	short T;
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
} __VtxRaw;

typedef struct {
	__VtxRaw Vtx;
	float RealS0;
	float RealT0;
	float RealS1;
	float RealT1;
} __Vertex;

typedef struct {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
} __Palette;

typedef struct {
	unsigned int Offset;

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
	union {
		struct {
			unsigned mirrort : 1;
			unsigned clampt : 1;
			unsigned pad0 : 30;
			unsigned mirrors : 1;
			unsigned clamps : 1;
			unsigned pad1 : 30;
		};
		struct {
			unsigned int CMT, CMS;
		};
	};
	float ScaleT, ScaleS;
	float ShiftScaleT, ShiftScaleS;

	bool IsTexRect;
	unsigned int TexRectW;
	unsigned int TexRectH;

	unsigned int CRC32;
} __Texture;

typedef struct {
	float R;
	float G;
	float B;
	float A;
} __RGBA;

typedef struct {
	float R;
	float G;
	float B;
	float A;
	float Z;
	float DZ;
} __FillColor;

typedef struct {
	float R;
	float G;
	float B;
	float A;
	float L;
	unsigned short M;
} __PrimColor;

typedef struct {
	unsigned int Combiner0;
	unsigned int Combiner1;
	GLuint ProgramID;
} __FragmentCache;

typedef struct {
	unsigned int Offset;
	unsigned int RealWidth;
	unsigned int RealHeight;
	unsigned int CRC32;
	GLuint TextureID;

	int MaterialID;
} __TextureCache;

typedef struct {
	int DLStack[16];
	int DLStackPos;

	unsigned int Update;
	unsigned int GeometryMode;

	struct {
		union {
			struct {
				unsigned int alphaCompare : 2;
				unsigned int depthSource : 1;

				unsigned int AAEnable : 1;
				unsigned int depthCompare : 1;
				unsigned int depthUpdate : 1;
				unsigned int imageRead : 1;
				unsigned int clearOnCvg : 1;

				unsigned int cvgDest : 2;
				unsigned int depthMode : 2;

				unsigned int cvgXAlpha : 1;
				unsigned int alphaCvgSel : 1;
				unsigned int forceBlender : 1;
				unsigned int textureEdge : 1;

				unsigned int c2_m2b : 2;
				unsigned int c1_m2b : 2;
				unsigned int c2_m2a : 2;
				unsigned int c1_m2a : 2;
				unsigned int c2_m1b : 2;
				unsigned int c1_m1b : 2;
				unsigned int c2_m1a : 2;
				unsigned int c1_m1a : 2;

				unsigned int blendMask : 4;
				unsigned int alphaDither : 2;
				unsigned int colorDither : 2;

				unsigned int combineKey : 1;
				unsigned int textureConvert : 3;
				unsigned int textureFilter : 2;
				unsigned int textureLUT : 2;

				unsigned int textureLOD : 1;
				unsigned int textureDetail : 2;
				unsigned int texturePersp : 1;
				unsigned int cycleType : 2;
				unsigned int pipelineMode : 1;

				unsigned int pad : 8;
			};

			uint64_t _u64;

			struct {
				unsigned int L, H;
			};
		};
	} OtherMode;

	GLfloat LightAmbient[4];
	GLfloat LightDiffuse[4];
	GLfloat LightSpecular[4];
	GLfloat LightPosition[4];
	unsigned int Store_RDPHalf1, Store_RDPHalf2;
	unsigned int Combiner0, Combiner1;

	__RGBA BlendColor;
	__RGBA EnvColor;
	__RGBA FogColor;
	__FillColor FillColor;
	__PrimColor PrimColor;

	bool IsMultiTexture;
	int CurrentTexture;

	GLuint GLTextureID[CACHE_TEXTURES];
	int GLTextureCount;
} __Gfx;

typedef struct {
	char * ExtensionList;
	char ExtSupported[256];
	char ExtUnsupported[256];
	bool IsExtUnsupported;
	bool Ext_MultiTexture;
	bool Ext_TexMirroredRepeat;
	bool Ext_FragmentProgram;
} __OpenGL;

extern __System System;
extern __Matrix Matrix;
extern __Gfx Gfx;
extern __Palette Palette[256];
extern __Vertex Vertex[32];
extern __Texture Texture[2];
extern __FragmentCache FragmentCache[CACHE_FRAGMENT];
extern __TextureCache TextureCache[CACHE_TEXTURES];
extern __OpenGL OpenGL;

// ----------------------------------------

#include "badrdp.h"

#include "opengl.h"
#include "dlparse.h"
#include "gbi.h"
#include "gdp.h"
#include "gsp.h"
#include "rdp.h"
#include "matrix.h"
#include "macro.h"
#include "f3d.h"
#include "f3dex.h"
#include "f3dex2.h"
#include "dump.h"
#include "combine.h"
#include "crc.h"

#ifndef max
  #define max(a, b)	((a) > (b) ? (a) : (b))
#endif
#ifndef min
  #define min(a, b)	((a) < (b) ? (a) : (b))
#endif

