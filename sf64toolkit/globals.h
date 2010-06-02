#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curses.h>

#include "misaka.h"

#include "main.h"
#include "n64sums.h"
#include "sf64.h"

#ifndef WIN32 /* MAX_PATH is Windows only */
 #include <limits.h>
 #define MAX_PATH	PATH_MAX
#endif

#define APPTITLE		"Star Fox 64 Toolkit"
#define VERSION			"v1"

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

struct __ROM {
	char Filepath[MAX_PATH];
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

struct __DMA {
	unsigned int VStart;
	unsigned int PStart;
	unsigned int PEnd;
	unsigned int CompFlag;
};

typedef struct {
	char TitleText[256];
	char GameID[5];
	int Version;
	unsigned int CRC1;
	unsigned int CRC2;
	unsigned int DMATableOffset;
} __GameVer;

extern char AppPath[MAX_PATH];
extern char Sep;

extern unsigned int DMATableOffset;
extern int DMAFileCount;

extern struct __ROM ROM;
extern struct __DMA DMA[256];

extern __GameVer ThisGame;
extern bool IsROMLoaded;

// ----------------------------------------
