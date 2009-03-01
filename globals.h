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

/* win32/main.c */
extern int Viewer_Initialize();
extern int Viewer_ResetVariables();
extern int Viewer_LoadAreaData();
extern int Viewer_RenderMapRefresh();
extern void GLUTCamera_Orientation(float,float);
extern void GLUTCamera_Movement(int);
extern void Camera_MouseMove(int, int);
extern void Dialog_OpenROM(HWND);
extern int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int);
extern LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/* actors.c */
extern int Viewer_RenderAllActors();
extern int Viewer_RenderActor(int, GLshort, GLshort, GLshort, signed int, signed int, signed int, bool);

/* gfx_emul.c */
extern int Viewer_RenderMap();
extern int Viewer_RenderMap_DListParser(bool, unsigned long);

/* helpers.c */
extern int Zelda_MemCopy(unsigned int, unsigned long, unsigned char *, unsigned long);
extern void Helper_SplitCurrentVals(bool);
extern int Helper_LogMessage(int, char[]);
extern int Helper_GFXLogCommand(unsigned int);
extern int Helper_CalculateFPS();

/* map_init.c */
extern int Zelda_GetMapHeaderList(int, int);
extern int Zelda_GetMapHeader(int, int);
extern int Zelda_GetSceneHeaderList(int);
extern int Zelda_GetSceneHeader(int);
extern int Zelda_GetMapActors(int, int);
extern int Zelda_GetSceneActors(int);
extern int Zelda_GetMapDisplayLists(unsigned long, int);
extern int Zelda_GetMapCollision(int);

/* ogl_mngr.c */
extern int GL_Init(void);
extern int GL_InitExtensions(void);
extern int GL_DrawScene(void);
extern void GL_KillTarget(void);
extern bool GL_CreateTarget(int, int, int);

/* uc_comb.c */
extern int F3DEX2_Cmd_SETCOMBINE();
extern int F3DEX2_BuildFragmentShader();
extern int F3DEX2_Cmd_SETFOGCOLOR();
extern int F3DEX2_Cmd_SETPRIMCOLOR();
extern int F3DEX2_Cmd_SETENVCOLOR();
extern int F3DEX2_HACKSelectClrAlpSource();

/* uc_misc.c */
extern int F3DEX2_Cmd_RDPHALF_1();
extern int F3DEX2_Cmd_BRANCH_Z();
extern int F3DEX2_Cmd_DL(bool);

/* uc_modes.c */
extern int F3DEX2_Cmd_GEOMETRYMODE();
extern int F3DEX2_UpdateGeoMode();
extern int F3DEX2_Cmd_SETOTHERMODE_H();
extern int F3DEX2_Cmd_SETOTHERMODE_L();

