/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer
	
	Written in October/November 2008 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------ */

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

int Viewer_Initialize();
int Viewer_OpenMapScene();
int Viewer_GetMapHeaderList(int);
int Viewer_GetMapHeader(int);
int Viewer_GetSceneHeaderList(int);
int Viewer_GetSceneHeader(int);
int Viewer_GetMapActors(int);
int Viewer_GetSceneActors(int);
int Viewer_GetDisplayLists(unsigned long);

int Viewer_RenderMap();
int Viewer_RenderMap_DListParser(bool, unsigned int, unsigned long);

int Viewer_RenderMap_CMDVertexList();
int Viewer_GetVertexList(unsigned long, int, unsigned int);
int Viewer_RenderMap_CMDDrawTri1();
int Viewer_RenderMap_CMDDrawTri2();
int Viewer_RenderMap_CMDTexture();
int Viewer_RenderMap_CMDSetTImage();
int Viewer_RenderMap_CMDSetTile();
int Viewer_RenderMap_CMDSetTileSize();
int Viewer_RenderMap_CMDGeometryMode();
int Viewer_RenderMap_CMDSetFogColor();
int Viewer_RenderMap_CMDSetPrimColor();
int Viewer_RenderMap_CMDLoadTLUT(unsigned int, unsigned long);
int Viewer_RenderMap_CMDRDPHalf1();
int Viewer_RenderMap_CMDSetOtherModeL();
int Viewer_RenderMap_CMDSetCombine();

GLuint Viewer_LoadTexture(int);

int Viewer_RenderAllActors();
int Viewer_RenderActorCube(int, GLshort, GLshort, GLshort, signed int, signed int, signed int, bool);

void HelperFunc_SplitCurrentVals(bool);
int HelperFunc_LogMessage(int, char[]);
int HelperFunc_GFXLogCommand(unsigned int);
int HelperFunc_CalculateFPS();

int InitGL(void);
int DrawGLScene(void);
void KillGLTarget(void);
BOOL CreateGLTarget(int, int, int);

void GLUTCamera_Orientation(float,float);
void GLUTCamera_Movement(int);
void Camera_MouseMove(int, int);

void Dialog_OpenZMap(HWND);
void Dialog_OpenZScene(HWND);

int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

PFNGLMULTITEXCOORD1FARBPROC		glMultiTexCoord1fARB		= NULL;
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB		= NULL;
PFNGLMULTITEXCOORD3FARBPROC		glMultiTexCoord3fARB		= NULL;
PFNGLMULTITEXCOORD4FARBPROC		glMultiTexCoord4fARB		= NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB			= NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB	= NULL;

/*	------------------------------------------------------------
	VARIABLES
	------------------------------------------------------------ */

/* WINDOW HANDLING VARIABLES, ETC. */
HWND			hwnd = NULL;
HMENU			hmenu = NULL;
HWND			hogl = NULL;
HWND			hstatus = NULL;

HDC				hDC_ogl = NULL;
HGLRC			hRC = NULL;
HINSTANCE		hInstance;

char			szClassName[ ] = "OZMAVClass";

GLuint			GLFontBase;

/* GENERAL GLOBAL PROGRAM VARIABLES */
bool			System_KbdKeys[256];

char			AppTitle[256] = "OZMAV";
char			AppVersion[256] = "V0.5";
char			AppBuildName[256] = "Welcome to the Project!";
char			AppPath[512] = "";
char			INIPath[512] = "";
char			WindowTitle[256] = "";
char			StatusMsg[256] = "";
char			ErrorMsg[256] = "";

char			MapActorMsg[256] = "";
char			SceneActorMsg[256] = "";

bool			MapLoaded = false;
bool			WndActive = true;
bool			ExitProgram = false;

char			CurrentGFXCmd[256] = "";
char			CurrentGFXCmdNote[256] = "";
char			GFXLogMsg[256] = "";
char			SystemLogMsg[256] = "";

bool			GFXLogOpened = false;

/* CAMERA / VIEWPOINT VARIABLES */
float			CamAngleX, CamAngleY;
float			CamX, CamY, CamZ;
float			CamLX, CamLY, CamLZ;

int				MousePosX = 0, MousePosY = 0;
int				MouseCenterX = 0, MouseCenterY = 0;

bool			MouseButtonDown = false;

/* FILE HANDLING VARIABLES */
FILE			* FileZMap;
FILE			* FileZScene;
FILE			* FileGameplayKeep;
FILE			* FileGameplayFDKeep;

unsigned int	* ZMapBuffer;
unsigned int	* ZSceneBuffer;
unsigned int	* GameplayKeepBuffer;
unsigned int	* GameplayFDKeepBuffer;

unsigned long	ZMapFilesize = 0;
unsigned long	ZSceneFilesize = 0;
unsigned long	GameplayKeepFilesize = 0;
unsigned long	GameplayFDKeepFilesize = 0;

char			Filename_ZMap[256] = "";
char			Filename_ZScene[256] = "";
char			Filename_GameplayKeep[256] = "gameplay_keep.zdata";
char			Filename_GameplayFDKeep[256] = "gameplay_dangeon_keep.zdata";

bool			ZMapExists = false;
bool			ZSceneExists = false;

FILE			* FileGFXLog;
FILE			* FileSystemLog;

/* DATA READOUT VARIABLES */
unsigned long	Readout_Current1 = 0;
unsigned long	Readout_Current2 = 0;
unsigned int	Readout_CurrentByte1 = 0;
unsigned int	Readout_CurrentByte2 = 0;
unsigned int	Readout_CurrentByte3 = 0;
unsigned int	Readout_CurrentByte4 = 0;
unsigned int	Readout_CurrentByte5 = 0;
unsigned int	Readout_CurrentByte6 = 0;
unsigned int	Readout_CurrentByte7 = 0;
unsigned int	Readout_CurrentByte8 = 0;

unsigned long	Readout_NextGFXCommand1 = 0;

/* F3DZEX DISPLAY LIST HANDLING VARIABLES */
unsigned long	DLists[2048];
signed long		DListInfo_CurrentCount = 0;
signed long		DListInfo_DListToRender = 0;
unsigned long	DLTempPosition = 0;

unsigned int	DLToRender = 0;
bool			DListHasEnded = false;

bool			SubDLCall = false;

/* F3DZEX TEXTURE HANDLING VARIABLES */
unsigned char	* TextureData_OGL;
unsigned char	* TextureData_N64;

unsigned char	* PaletteData;

bool			IsMultitex = false;
unsigned int	MTexScaler = 1;

/* ZELDA MAP & SCENE HEADER HANDLING VARIABLES */
bool			MapHeader_MultiHeaderMap = false;
int				MapHeader_Current;
int				MapHeader_TotalCount = 0;
unsigned long	MapHeader_List[256];
unsigned long	MapHeader_CurrentPosInList = 0;

bool			SceneHeader_MultiHeaderMap = false;
int				SceneHeader_Current;
int				SceneHeader_TotalCount = 0;
unsigned long	SceneHeader_List[256];
unsigned long	SceneHeader_CurrentPosInList = 0;

/* ZELDA ACTOR DATA HANDLING VARIABLES */
int				ActorInfo_CurrentCount = 0;
int				ActorInfo_Selected = 0;

int				ScActorInfo_CurrentCount = 0;
int				ScActorInfo_Selected = 0;

/* GENERAL RENDERER VARIABLES */
GLuint			Renderer_GLDisplayList = 0;
GLuint			Renderer_GLDisplayList_Current = 0;

GLuint			Renderer_GLTexture = 0;

GLuint			Renderer_FilteringMode_Min = GL_LINEAR;
GLuint			Renderer_FilteringMode_Mag = GL_LINEAR;

DWORD			Renderer_LastFPS = 0;
int				Renderer_FPS, Renderer_FrameNo = 0;
char			Renderer_FPSMessage[32] = "";

char			Renderer_CoordDisp[256] = "";

bool			Renderer_EnableLighting = true;

