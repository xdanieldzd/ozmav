#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "n64.h"
#include "sm64.h"

typedef int bool;
enum { true = 1, false = 0 };

#define ZMAP_HEADERGAP	0x1C00	// gap between beginning of map file and converted data, used to make room for header, etc.
#define ZMAP_BASEDLIST	0x0080	// offset where base Display List is written to
#define ZMAP_BASEOBJS	0x0380	// offset where object data is written to
#define ZMAP_BASEACTORS	0x0400	// offset where actor data is written to
#define Z_PADDING	0x1000	// map and scene files get padded to this
#define SCALE		3	//M64:Z64

//#define DEBUG		/* IDEALLY: use the flag `-DDEBUG' when compiling */

/* Buffer-writing macros */
#define Write16(Buffer, Offset, Value)\
	Buffer[Offset]	= (Value&0xFF00)>>8;\
	Buffer[Offset+1]= (Value & 0x00FF);

#define Write32(Buffer, Offset, Value)\
	Buffer[Offset] = (Value & 0xFF000000) >> 24;\
	Buffer[Offset + 1] = (Value & 0x00FF0000) >> 16;\
	Buffer[Offset + 2] = (Value & 0x0000FF00) >> 8;\
	Buffer[Offset + 3] = (Value & 0x000000FF);\

#define Write64(Buffer, Offset, Value1, Value2)\
	Buffer[Offset] = (Value1 & 0xFF000000) >> 24;\
	Buffer[Offset + 1] = (Value1 & 0x00FF0000) >> 16;\
	Buffer[Offset + 2] = (Value1 & 0x0000FF00) >> 8;\
	Buffer[Offset + 3] = (Value1 & 0x000000FF);\
	Buffer[Offset + 4] = (Value2 & 0xFF000000) >> 24;\
	Buffer[Offset + 5] = (Value2 & 0x00FF0000) >> 16;\
	Buffer[Offset + 6] = (Value2 & 0x0000FF00) >> 8;\
	Buffer[Offset + 7] = (Value2 & 0x000000FF);

#define Read16(Buffer, Offset)\
	(Buffer[Offset]<<8)|\
	Buffer[Offset+1]

/* for reading the offset in (segment<<24)|offset pairs */
#define Read24(Buffer, Offset)\
	(Buffer[Offset] << 16)|\
	(Buffer[Offset + 1] << 8)|\
	Buffer[Offset + 2]

#define Read32(Buffer, Offset)\
	(Buffer[Offset] << 24)|\
	(Buffer[Offset + 1] << 16)|\
	(Buffer[Offset + 2] << 8)|\
	Buffer[Offset + 3]

#define Copy16(SrcBuffer, DestBuffer, SrcOffset, DestOffset)\
	DestBuffer[DestOffset+0] = SrcBuffer[SrcOffset+0];\
	DestBuffer[DestOffset+1] = SrcBuffer[SrcOffset+1];

#define Copy32(SrcBuffer, DestBuffer, SrcOffset, DestOffset)\
	DestBuffer[DestOffset+0] = SrcBuffer[SrcOffset+0];\
	DestBuffer[DestOffset+1] = SrcBuffer[SrcOffset+1];\
	DestBuffer[DestOffset+2] = SrcBuffer[SrcOffset+2];\
	DestBuffer[DestOffset+3] = SrcBuffer[SrcOffset+3];	

#define ZActor(Buffer, Offset, ANum, XPos, YPos, ZPos, XRot, YRot, ZRot, Var)\
	Write16(Buffer, Offset+0x0, ANum);\
	Write16(Buffer, Offset+0x2, XPos);\
	Write16(Buffer, Offset+0x4, YPos);\
	Write16(Buffer, Offset+0x6, ZPos);\
	Write16(Buffer, Offset+0x8, XRot);\
	Write16(Buffer, Offset+0xA, YRot);\
	Write16(Buffer, Offset+0xC, ZRot);\
	Write16(Buffer, Offset+0xE, Var);

#define ZLink(Buffer, Offset, XPos, YPos, ZPos, XRot, YRot, ZRot, Var)	ZActor(Buffer, Offset, 0, XPos, YPos, ZPos, XRot, YRot, ZRot, Var)