/* uc_tex.c */
extern int F3DEX2_Cmd_TEXTURE();
extern int F3DEX2_Cmd_SETTIMG();
extern int F3DEX2_Cmd_SETTILE();
extern int F3DEX2_Cmd_SETTILESIZE();
extern int F3DEX2_ChangeTileSize(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
extern int F3DEX2_Cmd_LOADTLUT(unsigned int, unsigned long);
extern GLuint F3DEX2_LoadTexture(int);

/* uc_tri.c */
extern int F3DEX2_Cmd_VTX();
extern int F3DEX2_GetVertexList(unsigned int, unsigned long, unsigned int, unsigned int);
extern int F3DEX2_Cmd_TRI1();
extern int F3DEX2_Cmd_TRI2();
extern int F3DEX2_DrawVertexPoint(unsigned int);
extern int F3DEX2_Cmd_MTX();

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

/*	------------------------------------------------------------
	VARIABLES
	------------------------------------------------------------ */

/* WINDOW HANDLING VARIABLES, ETC. */
extern HWND				hwnd;
extern HMENU			hmenu;
extern HWND				hogl;
extern HWND				hstatus;

extern HDC				hDC_ogl;
extern HGLRC			hRC;
extern HINSTANCE		hInstance;

extern char				szClassName[];

/* GENERAL GLOBAL PROGRAM VARIABLES */
extern bool				System_KbdKeys[256];

extern char				AppTitle[256];
extern char				AppVersion[256];
extern char				AppBuildName[256];
extern char				AppPath[512];
extern char				INIPath[512];
extern char				WindowTitle[256];
extern char				StatusMsg[256];
extern char				ErrorMsg[2048];

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

extern bool				GFXLogOpened;

extern bool				WavefrontObjOpened;
extern bool				WavefrontMtlOpened;

extern unsigned int		WavefrontObjVertCount;
extern unsigned int		WavefrontObjVertCount_Previous;

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
extern FILE				* FileWavefrontObj;
extern FILE				* FileWavefrontMtl;

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

extern unsigned long	Readout_NextGFXCommand1 ;

/* F3DEX2 DISPLAY LIST HANDLING VARIABLES */
extern unsigned long	DLists[256][2048];
extern signed long		DListInfo_CurrentCount[256];
extern signed long		DListInfo_TotalCount;
extern signed long		DListInfo_DListToRender;
extern unsigned long	DLTempPosition;

extern unsigned int		DLToRender;
extern bool				DListHasEnded;

extern bool				SubDLCall;

extern unsigned long	Storage_RDPHalf1;

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
unsigned int fragProg;
int COMBINE0,COMBINE1;
int cA0,cB0,cC0,cD0,aA0,aB0,aC0,aD0;
int cA1,cB1,cC1,cD1,aA1,aB1,aC1,aD1;

/* ZELDA ROM HANDLING VARIABLES */
extern unsigned long	ROM_SceneTableOffset;
extern unsigned int		ROM_SceneToLoad;

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

/* GENERAL RENDERER VARIABLES */
extern GLuint			Renderer_GLDisplayList;
extern GLuint			Renderer_GLDisplayList_Current;
extern GLuint			Renderer_GLDisplayList_Total;

extern GLuint			Renderer_GLTexture;

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
extern GLfloat			LightPosition[];

extern GLfloat			FogColor[];
extern GLfloat			PrimColor[];
extern GLfloat          EnvColor[];

extern int				HACK_UseColorSource;
extern int				HACK_UseAlphaSource;

extern bool				Renderer_EnableMapActors;
extern bool				Renderer_EnableSceneActors;

extern bool				Renderer_EnableMap;
extern bool				Renderer_EnableCollision;
extern GLfloat			Renderer_CollisionAlpha;

/* OPENGL EXTENSION VARIABLES */
extern char				* GLExtension_List;
extern bool				GLExtension_MultiTexture;
extern bool				GLExtension_TextureMirror;
extern bool				GLExtension_AnisoFilter;
extern bool				GLExtension_FragmentProgram;
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
	char EchoLevel;
	char Skybox;
	unsigned long MapTime;
	unsigned char TimeFlow;
	unsigned long MeshDataHeader;
	unsigned char Group_Count;
	unsigned long Group_DataOffset;
	unsigned char Actor_Count;
	unsigned long Actor_DataOffset;
};
extern struct MapHeader_Struct MapHeader[256][256];

struct SceneHeader_Struct {
	unsigned char Map_Count;
	unsigned long Map_ListOffset;
	unsigned long Unknown2;
	unsigned char Col_DataSource;
	unsigned long Col_DataOffset;
	unsigned long Unknown4;
	unsigned long Unknown5;
	unsigned char ScActor_Count;
	unsigned long ScActor_DataOffset;
	unsigned long Unknown7;
	unsigned long Unknown8;
};
extern struct SceneHeader_Struct SceneHeader[256];

/* ZELDA MAP ACTOR DATA STRUCTURE */
struct Actors_Struct {
	int Number;
	short X_Position;
	short Y_Position;
	short Z_Position;
	short X_Rotation;
	short Y_Rotation;
	short Z_Rotation;
	int Variable;
};
extern struct Actors_Struct Actors[256][1024];

/* ZELDA SCENE ACTOR DATA STRUCTURE */
struct ScActors_Struct {
	int Number;
	short X_Position;
	short Y_Position;
	short Z_Position;
	short X_Rotation;
	short Y_Rotation;
	short Z_Rotation;
	int Variable;
};
extern struct ScActors_Struct ScActors[1024];

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
	unsigned int HeightRender;
	unsigned int Width;
	unsigned int WidthRender;
	unsigned int DataSource;
	unsigned int PalDataSource;
	unsigned long Offset;
	unsigned long PalOffset;
	unsigned int Format_N64;
	GLuint Format_OGL;
	unsigned int Format_OGLPixel;
	unsigned int Y_Parameter;
	unsigned int X_Parameter;
	signed short S_Scale;
	signed short T_Scale;
	unsigned int LineSize;
	unsigned int Palette;
};
extern struct Texture_Struct Texture[0];

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
