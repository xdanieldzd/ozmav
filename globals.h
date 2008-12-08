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

/*	------------------------------------------------------------
	DEFINES
	------------------------------------------------------------ */

#define HACKS_ENABLED   false				/* EN-/DISABLE MISC HACKS (not used at the moment) */

/*	------------------------------------------------------------
	SYSTEM FUNCTIONS - OPENGL & WINDOWS
	------------------------------------------------------------ */

extern int Viewer_Initialize();
extern int Viewer_LoadAreaData();
extern int Viewer_GetMapHeaderList(int);
extern int Viewer_GetMapHeader(int);
extern int Viewer_GetSceneHeaderList(int);
extern int Viewer_GetSceneHeader(int);
extern int Viewer_GetMapActors(int);
extern int Viewer_GetSceneActors(int);
extern int Viewer_GetDisplayLists(unsigned long);

extern int Viewer_RenderMap();
extern int Viewer_RenderMap_DListParser(bool, unsigned int, unsigned long);

extern int Viewer_RenderMap_CMDVertexList();
extern int Viewer_GetVertexList(unsigned int, unsigned long, unsigned int, unsigned int);
extern int Viewer_RenderMap_CMDDrawTri1();
extern int Viewer_RenderMap_CMDDrawTri2();
extern int Viewer_RenderMap_CMDTexture();
extern int Viewer_RenderMap_CMDSetTImage();
extern int Viewer_RenderMap_CMDSetTile();
extern int Viewer_RenderMap_CMDSetTileSize();
extern int Viewer_RenderMap_CMDGeometryMode();
extern int Viewer_RenderMap_CMDSetFogColor();
extern int Viewer_RenderMap_CMDSetPrimColor();
extern int Viewer_RenderMap_CMDLoadTLUT(unsigned int, unsigned long);
extern int Viewer_RenderMap_CMDRDPHalf1();
extern int Viewer_RenderMap_CMDSetOtherModeH();
extern int Viewer_RenderMap_CMDSetOtherModeL();
extern int Viewer_RenderMap_CMDSetCombine();

extern int Viewer_SetGLCombiner();
extern GLuint Viewer_LoadTexture(int);

extern int Viewer_ZMemCopy(unsigned int, unsigned long, unsigned char *, unsigned long);

extern int Viewer_RenderAllActors();
extern int Viewer_RenderActorCube(int, GLshort, GLshort, GLshort, signed int, signed int, signed int, bool);

extern void HelperFunc_SplitCurrentVals(bool);
extern int HelperFunc_LogMessage(int, char[]);
extern int HelperFunc_GFXLogCommand(unsigned int);
extern int HelperFunc_CalculateFPS();

extern int InitGL(void);
extern int InitGLExtensions(void);
extern int DrawGLScene(void);
extern void KillGLTarget(void);
extern bool CreateGLTarget(int, int, int);

extern void GLUTCamera_Orientation(float,float);
extern void GLUTCamera_Movement(int);
extern void Camera_MouseMove(int, int);

extern void Dialog_OpenROM(HWND);
extern void Dialog_OpenZMap(HWND);
extern void Dialog_OpenZScene(HWND);

extern PFNGLMULTITEXCOORD1FARBPROC		glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD3FARBPROC		glMultiTexCoord3fARB;
extern PFNGLMULTITEXCOORD4FARBPROC		glMultiTexCoord4fARB;
extern PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB;

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
extern char				ErrorMsg[1024];

extern char				MapActorMsg[256];
extern char				SceneActorMsg[256];

extern bool				AreaLoaded;
extern bool				WndActive;
extern bool				ExitProgram;

extern char				CurrentGFXCmd[256];
extern char				CurrentGFXCmdNote[256];
extern char				GFXLogMsg[256];
extern char				SystemLogMsg[256];

extern bool				GFXLogOpened;

/* CAMERA / VIEWPOINT VARIABLES */
extern float			CamAngleX, CamAngleY;
extern float			CamX, CamY, CamZ;
extern float			CamLX, CamLY, CamLZ;

extern int				MousePosX, MousePosY;
extern int				MouseCenterX, MouseCenterY;

extern bool				MouseButtonDown;

/* FILE HANDLING VARIABLES */
extern FILE				* FileROM;
extern FILE				* FileZMap;
extern FILE				* FileZScene;
extern FILE				* FileGameplayKeep;
extern FILE				* FileGameplayFDKeep;

extern unsigned int		* ROMBuffer;
extern unsigned int		* ZMapBuffer[256];
extern unsigned int		* ZSceneBuffer;
extern unsigned int		* GameplayKeepBuffer;
extern unsigned int		* GameplayFDKeepBuffer;

