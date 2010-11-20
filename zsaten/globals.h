#if 0
#define MEMWATCH
#define MW_STDIO
#endif

#define APP_TITLE 		"Project ZSaten"
#define APP_VERSION		"v0.1"

#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480

typedef int bool;
enum { false, true };

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
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#include <GL/gl.h>
#include <GL/glext.h>

#include "badrdp.h"
#include "misaka.h"
#include "hud.h"

#include "camera.h"
#include "mips.h"
#include "mips-eval.h"
#include "oz.h"
#include "render.h"
#include "shared.h"
#include "zactors.h"
#include "zelda.h"

#include "memwatch.h"

#define Read16(Buffer, Offset) \
	((Buffer[Offset] << 8) | Buffer[(Offset) + 1])

#define Read32(Buffer, Offset) \
	((Buffer[Offset] << 24) | (Buffer[(Offset) + 1] << 16) | (Buffer[(Offset) + 2] << 8) | Buffer[(Offset) + 3])

#define Write16(Buffer, Offset, Value) \
	Buffer[Offset] = (Value & 0xFF00) >> 8; \
	Buffer[Offset + 1] = (Value & 0x00FF);

#define arraySize(x)	(sizeof((x)) / sizeof((x)[0]))

#define MAX_ANIMATIONS	1024

typedef struct {
	bool isRunning;
	bool key[256];

	GLuint actorAxisMarkerDL;
	GLuint sceneAxisMarkerDL;

	char * tempString;

	int windowWidth, windowHeight;
	int mousePosX, mousePosY;
	int mouseCenterX, mouseCenterY;
	bool mouseButtonLDown, mouseButtonRDown;

	char appPath[MAX_PATH];
	char wndTitle[BUFSIZ];

	int guiHandleAbout;
	int guiHandleOptions;

	bool enableTextures;
	bool enableWireframe;

	bool enableHUD;

	int debugLevel;
	bool showBones;
	bool animPlay;

	float targetFPS;
} vProgramStruct;

typedef struct {
	float angleX, angleY;
	float X, Y, Z;
	float LX, LY, LZ;
	float rotX, rotY;
	float actorRotX, actorRotY;
} vCameraStruct;

typedef struct {
	bool useActorOvl;
	bool useExtAnim;
	bool isLink;

	unsigned short actorNumber;
	unsigned short variable;

	unsigned int offsetDList;
	float actorScale;

	unsigned int offsetBoneSetup[64];
	int boneSetupTotal;
	int boneSetupCurrent;

	unsigned int offsetAnims[MAX_ANIMATIONS];
	short animFrames[MAX_ANIMATIONS]; // only used for link
	int animTotal;
	int animCurrent;

	int frameTotal;
	int frameCurrent;

	char oName[128];
	char eaName[128];
} vCurrentActorStruct;

typedef struct {
	unsigned int PStart;
	unsigned int PEnd;
	unsigned int VStart;
	unsigned int VEnd;
	unsigned int ProfileVStart;
	unsigned int NameRStart;
	unsigned int NameCStart;
	char ActorName[128];

	unsigned short ObjectNumber;
	unsigned short AltObjectNumber;

	unsigned char * ActorData;
	unsigned int ActorSize;

	bool isValid;
} vActorStruct;

typedef struct {
	unsigned int PStart;
	unsigned int PEnd;
	unsigned int VStart;
	unsigned int VEnd;
	char ObjectName[128];

	unsigned char * ObjectData;
	unsigned int ObjectSize;

	unsigned char ObjectSegment;

	bool isValid;
} vObjectStruct;

typedef struct {
	char filePath[MAX_PATH];
	char fileName[MAX_PATH];
	char _pad0;
	bool isROMLoaded;
	unsigned int size;
	char title[20];
	char _pad1;
	char gameID[4];
	char _pad2;
	int version;
	char _pad3;
	unsigned int CRC1;
	unsigned int CRC2;
	unsigned char * data;
} vGameROMStruct;

typedef struct {
	int gameType;					// 0 = OoT, 1 = MM
	unsigned int DMATableOffset;
	int codeFileNo;
	int codeRAMOffset;
	unsigned int objectTableOffset;
	unsigned int actorTableOffset;
	char titleText[256];
	bool isCompressed;
	bool hasFilenames;
	unsigned int filenameTableOffset;

	DMA DMATable;
	DMA bootfile;

	unsigned char * codeBuffer;
	int objectCount;
	int actorCount;
} vZeldaInfoStruct;

typedef struct {
	float R, G, B, A;
} vRGBAStruct;

extern vProgramStruct vProgram;
extern vCameraStruct vCamera;
extern vCurrentActorStruct vCurrentActor;

extern vGameROMStruct vGameROM;
extern vZeldaInfoStruct vZeldaInfo;
extern vActorStruct vActors[768];
extern vObjectStruct vObjects[768];

extern vRGBAStruct vBoneColorFactor;

inline void dbgprintf(int Level, int Type, char * Format, ...);
void die(int Code);
