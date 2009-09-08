/*	------------------------------------------------------------
	INCLUDES
	------------------------------------------------------------ */

#include <windows.h>
#include <commctrl.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#include "n64.h"
#include "resource.h"

typedef int bool;
enum { true = 1, false = 0 };

/* macros from glN64 source code */
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

/*	------------------------------------------------------------
	DEFINES
	------------------------------------------------------------ */

#define HACKS_ENABLED   false				/* EN-/DISABLE MISC HACKS (not used at the moment) */

/*	------------------------------------------------------------
	SYSTEM FUNCTIONS - OPENGL & WINDOWS
	------------------------------------------------------------ */

/* win32/bmp.c */
extern bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, char* bmpfile);
extern BYTE* ConvertToBMP(BYTE* Buffer, int width, int height, long* newsize);

/* win32/main.c */
extern int Viewer_Initialize();
extern int Viewer_ResetVariables();
extern int Viewer_LoadAreaData();
extern int Viewer_GetGameVersion();
extern int Viewer_InitLevelSelector();
extern int Viewer_RenderMapRefresh();
extern void GLUTCamera_Orientation(float, float);
extern void GLUTCamera_Movement(int, bool);
extern void Camera_MouseMove(int, int);
extern int Viewer_SelectActor(int, int);
extern int CheckUncheckMenu(unsigned int, int);
extern int CheckUncheckMenu_Filters();
extern void Dialog_OpenROM(HWND);
extern int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int);
extern LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/* actors.c */
extern int Viewer_RenderAllActors();
extern int Viewer_RenderActor(int, GLshort, GLshort, GLshort, signed int, signed int, signed int, float, float, float);

/* gfx_emul.c */
extern int Viewer_RenderMap();
extern int Viewer_RenderMap_DListParser(bool, unsigned long);

/* helpers.c */
extern int Zelda_MemCopy(unsigned int, unsigned long, unsigned char *, unsigned long);
extern void Helper_SplitCurrentVals(bool);
extern int Helper_LogMessage(int, char[]);
extern int Helper_GFXLogCommand(unsigned int);
extern int Helper_CalculateFPS();
extern int Helper_FileReadLine();

/* map_init.c */
extern int Zelda_GetMapHeaderList(int, int);
extern int Zelda_GetMapHeader(int, int);
extern int Zelda_GetSceneHeaderList(int);
extern int Zelda_GetSceneHeader(int);
extern int Zelda_GetMapActors(int, int);
extern int Zelda_GetSceneActors(int);
extern int Zelda_GetDoorData(int);
extern int Zelda_GetMapDisplayLists(unsigned long, int);
extern int Zelda_GetMapCollision(int);
extern int Zelda_GetEnvironmentSettings(int);
extern int Zelda_SelectEnvSettings();
extern int Zelda_GetSceneName();

/* ogl_mngr.c */
extern int OGL_Init(void);
extern int OGL_ResetProperties(void);
extern int OGL_InitExtensions(void);
extern int OGL_DrawScene(void);
extern void OGL_ResizeScene(GLsizei, GLsizei);
extern void OGL_KillTarget(void);
extern bool OGL_CreateTarget(int, int, int);

/* uc_comb.c */
extern int F3DEX2_Cmd_SETCOMBINE();
extern int F3DEX2_BuildFragmentShader();
extern int F3DEX2_Cmd_SETFOGCOLOR();
extern int F3DEX2_Cmd_SETBLENDCOLOR();
extern int F3DEX2_Cmd_SETPRIMCOLOR();
extern int F3DEX2_Cmd_SETENVCOLOR();
extern int F3DEX2_BuildVertexShader();
extern int F3DEX2_BindVertexShader();

/* uc_misc.c */
extern int F3DEX2_Cmd_RDPHALF_1();
extern int F3DEX2_Cmd_RDPHALF_2();
extern int F3DEX2_Cmd_BRANCH_Z();
extern int F3DEX2_Cmd_DL(bool);