GLfloat			LightAmbient[]= { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat			LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat			LightPosition[]= { 1.0f, 1.0f, 1.0f, 1.0f };

GLfloat			FogColor[]= { 0.0f, 0.0f, 0.0f, 0.5f };
GLfloat			PrimColor[]= { 0.0f, 0.0f, 0.0f, 1.0f };

bool			Renderer_EnableMapActors = true;
bool			Renderer_EnableSceneActors = true;

bool			Renderer_EnableMultiTexturing = false;

/* OPENGL EXTENSION VARIABLES */
char			* GLExtension_List;
bool			GLExtension_MultiTexture = false;
bool			GLExtension_TextureMirror = false;
bool			GLExtension_AnisoFilter = false;
char			GLExtensionsSupported[256] = "";

/* N64 BLENDING & COMBINER SIMULATION VARIABLES */
unsigned long	Blender_Cycle1 = 0x00;
unsigned long	Blender_Cycle2 = 0x00;

unsigned long	Combiner_Cycle1 = 0x00;
unsigned long	Combiner_Cycle2 = 0x00;

GLenum			Combiner_Color_CmbMode[4];
GLenum			Combiner_Color_Arg0_Source[4];
GLenum			Combiner_Color_Arg0_Op[4];
GLenum			Combiner_Color_Arg1_Source[4];
GLenum			Combiner_Color_Arg1_Op[4];
GLenum			Combiner_Color_Arg2_Source[4];
GLenum			Combiner_Color_Arg2_Op[4];

GLenum			Combiner_Alpha_CmbMode[4];
GLenum			Combiner_Alpha_Arg0_Source[4];
GLenum			Combiner_Alpha_Arg0_Op[4];
GLenum			Combiner_Alpha_Arg1_Source[4];
GLenum			Combiner_Alpha_Arg1_Op[4];
GLenum			Combiner_Alpha_Arg2_Source[4];
GLenum			Combiner_Alpha_Arg2_Op[4];

/*	------------------------------------------------------------
	STRUCTURES
	------------------------------------------------------------ */

/* ZELDA MAP HEADER STRUCTURE */
struct {
	char EchoLevel;
	char Skybox;
	unsigned long MapTime;
	unsigned char TimeFlow;
	unsigned long MeshDataHeader;
	unsigned char Group_Count;
	unsigned long Group_DataOffset;
	unsigned char Actor_Count;
	unsigned long Actor_DataOffset;
} MapHeader[256];

struct {
	unsigned long Unknown1;
	unsigned long Unknown2;
	unsigned long Unknown3;
	unsigned long Unknown4;
	unsigned long Unknown5;
	unsigned char ScActor_Count;
	unsigned long ScActor_DataOffset;
	unsigned long Unknown7;
	unsigned long Unknown8;
} SceneHeader[256];

/* ZELDA MAP ACTOR DATA STRUCTURE */
struct {
	int Number;
	short X_Position;
	short Y_Position;
	short Z_Position;
	short X_Rotation;
	short Y_Rotation;
	short Z_Rotation;
	int Variable;
} Actors[1024];

/* ZELDA SCENE ACTOR DATA STRUCTURE */
struct {
	int Number;
	short X_Position;
	short Y_Position;
	short Z_Position;
	short X_Rotation;
	short Y_Rotation;
	short Z_Rotation;
	int Variable;
} ScActors[1024];

/* F3DZEX VERTEX DATA STRUCTURE */
struct {
	signed short X;
	signed short Y;
	signed short Z;
	signed short H;
	signed short V;
	GLbyte R;
	GLbyte G;
	GLbyte B;
	GLbyte A;
} Vertex[4096];

/* F3DZEX TEXTURE DATA STRUCTURE */
struct {
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
} Texture[1];

/* CI TEXTURE PALETTE STRUCTURE */
struct {
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
} Palette[512];

/*	------------------------------------------------------------ */

/* VIEWER_INITIALIZE - CALLED AFTER SELECTING THE MAP AND SCENE FILES */
int Viewer_Initialize()
{
	FileSystemLog = fopen("log.txt", "w");
	
	Viewer_OpenMapScene();
	
	Viewer_RenderMap();
	
	EnableMenuItem(hmenu, IDM_CAMERA_RESETCOORDS, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_MAP_PREVDLIST, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_MAP_NEXTDLIST, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_MAP_PREVHEADER, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_MAP_NEXTHEADER, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_SCENE_PREVHEADER, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_SCENE_NEXTHEADER, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_ACTORS_MAPRENDER, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_ACTORS_SELECTPREV, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_ACTORS_SELECTNEXT, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_ACTORS_JUMPFIRST, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_ACTORS_JUMPLAST, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_ACTORS_SCENERENDER, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_ACTORS_SCENEPREV, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_ACTORS_SCENENEXT, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_FILTERNEAREST, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_FILTERLINEAR, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_FILTERMIPMAP, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_MULTITEXTURE, MF_BYCOMMAND | MF_ENABLED);
	
	sprintf(WindowTitle, "%s %s - %s", AppTitle, AppVersion, Filename_ZMap);
	SetWindowText(hwnd, WindowTitle);
	
	return 0;
}

/* VIEWER_OPENMAPSCENE - CALLED IN INITIALIZATION, LOADS MAP AND SCENE DATA INTO BUFFERS AND DOES SOME PRE-ANALYZING */
int Viewer_OpenMapScene()
{
	MapLoaded = false;
	
	/* FREE PREVIOUSLY ALLOCATED MEMORY */
	if(ZMapBuffer != NULL) free(ZMapBuffer);
	if(ZSceneBuffer != NULL) free(ZSceneBuffer);
	if(GameplayKeepBuffer != NULL) free(GameplayKeepBuffer);
	if(GameplayFDKeepBuffer != NULL) free(GameplayFDKeepBuffer);
	if(PaletteData != NULL) free(PaletteData);
	
	/* OPEN FILE */
	FileZMap = fopen(Filename_ZMap, "r+b");
	/* GET FILESIZE */
	size_t Result;
	fseek(FileZMap, 0, SEEK_END);
	ZMapFilesize = ftell(FileZMap);
	rewind(FileZMap);
	/* LOAD FILE INTO BUFFER */
	ZMapBuffer = (unsigned int*) malloc (sizeof(int) * ZMapFilesize);
	Result = fread(ZMapBuffer, 1, ZMapFilesize, FileZMap);
	/* CLOSE FILE */
	fclose(FileZMap);
	
	/* OPEN FILE */
	FileZScene = fopen(Filename_ZScene, "r+b");
	/* GET FILESIZE */
	fseek(FileZMap, 0, SEEK_END);
	ZSceneFilesize = ftell(FileZScene);
	rewind(FileZScene);
	/* LOAD FILE INTO BUFFER */
	ZSceneBuffer = (unsigned int*) malloc (sizeof(int) * ZSceneFilesize);
	Result = fread(ZSceneBuffer, 1, ZSceneFilesize, FileZScene);
	/* CLOSE FILE */
	fclose(FileZScene);
	
	/* OPEN FILE */
	FileGameplayKeep = fopen(Filename_GameplayKeep, "r+b");
	/* GET FILESIZE */
	fseek(FileGameplayKeep, 0, SEEK_END);
	GameplayKeepFilesize = ftell(FileGameplayKeep);
	rewind(FileGameplayKeep);
	/* LOAD FILE INTO BUFFER */
	GameplayKeepBuffer = (unsigned int*) malloc (sizeof(int) * GameplayKeepFilesize);
	Result = fread(GameplayKeepBuffer, 1, GameplayKeepFilesize, FileGameplayKeep);
	/* CLOSE FILE */
	fclose(FileGameplayKeep);
	
	/* OPEN FILE */
	FileGameplayFDKeep = fopen(Filename_GameplayFDKeep, "r+b");
	/* GET FILESIZE */
	fseek(FileGameplayFDKeep, 0, SEEK_END);
	GameplayFDKeepFilesize = ftell(FileGameplayFDKeep);
	rewind(FileGameplayFDKeep);
	/* LOAD FILE INTO BUFFER */
	GameplayFDKeepBuffer = (unsigned int*) malloc (sizeof(int) * GameplayFDKeepFilesize);
	Result = fread(GameplayFDKeepBuffer, 1, GameplayFDKeepFilesize, FileGameplayFDKeep);
	/* CLOSE FILE */
	fclose(FileGameplayFDKeep);
	
	memcpy(&Readout_Current1, &ZMapBuffer[0], 4);
	memcpy(&Readout_Current2, &ZMapBuffer[0 + 1], 4);
	HelperFunc_SplitCurrentVals(true);
	
	if((Readout_CurrentByte1 == 0x08) || (Readout_CurrentByte1 == 0x16) || (Readout_CurrentByte1 == 0x18)) {
		if((Readout_CurrentByte1 == 0x18)) {
			MapHeader_MultiHeaderMap = true;
			unsigned long MapHeaderListPos = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			Viewer_GetMapHeaderList(MapHeaderListPos);
		} else {
			MapHeader_TotalCount = 0;
		}
		
		memcpy(&Readout_Current1, &ZSceneBuffer[0], 4);
		memcpy(&Readout_Current2, &ZSceneBuffer[0 + 1], 4);
		HelperFunc_SplitCurrentVals(true);
		
		if((Readout_CurrentByte1 == 0x18)) {
			SceneHeader_MultiHeaderMap = true;
			unsigned long SceneHeaderListPos = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			Viewer_GetSceneHeaderList(SceneHeaderListPos);
		} else {
			SceneHeader_TotalCount = 0;
		}
		
		MapHeader_Current = 0;
		SceneHeader_Current = 0;
		DListInfo_DListToRender = -1;
		ActorInfo_Selected = 0;
		
		Renderer_EnableLighting = true;
		
		memset(MapHeader, 0x00, sizeof(MapHeader));
		memset(SceneHeader, 0x00, sizeof(SceneHeader));
		memset(Actors, 0x00, sizeof(Actors));
		memset(ScActors, 0x00, sizeof(ScActors));
		memset(Vertex, 0x00, sizeof(Vertex));
		
		PaletteData = (unsigned char *) malloc (1024);
		memset(PaletteData, 0x00, sizeof(PaletteData));
		
		Viewer_GetMapHeader(MapHeader_Current);
		Viewer_GetSceneHeader(SceneHeader_Current);
		Viewer_GetMapActors(MapHeader_Current);
		Viewer_GetSceneActors(SceneHeader_Current);
		Viewer_GetDisplayLists(ZMapFilesize);
		
		CamAngleX = 0.0f, CamAngleY = 0.0f;
		CamX = 0.0f, CamY = 0.0f, CamZ = 5.0f;
		CamLX = 0.0f, CamLY = 0.0f, CamLZ = -1.0f;
		
		sprintf(StatusMsg, "Map loaded successfully!");
	} else {
		MessageBox(hwnd, "Error: Selected file is not a ZMap file!", "Error", MB_OK | MB_ICONERROR);
	}
	
	return 0;
}

/* VIEWER_GETMAPHEADERLIST - IF THE LOADED MAP FILE CONTAINS MULTIPLE MAP HEADERS, STORE THE HEADER LIST FOR LATER USE */
int Viewer_GetMapHeaderList(int HeaderListPos)
{
	bool EndOfList = false;
	
	HeaderListPos = HeaderListPos / 4;
	
	MapHeader_List[0] = 0x08;
	MapHeader_TotalCount = 1;
	
	while (!EndOfList) {
		memcpy(&Readout_Current1, &ZMapBuffer[HeaderListPos], 4);
		
		HelperFunc_SplitCurrentVals(false);
		
		if ((Readout_CurrentByte1 == 0x03)) {
			MapHeader_List[MapHeader_TotalCount] = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			MapHeader_TotalCount++;
		} else if ((Readout_Current1 == 0x00000000)) {
			//ignore
		} else {
			EndOfList = true;
		}
		
		HeaderListPos++;
	}
	
	return 0;
}

/* VIEWER_GETMAPHEADER - READ THE CURRENTLY SELECTED MAP HEADER AND STORE ITS INFORMATION IN THE MAPHEADER STRUCT */
int Viewer_GetMapHeader(int CurrentHeader)
{
	bool EndOfHeader = false;
	
	int InHeaderPos = MapHeader_List[CurrentHeader] / 4;
	
	sprintf(SystemLogMsg, "Map Header #%d (0x%08X):\n", CurrentHeader + 1, InHeaderPos * 4);
	HelperFunc_LogMessage(2, SystemLogMsg);
	
	while(!EndOfHeader) {
		memcpy(&Readout_Current1, &ZMapBuffer[InHeaderPos], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[InHeaderPos + 1], 4);
		
		HelperFunc_SplitCurrentVals(true);
		
		switch(Readout_CurrentByte1) {
		case 0x01:
			MapHeader[CurrentHeader].Actor_Count = Readout_CurrentByte2;
			MapHeader[CurrentHeader].Actor_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			break;
		case 0x0A:
			MapHeader[CurrentHeader].MeshDataHeader = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			break;
		case 0x0B:
			MapHeader[CurrentHeader].Group_Count = Readout_CurrentByte2;
			MapHeader[CurrentHeader].Group_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			break;
		case 0x10:
			MapHeader[CurrentHeader].MapTime = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			MapHeader[CurrentHeader].TimeFlow = Readout_CurrentByte7;
			break;
		case 0x12:
			MapHeader[CurrentHeader].Skybox = Readout_CurrentByte5;
			break;
		case 0x14:
			EndOfHeader = true;
			break;
		case 0x16:
			MapHeader[CurrentHeader].EchoLevel = Readout_CurrentByte8;
			break;
		default:
			sprintf(SystemLogMsg, "  0x%08X:\tUnknown header option\t\t [%02X%02X%02X%02X %02X%02X%02X%02X]\n",
				InHeaderPos * 4,
				Readout_CurrentByte1, Readout_CurrentByte2, Readout_CurrentByte3, Readout_CurrentByte4,
				Readout_CurrentByte5, Readout_CurrentByte6, Readout_CurrentByte7, Readout_CurrentByte8);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		}
		
		InHeaderPos += 2;
	}
	
	Readout_Current1 = 0x00;
	Readout_Current2 = 0x00;
	
	HelperFunc_LogMessage(2, "\n");
	
	return 0;
}

/* VIEWER_GETSCENEHEADERLIST - IF THE LOADED SCENE FILE CONTAINS MULTIPLE SCENE HEADERS, STORE THE HEADER LIST FOR LATER USE */
int Viewer_GetSceneHeaderList(int HeaderListPos)
{
	bool EndOfList = false;
	
	HeaderListPos = HeaderListPos / 4;
	
	SceneHeader_List[0] = 0x08;
	SceneHeader_TotalCount = 1;
	
	while (!EndOfList) {
		memcpy(&Readout_Current1, &ZSceneBuffer[HeaderListPos], 4);
		
		HelperFunc_SplitCurrentVals(false);
		
		if ((Readout_CurrentByte1 == 0x02)) {
			SceneHeader_List[SceneHeader_TotalCount] = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			SceneHeader_TotalCount++;
		} else if ((Readout_Current1 == 0x00000000)) {
			//ignore
		} else {
			EndOfList = true;
		}
		
		HeaderListPos++;
	}
	
	return 0;
}

/* VIEWER_GETSCENEHEADER - READ THE CURRENT SCENE HEADER AND STORE ITS INFORMATION IN THE SCENEHEADER STRUCT */
int Viewer_GetSceneHeader(int CurrentHeader)
{
	bool EndOfHeader = false;
	
	int InHeaderPos = SceneHeader_List[CurrentHeader] / 4;
	
	sprintf(SystemLogMsg, "Scene Header #%d (0x%08X):\n", CurrentHeader + 1, InHeaderPos * 4);
	HelperFunc_LogMessage(2, SystemLogMsg);
	
	while(!EndOfHeader) {
		memcpy(&Readout_Current1, &ZSceneBuffer[InHeaderPos], 4);
		memcpy(&Readout_Current2, &ZSceneBuffer[InHeaderPos + 1], 4);
		
		HelperFunc_SplitCurrentVals(true);
		
		switch(Readout_CurrentByte1) {
		case 0x00:
			SceneHeader[CurrentHeader].ScActor_Count = Readout_CurrentByte2;
			SceneHeader[CurrentHeader].ScActor_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			break;
		case 0x14:
			EndOfHeader = true;
			break;
		default:
			sprintf(SystemLogMsg, "  0x%08X:\tUnknown header option\t\t [%02X%02X%02X%02X %02X%02X%02X%02X]\n",
				InHeaderPos * 4,
				Readout_CurrentByte1, Readout_CurrentByte2, Readout_CurrentByte3, Readout_CurrentByte4,
				Readout_CurrentByte5, Readout_CurrentByte6, Readout_CurrentByte7, Readout_CurrentByte8);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		}
		
		InHeaderPos += 2;
	}
	
	Readout_Current1 = 0x00;
	Readout_Current2 = 0x00;
	
	HelperFunc_LogMessage(2, "\n");
	
	return 0;
}

/* VIEWER_GETMAPACTORS - READ THE MAP ACTOR DATA FROM THE OFFSET SPECIFIED INSIDE THE CURRENT MAP HEADER */
int Viewer_GetMapActors(int CurrentHeader)
{
	int InActorDataPos = MapHeader[CurrentHeader].Actor_DataOffset / 4;
	
	ActorInfo_CurrentCount = 0;
	ActorInfo_Selected = 0;
	
	if(!(MapHeader[CurrentHeader].Actor_Count) == 0) {
		while (!(ActorInfo_CurrentCount == MapHeader[CurrentHeader].Actor_Count)) {
			memcpy(&Readout_Current1, &ZMapBuffer[InActorDataPos], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[InActorDataPos + 1], 4);
			
			HelperFunc_SplitCurrentVals(true);
			
			Actors[ActorInfo_CurrentCount].Number = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			Actors[ActorInfo_CurrentCount].X_Position = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Actors[ActorInfo_CurrentCount].Y_Position = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			Actors[ActorInfo_CurrentCount].Z_Position = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			
			memcpy(&Readout_Current1, &ZMapBuffer[InActorDataPos + 2], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[InActorDataPos + 3], 4);
			
			HelperFunc_SplitCurrentVals(true);
			
			Actors[ActorInfo_CurrentCount].X_Rotation = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			Actors[ActorInfo_CurrentCount].Y_Rotation = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Actors[ActorInfo_CurrentCount].Z_Rotation = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			Actors[ActorInfo_CurrentCount].Variable = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			
			ActorInfo_CurrentCount++;
			InActorDataPos += 4;
		}
	}
	
	return 0;
}

/* VIEWER_GETSCENEACTORS - READ THE SCENE ACTOR DATA FROM THE OFFSET SPECIFIED INSIDE THE CURRENT SCENE HEADER */
int Viewer_GetSceneActors(int CurrentHeader)
{
	int InScActorDataPos = SceneHeader[CurrentHeader].ScActor_DataOffset / 4;
	
	ScActorInfo_CurrentCount = 0;
	ScActorInfo_Selected = 0;
	
	if(!(SceneHeader[CurrentHeader].ScActor_Count) == 0) {
		while (!(ScActorInfo_CurrentCount == SceneHeader[CurrentHeader].ScActor_Count)) {
			memcpy(&Readout_Current1, &ZSceneBuffer[InScActorDataPos], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[InScActorDataPos + 1], 4);
			
			HelperFunc_SplitCurrentVals(true);
			
			ScActors[ScActorInfo_CurrentCount].Number = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			ScActors[ScActorInfo_CurrentCount].X_Position = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			ScActors[ScActorInfo_CurrentCount].Y_Position = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			ScActors[ScActorInfo_CurrentCount].Z_Position = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			
			memcpy(&Readout_Current1, &ZMapBuffer[InScActorDataPos + 2], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[InScActorDataPos + 3], 4);
			
			HelperFunc_SplitCurrentVals(true);
			
			ScActors[ScActorInfo_CurrentCount].X_Rotation = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			ScActors[ScActorInfo_CurrentCount].Y_Rotation = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			ScActors[ScActorInfo_CurrentCount].Z_Rotation = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			ScActors[ScActorInfo_CurrentCount].Variable = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			
			ScActorInfo_CurrentCount++;
			InScActorDataPos += 4;
		}
	}
	
	return 0;
}

/* VIEWER_GETDISPLAYLISTS - SCAN THE CURRENT MAP FOR F3DZEX DISPLAY LISTS AND STORE THEIR OFFSETS IN THE DLISTS STRUCT */
int Viewer_GetDisplayLists(unsigned long Fsize)
{
	unsigned int DListScanPosition = 0;
	DListInfo_CurrentCount = -1;
	unsigned long TempOffset = 0;
	
	while ((DListScanPosition < Fsize / 4)) {
		memcpy(&Readout_Current1, &ZMapBuffer[DListScanPosition], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[DListScanPosition + 1], 4);
		
		HelperFunc_SplitCurrentVals(true);
		
		if ((Readout_CurrentByte1 == F3DEX2_DL) && (Readout_CurrentByte2 == 0x00)) {
			if((Readout_CurrentByte3 == 0x00) && (Readout_CurrentByte4 == 0x00)) {
				if((Readout_CurrentByte5 == 0x03)) {
					TempOffset = Readout_CurrentByte6 << 16;
					TempOffset = TempOffset + (Readout_CurrentByte7 << 8);
					TempOffset = TempOffset + Readout_CurrentByte8;
					
					DListInfo_CurrentCount++;
					DLists[DListInfo_CurrentCount] = TempOffset;
				}
			}
		}
		
		DListScanPosition += 2;
	}
	
	return 0;
}

/* VIEWER_RENDERMAP - MAIN FUNCTION FOR DISPLAY LIST HANDLING TO RENDER MAPS, FOR EITHER ALL OR A GIVEN DISPLAY LIST(S) */
int Viewer_RenderMap()
{
	/* OPEN GFX COMMAND LOG */
	if(!GFXLogOpened) FileGFXLog = fopen("gfxlog.txt", "w"); GFXLogOpened = true;
	
	/* IF GL DLISTS EXIST, DELETE THEM ALL */
	if(Renderer_GLDisplayList != 0) { glDeleteLists(Renderer_GLDisplayList, 256); }
	/* REGENERATE GL DLISTS */
	Renderer_GLDisplayList = glGenLists(256);
	
	/* SET GL DLIST BASE */
	glListBase(Renderer_GLDisplayList);
	
	/* RESET SOME VALUES FOR DLIST READING/BUILDING */
	Renderer_GLDisplayList_Current = Renderer_GLDisplayList;
	DLToRender = 0;
	
	while(!(Renderer_GLDisplayList_Current == Renderer_GLDisplayList + (DListInfo_CurrentCount + 1))) {
		if(Renderer_GLDisplayList_Current != 0) {
			glNewList(Renderer_GLDisplayList_Current, GL_COMPILE);
				DLTempPosition = DLists[DLToRender] / 4;
				
				SubDLCall = false;
				Viewer_RenderMap_DListParser(false, DLToRender, DLTempPosition);
				
				PrimColor[0] = 0.0f;
				PrimColor[1] = 0.0f;
				PrimColor[2] = 0.0f;
				PrimColor[3] = 1.0f;
				
				Blender_Cycle1 = 0x00;
				Blender_Cycle2 = 0x00;
				
				IsMultitex = false;
				
				DListHasEnded = false;
			glEndList();
			HelperFunc_LogMessage(2, "end of dlist\n");
		}
		
		Renderer_GLDisplayList_Current++;
		DLToRender++;
	}
	
	MapLoaded = true;
	
	fclose(FileGFXLog); GFXLogOpened = false;
	fclose(FileSystemLog);
	
	return 0;
}

/*	------------------------------------------------------------ */

/* VIEWER_RENDERMAP_DLISTPARSER - DLIST INTERPRETER MAIN LOOP, SCANS AND EXECUTES DLIST COMMANDS */
int Viewer_RenderMap_DListParser(bool CalledFromRDPHalf, unsigned int DLToRender, unsigned long Position)
{
	if(CalledFromRDPHalf) {
		sprintf(GFXLogMsg, "  [DList called via RDPHALF_1 (0x%08X)]\n", (unsigned int)Position * 4);
		HelperFunc_LogMessage(1, GFXLogMsg);
	} else {
		sprintf(GFXLogMsg, "Display List #%d (0x%08X):\n", DLToRender + 1, (unsigned int)Position * 4);
		HelperFunc_LogMessage(1, GFXLogMsg);
	}
	
	while (!DListHasEnded) {
		memcpy(&Readout_Current1, &ZMapBuffer[Position], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[Position + 1], 4);
		
		memcpy(&Readout_NextGFXCommand1, &ZMapBuffer[Position + 2], 4);
		
		HelperFunc_SplitCurrentVals(true);
		
		if(CalledFromRDPHalf) HelperFunc_LogMessage(1, " ");
		
		switch(Readout_CurrentByte1) {
		case F3DEX2_VTX:
			sprintf(CurrentGFXCmd, "F3DEX2_VTX           ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDVertexList();
			break;
		case F3DEX2_TRI1:
			sprintf(CurrentGFXCmd, "F3DEX2_TRI1          ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDDrawTri1();
			break;
		case F3DEX2_TRI2:
			sprintf(CurrentGFXCmd, "F3DEX2_TRI2          ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDDrawTri2();
			break;
		case F3DEX2_TEXTURE:
			sprintf(CurrentGFXCmd, "F3DEX2_TEXTURE       ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDTexture();
			break;
		case G_SETTIMG:
			sprintf(CurrentGFXCmd, "G_SETTIMG            ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetTImage();
			break;
		case G_SETTILE:
			sprintf(CurrentGFXCmd, "G_SETTILE            ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetTile();
			break;
		case G_LOADBLOCK:
			sprintf(CurrentGFXCmd, "G_LOADBLOCK          ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_SETTILESIZE:
			sprintf(CurrentGFXCmd, "G_SETTILESIZE        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetTileSize();
			break;
		case G_RDPFULLSYNC:
			sprintf(CurrentGFXCmd, "G_RDPFULLSYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_RDPTILESYNC:
			sprintf(CurrentGFXCmd, "G_RDPTILESYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_RDPPIPESYNC:
			sprintf(CurrentGFXCmd, "G_RDPPIPESYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_RDPLOADSYNC:
			sprintf(CurrentGFXCmd, "G_RDPLOADSYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		case F3DEX2_GEOMETRYMODE:
			sprintf(CurrentGFXCmd, "F3DEX2_GEOMETRYMODE  ");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDGeometryMode();
			break;
		case F3DEX2_CULLDL:
			sprintf(CurrentGFXCmd, "F3DEX2_CULLDL        ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case F3DEX2_SETOTHERMODE_H:
			sprintf(CurrentGFXCmd, "F3DEX2_SETOTHERMODE_H");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case F3DEX2_SETOTHERMODE_L:
			sprintf(CurrentGFXCmd, "F3DEX2_SETOTHERMODE_L");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetOtherModeL();
			break;
		case G_SETFOGCOLOR:
			sprintf(CurrentGFXCmd, "G_SETFOGCOLOR        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetFogColor();
			break;
		case G_SETPRIMCOLOR:
			sprintf(CurrentGFXCmd, "G_SETPRIMCOLOR       ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetPrimColor();
			break;
		case F3DEX2_RDPHALF_1:
			sprintf(CurrentGFXCmd, "F3DEX2_RDPHALF_1     ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDRDPHalf1();
			break;
		case G_LOADTLUT:
			sprintf(CurrentGFXCmd, "G_LOADTLUT           ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDLoadTLUT(Texture[0].PalDataSource, Texture[0].PalOffset);
			break;
		case G_SETCOMBINE:
			sprintf(CurrentGFXCmd, "G_SETCOMBINE         ");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetCombine();
			break;
		case F3DEX2_DL:
			sprintf(CurrentGFXCmd, "F3DEX2_DL            ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case F3DEX2_BRANCH_Z:
			sprintf(CurrentGFXCmd, "F3DEX2_BRANCH_Z      ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case F3DEX2_ENDDL:
			sprintf(CurrentGFXCmd, "F3DEX2_ENDDL         ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			DListHasEnded = true;
			break;
		default:
			sprintf(CurrentGFXCmd, "<unknown>            ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		}
		
		if(DListHasEnded) {
			if(CalledFromRDPHalf) {
				HelperFunc_LogMessage(1, "  [Return to original DList]\n");
			} else {
				HelperFunc_LogMessage(1, "\n");
			}
		}
		
		Position+=2;
	}
	
	if(CalledFromRDPHalf) DListHasEnded = false;
	
	SubDLCall = false;
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDVERTEXLIST - F3DEX2_VTX - GET OFFSET, AMOUNT & VERTEX BUFFER POSITION OF VERTICES TO USE */
int Viewer_RenderMap_CMDVertexList()
{
	if((GLExtension_MultiTexture) && (Renderer_EnableMultiTexturing)) {
		if(IsMultitex) {
			/* texture 0 */
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D);
			
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
			
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, Combiner_Color_CmbMode[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, Combiner_Color_Arg0_Source[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, Combiner_Color_Arg0_Op[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, Combiner_Color_Arg1_Source[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, Combiner_Color_Arg1_Op[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, Combiner_Color_Arg2_Source[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, Combiner_Color_Arg2_Op[0]);
			
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, Combiner_Alpha_CmbMode[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, Combiner_Alpha_Arg0_Source[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, Combiner_Alpha_Arg0_Op[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, Combiner_Alpha_Arg1_Source[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, Combiner_Alpha_Arg1_Op[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_ARB, Combiner_Alpha_Arg2_Source[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_ARB, Combiner_Alpha_Arg2_Op[0]);
			
			Renderer_GLTexture = Viewer_LoadTexture(1);
			glBindTexture(GL_TEXTURE_2D, Renderer_GLTexture);
			
			/* texture 1 */
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);
			
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
			
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, Combiner_Color_CmbMode[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, Combiner_Color_Arg0_Source[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, Combiner_Color_Arg0_Op[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, Combiner_Color_Arg1_Source[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, Combiner_Color_Arg1_Op[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, Combiner_Color_Arg2_Source[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, Combiner_Color_Arg2_Op[1]);
			
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, Combiner_Alpha_CmbMode[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, Combiner_Alpha_Arg0_Source[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, Combiner_Alpha_Arg0_Op[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, Combiner_Alpha_Arg1_Source[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, Combiner_Alpha_Arg1_Op[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_ARB, Combiner_Alpha_Arg2_Source[1]);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_ARB, Combiner_Alpha_Arg2_Op[1]);
			
			Renderer_GLTexture = Viewer_LoadTexture(0);
			glBindTexture(GL_TEXTURE_2D, Renderer_GLTexture);
			
			glActiveTextureARB(GL_TEXTURE0_ARB);
		} else {
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D);
			
			Renderer_GLTexture = Viewer_LoadTexture(0);
			glBindTexture(GL_TEXTURE_2D, Renderer_GLTexture);
			
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			
			glActiveTextureARB(GL_TEXTURE0_ARB);
		}
	} else {
		Renderer_GLTexture = Viewer_LoadTexture(0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Renderer_GLTexture);
	}
	
	unsigned long TempVertListOffset = 0;
	int TempVertCount = 0;
	unsigned int TempVertListStartEntry = 0;
	
	TempVertListOffset = Readout_CurrentByte6 << 16;
	TempVertListOffset = TempVertListOffset + (Readout_CurrentByte7 << 8);
	TempVertListOffset = TempVertListOffset + Readout_CurrentByte8;
	TempVertCount = ((Readout_CurrentByte4 / 2));
	
	TempVertListStartEntry = TempVertCount - ((Readout_CurrentByte2 << 4) + (Readout_CurrentByte3 >> 4));
	
	Viewer_GetVertexList(TempVertListOffset, TempVertCount, TempVertListStartEntry);
	
	return 0;
}

/* VIEWER_GETVERTEXLIST - FOR F3DEX2_VTX - READ THE VERTEX LIST SPECIFIED BEFORE AND STORE THE DATA IN VERTEX STRUCT */
int Viewer_GetVertexList(unsigned long Offset, int VertCount, unsigned int TempVertListStartEntry)
{
	unsigned long CurrentVert = TempVertListStartEntry;
	unsigned long VertListPosition = Offset / 4;
	
	unsigned long TempVal1 = 0;
	unsigned short TempVal2 = 0;
	
	while(CurrentVert < VertCount + 1) {
		// X + Y
		memcpy(&TempVal1, &ZMapBuffer[VertListPosition], 4);
		Vertex[CurrentVert].Y = TempVal1 >> 16 << 8;
		Vertex[CurrentVert].Y = Vertex[CurrentVert].Y + (TempVal1 >> 24);
		Vertex[CurrentVert].X = TempVal1 << 8;
		TempVal2 = (TempVal1 >> 8 << 8);
		TempVal2 = TempVal2 >> 8;
		Vertex[CurrentVert].X = Vertex[CurrentVert].X + TempVal2;
		VertListPosition++;
		
		// Z
		memcpy(&TempVal1, &ZMapBuffer[VertListPosition], 4);
		Vertex[CurrentVert].Z = TempVal1 << 8;
		TempVal2 = (TempVal1 >> 8 << 8);
		TempVal2 = TempVal2 >> 8;
		Vertex[CurrentVert].Z = Vertex[CurrentVert].Z + TempVal2;
		VertListPosition++;
		
		// H + V
		memcpy(&TempVal1, &ZMapBuffer[VertListPosition], 4);
		Vertex[CurrentVert].V = TempVal1 >> 16 << 8;
		Vertex[CurrentVert].V = Vertex[CurrentVert].V + (TempVal1 >> 24);
		Vertex[CurrentVert].H = TempVal1 << 8;
		TempVal2 = (TempVal1 >> 8 << 8);
		TempVal2 = TempVal2 >> 8;
		Vertex[CurrentVert].H = Vertex[CurrentVert].H + TempVal2;
		VertListPosition++;
		
		// R, G, B, A
		memcpy(&TempVal1, &ZMapBuffer[VertListPosition], 4);
		Vertex[CurrentVert].A = TempVal1 >> 24;
		Vertex[CurrentVert].B = TempVal1 >> 16;
		Vertex[CurrentVert].G = TempVal1 >> 8;
		Vertex[CurrentVert].R = TempVal1;
		VertListPosition++;
		
		CurrentVert++;
	}
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDDRAWTRI1 - F3DEX2_TRI1 - GET VERTEX COORDINATES AND COLOR FROM VERTEX STRUCT & RENDER SINGLE TRIANGLE */
int Viewer_RenderMap_CMDDrawTri1()
{
	signed int CurrentX1_1 = Vertex[Readout_CurrentByte2 / 2].X;	// triangle 1, vertex 1, X
	signed int CurrentY1_1 = Vertex[Readout_CurrentByte2 / 2].Y;	// triangle 1, vertex 1, Y
	signed int CurrentZ1_1 = Vertex[Readout_CurrentByte2 / 2].Z;	// triangle 1, vertex 1, Z
	signed int CurrentX1_2 = Vertex[Readout_CurrentByte3 / 2].X;	// triangle 1, vertex 2, X
	signed int CurrentY1_2 = Vertex[Readout_CurrentByte3 / 2].Y;	// triangle 1, vertex 2, Y
	signed int CurrentZ1_2 = Vertex[Readout_CurrentByte3 / 2].Z;	// triangle 1, vertex 2, Z
	signed int CurrentX1_3 = Vertex[Readout_CurrentByte4 / 2].X;	// triangle 1, vertex 3, X
	signed int CurrentY1_3 = Vertex[Readout_CurrentByte4 / 2].Y;	// triangle 1, vertex 3, Y
	signed int CurrentZ1_3 = Vertex[Readout_CurrentByte4 / 2].Z;	// triangle 1, vertex 3, Z
	
	GLbyte CurrentR1_1 = Vertex[Readout_CurrentByte2 / 2].R;	// triangle 1, vertex 1, R
	GLbyte CurrentG1_1 = Vertex[Readout_CurrentByte2 / 2].G;	// triangle 1, vertex 1, G
	GLbyte CurrentB1_1 = Vertex[Readout_CurrentByte2 / 2].B;	// triangle 1, vertex 1, B
	GLbyte CurrentA1_1 = Vertex[Readout_CurrentByte2 / 2].A;	// triangle 1, vertex 1, A
	GLbyte CurrentR1_2 = Vertex[Readout_CurrentByte3 / 2].R;	// triangle 1, vertex 2, R
	GLbyte CurrentG1_2 = Vertex[Readout_CurrentByte3 / 2].G;	// triangle 1, vertex 2, G
	GLbyte CurrentB1_2 = Vertex[Readout_CurrentByte3 / 2].B;	// triangle 1, vertex 2, B
	GLbyte CurrentA1_2 = Vertex[Readout_CurrentByte3 / 2].A;	// triangle 1, vertex 2, A
	GLbyte CurrentR1_3 = Vertex[Readout_CurrentByte4 / 2].R;	// triangle 1, vertex 3, R
	GLbyte CurrentG1_3 = Vertex[Readout_CurrentByte4 / 2].G;	// triangle 1, vertex 3, G
	GLbyte CurrentB1_3 = Vertex[Readout_CurrentByte4 / 2].B;	// triangle 1, vertex 3, B
	GLbyte CurrentA1_3 = Vertex[Readout_CurrentByte4 / 2].A;	// triangle 1, vertex 3, A
	
	signed short CurrentH1_1 = Vertex[Readout_CurrentByte2 / 2].H;
	signed short CurrentH1_2 = Vertex[Readout_CurrentByte3 / 2].H;
	signed short CurrentH1_3 = Vertex[Readout_CurrentByte4 / 2].H;
	signed short CurrentV1_1 = Vertex[Readout_CurrentByte2 / 2].V;
	signed short CurrentV1_2 = Vertex[Readout_CurrentByte3 / 2].V;
	signed short CurrentV1_3 = Vertex[Readout_CurrentByte4 / 2].V;
	
	float TempU = 0;
	float TempV = 0;
	
	glBegin(GL_TRIANGLES);
		glColor4f(PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);
		
		TempU = (float) CurrentH1_1 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_1 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		if((GLExtension_MultiTexture) && (Renderer_EnableMultiTexturing)) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempU, TempV);
			if(IsMultitex) glMultiTexCoord2fARB(GL_TEXTURE1_ARB, TempU / MTexScaler, TempV / MTexScaler);
		} else {
			glTexCoord2f(TempU, TempV);
		}
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_1, CurrentG1_1, CurrentB1_1, CurrentA1_1); }
		glNormal3f (CurrentR1_1 / 255.0f, CurrentG1_1 / 255.0f, CurrentB1_1 / 255.0f);
		glVertex3d(CurrentX1_1, CurrentY1_1, CurrentZ1_1);
		
		TempU = (float) CurrentH1_2 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_2 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		if((GLExtension_MultiTexture) && (Renderer_EnableMultiTexturing)) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempU, TempV);
			if(IsMultitex) glMultiTexCoord2fARB(GL_TEXTURE1_ARB, TempU / MTexScaler, TempV / MTexScaler);
		} else {
			glTexCoord2f(TempU, TempV);
		}
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_2, CurrentG1_2, CurrentB1_2, CurrentA1_2); }
		glNormal3f (CurrentR1_2 / 255.0f, CurrentG1_2 / 255.0f, CurrentB1_2 / 255.0f);
		glVertex3d(CurrentX1_2, CurrentY1_2, CurrentZ1_2);
		
		TempU = (float) CurrentH1_3 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_3 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		if((GLExtension_MultiTexture) && (Renderer_EnableMultiTexturing)) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempU, TempV);
			if(IsMultitex) glMultiTexCoord2fARB(GL_TEXTURE1_ARB, TempU / MTexScaler, TempV / MTexScaler);
		} else {
			glTexCoord2f(TempU, TempV);
		}
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_3, CurrentG1_3, CurrentB1_3, CurrentA1_3); }
		glNormal3f (CurrentR1_3 / 255.0f, CurrentG1_3 / 255.0f, CurrentB1_3 / 255.0f);
		glVertex3d(CurrentX1_3, CurrentY1_3, CurrentZ1_3);
	glEnd();
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDDRAWTRI2 - F3DEX2_TRI2 - GET VERTEX COORDINATES AND COLOR FROM VERTEX STRUCT & RENDER TWO TRIANGLES */
int Viewer_RenderMap_CMDDrawTri2()
{
	signed int CurrentX1_1 = Vertex[Readout_CurrentByte2 / 2].X;	// triangle 1, vertex 1, X
	signed int CurrentY1_1 = Vertex[Readout_CurrentByte2 / 2].Y;	// triangle 1, vertex 1, Y
	signed int CurrentZ1_1 = Vertex[Readout_CurrentByte2 / 2].Z;	// triangle 1, vertex 1, Z
	signed int CurrentX1_2 = Vertex[Readout_CurrentByte3 / 2].X;	// triangle 1, vertex 2, X
	signed int CurrentY1_2 = Vertex[Readout_CurrentByte3 / 2].Y;	// triangle 1, vertex 2, Y
	signed int CurrentZ1_2 = Vertex[Readout_CurrentByte3 / 2].Z;	// triangle 1, vertex 2, Z
	signed int CurrentX1_3 = Vertex[Readout_CurrentByte4 / 2].X;	// triangle 1, vertex 3, X
	signed int CurrentY1_3 = Vertex[Readout_CurrentByte4 / 2].Y;	// triangle 1, vertex 3, Y
	signed int CurrentZ1_3 = Vertex[Readout_CurrentByte4 / 2].Z;	// triangle 1, vertex 3, Z
	
	GLbyte CurrentR1_1 = Vertex[Readout_CurrentByte2 / 2].R;	// triangle 1, vertex 1, R
	GLbyte CurrentG1_1 = Vertex[Readout_CurrentByte2 / 2].G;	// triangle 1, vertex 1, G
	GLbyte CurrentB1_1 = Vertex[Readout_CurrentByte2 / 2].B;	// triangle 1, vertex 1, B
	GLbyte CurrentA1_1 = Vertex[Readout_CurrentByte2 / 2].A;	// triangle 1, vertex 1, A
	GLbyte CurrentR1_2 = Vertex[Readout_CurrentByte3 / 2].R;	// triangle 1, vertex 2, R
	GLbyte CurrentG1_2 = Vertex[Readout_CurrentByte3 / 2].G;	// triangle 1, vertex 2, G
	GLbyte CurrentB1_2 = Vertex[Readout_CurrentByte3 / 2].B;	// triangle 1, vertex 2, B
	GLbyte CurrentA1_2 = Vertex[Readout_CurrentByte3 / 2].A;	// triangle 1, vertex 2, A
	GLbyte CurrentR1_3 = Vertex[Readout_CurrentByte4 / 2].R;	// triangle 1, vertex 3, R
	GLbyte CurrentG1_3 = Vertex[Readout_CurrentByte4 / 2].G;	// triangle 1, vertex 3, G
	GLbyte CurrentB1_3 = Vertex[Readout_CurrentByte4 / 2].B;	// triangle 1, vertex 3, B
	GLbyte CurrentA1_3 = Vertex[Readout_CurrentByte4 / 2].A;	// triangle 1, vertex 3, A
	
	signed short CurrentH1_1 = Vertex[Readout_CurrentByte2 / 2].H;
	signed short CurrentH1_2 = Vertex[Readout_CurrentByte3 / 2].H;
	signed short CurrentH1_3 = Vertex[Readout_CurrentByte4 / 2].H;
	signed short CurrentV1_1 = Vertex[Readout_CurrentByte2 / 2].V;
	signed short CurrentV1_2 = Vertex[Readout_CurrentByte3 / 2].V;
	signed short CurrentV1_3 = Vertex[Readout_CurrentByte4 / 2].V;
	
	signed int CurrentX2_1 = Vertex[Readout_CurrentByte6 / 2].X;	// triangle 2, vertex 1, X
	signed int CurrentY2_1 = Vertex[Readout_CurrentByte6 / 2].Y;	// triangle 2, vertex 1, Y
	signed int CurrentZ2_1 = Vertex[Readout_CurrentByte6 / 2].Z;	// triangle 2, vertex 1, Z
	signed int CurrentX2_2 = Vertex[Readout_CurrentByte7 / 2].X;	// triangle 2, vertex 2, X
	signed int CurrentY2_2 = Vertex[Readout_CurrentByte7 / 2].Y;	// triangle 2, vertex 2, Y
	signed int CurrentZ2_2 = Vertex[Readout_CurrentByte7 / 2].Z;	// triangle 2, vertex 2, Z
	signed int CurrentX2_3 = Vertex[Readout_CurrentByte8 / 2].X;	// triangle 2, vertex 3, X
	signed int CurrentY2_3 = Vertex[Readout_CurrentByte8 / 2].Y;	// triangle 2, vertex 3, Y
	signed int CurrentZ2_3 = Vertex[Readout_CurrentByte8 / 2].Z;	// triangle 2, vertex 3, Z
	
	GLbyte CurrentR2_1 = Vertex[Readout_CurrentByte6 / 2].R;	// triangle 2, vertex 1, R
	GLbyte CurrentG2_1 = Vertex[Readout_CurrentByte6 / 2].G;	// triangle 2, vertex 1, G
	GLbyte CurrentB2_1 = Vertex[Readout_CurrentByte6 / 2].B;	// triangle 2, vertex 1, B
	GLbyte CurrentA2_1 = Vertex[Readout_CurrentByte6 / 2].A;	// triangle 2, vertex 1, A
	GLbyte CurrentR2_2 = Vertex[Readout_CurrentByte7 / 2].R;	// triangle 2, vertex 2, R
	GLbyte CurrentG2_2 = Vertex[Readout_CurrentByte7 / 2].G;	// triangle 2, vertex 2, G
	GLbyte CurrentB2_2 = Vertex[Readout_CurrentByte7 / 2].B;	// triangle 2, vertex 2, B
	GLbyte CurrentA2_2 = Vertex[Readout_CurrentByte7 / 2].A;	// triangle 2, vertex 2, A
	GLbyte CurrentR2_3 = Vertex[Readout_CurrentByte8 / 2].R;	// triangle 2, vertex 3, R
	GLbyte CurrentG2_3 = Vertex[Readout_CurrentByte8 / 2].G;	// triangle 2, vertex 3, G
	GLbyte CurrentB2_3 = Vertex[Readout_CurrentByte8 / 2].B;	// triangle 2, vertex 3, B
	GLbyte CurrentA2_3 = Vertex[Readout_CurrentByte8 / 2].A;	// triangle 2, vertex 3, A
	
	signed short CurrentH2_1 = Vertex[Readout_CurrentByte6 / 2].H;
	signed short CurrentH2_2 = Vertex[Readout_CurrentByte7 / 2].H;
	signed short CurrentH2_3 = Vertex[Readout_CurrentByte8 / 2].H;
	signed short CurrentV2_1 = Vertex[Readout_CurrentByte6 / 2].V;
	signed short CurrentV2_2 = Vertex[Readout_CurrentByte7 / 2].V;
	signed short CurrentV2_3 = Vertex[Readout_CurrentByte8 / 2].V;
	
	float TempU = 0;
	float TempV = 0;
	
	glBegin(GL_TRIANGLES);
		glColor4f(PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);
		
		TempU = (float) CurrentH1_1 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_1 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		if((GLExtension_MultiTexture) && (Renderer_EnableMultiTexturing)) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempU, TempV);
			if(IsMultitex) glMultiTexCoord2fARB(GL_TEXTURE1_ARB, TempU / MTexScaler, TempV / MTexScaler);
		} else {
			glTexCoord2f(TempU, TempV);
		}
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_1, CurrentG1_1, CurrentB1_1, CurrentA1_1); }
		glNormal3f (CurrentR1_1 / 255.0f, CurrentG1_1 / 255.0f, CurrentB1_1 / 255.0f);
		glVertex3d(CurrentX1_1, CurrentY1_1, CurrentZ1_1);
		
		TempU = (float) CurrentH1_2 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_2 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		if((GLExtension_MultiTexture) && (Renderer_EnableMultiTexturing)) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempU, TempV);
			if(IsMultitex) glMultiTexCoord2fARB(GL_TEXTURE1_ARB, TempU / MTexScaler, TempV / MTexScaler);
		} else {
			glTexCoord2f(TempU, TempV);
		}
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_2, CurrentG1_2, CurrentB1_2, CurrentA1_2); }
		glNormal3f (CurrentR1_2 / 255.0f, CurrentG1_2 / 255.0f, CurrentB1_2 / 255.0f);
		glVertex3d(CurrentX1_2, CurrentY1_2, CurrentZ1_2);
		
		TempU = (float) CurrentH1_3 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_3 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		if((GLExtension_MultiTexture) && (Renderer_EnableMultiTexturing)) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempU, TempV);
			if(IsMultitex) glMultiTexCoord2fARB(GL_TEXTURE1_ARB, TempU / MTexScaler, TempV / MTexScaler);
		} else {
			glTexCoord2f(TempU, TempV);
		}
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_3, CurrentG1_3, CurrentB1_3, CurrentA1_3); }
		glNormal3f (CurrentR1_3 / 255.0f, CurrentG1_3 / 255.0f, CurrentB1_3 / 255.0f);
		glVertex3d(CurrentX1_3, CurrentY1_3, CurrentZ1_3);
		
		TempU = (float) CurrentH2_1 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV2_1 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		if((GLExtension_MultiTexture) && (Renderer_EnableMultiTexturing)) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempU, TempV);
			if(IsMultitex) glMultiTexCoord2fARB(GL_TEXTURE1_ARB, TempU / MTexScaler, TempV / MTexScaler);
		} else {
			glTexCoord2f(TempU, TempV);
		}
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR2_1, CurrentG2_1, CurrentB2_1, CurrentA2_1); }
		glNormal3f (CurrentR2_1 / 255.0f, CurrentG2_1 / 255.0f, CurrentB2_1 / 255.0f);
		glVertex3d(CurrentX2_1, CurrentY2_1, CurrentZ2_1);
		
		TempU = (float) CurrentH2_2 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV2_2 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		if((GLExtension_MultiTexture) && (Renderer_EnableMultiTexturing)) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempU, TempV);
			if(IsMultitex) glMultiTexCoord2fARB(GL_TEXTURE1_ARB, TempU / MTexScaler, TempV / MTexScaler);
		} else {
			glTexCoord2f(TempU, TempV);
		}
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR2_2, CurrentG2_2, CurrentB2_2, CurrentA2_2); } 
		glNormal3f (CurrentR2_2 / 255.0f, CurrentG2_2 / 255.0f, CurrentB2_2 / 255.0f);
		glVertex3d(CurrentX2_2, CurrentY2_2, CurrentZ2_2);
		
		TempU = (float) CurrentH2_3 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV2_3 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		if((GLExtension_MultiTexture) && (Renderer_EnableMultiTexturing)) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempU, TempV);
			if(IsMultitex) glMultiTexCoord2fARB(GL_TEXTURE1_ARB, TempU / MTexScaler, TempV / MTexScaler);
		} else {
			glTexCoord2f(TempU, TempV);
		}
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR2_3, CurrentG2_3, CurrentB2_3, CurrentA2_3); }
		glNormal3f (CurrentR2_3 / 255.0f, CurrentG2_3 / 255.0f, CurrentB2_3 / 255.0f);
		glVertex3d(CurrentX2_3, CurrentY2_3, CurrentZ2_3);
	glEnd();
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDTEXTURE - F3DEX2_TEXTURE */
int Viewer_RenderMap_CMDTexture()
{
	Texture[0].S_Scale = ((Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6) + 2;
	Texture[0].T_Scale = ((Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8) + 2;
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDSETTIMAGE - G_SETTIMG - GET TEXTURE OFFSET AND STORE FOR FUTURE USE */
int Viewer_RenderMap_CMDSetTImage()
{
	switch(Readout_NextGFXCommand1) {
		case 0x000000E8:
			Texture[0].PalDataSource = Readout_CurrentByte5;
			
			Texture[0].PalOffset = Readout_CurrentByte6 << 16;
			Texture[0].PalOffset = Texture[0].PalOffset + (Readout_CurrentByte7 << 8);
			Texture[0].PalOffset = Texture[0].PalOffset + Readout_CurrentByte8;
			
			IsMultitex = false;
			break;
		case 0x000110F5:
			Texture[1].DataSource = Readout_CurrentByte5;
			
			Texture[1].Offset = Readout_CurrentByte6 << 16;
			Texture[1].Offset = Texture[1].Offset + (Readout_CurrentByte7 << 8);
			Texture[1].Offset = Texture[1].Offset + Readout_CurrentByte8;
			
			IsMultitex = true;
			break;
		default:
			Texture[0].DataSource = Readout_CurrentByte5;
			
			Texture[0].Offset = Readout_CurrentByte6 << 16;
			Texture[0].Offset = Texture[0].Offset + (Readout_CurrentByte7 << 8);
			Texture[0].Offset = Texture[0].Offset + Readout_CurrentByte8;
			
			IsMultitex = false;
			break;
	}
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDSETTILE - G_SETTILE - GET TEXTURE PROPERTIES AND STORE THEM FOR FUTURE USE (MIRROR, ETC.) */
int Viewer_RenderMap_CMDSetTile()
{
	/* FIX FOR CI TEXTURES - IF BYTES 5-8 ARE 07000000, LEAVE CURRENT PROPERTY SETTINGS ALONE */
	if(Readout_Current2 == 0x00000007) return 0;
	
	unsigned char TempXParameter = Readout_CurrentByte7 * 0x10;
	
	if(!IsMultitex) {
		switch(Readout_CurrentByte6) {
			case 0x01:
				Texture[0].Y_Parameter = 1;
				break;
			case 0x09:
				Texture[0].Y_Parameter = 2;
				break;
			case 0x05:
				Texture[0].Y_Parameter = 3;
				break;
			default:
				Texture[0].Y_Parameter = 1;
				break;
		}
		
		switch(TempXParameter) {
			case 0x00:
				Texture[0].X_Parameter = 1;
				break;
			case 0x20:
				Texture[0].X_Parameter = 2;
				break;
			case 0x10:
				Texture[0].X_Parameter = 3;
				break;
			default:
				Texture[0].X_Parameter = 1;
				break;
		}
		
		Texture[0].LineSize = Readout_CurrentByte3 / 2;
		Texture[0].Format_N64 = Readout_CurrentByte2;
		Texture[0].Palette = (Readout_Current1 >> 20) & 0x0F;
		Texture[0].Format_OGL = GL_RGBA;
		Texture[0].Format_OGLPixel = GL_RGBA;
	} else {
		switch(Readout_CurrentByte6) {
			case 0x01:
				Texture[1].Y_Parameter = 1;
				break;
			case 0x09:
				Texture[1].Y_Parameter = 2;
				break;
			case 0x05:
				Texture[1].Y_Parameter = 3;
				break;
			default:
				Texture[1].Y_Parameter = 1;
				break;
		}
		
		switch(TempXParameter) {
			case 0x00:
				Texture[1].X_Parameter = 1;
				break;
			case 0x20:
				Texture[1].X_Parameter = 2;
				break;
			case 0x10:
				Texture[1].X_Parameter = 3;
				break;
			default:
				Texture[1].X_Parameter = 1;
				break;
		}
		
		Texture[1].LineSize = Readout_CurrentByte3 / 2;
		Texture[1].Format_N64 = Readout_CurrentByte2;
		Texture[1].Palette = (Readout_Current1 >> 20) & 0x0F;
		Texture[1].Format_OGL = GL_RGBA;
		Texture[1].Format_OGLPixel = GL_RGBA;
	}
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDSETTILESIZE - G_SETTILESIZE - GET TEXTURE SIZE/COORDINATES AND STORE THEM FOR FUTURE USE */
int Viewer_RenderMap_CMDSetTileSize()
{
	unsigned int TileSize_Temp1 = (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned int TileSize_Temp2 = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
	
	unsigned int ULS = (TileSize_Temp1 & 0xFFF000) >> 14;
	unsigned int ULT = (TileSize_Temp1 & 0x000FFF) >> 2;
	unsigned int LRS = (TileSize_Temp2 & 0xFFF000) >> 14;
	unsigned int LRT = (TileSize_Temp2 & 0x000FFF) >> 2;
	
	MTexScaler = (Readout_CurrentByte5 + 1) * 2;
	
	if(!IsMultitex) {
		Texture[0].Width  = ((LRS - ULS) + 1);
		Texture[0].Height = ((LRT - ULT) + 1);
		
		if(Texture[0].Width > 256) {
			Texture[0].WidthRender = Texture[0].Width - 64;
			Texture[0].Width = (Texture[0].Width & 0xFF);				/* hack for MM commands that set insane texture coordinates */
		} else {
			Texture[0].WidthRender = Texture[0].Width;
		}
		
		if(Texture[0].Height > 256) {
			Texture[0].HeightRender = Texture[0].Height - 64;
			Texture[0].Height = (Texture[0].Height & 0xFF);				/* hack for MM commands that set insane texture coordinates */
		} else {
			Texture[0].HeightRender = Texture[0].Height;
		}
	} else {
		Texture[1].Width  = ((LRS - ULS) + 1);
		Texture[1].Height = ((LRT - ULT) + 1);
		
		if(Texture[1].Width > 256) {
			Texture[1].WidthRender = Texture[1].Width - 64;
			Texture[1].Width = (Texture[1].Width & 0xFF);				/* hack for MM commands that set insane texture coordinates */
		} else {
			Texture[1].WidthRender = Texture[1].Width;
		}
		
		if(Texture[1].Height > 256) {
			Texture[1].HeightRender = Texture[1].Height - 64;
			Texture[1].Height = (Texture[1].Height & 0xFF);				/* hack for MM commands that set insane texture coordinates */
		} else {
			Texture[1].HeightRender = Texture[1].Height;
		}
	}
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDGEOMETRYMODE - F3DEX2_GEOMETRYMODE - GET GEOMETRY PARAMETERS AND EN-/DISABLE THEIR OGL EQUIVALENTS */
int Viewer_RenderMap_CMDGeometryMode()
{
	int Convert;
	int Counter, Counter2;
	bool Binary[4];
	
	/* ---- FOG, LIGHTING, AUTOMATIC TEXTURE MAPPING (SPHERICAL + LINEAR) ---- */
	Convert = Readout_CurrentByte2;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(!Binary[0]) {
		/*disable fog*/ }
		glDisable(GL_FOG);
	if(!Binary[1]) {
		/*disable lighting*/
		glDisable(GL_LIGHTING); glDisable(GL_NORMALIZE); Renderer_EnableLighting = false; }
	if(!Binary[2]) {
		/*disable tex-mapping spherical*/ }
	if(!Binary[3]) {
		/*disable tex-mapping linear*/ }
	
	Convert = Readout_CurrentByte6;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(Binary[0]) {
		/*enable fog*/ }
//		glEnable(GL_FOG);
	if(Binary[1]) {
		/*enable lighting*/
		glEnable(GL_LIGHTING); glEnable(GL_NORMALIZE); Renderer_EnableLighting = true; }
	if(Binary[2]) {
		/*enable tex-mapping spherical*/ }
	if(Binary[3]) {
		/*disable tex-mapping linear*/ }
	
	/* ---- FACE CULLING ---- */
	Convert = Readout_CurrentByte3;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(!Binary[0]) {
		/*UNUSED?*/ }
	if(!Binary[1]) {
		/*disable front-face culling*/
		glDisable(GL_CULL_FACE); }
	if(!Binary[2]) {
		/*disable back-face culling*/
		glDisable(GL_CULL_FACE); }
	if(!Binary[3]) {
		/*UNUSED?*/ }
	
	Convert = Readout_CurrentByte7;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(Binary[0]) {
		/*UNUSED?*/ }
	if(Binary[1]) {
		/*enable front-face culling*/
		glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); }
	if(Binary[2]) {
		/*enable back-face culling*/
		glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }
	if(Binary[3]) {
		/*UNUSED?*/ }
	
	/* ---- VERTEX COLOR SHADING, Z-BUFFERING ---- */
	Convert = Readout_CurrentByte4;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(!Binary[0]) {
		/*UNUSED?*/ }
	if(!Binary[1]) {
		/*disable vertex color shading*/ }
	if(!Binary[2]) {
		/*disable z-buffering*/
		glDisable(GL_DEPTH_TEST); }
	if(!Binary[3]) {
		/*UNUSED?*/ }
	
	Convert = Readout_CurrentByte8;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(Binary[0]) {
		/*UNUSED?*/ }
	if(Binary[1]) {
		/*enable vertex color shading*/ }
	if(Binary[2]) {
		/*enable z-buffering*/
		glEnable(GL_DEPTH_TEST); }
	if(Binary[3]) {
		/*UNUSED?*/ }
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDSETFOGCOLOR - G_SETFOGCOLOR - SET THE FOG COLOR */
int Viewer_RenderMap_CMDSetFogColor()
{
	FogColor[0] = (Readout_CurrentByte5 / 255.0f);
	FogColor[1] = (Readout_CurrentByte6 / 255.0f);
	FogColor[2] = (Readout_CurrentByte7 / 255.0f);
	FogColor[3] = (Readout_CurrentByte8 / 255.0f);
	
	glFogfv(GL_FOG_COLOR, FogColor);
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDSETPRIMCOLOR - G_SETPRIMCOLOR - SET THE PRIMARY COLOR */
int Viewer_RenderMap_CMDSetPrimColor()
{
	PrimColor[0] = (Readout_CurrentByte5 / 255.0f);
	PrimColor[1] = (Readout_CurrentByte6 / 255.0f);
	PrimColor[2] = (Readout_CurrentByte7 / 255.0f);
	PrimColor[3] = (Readout_CurrentByte8 / 255.0f);
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDLOADTLUT - G_LOADTLUT - GET PALETTE FOR CI TEXTURES FROM PREVIOUS TEXTURE OFFSET */
int Viewer_RenderMap_CMDLoadTLUT(unsigned int PaletteSrc, unsigned long PaletteOffset)
{
	/* clear the palette buffer */
	memset(PaletteData, 0x00, sizeof(PaletteData));
	
	/* calculate palette size, 16 or 256, from parameters */
	unsigned int TempPaletteSize = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
	unsigned int PaletteSize = ((TempPaletteSize & 0xFFF000) >> 14) + 1;
	
	/* copy raw palette into buffer */
	switch(PaletteSrc) {
	case 0x02:
		/* palette comes from scene file */
		memcpy(PaletteData, &ZSceneBuffer[PaletteOffset / 4], PaletteSize * 2);
		break;
	case 0x03:
		/* palette comes from map file */
		memcpy(PaletteData, &ZMapBuffer[PaletteOffset / 4], PaletteSize * 2);
		break;
	case 0x04:
		/* palette comes from gameplay_keep */
		memcpy(PaletteData, &GameplayKeepBuffer[PaletteOffset / 4], PaletteSize * 2);
		break;
	case 0x05:
		/* palette comes from gameplay_dangeon_keep */
		/* (gameplay_field_keep is not handled yet, always loading in file specified in Filename_GameplayFDKeep */
		memcpy(PaletteData, &GameplayFDKeepBuffer[PaletteOffset / 4], PaletteSize * 2);
		break;
	default:
		/* fallback if source is not handled / no valid source was found */
		memset(PaletteData, 0xFF, PaletteSize * 2);
		break;
	}
	
	/* initialize variables for palette conversion */
	unsigned int CurrentPaletteEntry = 0;
	unsigned int SourcePaletteData = 0;
	unsigned int InPalettePosition = 0;
	
	unsigned int PalLoop = 0;
	
	unsigned int RExtract = 0;
	unsigned int GExtract = 0;
	unsigned int BExtract = 0;
	unsigned int AExtract = 0;
	
	for(PalLoop = 0; PalLoop < PaletteSize; PalLoop++) {
		/* get raw rgba5551 data */
		SourcePaletteData = (PaletteData[InPalettePosition] * 0x100) + PaletteData[InPalettePosition + 1];
		
		/* extract r, g, b and a elements */
		RExtract = (SourcePaletteData & 0xF800);
		RExtract >>= 8;
		GExtract = (SourcePaletteData & 0x07C0);
		GExtract <<= 5;
		GExtract >>= 8;
		BExtract = (SourcePaletteData & 0x003E);
		BExtract <<= 18;
		BExtract >>= 16;
		
		if((SourcePaletteData & 0x0001)) { AExtract = 0xFF; } else { AExtract = 0x00; }
		
		/* set the current ci palette index to the rgba values from above */
		Palette[CurrentPaletteEntry].R = RExtract;
		Palette[CurrentPaletteEntry].G = GExtract;
		Palette[CurrentPaletteEntry].B = BExtract;
		Palette[CurrentPaletteEntry].A = AExtract;
		
		/* go on */
		CurrentPaletteEntry++;
		InPalettePosition += 2;
	}
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDRDPHALF1 - F3DEX2_RDPHALF_1 - CALL AND RENDER ADDITIONAL DISPLAY LISTS FROM INSIDE OTHERS */
int Viewer_RenderMap_CMDRDPHalf1()
{
	if(Readout_CurrentByte5 == 0x03) {
		unsigned long TempOffset;
		
		TempOffset = Readout_CurrentByte6 << 16;
		TempOffset = TempOffset + (Readout_CurrentByte7 << 8);
		TempOffset = TempOffset + Readout_CurrentByte8;
		
		SubDLCall = true;
		Viewer_RenderMap_DListParser(true, 0, TempOffset / 4);
	}
	
	return 0;
}

int Viewer_RenderMap_CMDSetOtherModeL()
{
	Blender_Cycle1 =	Readout_CurrentByte5 * 0x1000000;
	Blender_Cycle1 +=	Readout_CurrentByte6 * 0x10000;
	Blender_Cycle1 >>=	16;
	
	Blender_Cycle2 =	Readout_CurrentByte7 * 0x100;
	Blender_Cycle2 +=	Readout_CurrentByte8;
	
	GLenum Blender_SrcFactor =		GL_SRC_ALPHA;
	GLenum Blender_DstFactor =		GL_ONE_MINUS_SRC_ALPHA;
	GLenum Blender_AlphaFunc =		GL_GEQUAL;
	GLclampf Blender_AlphaRef =		0.5f;
	
	switch (Blender_Cycle1 + Blender_Cycle2) {
		case 0xC811 + 0x2078:								//no blending
		case 0xC811 + 0x3078:
		case 0x0C19 + 0x2078:
		case 0x0C19 + 0x3078:
			Blender_SrcFactor = GL_ONE;
			Blender_DstFactor = GL_ZERO;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.5f;
			break;
			
		case 0xC810 + 0x3478:								//syotes2 - everything
			Blender_SrcFactor = GL_ONE;
			Blender_DstFactor = GL_ZERO;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.5f;
			break;
		case 0xC810 + 0x49D8:								//spot00 + most other maps - water (does not always work)
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GREATER;
			Blender_AlphaRef = 0.0f;
			break;
		case 0x0C18 + 0x49D8:								//spot03 - water at waterfall
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_DST_COLOR;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.5f;
			break;
		case 0xC810 + 0x4A50:								//MAJORA north clocktown etc - misc things (nct: greenery on walls)
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GREATER;
			Blender_AlphaRef = 0.0f;
			break;
		case 0xC810 + 0x4B50:								//spot00 - death mountain plane, spot04 - drawing at link's house
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.4f;
			break;
		case 0xC810 + 0x4DD8:								//spot00 - (used near path to gerudo valley?)
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GREATER;
			Blender_AlphaRef = 0.0f;
			break;
		case 0x0C18 + 0x4DD8:								//spot11 - around oasis
			Blender_SrcFactor = GL_ONE;
			Blender_DstFactor = GL_ZERO;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.5f;
			break;
		case 0xC810 + 0x4F50:								//spot00, spot02, spot04 - pathways
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_NOTEQUAL;
			Blender_AlphaRef = 0.0f;
			break;
		case 0xC811 + 0x2D58:								//spot01 - doorways
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GREATER;
			Blender_AlphaRef = 0.0f;
			break;
		default:
			sprintf(ErrorMsg, "Unsupported mode 0x%04X 0x%04X.\nUsing default settings...", (unsigned int)Blender_Cycle1, (unsigned int)Blender_Cycle2);
			MessageBox(hwnd, ErrorMsg, "SetOtherMode_L Error", MB_OK | MB_ICONERROR);
			break;
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(Blender_SrcFactor, Blender_DstFactor);
	glAlphaFunc(Blender_AlphaFunc, Blender_AlphaRef);
	
	return 0;
}

int Viewer_RenderMap_CMDSetCombine()
{
	Combiner_Cycle1 =	Readout_CurrentByte5 * 0x1000000;
	Combiner_Cycle1 +=	Readout_CurrentByte6 * 0x10000;
	Combiner_Cycle1 >>=	16;
	
	Combiner_Cycle2 =	Readout_CurrentByte7 * 0x100;
	Combiner_Cycle2 +=	Readout_CurrentByte8;
	
	switch (Combiner_Cycle1 + Combiner_Cycle2) {
		default:
//			sprintf(ErrorMsg, "Unsupported mode 0x%04X 0x%04X.\nUsing default settings...", (unsigned int)Combiner_Cycle1, (unsigned int)Combiner_Cycle2);
//			MessageBox(hwnd, ErrorMsg, "SetCombine Error", MB_OK | MB_ICONERROR);
			
/*			glActiveTextureARB(GL_TEXTURE1_ARB);
			Renderer_GLTexture = Viewer_LoadTexture(0);
			glEnable(GL_TEXTURE_2D);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			glBindTexture(GL_TEXTURE_2D, Renderer_GLTexture);
*/
			Combiner_Color_CmbMode[0] = GL_MODULATE;
			Combiner_Color_Arg0_Source[0] = GL_TEXTURE0;
			Combiner_Color_Arg0_Op[0] = GL_SRC_COLOR;
			Combiner_Color_Arg1_Source[0] = GL_PRIMARY_COLOR;
			Combiner_Color_Arg1_Op[0] = GL_SRC_COLOR;
			
			Combiner_Alpha_CmbMode[0] = GL_MODULATE;
			Combiner_Alpha_Arg0_Source[0] = GL_TEXTURE0;
			Combiner_Alpha_Arg0_Op[0] = GL_SRC_ALPHA;
			Combiner_Alpha_Arg1_Source[0] = GL_PRIMARY_COLOR;
			Combiner_Alpha_Arg1_Op[0] = GL_SRC_ALPHA;
			
			Combiner_Color_CmbMode[1] = GL_MODULATE;
			Combiner_Color_Arg0_Source[1] = GL_PREVIOUS;
			Combiner_Color_Arg0_Op[1] = GL_ONE;
			Combiner_Color_Arg1_Source[1] = GL_TEXTURE1;
			Combiner_Color_Arg1_Op[1] = GL_SRC_COLOR;
			Combiner_Color_Arg2_Source[1] = GL_CONSTANT;
			Combiner_Color_Arg2_Op[1] = GL_SRC_COLOR;
			
			Combiner_Alpha_CmbMode[1] = GL_ADD_SIGNED;
			Combiner_Alpha_Arg0_Source[1] = GL_PREVIOUS;
			Combiner_Alpha_Arg0_Op[1] = GL_SRC_ALPHA;
			Combiner_Alpha_Arg1_Source[1] = GL_TEXTURE1;
			Combiner_Alpha_Arg1_Op[1] = GL_SRC_ALPHA;
			Combiner_Alpha_Arg2_Source[1] = GL_CONSTANT;
			Combiner_Alpha_Arg2_Op[1] = GL_SRC_ALPHA;
			
			break;
	}
	
	return 0;
}

/*	------------------------------------------------------------ */

/* VIEWER_LOADTEXTURE - FETCH THE TEXTURE DATA AND CREATE AN OGL TEXTURE */
GLuint Viewer_LoadTexture(int TextureID)
{
	GLuint GLTexture;
	
	if((Readout_NextGFXCommand1 == 0x00000003) || (Readout_NextGFXCommand1 == 0x000000E1)) return 0;
	
	int i, j = 0;
	
	unsigned long TextureBufferSize = 0x8000;
	
	bool UnhandledTextureSource = false;
	
	TextureData_OGL = (unsigned char *) malloc (TextureBufferSize);
	TextureData_N64 = (unsigned char *) malloc (TextureBufferSize);
	
	memset(TextureData_OGL, 0x00, TextureBufferSize);
	memset(TextureData_N64, 0x00, TextureBufferSize);
	
	/* create solid red texture for unsupported stuff, such as kakariko windows */
	unsigned char * EmptyTexture_Red;
	unsigned char * EmptyTexture_Green;
	EmptyTexture_Red = (unsigned char *) malloc (TextureBufferSize);
	EmptyTexture_Green = (unsigned char *) malloc (TextureBufferSize);
	for(i = 0; i < TextureBufferSize; i+=4) {
		EmptyTexture_Red[i]			= 0xFF;
		EmptyTexture_Red[i + 1]		= 0x00;
		EmptyTexture_Red[i + 2]		= 0x00;
		EmptyTexture_Red[i + 3]		= 0xFF;
		
		EmptyTexture_Green[i]		= 0x00;
		EmptyTexture_Green[i + 1]	= 0xFF;
		EmptyTexture_Green[i + 2]	= 0x00;
		EmptyTexture_Green[i + 3]	= 0xFF;
	}
	
	switch(Texture[TextureID].DataSource) {
	case 0x02:
		memcpy(TextureData_N64, &ZSceneBuffer[Texture[TextureID].Offset / 4], TextureBufferSize);
		break;
	case 0x03:
		memcpy(TextureData_N64, &ZMapBuffer[Texture[TextureID].Offset / 4], TextureBufferSize);
		break;
	case 0x04:
		memcpy(TextureData_N64, &GameplayKeepBuffer[Texture[TextureID].Offset / 4], TextureBufferSize);
		break;
	case 0x05:
		memcpy(TextureData_N64, &GameplayFDKeepBuffer[Texture[TextureID].Offset / 4], TextureBufferSize);
		break;
	default:
		UnhandledTextureSource = true;
		sprintf(ErrorMsg, "Unhandled Source Bank 0x%02X!", Texture[TextureID].DataSource);
		MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
		Texture[TextureID].Format_OGL = GL_RGBA;
		Texture[TextureID].Format_OGLPixel = GL_RGBA;
		memcpy(TextureData_OGL, EmptyTexture_Red, TextureBufferSize);
		break;
	}
	
	if(!UnhandledTextureSource) {
		switch(Texture[TextureID].Format_N64) {
		/* RGBA FORMAT */
		case 0x00:
		case 0x08:
		case 0x10:
			{
			unsigned int LoadRGBA_RGBA5551 = 0;
			
			unsigned int LoadRGBA_RExtract = 0;
			unsigned int LoadRGBA_GExtract = 0;
			unsigned int LoadRGBA_BExtract = 0;
			unsigned int LoadRGBA_AExtract = 0;
			
			unsigned int LoadRGBA_InTexturePosition_N64 = 0;
			unsigned int LoadRGBA_InTexturePosition_OGL = 0;
			
			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width; i++) {
					LoadRGBA_RGBA5551 = (TextureData_N64[LoadRGBA_InTexturePosition_N64] * 0x100) + TextureData_N64[LoadRGBA_InTexturePosition_N64 + 1];
					
					LoadRGBA_RExtract = (LoadRGBA_RGBA5551 & 0xF800);
					LoadRGBA_RExtract >>= 8;
					LoadRGBA_GExtract = (LoadRGBA_RGBA5551 & 0x07C0);
					LoadRGBA_GExtract <<= 5;
					LoadRGBA_GExtract >>= 8;
					LoadRGBA_BExtract = (LoadRGBA_RGBA5551 & 0x003E);
					LoadRGBA_BExtract <<= 18;
					LoadRGBA_BExtract >>= 16;
					
					if((LoadRGBA_RGBA5551 & 0x0001)) {
						LoadRGBA_AExtract = 0xFF;
					} else {
						LoadRGBA_AExtract = 0x00;
					}
					
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL]     = LoadRGBA_RExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 1] = LoadRGBA_GExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 2] = LoadRGBA_BExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 3] = LoadRGBA_AExtract;
					
					LoadRGBA_InTexturePosition_N64 += 2;
					LoadRGBA_InTexturePosition_OGL += 4;
				}
				LoadRGBA_InTexturePosition_N64 += Texture[TextureID].LineSize * 4 - Texture[TextureID].Width;
			}
			
			break;
			}
		/* CI FORMAT */
		case 0x40:
		case 0x50:
			{
			unsigned int LoadCI_CIData1 = 0;
			unsigned int LoadCI_CIData2 = 0;
			
			unsigned int LoadCI_InTexturePosition_N64 = 0;
			unsigned int LoadCI_InTexturePosition_OGL = 0;
			
			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width / 2; i++) {
					LoadCI_CIData1 = (TextureData_N64[LoadCI_InTexturePosition_N64] & 0xF0) >> 4;
					LoadCI_CIData2 = TextureData_N64[LoadCI_InTexturePosition_N64] & 0x0F;
					
					TextureData_OGL[LoadCI_InTexturePosition_OGL]     = Palette[LoadCI_CIData1].R;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 1] = Palette[LoadCI_CIData1].G;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 2] = Palette[LoadCI_CIData1].B;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 3] = Palette[LoadCI_CIData1].A;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 4] = Palette[LoadCI_CIData2].R;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 5] = Palette[LoadCI_CIData2].G;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 6] = Palette[LoadCI_CIData2].B;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 7] = Palette[LoadCI_CIData2].A;
					
					LoadCI_InTexturePosition_N64 += 1;
					LoadCI_InTexturePosition_OGL += 8;
				}
				LoadCI_InTexturePosition_N64 += Texture[TextureID].LineSize * 8 - (Texture[TextureID].Width / 2);
			}
			
			break;
			}
		case 0x48:
			{
			unsigned int LoadCI_CIData = 0;
			
			unsigned int LoadCI_InTexturePosition_N64 = 0;
			unsigned int LoadCI_InTexturePosition_OGL = 0;
			
			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width; i++) {
					LoadCI_CIData = TextureData_N64[LoadCI_InTexturePosition_N64];
					
					TextureData_OGL[LoadCI_InTexturePosition_OGL]     = Palette[LoadCI_CIData].R;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 1] = Palette[LoadCI_CIData].G;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 2] = Palette[LoadCI_CIData].B;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 3] = Palette[LoadCI_CIData].A;
					
					LoadCI_InTexturePosition_N64 += 1;
					LoadCI_InTexturePosition_OGL += 4;
				}
				LoadCI_InTexturePosition_N64 += Texture[TextureID].LineSize * 8 - Texture[TextureID].Width;
			}
			
			break;
			}
		/* IA FORMAT */
		case 0x60:
			{
			unsigned int LoadIA_IAData = 0;
			
			unsigned int LoadIA_IExtract1 = 0;
			unsigned int LoadIA_AExtract1 = 0;
			unsigned int LoadIA_IExtract2 = 0;
			unsigned int LoadIA_AExtract2 = 0;
			
			unsigned int LoadIA_InTexturePosition_N64 = 0;
			unsigned int LoadIA_InTexturePosition_OGL = 0;
			
			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width / 2; i++) {
					LoadIA_IAData = (TextureData_N64[LoadIA_InTexturePosition_N64] & 0xF0) >> 4;
					LoadIA_IExtract1 = (LoadIA_IAData & 0x0E) << 4;
					if((LoadIA_IAData & 0x01)) { LoadIA_AExtract1 = 0xFF; } else { LoadIA_AExtract1 = 0x00; }
					
					LoadIA_IAData = (TextureData_N64[LoadIA_InTexturePosition_N64] & 0x0F);
					LoadIA_IExtract2 = (LoadIA_IAData & 0x0E) << 4;
					if((LoadIA_IAData & 0x01)) { LoadIA_AExtract2 = 0xFF; } else { LoadIA_AExtract2 = 0x00; }
					
					TextureData_OGL[LoadIA_InTexturePosition_OGL]     = LoadIA_IExtract1;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 1] = LoadIA_IExtract1;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 2] = LoadIA_IExtract1;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 3] = LoadIA_AExtract1;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 4] = LoadIA_IExtract2;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 5] = LoadIA_IExtract2;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 6] = LoadIA_IExtract2;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 7] = LoadIA_AExtract2;
					
					LoadIA_InTexturePosition_N64 += 1;
					LoadIA_InTexturePosition_OGL += 8;
				}
				LoadIA_InTexturePosition_N64 += Texture[TextureID].LineSize * 8 - (Texture[TextureID].Width / 2);
			}
			
			break;
			}
		case 0x68:
			{
			unsigned int LoadIA_IAData = 0;
			
			unsigned int LoadIA_IExtract = 0;
			unsigned int LoadIA_AExtract = 0;
			
			unsigned int LoadIA_InTexturePosition_N64 = 0;
			unsigned int LoadIA_InTexturePosition_OGL = 0;
			
			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width; i++) {
					LoadIA_IAData = TextureData_N64[LoadIA_InTexturePosition_N64];
					LoadIA_IExtract = (LoadIA_IAData & 0xFE);
					if((LoadIA_IAData & 0x01)) { LoadIA_AExtract = 0xFF; } else { LoadIA_AExtract = 0x00; }
					
					TextureData_OGL[LoadIA_InTexturePosition_OGL]     = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 1] = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 2] = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 3] = LoadIA_AExtract;
					
					LoadIA_InTexturePosition_N64 += 1;
					LoadIA_InTexturePosition_OGL += 4;
				}
				LoadIA_InTexturePosition_N64 += Texture[TextureID].LineSize * 8 - Texture[TextureID].Width;
			}
			
			break;
			}
		case 0x70:
			{
			unsigned int LoadIA_IAData = 0;
			
			unsigned int LoadIA_IExtract = 0;
			unsigned int LoadIA_AExtract = 0;
			
			unsigned int LoadIA_InTexturePosition_N64 = 0;
			unsigned int LoadIA_InTexturePosition_OGL = 0;
			
			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width; i++) {
					LoadIA_IAData = TextureData_N64[LoadIA_InTexturePosition_N64];
					LoadIA_IExtract = LoadIA_IAData;
					
					LoadIA_IAData = TextureData_N64[LoadIA_InTexturePosition_N64 + 1];
					LoadIA_AExtract = LoadIA_IAData;
					
					TextureData_OGL[LoadIA_InTexturePosition_OGL]     = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 1] = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 2] = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 3] = LoadIA_AExtract;
					
					LoadIA_InTexturePosition_N64 += 2;
					LoadIA_InTexturePosition_OGL += 4;
				}
				LoadIA_InTexturePosition_N64 += Texture[TextureID].LineSize * 4 - Texture[TextureID].Width;
			}
			
			break;
			}
		/* I FORMAT */
		case 0x80:
		case 0x90:
			{
			/* 4bit - spot00 pathways, castle town walls, treeline near kokiri entrance, besitu blue patterned walls */
			unsigned int LoadI_IData = 0;
			
			unsigned int LoadI_IExtract1 = 0;
			unsigned int LoadI_IExtract2 = 0;
			
			unsigned int LoadI_InTexturePosition_N64 = 0;
			unsigned int LoadI_InTexturePosition_OGL = 0;
			
			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width / 2; i++) {
					LoadI_IData = (TextureData_N64[LoadI_InTexturePosition_N64] & 0xF0) >> 4;
					LoadI_IExtract1 = (LoadI_IData & 0x0F) << 4;
					
					LoadI_IData = (TextureData_N64[LoadI_InTexturePosition_N64] & 0x0F);
					LoadI_IExtract2 = (LoadI_IData & 0x0F) << 4;
					
					TextureData_OGL[LoadI_InTexturePosition_OGL]     = LoadI_IExtract1;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 1] = LoadI_IExtract1;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 2] = LoadI_IExtract1;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 3] = (PrimColor[3] * 255);
					
					TextureData_OGL[LoadI_InTexturePosition_OGL + 4] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 5] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 6] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 7] = (PrimColor[3] * 255);
					
					LoadI_InTexturePosition_N64 += 1;
					LoadI_InTexturePosition_OGL += 8;
				}
				LoadI_InTexturePosition_N64 += Texture[TextureID].LineSize * 8 - (Texture[TextureID].Width / 2);
			}
			
			break;
			}
		case 0x88:
			{
			/* 8bit - Bmori_0 l/r side walls */
			unsigned int LoadI_IData = 0;
			
			unsigned int LoadI_InTexturePosition_N64 = 0;
			unsigned int LoadI_InTexturePosition_OGL = 0;
			
			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width; i++) {
					LoadI_IData = TextureData_N64[LoadI_InTexturePosition_N64];
					
					TextureData_OGL[LoadI_InTexturePosition_OGL]     = LoadI_IData;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 1] = LoadI_IData;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 2] = LoadI_IData;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 3] = (PrimColor[3] * 255);
					
					LoadI_InTexturePosition_N64 += 1;
					LoadI_InTexturePosition_OGL += 4;
				}
				LoadI_InTexturePosition_N64 += Texture[TextureID].LineSize * 8 - Texture[TextureID].Width;
			}
			
			break;
			}
		/* FALLBACK - gives us an empty texture */
		default:
			{
			sprintf(ErrorMsg, "Unhandled Texture Type 0x%02X!", Texture[TextureID].Format_N64);
			MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
			Texture[TextureID].Format_OGL = GL_RGBA;
			Texture[TextureID].Format_OGLPixel = GL_RGBA;
			memcpy(TextureData_OGL, EmptyTexture_Green, TextureBufferSize);
			break;
			}
		}
	}
	
	glGenTextures(1, &GLTexture);
	glBindTexture(GL_TEXTURE_2D, GLTexture);
	
	switch(Texture[TextureID].Y_Parameter) {
		case 1:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); break;
		case 2:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); break;
		case 3:  if(GLExtension_TextureMirror) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB); break;
		default: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); break;
	}
	
	switch(Texture[TextureID].X_Parameter) {
		case 1:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); break;
		case 2:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); break;
		case 3:  if(GLExtension_TextureMirror) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB); break;
		default: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); break;
	}
	
	if(GLExtension_AnisoFilter) {
		float AnisoMax;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &AnisoMax);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, AnisoMax);
	} else {
		//
	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Renderer_FilteringMode_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Renderer_FilteringMode_Mag);
	
	gluBuild2DMipmaps(GL_TEXTURE_2D, Texture[TextureID].Format_OGL, Texture[TextureID].WidthRender, Texture[TextureID].HeightRender, Texture[TextureID].Format_OGLPixel, GL_UNSIGNED_BYTE, TextureData_OGL);
	
	free(TextureData_N64);
	free(TextureData_OGL);
	
	free(EmptyTexture_Red);
	free(EmptyTexture_Green);
	
	return GLTexture;
}