#define ZDoor(Buffer, Offset, RoomFront, RoomRear, FadeFront, FadeRear, ANum, XPos, YPos, ZPos, YRot, Var)\
	Buffer[Offset+0x0] = RoomFront;\
	Buffer[Offset+0x1] = FadeFront;\
	Buffer[Offset+0x2] = RoomRear;\
	Buffer[Offset+0x3] = FadeRear;\
	Write16(Buffer, Offset+0x4, ANum);\
	Write16(Buffer, Offset+0x6, XPos);\
	Write16(Buffer, Offset+0x8, YPos);\
	Write16(Buffer, Offset+0xA, ZPos);\
	Write16(Buffer, Offset+0xC, YRot);\
	Write16(Buffer, Offset+0xE, Var)

// padding size calculation macro
#define GetPaddingSize(Filesize, Factor) \
	(((Filesize / Factor) + 1) * Factor) - Filesize

/* Messages */
#ifdef DEBUG
 /* debug message */
 #define dmsg( ... )\
 {\
	printf("debug: ");\
	printf(__VA_ARGS__);\
 }
#else
 /* so #ifdef DEBUG is not needed */
 #define dmsg( ... )	{}
#endif

/* Error - display it and quit */
#define error( ... )\
{\
	fprintf(stderr, "error: ");\
	fprintf(stderr, __VA_ARGS__);\
	exit(EXIT_FAILURE);\
}

/* General purpose message */
#define msg(pri, ... )\
if(pri <= verbose)\
{\
	printf(__VA_ARGS__);\
}

#define SEG_COUNT	16

extern int verbose;

extern void SM64_Behavior_To_OoT(unsigned int ScriptSegment, unsigned int ScriptPos, unsigned int BehaviorCode);

extern int ConvertDList(unsigned int Seg, unsigned int Offset, bool CalledViaCmd);

extern int InitLevelScriptInterpreter();
extern int ExecuteLevelScript();
extern int ExecuteGeoScript(bool IsObject);
extern int InsertSpecialObjects(unsigned int ObjID, unsigned int GeoOffset);

extern char ROMFilename[1024];
extern char ZMapFilename[1024];
extern char ZSceneFilename[1024];

extern unsigned char * ROMBuffer;
extern unsigned char * ZMapBuffer;
extern unsigned char * ZSceneBuffer;
extern unsigned char * ColVtxBuffer;
extern unsigned char * ColTriBuffer;
extern unsigned char * ZWaterBuffer;
extern unsigned char * ZActorBuffer;
extern unsigned char * ZObjectBuffer;
extern unsigned char * ZLinkBuffer;

extern int ROMFilesize;
extern int ZMapFilesize;
extern int ZSceneFilesize;
extern int TextureSize;

/* LEVEL SCRIPT INTERPRETER VARIABLES */
extern unsigned int	LevelLayoutScript_Base;
extern unsigned int	MacroPreset_Offset;
extern unsigned int	MacroPreset_Length;
extern unsigned int	TempOffset;

extern unsigned int	LvlScript_Start;
extern unsigned int	LvlScript_End;
extern unsigned int	LvlScript_Entry;
extern unsigned int	LvlScript_Length;

extern unsigned int	TempScriptPos;
extern unsigned int	TempScriptSegment;
extern unsigned int	TempScriptPos_Backup;
extern unsigned int	TempScriptSegment_Backup;

extern unsigned int	TempGeoScriptPos;
extern unsigned int	TempGeoScriptSegment;
extern unsigned int	TempGeoScriptPos_Backup;
extern unsigned int	TempGeoScriptSegment_Backup;

extern char		CurrentScriptCmdText[256];
extern unsigned int	CurrentCmd;
extern unsigned int	CurrentCmdLength;
extern bool		JumpInScript;
extern bool		JumpInGeoScript;

extern unsigned int	CurrentLevelArea;
extern unsigned int	LevelArea;
extern unsigned int	LevelID;

extern signed short linkx, linky, linkz;
extern unsigned short linkrot;

extern signed short Xrepos, Yrepos, Zrepos;

/* DL vars */
extern bool DListHasEnded;
extern unsigned int DListOffsets[512];
extern int DLCount;

/* Some of these are pretty hacky */
extern int ColTriCount;
extern int ColVtxCount;
extern int _ColVtxCount;
extern int ZTriOffset;
extern int ZWaterCount;
extern int ZWaterOffset;
extern bool collision_read;
extern bool fix_collision;

extern int ZActorCount;
extern int ZDoorCount;
extern int ZObjectCount;

struct __RAMSegment {
	unsigned char * Data;
	bool IsSet;
	unsigned int Length;
};
extern struct __RAMSegment RAMSegment[SEG_COUNT];