/* uc_modes.c */
extern int F3DEX2_Cmd_GEOMETRYMODE();
extern int F3DEX2_UpdateGeoMode();
extern int F3DEX2_Cmd_SETOTHERMODE_H();
extern int F3DEX2_Cmd_SETOTHERMODE_L();
extern int F3DEX2_ForceBlender();

/* uc_tex.c */
extern int F3DEX2_Cmd_TEXTURE();
extern int F3DEX2_Cmd_SETTIMG();
extern int F3DEX2_Cmd_SETTILE();
extern int F3DEX2_Cmd_SETTILESIZE();
extern int F3DEX2_ChangeTileSize(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
extern int F3DEX2_Cmd_LOADTLUT(unsigned int, unsigned long);
extern int F3DEX2_Cmd_LOADBLOCK();
extern GLuint F3DEX2_LoadTexture(int);
extern int F3DEX2_ResetTextureStruct();

/* uc_tri.c */
extern int F3DEX2_Cmd_VTX();
extern int F3DEX2_GetVertexList(unsigned int, unsigned long, unsigned int, unsigned int);
extern int F3DEX2_Cmd_TRI1();
extern int F3DEX2_Cmd_TRI2();
extern int F3DEX2_Cmd_QUAD();
extern int F3DEX2_DrawVertexPoint(unsigned int);
extern int F3DEX2_Cmd_MTX();
extern int F3DEX2_Cmd_POPMTX();

/* GL extension functions */

extern PFNGLMULTITEXCOORD1FARBPROC			glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD3FARBPROC			glMultiTexCoord3fARB;
extern PFNGLMULTITEXCOORD4FARBPROC			glMultiTexCoord4fARB;
extern PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB;

extern PFNGLGENPROGRAMSARBPROC				glGenProgramsARB;
extern PFNGLBINDPROGRAMARBPROC				glBindProgramARB;
extern PFNGLDELETEPROGRAMSARBPROC			glDeleteProgramsARB;
extern PFNGLPROGRAMSTRINGARBPROC			glProgramStringARB;
extern PFNGLPROGRAMENVPARAMETER4FARBPROC	glProgramEnvParameter4fARB;
extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC	glProgramLocalParameter4fARB;

/*	------------------------------------------------------------
	VARIABLES
	------------------------------------------------------------ */

/* WINDOW HANDLING VARIABLES, ETC. */
extern HWND				hwnd;
extern HMENU			hmenu;
extern HWND				hogl;
extern HWND				hstatus;
extern HWND				hlvlcombo;

extern HDC				hDC_ogl;
extern HGLRC			hRC;
extern HINSTANCE		hInstance;

extern char				szClassName[];

/* GENERAL GLOBAL PROGRAM VARIABLES */
extern bool				System_KbdKeys[256];

extern char				AppTitle[32];
extern char				AppVersion[32];
extern char				AppBuildName[64];
extern char				AppPath[512];
extern char				INIPath[512];
extern char				WindowTitle[256];
extern char				StatusMsg[256];
extern char				ErrorMsg[8192];

extern char				MapActorMsg[256];
extern char				SceneActorMsg[256];

extern bool				AreaLoaded;
extern bool				WndActive;
extern bool				ExitProgram;

extern char				CurrentGFXCmd[256];
extern char				CurrentGFXCmdNote[256];
extern char				GFXLogMsg[1024];
extern char				SystemLogMsg[1024];
extern char				WavefrontObjMsg[1024];
extern char				WavefrontMtlMsg[1024];
extern char				WavefrontObjColMsg[1024];

extern bool				GFXLogOpened;

extern bool				WavefrontObjOpened;
extern bool				WavefrontMtlOpened;
extern bool				WavefrontObjColOpened;

extern unsigned int		WavefrontObjVertCount;
extern unsigned int		WavefrontObjVertCount_Previous;

extern unsigned int		WavefrontObjMaterialCnt;

extern unsigned int		WavefrontObjColVertCount;
extern unsigned int		WavefrontObjColVertCount_Previous;

/* CAMERA / VIEWPOINT VARIABLES */
extern float			CamAngleX, CamAngleY;
extern float			CamX, CamY, CamZ;
extern float			CamLX, CamLY, CamLZ;

extern int				MousePosX, MousePosY;
extern int				MouseCenterX, MouseCenterY;

extern bool				MouseButtonDown;

/* FILE HANDLING VARIABLES */
extern FILE				* FileROM;

extern unsigned int		* ROMBuffer;
extern unsigned int		* ZMapBuffer[256];
extern unsigned int		* ZSceneBuffer;
extern unsigned int		* GameplayKeepBuffer;
extern unsigned int		* GameplayFDKeepBuffer;

extern unsigned int		* TempActorBuffer;
extern unsigned int		* TempObjectBuffer;
extern bool				TempActorBuffer_Allocated;
extern bool				TempObjectBuffer_Allocated;

extern unsigned int		Debug_MallocOperations;
extern unsigned int		Debug_FreeOperations;

extern bool				GetDLFromZMapScene;

extern unsigned int		Scene_Start;
extern char				Scene_Name[256];

extern unsigned int		Map_Start[256];

extern unsigned long	ROMFilesize;
extern unsigned long	ZMapFilesize[256];
extern unsigned long	ZSceneFilesize;
extern unsigned long	GameplayKeepFilesize;
extern unsigned long	GameplayFDKeepFilesize;

extern unsigned long	CurrentObject_Length;
extern unsigned int		ObjectID;

extern char				Filename_ROM[256];

extern bool				ROMExists;

extern FILE				* FileGFXLog;
extern FILE				* FileSystemLog;
extern FILE				* FileCombinerLog;
extern FILE				* FileWavefrontObj;
extern FILE				* FileWavefrontMtl;
extern FILE				* FileWavefrontObjCol;

extern int				GameMode;				/* 0 = OoT, 1 = MM */

/* DATA READOUT VARIABLES */
extern unsigned long	Readout_Current1;
extern unsigned long	Readout_Current2;
extern unsigned int		Readout_CurrentByte1;
extern unsigned int		Readout_CurrentByte2;
extern unsigned int		Readout_CurrentByte3;
extern unsigned int		Readout_CurrentByte4;
extern unsigned int		Readout_CurrentByte5;
extern unsigned int		Readout_CurrentByte6;
extern unsigned int		Readout_CurrentByte7;
extern unsigned int		Readout_CurrentByte8;

extern unsigned long	Readout_NextGFXCommand1;
extern unsigned long	Readout_PrevGFXCommand1;

/* F3DEX2 DISPLAY LIST HANDLING VARIABLES */
extern unsigned long	DLists[256][2048];
extern signed long		DListInfo_CurrentCount[256];
extern signed long		DListInfo_TotalCount;
extern signed long		DListInfo_DListToRender;
extern unsigned long	DLTempPosition;

extern unsigned int		DLToRender;
extern bool				DListHasEnded;

extern unsigned long	Storage_RDPHalf1;
extern unsigned long	Storage_RDPHalf2;

extern GLfloat			Matrix[4][4];
extern GLfloat			Matrix_Stack[32][4][4];
extern GLfloat			ProjMatrix[4][4];
extern int				MVMatrixCount;

extern unsigned long	N64_GeometryMode;

/* F3DEX2 TEXTURE HANDLING VARIABLES */
extern unsigned char	* TextureData_OGL;
extern unsigned char	* TextureData_N64;

extern unsigned char	* PaletteData;

extern unsigned long	TexCachePosition;
extern unsigned long	TotalTexCount;

/* Combiner variables */
extern unsigned int		Combine0, Combine1;
extern int				cA[2], cB[2], cC[2], cD[2], aA[2], aB[2], aC[2], aD[2];
extern int				VertProg;

extern unsigned int		FPCachePosition;

extern bool				RDPOtherMode_ForceBlender;
extern unsigned short	RDPOtherMode_BlendMode;

/* ZELDA ROM HANDLING VARIABLES */
extern unsigned long	ROM_SceneTableOffset;
extern unsigned int		ROM_SceneToLoad;
extern unsigned int		ROM_MaxSceneCount;
extern unsigned int		ROM_SceneEntryLength;

extern unsigned long	ROM_ObjectTableOffset;
extern unsigned long	ROM_ActorTableOffset;

extern int				ROM_CurrentMap;
extern int				ROM_CurrentMap_Temp;

extern int				DListParser_CurrentMap;

/* ZELDA MAP & SCENE HEADER HANDLING VARIABLES */
extern bool				MapHeader_MultiHeaderMap;
extern int				MapHeader_Current;
extern int				MapHeader_TotalCount;
extern unsigned long	MapHeader_List[256];
extern unsigned long	MapHeader_CurrentPosInList;

extern bool				SceneHeader_MultiHeaderMap;
extern int				SceneHeader_Current;
extern int				SceneHeader_TotalCount;
extern unsigned long	SceneHeader_List[256];
extern unsigned long	SceneHeader_CurrentPosInList;

/* ZELDA ACTOR DATA HANDLING VARIABLES */
extern int				ActorInfo_CurrentCount[256];
extern int				ActorInfo_Selected;

extern int				ScActorInfo_CurrentCount;
extern int				ScActorInfo_Selected;

extern int				DoorInfo_CurrentCount;
extern int				DoorInfo_Selected;

/* GENERAL RENDERER VARIABLES */
extern GLuint			Renderer_GLDisplayList;
extern GLuint			Renderer_GLDisplayList_Current;
extern GLuint			Renderer_GLDisplayList_Total;

extern GLuint			Renderer_GLTexture;
extern int				CurrentTextureID;
extern bool				IsMultiTexture;

extern GLuint			TempGLTexture;

extern GLuint			Renderer_FilteringMode_Min;
extern GLuint			Renderer_FilteringMode_Mag;

extern DWORD			Renderer_LastFPS;
extern int				Renderer_FPS, Renderer_FrameNo;
extern char				Renderer_FPSMessage[32];

extern char				Renderer_CoordDisp[256];

extern bool				Renderer_IsRGBANormals;

extern GLfloat			LightAmbient[];
extern GLfloat			LightDiffuse[];
extern GLfloat			LightSpecular[];
extern GLfloat			LightPosition[];

extern float			FogColor[];
extern float			BlendColor[];
extern float			PrimColor[];
extern float			EnvColor[];

extern char				ShaderString[16384];

extern bool				Renderer_EnableMapActors;
extern bool				Renderer_EnableSceneActors;
extern bool				Renderer_EnableDoors;

extern bool				Renderer_EnableMap;
extern bool				Renderer_EnableCollision;
extern GLfloat			Renderer_CollisionAlpha;

extern bool				Renderer_EnableWavefrontDump;

extern bool				Renderer_EnableFog;

extern bool				Renderer_EnableWireframe;

extern bool				Renderer_EnableFragShader;

extern int				CurrentEnvSetting;

/* OPENGL EXTENSION VARIABLES */
extern char				* GLExtension_List;
extern bool				GLExtension_MultiTexture;
extern bool				GLExtension_TextureMirror;
extern bool				GLExtension_AnisoFilter;
extern bool				GLExtension_VertFragProgram;
extern char				GLExtensionsSupported[256];

extern bool				GLExtensionsUnsupported;
extern char				GLExtensionsErrorMsg[512];

/* N64 BLENDING & COMBINER SIMULATION VARIABLES */
extern unsigned int		RDPCycleMode;

extern unsigned long	Blender_Cycle1;
extern unsigned long	Blender_Cycle2;

/*	------------------------------------------------------------
	STRUCTURES
	------------------------------------------------------------ */

/* ZELDA MAP HEADER STRUCTURE */
struct MapHeader_Struct {
	/* 01 */
	unsigned char Actor_Count;
	unsigned long Actor_DataOffset;
	/* 0A */
	unsigned long MeshDataHeader;
	/* 0B */
	unsigned char Group_Count;
	unsigned long Group_DataOffset;
	/* 10 */
	unsigned long MapTime;
	unsigned char TimeFlow;
	/* 12 */
	char Skybox;
	/* 16 */
	char EchoLevel;
};
extern struct MapHeader_Struct MapHeader[256][256];

struct SceneHeader_Struct {
	/* 00 */
	unsigned char ScActor_Count;
	unsigned long ScActor_DataOffset;
	/* 03 */
	unsigned char Col_DataSource;
	unsigned long Col_DataOffset;
	/* 04 */
	unsigned char Map_Count;
	unsigned long Map_ListOffset;
	/* 0E */
	unsigned char Door_Count;
	unsigned long Door_DataOffset;
	/* 0F */
	unsigned char EnvSetting_Count;
	unsigned long EnvSetting_DataOffset;
};
extern struct SceneHeader_Struct SceneHeader[256];

typedef struct {
	unsigned char R;
	unsigned char G;
	unsigned char B;
} RGB;

struct EnvSetting_Struct {
	RGB Color1;
	RGB Color2;
	RGB Color3;
	RGB Color4;
	RGB Color5;
	RGB FogColor;
	unsigned short FogDistance;
	unsigned short DrawDistance;
};
extern struct EnvSetting_Struct EnvSetting[256];

/* ZELDA MAP ACTOR DATA STRUCTURE */
struct Actors_Struct {
	unsigned short Number;
	short X_Position;
	short Y_Position;
	short Z_Position;
	short X_Rotation;
	short Y_Rotation;
	short Z_Rotation;
	unsigned short Variable;
};
extern struct Actors_Struct Actors[256][1024];

/* ZELDA SCENE ACTOR DATA STRUCTURE */
struct ScActors_Struct {
	unsigned short Number;
	short X_Position;
	short Y_Position;
	short Z_Position;
	short X_Rotation;
	short Y_Rotation;
	short Z_Rotation;
	unsigned short Variable;
};
extern struct ScActors_Struct ScActors[1024];

/* ZELDA DOOR DATA STRUCTURE */
struct Door_Struct {
	unsigned short Unknown1;
	unsigned short Unknown2;
	unsigned short Number;
	short X_Position;
	short Y_Position;
	short Z_Position;
	short Y_Rotation;
	unsigned short Variable;
};
extern struct Door_Struct Doors[1024];

/* F3DEX2 VERTEX DATA STRUCTURE */
struct Vertex_Struct {
	signed short X;
	signed short Y;
	signed short Z;
	signed short H;
	signed short V;
	GLbyte R;
	GLbyte G;
	GLbyte B;
	GLbyte A;
};
extern struct Vertex_Struct Vertex[4096];

/* F3DEX2 TEXTURE DATA STRUCTURE */
struct Texture_Struct {
	unsigned int Height;
	unsigned int Width;
	unsigned int DataSource;
	unsigned int PalDataSource;
	unsigned long Offset;
	unsigned long PalOffset;
	unsigned int Format_N64;
	GLuint Format_OGL;
	unsigned int Format_OGLPixel;
	unsigned int Y_Parameter;
	unsigned int X_Parameter;
	float S_Scale;
	float T_Scale;
	float S_ShiftScale;
	float T_ShiftScale;
	unsigned int S_Mask;
	unsigned int T_Mask;
	unsigned int LineSize;
	unsigned int Palette;
	unsigned int AnimDXT;
	unsigned short Checksum;
};
extern struct Texture_Struct Texture[2];

/* CI TEXTURE PALETTE STRUCTURE */
struct Palette_Struct {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};
extern struct Palette_Struct Palette[512];

struct CurrentTextures_Struct {
	GLuint GLTextureID;
	unsigned int DataSource;
	unsigned long Offset;
	unsigned int OtherCriteria;
	unsigned short Checksum;
};
extern struct CurrentTextures_Struct CurrentTextures[1024];

struct ObjectActorTable_Struct {
	unsigned long StartOffset;
	unsigned long EndOffset;
	bool Valid;
};
extern struct ObjectActorTable_Struct ObjectTable[8192];
extern struct ObjectActorTable_Struct ActorTable[8192];

extern struct Vertex_Struct CollisionVertex[8192];

struct FPCache_Struct {
	unsigned long Combine0;
	unsigned long Combine1;
	unsigned int FragProg;
};
extern struct FPCache_Struct FPCache[256];