/*	------------------------------------------------------------ */

/* VIEWER_RENDERALLACTORS - RENDERS THE CURRENT MAP'S MAP AND/OR SCENE ACTORS */
int Viewer_RenderAllActors()
{
	if(Renderer_EnableMapActors) {
		if (MapHeader[MapHeader_Current].Actor_Count > 0) {
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_FOG);
			
			while (!(ActorInfo_CurrentCount == MapHeader[MapHeader_Current].Actor_Count)) {
				glEnable(GL_LIGHT1);
				glDisable(GL_LIGHTING);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				Viewer_RenderActorCube(ActorInfo_CurrentCount, Actors[ActorInfo_CurrentCount].X_Position, Actors[ActorInfo_CurrentCount].Y_Position, Actors[ActorInfo_CurrentCount].Z_Position, Actors[ActorInfo_CurrentCount].X_Rotation, Actors[ActorInfo_CurrentCount].Y_Rotation, Actors[ActorInfo_CurrentCount].Z_Rotation, true);
				glEnable(GL_LIGHTING);
				glDisable(GL_LIGHT1);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glEnable(GL_POLYGON_OFFSET_LINE);
				glPolygonOffset(-1.0f,-1.0f);
				glColor3f(1.0f, 1.0f, 1.0f);
				Viewer_RenderActorCube(ActorInfo_CurrentCount, Actors[ActorInfo_CurrentCount].X_Position, Actors[ActorInfo_CurrentCount].Y_Position, Actors[ActorInfo_CurrentCount].Z_Position, Actors[ActorInfo_CurrentCount].X_Rotation, Actors[ActorInfo_CurrentCount].Y_Rotation, Actors[ActorInfo_CurrentCount].Z_Rotation ,true);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_LIGHT1);
				glDisable(GL_LIGHTING);
				ActorInfo_CurrentCount++;
			}
		}
	}
	
	if(Renderer_EnableSceneActors) {
		if (SceneHeader[SceneHeader_Current].ScActor_Count > 0) {
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_FOG);
			
			while (!(ScActorInfo_CurrentCount == SceneHeader[SceneHeader_Current].ScActor_Count)) {
				glEnable(GL_LIGHT1);
				glDisable(GL_LIGHTING);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				Viewer_RenderActorCube(ScActorInfo_CurrentCount, ScActors[ScActorInfo_CurrentCount].X_Position, ScActors[ScActorInfo_CurrentCount].Y_Position, ScActors[ScActorInfo_CurrentCount].Z_Position, ScActors[ScActorInfo_CurrentCount].X_Rotation, ScActors[ScActorInfo_CurrentCount].Y_Rotation, ScActors[ScActorInfo_CurrentCount].Z_Rotation, false);
				glEnable(GL_LIGHTING);
				glDisable(GL_LIGHT1);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glEnable(GL_POLYGON_OFFSET_LINE);
				glPolygonOffset(-1.0f,-1.0f);
				glColor3f(1.0f, 1.0f, 1.0f);
				Viewer_RenderActorCube(ScActorInfo_CurrentCount, ScActors[ScActorInfo_CurrentCount].X_Position, ScActors[ScActorInfo_CurrentCount].Y_Position, ScActors[ScActorInfo_CurrentCount].Z_Position, ScActors[ScActorInfo_CurrentCount].X_Rotation, ScActors[ScActorInfo_CurrentCount].Y_Rotation, ScActors[ScActorInfo_CurrentCount].Z_Rotation, false);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_LIGHT1);
				glDisable(GL_LIGHTING);
				ScActorInfo_CurrentCount++;
			}
		}
	}
	
	return 0;
}