extern unsigned long	ROMFilesize;
extern unsigned long	ZMapFilesize[256];
extern unsigned long	ZSceneFilesize;
extern unsigned long	GameplayKeepFilesize;
extern unsigned long	GameplayFDKeepFilesize;

extern char				Filename_ROM[256];
extern char				Filename_ZMap[256];
extern char				Filename_ZScene[256];
extern char				Filename_GameplayKeep[256];
extern char				Filename_GameplayFDKeep[256];

extern bool				ROMExists;
extern bool				ZMapExists;
extern bool				ZSceneExists;

extern FILE				* FileGFXLog;
extern FILE				* FileSystemLog;

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

/* F3DZEX DISPLAY LIST HANDLING VARIABLES */
extern unsigned long	DLists[256][2048];
extern signed long		DListInfo_CurrentCount[256];
extern signed long		DListInfo_TotalCount;
extern signed long		DListInfo_DListToRender;
extern unsigned long	DLTempPosition;

extern unsigned int		DLToRender;
extern bool				DListHasEnded;

extern bool				SubDLCall;

/* F3DZEX TEXTURE HANDLING VARIABLES */
extern unsigned char	* TextureData_OGL;
extern unsigned char	* TextureData_N64;

extern unsigned char	* PaletteData;

extern bool				IsMultitex;
extern unsigned int		MTexScaler;

/* ZELDA ROM HANDLING VARIABLES */
extern unsigned long	ROM_SceneTableOffset;
extern unsigned int		ROM_SceneToLoad;
extern unsigned int		ROM_CurrentMap;

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

extern GLuint			Renderer_GLTexture;

extern GLuint			Renderer_FilteringMode_Min;
extern GLuint			Renderer_FilteringMode_Mag;

extern DWORD			Renderer_LastFPS;
extern int				Renderer_FPS, Renderer_FrameNo;
extern char				Renderer_FPSMessage[32];

extern char				Renderer_CoordDisp[256];

extern bool				Renderer_EnableLighting;

extern GLfloat			LightAmbient[];
extern GLfloat			LightDiffuse[];
extern GLfloat			LightPosition[];

extern GLfloat			FogColor[];
extern GLfloat			PrimColor[];

extern bool				Renderer_EnableMapActors;
extern bool				Renderer_EnableSceneActors;

extern bool				Renderer_EnableCombiner;

/* OPENGL EXTENSION VARIABLES */
extern char				* GLExtension_List;
extern bool				GLExtension_MultiTexture;
extern bool				GLExtension_TextureMirror;
extern bool				GLExtension_AnisoFilter;
extern char				GLExtensionsSupported[256];

extern bool				GLExtensionsUnsupported;
extern char				GLExtensionsErrorMsg[512];

/* N64 BLENDING & COMBINER SIMULATION VARIABLES */
extern unsigned int		RDPCycleMode;

extern unsigned long	Blender_Cycle1;
extern unsigned long	Blender_Cycle2;

extern unsigned int		Combiner_TextureMode;
extern unsigned int		Combiner_Texture;
extern unsigned int		Combiner_AlphaMode;
extern unsigned int		Combiner_AlphaCycles;

extern unsigned long	Combiner_Cycle1;
extern unsigned long	Combiner_Cycle2;

extern GLenum			Combiner_Color_A0;
extern GLenum			Combiner_Color_B0;
extern GLenum			Combiner_Color_C0;
extern GLenum			Combiner_Color_D0;
extern GLenum			Combiner_Alpha_A0;
extern GLenum			Combiner_Alpha_B0;
extern GLenum			Combiner_Alpha_C0;
extern GLenum			Combiner_Alpha_D0;

extern GLenum			Combiner_Color_A1;
extern GLenum			Combiner_Color_B1;
extern GLenum			Combiner_Color_C1;
extern GLenum			Combiner_Color_D1;
extern GLenum			Combiner_Alpha_A1;
extern GLenum			Combiner_Alpha_B1;
extern GLenum			Combiner_Alpha_C1;
extern GLenum			Combiner_Alpha_D1;

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
	unsigned long Unknown3;
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

/* F3DZEX VERTEX DATA STRUCTURE */
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

/* F3DZEX TEXTURE DATA STRUCTURE */
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
extern struct Texture_Struct Texture[1];

/* CI TEXTURE PALETTE STRUCTURE */
struct Palette_Struct {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};
extern struct Palette_Struct Palette[512];