/* VIEWER_RENDERACTORCUBE - RENDERS AN INFAMOUS 'ACTOR CUBE' AT THE SPECIFIED LOCATION ON THE MAP */
int Viewer_RenderActorCube(int ActorToRender, GLshort X, GLshort Y, GLshort Z, signed int X_Rot, signed int Y_Rot, signed int Z_Rot, bool IsMapActor)
{
	glPushMatrix();
	
	glTranslated(X, Y, Z);
	glRotatef((X_Rot / 180), 1, 0, 0);
	glRotatef((Y_Rot / 180), 0, 1, 0);
	glRotatef((Z_Rot / 180), 0, 0, 1);
	
	glBegin(GL_QUADS);
		/* CHECK IF ACTOR IS MAP ACTOR OR SCENE ACTOR */
		if(IsMapActor) {
			if((ActorToRender == ActorInfo_Selected)) {
				glColor3f(1.0f, 0.0f, 0.0f);
			} else {
				glColor3f(0.0f, 1.0f, 0.0f);
			}
		} else {
			if((ActorToRender == ScActorInfo_Selected)) {
				glColor3f(1.0f, 1.0f, 0.0f);
			} else {
				glColor3f(0.0f, 0.0f, 1.0f);
			}
		}
		
		glVertex3s( 12, 12, 12);   //V2
		glVertex3s( 12,-12, 12);   //V1
		glVertex3s( 12,-12,-12);   //V3
		glVertex3s( 12, 12,-12);   //V4
		
		glVertex3s( 12, 12,-12);   //V4
		glVertex3s( 12,-12,-12);   //V3
		glVertex3s(-12,-12,-12);   //V5
		glVertex3s(-12, 12,-12);   //V6
		
		glVertex3s(-12, 12,-12);   //V6
		glVertex3s(-12,-12,-12);   //V5
		glVertex3s(-12,-12, 12);   //V7
		glVertex3s(-12, 12, 12);   //V8
		
		glVertex3s(-12, 12,-12);   //V6
		glVertex3s(-12, 12, 12);   //V8
		glVertex3s( 12, 12, 12);   //V2
		glVertex3s( 12, 12,-12);   //V4
		
		glVertex3s(-12,-12, 12);   //V7
		glVertex3s(-12,-12,-12);   //V5
		glVertex3s( 12,-12,-12);   //V3
		glVertex3s( 12,-12, 12);   //V1
		
		//front
		glColor3f(1.0f, 1.0f, 1.0f);
		
		glVertex3s(-12, 12, 12);   //V8
		glVertex3s(-12,-12, 12);   //V7
		glVertex3s( 12,-12, 12);   //V1
		glVertex3s( 12, 12, 12);   //V2
	glEnd();
	
	glPopMatrix();
	
	return 0;
}

/*	------------------------------------------------------------ */

/* HELPERFUNC_SPLITCURRENTVALS - TAKES THE 4 BYTES OF THE LONG VARIABLE(S) USED FOR DATA FETCHING AND SPLITS THEM APART */
void HelperFunc_SplitCurrentVals(bool SplitDual)
{
	Readout_CurrentByte1 = Readout_Current1 << 24;
	Readout_CurrentByte1 = Readout_CurrentByte1 >> 24;
	Readout_CurrentByte2 = Readout_Current1 << 16;
	Readout_CurrentByte2 = Readout_CurrentByte2 >> 24;
	Readout_CurrentByte3 = Readout_Current1 << 8;
	Readout_CurrentByte3 = Readout_CurrentByte3 >> 24;
	Readout_CurrentByte4 = Readout_Current1;
	Readout_CurrentByte4 = Readout_CurrentByte4 >> 24;
	
	if(SplitDual) {
		Readout_CurrentByte5 = Readout_Current2 << 24;
		Readout_CurrentByte5 = Readout_CurrentByte5 >> 24;
		Readout_CurrentByte6 = Readout_Current2 << 16;
		Readout_CurrentByte6 = Readout_CurrentByte6 >> 24;
		Readout_CurrentByte7 = Readout_Current2 << 8;
		Readout_CurrentByte7 = Readout_CurrentByte7 >> 24;
		Readout_CurrentByte8 = Readout_Current2;
		Readout_CurrentByte8 = Readout_CurrentByte8 >> 24;
	}
}

/* HELPERFUNC_LOGMESSAGE - WRITES GIVEN STRING INTO PREVIOUSLY OPENED LOG FILE */
int HelperFunc_LogMessage(int LogType, char Message[])
{
	switch(LogType) {
	case 1:
		fprintf(FileGFXLog, Message);
		break;
	case 2:
		fprintf(FileSystemLog, Message);
		break;
	}
	return 0;
}

/* HELPERFUNC_GFXLOGCOMMAND - WRITES INFORMATION ABOUT CURRENTLY PROCESSED COMMAND INTO LOG FILE */
int HelperFunc_GFXLogCommand(unsigned int Position)
{				
	sprintf(GFXLogMsg, "  0x%08X:\t%s\t\t[%02X%02X%02X%02X %02X%02X%02X%02X] %s\n",
		Position * 4, CurrentGFXCmd,
		Readout_CurrentByte1, Readout_CurrentByte2, Readout_CurrentByte3, Readout_CurrentByte4,
		Readout_CurrentByte5, Readout_CurrentByte6, Readout_CurrentByte7, Readout_CurrentByte8,
		CurrentGFXCmdNote);
	HelperFunc_LogMessage(1, GFXLogMsg);
	
	return 0;
}

/* HELPERFUNC_CALCULATEFPS - CALCULATE THE VIEWER'S CURRENT FPS */
int HelperFunc_CalculateFPS()
{
	if(GetTickCount() - Renderer_LastFPS >= 1000)
	{
		Renderer_LastFPS = GetTickCount();
		Renderer_FPS = Renderer_FrameNo;
		Renderer_FrameNo = 0;
	}
	Renderer_FrameNo++;
	
	sprintf(Renderer_FPSMessage, "%d FPS", Renderer_FPS);
	SendMessage(hstatus, SB_SETTEXT, 0, (LPARAM)Renderer_FPSMessage);
	
	return 0;
}

/*	------------------------------------------------------------ */

/* INITGL - INITIALIZE OPENGL RENDERING SYSTEM */
int InitGL(void)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
	glClearDepth(1.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
	glEnable(GL_LIGHT1);
	
	glEnable(GL_ALPHA_TEST);
	
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_DENSITY, 0.15f);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 1.0f);
	glFogf(GL_FOG_END, 75.0f);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	
	GLExtension_List = strdup(glGetString(GL_EXTENSIONS));
	int ExtListLen = strlen(GLExtension_List);
	for(int i = 0; i < ExtListLen; i++) {
		if(GLExtension_List[i] == ' ') GLExtension_List[i] = '\n';
	}
	
	if(strstr(GLExtension_List, "GL_ARB_multitexture")) {
		GLExtension_MultiTexture = true;
		glMultiTexCoord1fARB		= (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB		= (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB		= (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB		= (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
		glActiveTextureARB			= (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB	= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
	} else {
		MessageBox(hwnd, "Extension 'GL_ARB_multitexture' not supported.\nMultitexturing will not work correctly.", "Extension Error", MB_OK | MB_ICONERROR);
	}
	
	if(strstr(GLExtension_List, "GL_ARB_texture_mirrored_repeat")) {
		GLExtension_TextureMirror = true;
	} else {
		MessageBox(hwnd, "Extension 'GL_ARB_texture_mirrored_repeat' not supported.\nTexture mirroring will not work correctly.", "Extension Error", MB_OK | MB_ICONERROR);
	}
	
	if(strstr(GLExtension_List, "GL_EXT_texture_filter_anisotropic")) {
		GLExtension_AnisoFilter = true;
	} else {
		MessageBox(hwnd, "Extension 'GL_EXT_texture_filter_anisotropic' not supported.\nAnisotropic filtering will not work correctly.", "Extension Error", MB_OK | MB_ICONERROR);
	}
	
	return true;
}

/* DRAWGLSCENE - DRAW THE CURRENT SCENE USING THE MAP AND ACTOR DATA GATHERED BEFORE */
int DrawGLScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(MapLoaded) {
		HelperFunc_CalculateFPS();
		
		sprintf(Renderer_CoordDisp, "Cam X: %4.2f, Y: %4.2f, Z: %4.2f", CamX, CamY, CamZ);
		
		glLoadIdentity();
		
		gluLookAt(CamX, CamY, CamZ, 
				CamX + CamLX, CamY + CamLY, CamZ + CamLZ,
				0.0f, 1.0f, 0.0f);
		
		glScalef(0.005, 0.005, 0.005);
		
		glDisable(GL_BLEND);
		ActorInfo_CurrentCount = 0;
		ScActorInfo_CurrentCount = 0;
		Viewer_RenderAllActors();
		
		Renderer_GLDisplayList_Current = 0;
		
		if(DListInfo_DListToRender == -1) {
			while(!(Renderer_GLDisplayList_Current == Renderer_GLDisplayList + (DListInfo_CurrentCount + 1))) {
				glCallList(Renderer_GLDisplayList_Current);
				Renderer_GLDisplayList_Current++;
			}
		} else {
			Renderer_GLDisplayList_Current = (Renderer_GLDisplayList + DListInfo_DListToRender);
			glCallList(Renderer_GLDisplayList_Current);
		}
		
		glDisable(GL_BLEND);
		ActorInfo_CurrentCount = 0;
		ScActorInfo_CurrentCount = 0;
		Viewer_RenderAllActors();
	}
	
	return true;
}

/* RESIZEGLSCENE - RESIZES THE OPENGL RENDERING TARGET ALONG WITH THE MAIN WINDOW */
void ReSizeGLScene(GLsizei width, GLsizei height)
{
	if (height == 0) height = 1;
	
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* KILLGLTARGET - DESTROYS THE OPENGL RENDERING TARGET FOR PROPER EXITING */
void KillGLTarget(void)
{
	if (hRC)
	{
		if (!wglMakeCurrent(NULL, NULL)) MessageBox(NULL, "Release of DC and RC failed!", "Error", MB_OK | MB_ICONERROR);
		if (!wglDeleteContext(hRC)) MessageBox(NULL, "Release of Rendering Context failed!", "Error", MB_OK | MB_ICONERROR);
		hRC = NULL;
	}
	
	if (hDC_ogl && !ReleaseDC(hwnd, hDC_ogl))
	{
		MessageBox(NULL, "Release of Device Context failed!", "Error", MB_OK | MB_ICONERROR);
		hDC_ogl = NULL;
	}
}

/* CREATEGLTARGET - CREATE AN OPENGL RENDERING TARGET WITH THE SPECIFIED PARAMETERS */
BOOL CreateGLTarget(int width, int height, int bits)
{
	GLuint		PixelFormat;
	
	static	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	pfd.cColorBits = bits;
	
	if (!(hDC_ogl = GetDC(hogl)))
	{
		KillGLTarget();
		MessageBox(NULL, "Can't create OpenGL Device Context!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	
	if (!(PixelFormat = ChoosePixelFormat(hDC_ogl, &pfd)))
	{
		KillGLTarget();
		MessageBox(NULL, "Can't find suitable PixelFormat!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	
	if(!SetPixelFormat(hDC_ogl, PixelFormat, &pfd))
	{
		KillGLTarget();
		MessageBox(NULL,"Can't set PixelFormat!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	
	if (!(hRC = wglCreateContext(hDC_ogl)))
	{
		KillGLTarget();
		MessageBox(NULL, "Can't create OpenGL Rendering Context!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	
	if(!wglMakeCurrent(hDC_ogl, hRC))
	{
		KillGLTarget();
		MessageBox(NULL, "Can't activate OpenGL Rendering Context!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	
	if (!InitGL())
	{
		KillGLTarget();
		MessageBox(NULL, "Initialization failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}
	
	return TRUE;
}

/*	------------------------------------------------------------ */

void GLUTCamera_Orientation(float ang, float ang2)
{
	CamLX = sin(ang);
	CamLY = ang2;
	CamLZ = -cos(ang);
}

void GLUTCamera_Movement(int direction)
{
	CamX = CamX + direction * (CamLX) * 0.1f;
	CamY = CamY + direction * (CamLY) * 0.1f;
	CamZ = CamZ + direction * (CamLZ) * 0.1f;
}

void Camera_MouseMove(int x, int y) 
{
	MousePosX = x - MouseCenterX;
	MousePosY = y - MouseCenterY;
	
	CamAngleX = CamAngleX + (0.01f * MousePosX);
	CamAngleY = CamAngleY - (0.01f * MousePosY);
	
	MouseCenterX = x;
	MouseCenterY = y;
}

/*	------------------------------------------------------------ */

/* DIALOG_OPENZMAP - COMMON DIALOG USED FOR SELECTING THE MAP FILE */
void Dialog_OpenZMap(HWND hwnd)
{
	char			filter[] = "Zelda Map files (*.zmap)\0;*.zmap\0";
	OPENFILENAME	ofn;
	
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= hwnd;
	ofn.lpstrFilter		= filter;
	ofn.nFilterIndex	= 1;
	ofn.lpstrFile		= Filename_ZMap;
	ofn.nMaxFile		= 256;
	ofn.Flags			= OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	
	if(GetOpenFileName(&ofn)) ZMapExists = true;
	
	return;
}

/* DIALOG_OPENZSCENE - COMMON DIALOG USED FOR SELECTING THE SCENE FILE */
void Dialog_OpenZScene(HWND hwnd)
{
	char			filter[] = "Zelda Scene files (*.zscene)\0*.zscene\0";
	OPENFILENAME	ofn;
	
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= hwnd;
	ofn.lpstrFilter		= filter;
	ofn.nFilterIndex	= 1;
	ofn.lpstrFile		= Filename_ZScene;
	ofn.nMaxFile		= 256;
	ofn.Flags			= OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	
	if(GetOpenFileName(&ofn)) ZSceneExists = true;
	
	return;
}

/*	------------------------------------------------------------ */

/* WINMAIN - WINDOWS PROGRAM MAIN FUNCTION */
int WINAPI WinMain (HINSTANCE hThisInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpszArgument,
					int nFunsterStil)
{
	MSG messages;
	WNDCLASSEX wincl;
	
	InitCommonControls();
	
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof (WNDCLASSEX);
	
	wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
	wincl.lpszMenuName = MAKEINTRESOURCE(IDM_MAINMENU);
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
	
	if (!RegisterClassEx (&wincl))
		return 0;
        
	hwnd = CreateWindowEx (
			0,
			szClassName,
			"",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			648,
			551,
			HWND_DESKTOP,
			NULL,
			hThisInstance,
			NULL
			);
			
	hmenu = GetMenu(hwnd);
	
	hogl = CreateWindowEx (
			WS_EX_CLIENTEDGE,
			"STATIC",
			"",
			WS_CHILD | WS_VISIBLE,
			0,
			0,
			128,
			128,
			hwnd,
			(HMENU)IDC_MAIN_OPENGL,
			GetModuleHandle(NULL),
			NULL
			);
			
	int hstatuswidths[] = {40, 220, -1};
	hstatus = CreateWindowEx(
			0,
			STATUSCLASSNAME,
			"",
			WS_CHILD | WS_VISIBLE,
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)IDC_MAIN_STATUSBAR,
			hThisInstance,
			NULL); 
			
	SendMessage(hstatus, SB_SETPARTS, sizeof(hstatuswidths)/sizeof(int), (LPARAM)hstatuswidths);
	
	if (!CreateGLTarget(640,480,16))
	{
		return 0;
	}
	
	sprintf(WindowTitle, "%s %s", AppTitle, AppVersion);
	SetWindowText(hwnd, WindowTitle);
	
	GetModuleFileName(NULL, AppPath, sizeof(AppPath) - 1);
	
	char *AppPathTemp = strrchr(AppPath, '\\');
	if(AppPathTemp) ++AppPathTemp; if(AppPathTemp) *AppPathTemp = 0;
	sprintf(INIPath, "%s\\ozmav.ini", AppPath);
	
	GetPrivateProfileString("Viewer", "LastMap", "", Filename_ZMap, sizeof(Filename_ZMap), INIPath);
	GetPrivateProfileString("Viewer", "LastScene", "", Filename_ZScene, sizeof(Filename_ZScene), INIPath);
	Renderer_FilteringMode_Min = GetPrivateProfileInt("Viewer", "TexFilterMin", GL_LINEAR_MIPMAP_LINEAR, INIPath);
	Renderer_FilteringMode_Mag = GetPrivateProfileInt("Viewer", "TexFilterMag", GL_LINEAR, INIPath);
	Renderer_EnableMapActors = GetPrivateProfileInt("Viewer", "RenderMapActors", true, INIPath);
	Renderer_EnableSceneActors = GetPrivateProfileInt("Viewer", "RenderSceneActors", false, INIPath);
	Renderer_EnableMultiTexturing = GetPrivateProfileInt("Viewer", "EnableMultiTex", false, INIPath);
	
	ShowWindow(hwnd, nFunsterStil);
	
	while(!ExitProgram) {
		if(PeekMessage (&messages, NULL, 0, 0, PM_REMOVE)) {
			if (messages.message == WM_CLOSE) {
				ExitProgram = true;
			} else {
				TranslateMessage(&messages);
				DispatchMessage(&messages);
			}
		} else {
			if (WndActive) {
				if (System_KbdKeys[VK_ESCAPE]) {
					ExitProgram = true;
				}
				
				if (System_KbdKeys[VK_F11]) {
					System_KbdKeys[VK_F11] = false;
				}
				
				if (MapLoaded) {
					if (System_KbdKeys[VK_F1]) {
						System_KbdKeys[VK_F1] = false;
						if(!(DListInfo_DListToRender == -1)) {
							DListInfo_DListToRender--;
						}
						if(DListInfo_DListToRender == -1) {
							sprintf(StatusMsg, "Display List: rendering all");
						} else {
							sprintf(StatusMsg, "Display List: #%d", (int)DListInfo_DListToRender + 1);
						}
					}
					if (System_KbdKeys[VK_F2]) {
						System_KbdKeys[VK_F2] = false;
						if(!(DListInfo_DListToRender == DListInfo_CurrentCount)) {
							DListInfo_DListToRender++;
						}
						sprintf(StatusMsg, "Display List: #%d", (int)DListInfo_DListToRender + 1);
					}
					if (System_KbdKeys[VK_F3]) {
						System_KbdKeys[VK_F3] = false;
						if(!(MapHeader_TotalCount == 0)) {
							if(!(MapHeader_Current == 0)) {
								MapHeader_Current--;
								FileSystemLog = fopen("log.txt", "w");
								Viewer_GetMapHeader(MapHeader_Current);
								Viewer_GetMapActors(MapHeader_Current);
								fclose(FileSystemLog);
							}
							sprintf(StatusMsg, "Map Header: #%d", MapHeader_Current);
						}
					}
					if (System_KbdKeys[VK_F4]) {
						System_KbdKeys[VK_F4] = false;
						if(!(MapHeader_TotalCount == 0)) {
							if(!(MapHeader_Current == MapHeader_TotalCount - 1)) {
								MapHeader_Current++;
								FileSystemLog = fopen("log.txt", "w");
								Viewer_GetMapHeader(MapHeader_Current);
								Viewer_GetMapActors(MapHeader_Current);
								fclose(FileSystemLog);
							}
							sprintf(StatusMsg, "Map Header: #%d", MapHeader_Current);
						}
					}
					if (System_KbdKeys[VK_F5]) {
						System_KbdKeys[VK_F5] = false;
						if(!(ActorInfo_Selected == 0)) {
							ActorInfo_Selected--;
						}
					}
					if (System_KbdKeys[VK_F6]) {
						System_KbdKeys[VK_F6] = false;
						if(!(ActorInfo_Selected == MapHeader[MapHeader_Current].Actor_Count - 1)) {
							ActorInfo_Selected++;
						}
					}
					if (System_KbdKeys[VK_F7]) {
						System_KbdKeys[VK_F7] = false;
						ActorInfo_Selected = 0;
					}
					if (System_KbdKeys[VK_F8]) {
						System_KbdKeys[VK_F8] = false;
						ActorInfo_Selected = MapHeader[MapHeader_Current].Actor_Count - 1;
					}
					
					if (System_KbdKeys[VK_DIVIDE]) {
						System_KbdKeys[VK_DIVIDE] = false;
						if(!(SceneHeader_TotalCount == 0)) {
							if(!(SceneHeader_Current == 0)) {
								SceneHeader_Current--;
								FileSystemLog = fopen("log.txt", "w");
								Viewer_GetSceneHeader(SceneHeader_Current);
								Viewer_GetSceneActors(SceneHeader_Current);
								fclose(FileSystemLog);
							}
							sprintf(StatusMsg, "Scene Header: #%d", SceneHeader_Current);
						}
					}
					if (System_KbdKeys[VK_MULTIPLY]) {
						System_KbdKeys[VK_MULTIPLY] = false;
						if(!(SceneHeader_TotalCount == 0)) {
							if(!(SceneHeader_Current == SceneHeader_TotalCount - 1)) {
								SceneHeader_Current++;
								FileSystemLog = fopen("log.txt", "w");
								Viewer_GetSceneHeader(SceneHeader_Current);
								Viewer_GetSceneActors(SceneHeader_Current);
								fclose(FileSystemLog);
							}
							sprintf(StatusMsg, "Scene Header: #%d", SceneHeader_Current);
						}
					}
					if (System_KbdKeys[VK_SUBTRACT]) {
						System_KbdKeys[VK_SUBTRACT] = false;
						if(!(ScActorInfo_Selected == 0)) {
							ScActorInfo_Selected--;
						}
					}
					if (System_KbdKeys[VK_ADD]) {
						System_KbdKeys[VK_ADD] = false;
						if(!(ScActorInfo_Selected == SceneHeader[SceneHeader_Current].ScActor_Count - 1)) {
							ScActorInfo_Selected++;
						}
					}
					
					if (System_KbdKeys['W']) {
						GLUTCamera_Movement(1);
					}
					if (System_KbdKeys['S']) {
						GLUTCamera_Movement(-1);
					}
					if (System_KbdKeys[VK_LEFT]) {
						CamAngleX -= 0.02f;
					}
					if (System_KbdKeys[VK_RIGHT]) {
						CamAngleX += 0.02f;
					}
					
					if (System_KbdKeys[VK_UP]) {
						CamAngleY += 0.01f;
					}
					if (System_KbdKeys[VK_DOWN]) {
						CamAngleY -= 0.01f;
					}
					
					if (System_KbdKeys[VK_TAB]) {
						System_KbdKeys[VK_TAB] = false;
						CamAngleX = 0.0f, CamAngleY = 0.0f;
						CamX = 0.0f, CamY = 0.0f, CamZ = 5.0f;
						CamLX = 0.0f, CamLY = 0.0f, CamLZ = -1.0f;
					}
					
					SendMessage(hstatus, SB_SETTEXT, 1, (LPARAM)Renderer_CoordDisp);
					SendMessage(hstatus, SB_SETTEXT, 2, (LPARAM)StatusMsg);
				}
			}
			
			GLUTCamera_Orientation(CamAngleX, CamAngleY);
			
			DrawGLScene();
			SwapBuffers(hDC_ogl);
		}
	}
	
	WritePrivateProfileString("Viewer", "LastMap", Filename_ZMap, INIPath);
	WritePrivateProfileString("Viewer", "LastScene", Filename_ZScene, INIPath);
	char TempStr[256];
	sprintf(TempStr, "%d", Renderer_FilteringMode_Min);
	WritePrivateProfileString("Viewer", "TexFilterMin", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_FilteringMode_Mag);
	WritePrivateProfileString("Viewer", "TexFilterMag", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableMapActors);
	WritePrivateProfileString("Viewer", "RenderMapActors", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableSceneActors);
	WritePrivateProfileString("Viewer", "RenderSceneActors", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableMultiTexturing);
	WritePrivateProfileString("Viewer", "EnableMultiTex", TempStr, INIPath);
	
	KillGLTarget();
	DestroyWindow(hwnd);
	
	return (messages.wParam);
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_ACTIVATE: {
			if (!HIWORD(wParam)) {
				WndActive = true;
			} else {
				WndActive = false;
			}
			break;
		}
		case WM_SIZE: {
			HWND hogl;
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			hogl = GetDlgItem(hwnd, IDC_MAIN_OPENGL);
			SetWindowPos(hogl, NULL, 0, 0, rcClient.right, rcClient.bottom - 25, SWP_NOZORDER);
			
			RECT rcStatus;
			GetDlgItem(hwnd, IDC_MAIN_STATUSBAR);
			SendMessage(hstatus, WM_SIZE, 0, 0);
			GetWindowRect(hstatus, &rcStatus);
			
			ReSizeGLScene(rcClient.right, rcClient.bottom);
			DrawGLScene();
			SwapBuffers(hDC_ogl);
			break;
		}
		case WM_COMMAND: {
			switch(LOWORD(wParam))
			{
				case IDM_FILE_OPEN:
					ZMapExists = false;
					ZSceneExists = false;
					
					Dialog_OpenZMap(hwnd);
					
					if(ZMapExists) Dialog_OpenZScene(hwnd);
					if(ZSceneExists) Viewer_Initialize();
					break;
				case IDM_FILE_SAVE:
					break;
				case IDM_FILE_EXIT:
					ExitProgram = true;
					break;
				case IDM_MAP_PREVDLIST:
					System_KbdKeys[VK_F1] = true;
					break;
				case IDM_MAP_NEXTDLIST:
					System_KbdKeys[VK_F2] = true;
					break;
				case IDM_MAP_PREVHEADER:
					System_KbdKeys[VK_F3] = true;
					break;
				case IDM_MAP_NEXTHEADER:
					System_KbdKeys[VK_F4] = true;
					break;
				case IDM_SCENE_PREVHEADER:
					System_KbdKeys[VK_DIVIDE] = true;
					break;
				case IDM_SCENE_NEXTHEADER:
					System_KbdKeys[VK_MULTIPLY] = true;
					break;
				case IDM_ACTORS_MAPRENDER:
					if(!Renderer_EnableMapActors) {
						Renderer_EnableMapActors = true;
					} else {
						Renderer_EnableMapActors = false;
					}
					break;
				case IDM_ACTORS_SELECTPREV:
					System_KbdKeys[VK_F5] = true;
					break;
				case IDM_ACTORS_SELECTNEXT:
					System_KbdKeys[VK_F6] = true;
					break;
				case IDM_ACTORS_JUMPFIRST:
					System_KbdKeys[VK_F7] = true;
					break;
				case IDM_ACTORS_JUMPLAST:
					System_KbdKeys[VK_F8] = true;
					break;
				case IDM_ACTORS_SCENERENDER:
					if(!Renderer_EnableSceneActors) {
						Renderer_EnableSceneActors = true;
					} else {
						Renderer_EnableSceneActors = false;
					}
					break;
				case IDM_ACTORS_SCENEPREV:
					System_KbdKeys[VK_SUBTRACT] = true;
					break;
				case IDM_ACTORS_SCENENEXT:
					System_KbdKeys[VK_ADD] = true;
					break;
				case IDM_CAMERA_RESETCOORDS:
					System_KbdKeys[VK_TAB] = true;
					break;
				case IDM_OPTIONS_FILTERNEAREST:
					Renderer_FilteringMode_Min = GL_NEAREST;
					Renderer_FilteringMode_Mag = GL_NEAREST;
					Viewer_RenderMap();
					break;
				case IDM_OPTIONS_FILTERLINEAR:
					Renderer_FilteringMode_Min = GL_LINEAR;
					Renderer_FilteringMode_Mag = GL_LINEAR;
					Viewer_RenderMap();
					break;
				case IDM_OPTIONS_FILTERMIPMAP:
					Renderer_FilteringMode_Min = GL_LINEAR_MIPMAP_LINEAR;
					Renderer_FilteringMode_Mag = GL_LINEAR;
					Viewer_RenderMap();
					break;
				case IDM_OPTIONS_MULTITEXTURE:
					if(Renderer_EnableMultiTexturing) {
						Renderer_EnableMultiTexturing = false;
					} else {
						Renderer_EnableMultiTexturing = true;
					}
					Viewer_RenderMap();
					break;
				case IDM_HELP_ABOUT: ;
					sprintf(GLExtensionsSupported, "OpenGL extensions supported and used:\n");
					if(GLExtension_MultiTexture) sprintf(GLExtensionsSupported, "%sGL_ARB_multitexture\n", GLExtensionsSupported);
					if(GLExtension_TextureMirror) sprintf(GLExtensionsSupported, "%sGL_ARB_texture_mirrored_repeat\n", GLExtensionsSupported);
					if(GLExtension_AnisoFilter) sprintf(GLExtensionsSupported, "%sGL_EXT_texture_filter_anisotropic\n", GLExtensionsSupported);
					
					char AboutMsg[256] = "";
					sprintf(AboutMsg, "%s %s (Build '%s') - OpenGL Zelda Map Viewer\n\nWritten in October/November 2008 by xdaniel & contributors\nhttp://ozmav.googlecode.com/\n\n%s", AppTitle, AppVersion, AppBuildName, GLExtensionsSupported);
					MessageBox(hwnd, AboutMsg, "About", MB_OK | MB_ICONINFORMATION);
					break;
			}
			break;
		}
		case WM_CLOSE: {
			ExitProgram = true;
			break;
		}
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		case WM_KEYDOWN: {
			System_KbdKeys[wParam] = true;
			break;
		}
		case WM_KEYUP: {
			System_KbdKeys[wParam] = false;
			break;
		}
		case WM_LBUTTONDOWN: {
			MouseButtonDown = true;
			MouseCenterX = (signed int)LOWORD(lParam);
			MouseCenterY = (signed int)HIWORD(lParam);
			break;
		}
		case WM_LBUTTONUP: {
			MouseButtonDown = false;
			break;
		}
		case WM_MOUSEMOVE: {
			if((MouseButtonDown) && (WndActive)) {
				MousePosX = (signed int)LOWORD(lParam);
				MousePosY = (signed int)HIWORD(lParam);
				Camera_MouseMove(MousePosX, MousePosY);
				GLUTCamera_Orientation(CamAngleX, CamAngleY);
			}
			break;
		}
		default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	
	return 0;
}

/*	------------------------------------------------------------ */
