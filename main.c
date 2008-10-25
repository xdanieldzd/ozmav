/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer
	
	Written in October 2008 by xdaniel
	http://magicstone.de/dzd/
	
	Partially based on my 'Experimental N64 Object Viewer'
	
	Pieces of code from	NeHe OpenGL tutorials (init, etc.),
						various Win32 API samples
	------------------------------------------------------------ */

/*	------------------------------------------------------------
	INCLUDES
	------------------------------------------------------------ */

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>

#include <glib.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#include "ucode.h"
#include "resource.h"

typedef gboolean bool;
enum { true = TRUE, false = FALSE };

/*	------------------------------------------------------------
	DEFINES
	------------------------------------------------------------ */

#define	HACKS_ENABLED	false				/* EN-/DISABLE MISC HACKS ("SUB-DLISTS" ETC) */

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
int Viewer_RenderMap(int);

int Viewer_RenderMap_CMDVertexList();
int Viewer_GetVertexList(unsigned long, int, unsigned int);
int Viewer_RenderMap_CMDDrawTri1();
int Viewer_RenderMap_CMDDrawTri2();
int Viewer_RenderMap_CMDTexture();
int Viewer_RenderMap_CMDSetTImage();
int Viewer_RenderMap_CMDSetTile();
int Viewer_RenderMap_CMDLoadBlock();
int Viewer_RenderMap_CMDSetTileSize();
GLuint Viewer_LoadTexture();
int Viewer_RenderMap_CMDGeometryMode();
int Viewer_RenderMap_CMDSetFogColor();
int Viewer_RenderMap_CMDRDPHalf1();

int Viewer_RenderActor(int, GLshort, GLshort, GLshort, GLshort, GLshort, GLshort, bool);

void HelperFunc_SplitCurrentVals(bool, bool);
int HelperFunc_GFXLogMessage(char[]);
int HelperFunc_GFXLogCommand();
int HelperFunc_CalculateFPS();

int InitGL(void);
int DrawGLScene(void);
void KillGLTarget(void);
BOOL CreateGLTarget(int, int, int);
void BuildFont(void);
void KillFont(void);
void glPrint(const char *, ...);

void GLUTCamera_Orientation(float,float);
void GLUTCamera_Movement(int);
void Camera_MouseMove(int, int);

void Dialog_OpenZMap(HWND);
void Dialog_OpenZScene(HWND);

int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*	------------------------------------------------------------
	VARIABLES
	------------------------------------------------------------ */

/* WINDOW HANDLING VARIABLES, ETC. */
HWND			hwnd = NULL;
HMENU			hmenu = NULL;
HWND			hogl = NULL;
HWND			hstatus = NULL;
int				StatusBarHeight;

HDC				hDC_ogl = NULL;
HGLRC			hRC = NULL;
HINSTANCE		hInstance;

char			szClassName[ ] = "OZMAVClass";

GLuint			GLFontBase;

/* GENERAL GLOBAL PROGRAM VARIABLES */
bool			System_KbdKeys[256];

char			AppTitle[256] = "OZMAV";
char			AppVersion[256] = "V0.3";
char			AppBuildName[256] = "Timotei";
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

int				OGLTargetWidth;
int				OGLTargetHeight;

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

unsigned int	* ZMapBuffer;
unsigned int	* ZSceneBuffer;

unsigned long	ZMapFilesize = 0;
unsigned long	ZSceneFilesize = 0;

char			Filename_ZMap[256] = "";
char			Filename_ZScene[256] = "";

FILE			* FileGFXLog;

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

unsigned long	Readout_Current1_Backup = 0;
unsigned long	Readout_Current2_Backup = 0;
unsigned int	Readout_CurrentByte1_Backup = 0;
unsigned int	Readout_CurrentByte2_Backup = 0;
unsigned int	Readout_CurrentByte3_Backup = 0;
unsigned int	Readout_CurrentByte4_Backup = 0;
unsigned int	Readout_CurrentByte5_Backup = 0;
unsigned int	Readout_CurrentByte6_Backup = 0;
unsigned int	Readout_CurrentByte7_Backup = 0;
unsigned int	Readout_CurrentByte8_Backup = 0;

/* F3DZEX DISPLAY LIST HANDLING VARIABLES */
unsigned long	DLists[2048];
signed long		DListInfo_CurrentCount;
signed long		DListInfo_DListToRender;
unsigned long	DLTempPosition;

unsigned long	Sub_DLTempPosition;

/* F3DZEX TEXTURE HANDLING VARIABLES */
unsigned long	TextureInfo_Current;
	
unsigned char	* TextureData_OGL;
unsigned char	* TextureData_N64;

unsigned int	LBUpper_Y;
unsigned int	LBUpper_X;
unsigned int	ScanlineChange_Y;
unsigned int	LowerRight_X;

unsigned int	LineSize;

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

DWORD			Renderer_LastFPS = 0;
int				Renderer_FPS, Renderer_FrameNo = 0;
char			Renderer_FPSMessage[32] = "";

char			Renderer_CoordDisp[256] = "";

bool			Renderer_EnableLighting = true;

GLfloat			LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat			LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat			LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };

GLfloat			FogColor[]= { 0.0f, 0.0f, 0.0f, 0.5f };

bool			Renderer_EnableMapActors = true;
bool			Renderer_EnableSceneActors = false;

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
	char Group_Count;
	unsigned long Group_DataOffset;
	char Actor_Count;
	unsigned long Actor_DataOffset;
} MapHeader[256];

struct {
	unsigned long Unknown1;
	unsigned long Unknown2;
	unsigned long Unknown3;
	unsigned long Unknown4;
	unsigned long Unknown5;
	char ScActor_Count;
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
} Actors[512];

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
} ScActors[512];

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
} Vertex[2048];

/* F3DZEX TEXTURE DATA STRUCTURE */
struct {
	unsigned int Height;
	unsigned int HeightRender;
	unsigned int Width;
	unsigned int WidthRender;
	unsigned int DataSource;
	unsigned long Offset;
	unsigned int Format_N64;
	GLuint Format_OGL;
	unsigned int Format_OGLPixel;
	unsigned int BPP;
	unsigned int Y_Parameter;
	unsigned int X_Parameter;
	signed short S_Scale;
	signed short T_Scale;
} Textures[256];

/*	------------------------------------------------------------ */

/* VIEWER_INITIALIZE - CALLED AFTER SELECTING THE MAP AND SCENE FILES */
int Viewer_Initialize()
{
	Viewer_OpenMapScene();
	
	Renderer_GLDisplayList = glGenLists(1);
	Viewer_RenderMap(DListInfo_DListToRender);
	
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
	
	sprintf(WindowTitle, "%s %s - %s", AppTitle, AppVersion, Filename_ZMap);
	SetWindowText(hwnd, WindowTitle);
	
	return 0;
}

/* VIEWER_OPENMAPSCENE - CALLED IN INITIALIZATION, LOADS MAP AND SCENE DATA INTO BUFFERS AND DOES SOME PRE-ANALYZING */
int Viewer_OpenMapScene()
{
	MapLoaded = false;
	
	/* OPEN FILE */
	FileZMap = fopen(Filename_ZMap, "r+b");
	/* GET FILESIZE */
	size_t Result;
	fseek(FileZMap, 0, SEEK_END);
	ZMapFilesize = ftell(FileZMap);
	rewind(FileZMap);
	/* LOAD FILE INTO BUFFER */
	ZMapBuffer = (unsigned int*) malloc (sizeof(int)*ZMapFilesize);
	Result = fread(ZMapBuffer,1,ZMapFilesize,FileZMap);
	/* CLOSE FILE */
	fclose(FileZMap);
	
	/* OPEN FILE */
	FileZScene = fopen(Filename_ZScene, "r+b");
	/* GET FILESIZE */
	fseek(FileZMap, 0, SEEK_END);
	ZSceneFilesize = ftell(FileZScene);
	rewind(FileZScene);
	/* LOAD FILE INTO BUFFER */
	ZSceneBuffer = (unsigned int*) malloc (sizeof(int)*ZSceneFilesize);
	Result = fread(ZSceneBuffer,1,ZSceneFilesize,FileZScene);
	/* CLOSE FILE */
	fclose(FileZScene);
	
	memcpy(&Readout_Current1, &ZMapBuffer[0], 4);
	memcpy(&Readout_Current2, &ZMapBuffer[0 + 1], 4);
	HelperFunc_SplitCurrentVals(true, false);
	
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
		HelperFunc_SplitCurrentVals(true, false);
		
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
		TextureInfo_Current = 0;
		ActorInfo_Selected = 0;
		
		Renderer_EnableLighting = true;
		
		memset(MapHeader, 0x00, sizeof(MapHeader));
		memset(SceneHeader, 0x00, sizeof(SceneHeader));
		memset(Actors, 0x00, sizeof(Actors));
		memset(ScActors, 0x00, sizeof(ScActors));
		memset(Vertex, 0x00, sizeof(Vertex));
		memset(Textures, 0x00, sizeof(Textures));
		
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
		
		HelperFunc_SplitCurrentVals(false, false);
		
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
	int InHeaderPos = MapHeader_List[CurrentHeader] / 4;
	
	while(!(Readout_Current1 == 0x00000014) || (Readout_Current2 == 0x00000014)) {
		memcpy(&Readout_Current1, &ZMapBuffer[InHeaderPos], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[InHeaderPos + 1], 4);
		
		HelperFunc_SplitCurrentVals(true, false);
		
		switch(Readout_CurrentByte1) {
		case 0x16:
			MapHeader[CurrentHeader].EchoLevel = Readout_CurrentByte8;
			break;
		case 0x12:
			MapHeader[CurrentHeader].Skybox = Readout_CurrentByte5;
			break;
		case 0x10:
			MapHeader[CurrentHeader].MapTime = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			MapHeader[CurrentHeader].TimeFlow = Readout_CurrentByte7;
			break;
		case 0x0A:
			MapHeader[CurrentHeader].MeshDataHeader = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			break;
		case 0x0B:
			MapHeader[CurrentHeader].Group_Count = Readout_CurrentByte2;
			MapHeader[CurrentHeader].Group_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			break;
		case 0x01:
			MapHeader[CurrentHeader].Actor_Count = Readout_CurrentByte2;
			MapHeader[CurrentHeader].Actor_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			break;
		}
		
		InHeaderPos += 2;
	}
	
	Readout_Current1 = 0x00;
	Readout_Current2 = 0x00;
	
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
		
		HelperFunc_SplitCurrentVals(false, false);
		
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
	int InHeaderPos = SceneHeader_List[CurrentHeader] / 4;
	
	while(!(Readout_Current1 == 0x00000014) || (Readout_Current2 == 0x00000014)) {
		memcpy(&Readout_Current1, &ZSceneBuffer[InHeaderPos], 4);
		memcpy(&Readout_Current2, &ZSceneBuffer[InHeaderPos + 1], 4);
		
		HelperFunc_SplitCurrentVals(true, false);
		
		switch(Readout_CurrentByte1) {
		case 0x00:
			SceneHeader[CurrentHeader].ScActor_Count = Readout_CurrentByte2;
			SceneHeader[CurrentHeader].ScActor_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			break;
		}
		
		InHeaderPos += 2;
	}
	
	Readout_Current1 = 0x00;
	Readout_Current2 = 0x00;
	
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
			
			HelperFunc_SplitCurrentVals(true, false);
			
			Actors[ActorInfo_CurrentCount].Number = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			Actors[ActorInfo_CurrentCount].X_Position = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Actors[ActorInfo_CurrentCount].Y_Position = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			Actors[ActorInfo_CurrentCount].Z_Position = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			
			memcpy(&Readout_Current1, &ZMapBuffer[InActorDataPos + 2], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[InActorDataPos + 3], 4);
			
			HelperFunc_SplitCurrentVals(true, false);
			
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
			
			HelperFunc_SplitCurrentVals(true, false);
			
			ScActors[ScActorInfo_CurrentCount].Number = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			ScActors[ScActorInfo_CurrentCount].X_Position = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			ScActors[ScActorInfo_CurrentCount].Y_Position = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			ScActors[ScActorInfo_CurrentCount].Z_Position = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			
			memcpy(&Readout_Current1, &ZMapBuffer[InScActorDataPos + 2], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[InScActorDataPos + 3], 4);
			
			HelperFunc_SplitCurrentVals(true, false);
			
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
	
	while ((DListScanPosition < Fsize)) {
		memcpy(&Readout_Current1, &ZMapBuffer[DListScanPosition], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[DListScanPosition + 1], 4);
		
		HelperFunc_SplitCurrentVals(true, false);
		
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
		
		/* HACK: ADD DLISTS CALLED BY RDPHALF_1 TO DLIST OFFSET LIST SO THAT THEY'RE DISPLAYED */
		if(HACKS_ENABLED) {
			if ((Readout_CurrentByte1 == F3DEX2_RDPHALF_1) && (Readout_CurrentByte2 == 0x00)) {
				if((Readout_CurrentByte3 == 0x00) && (Readout_CurrentByte4 == 0x00)) {
					if((Readout_CurrentByte5 == 0x03)) {
						sprintf(StatusMsg, "Warning: Display List call via F3DEX2_RDPHALF_1 found at 0x%08X. Display list is at 0x%08X\n\nThis type of call is not being handled properly. Adding to regular DList offset list...", DListScanPosition * 4, TempOffset);
						MessageBox(hwnd, StatusMsg, "Message", MB_OK | MB_ICONEXCLAMATION);
						
						TempOffset = Readout_CurrentByte6 << 16;
						TempOffset = TempOffset + (Readout_CurrentByte7 << 8);
						TempOffset = TempOffset + Readout_CurrentByte8;
						
						DListInfo_CurrentCount++;
						DLists[DListInfo_CurrentCount] = TempOffset;
					}
				}
			}
		}
		
		DListScanPosition += 2;
	}
	
	return 0;
}

/* VIEWER_RENDERMAP - SCAN EITHER ALL OR A GIVEN DISPLAY LIST(S), INTERPRET ITS COMMANDS AND PREPARE AN OPENGL DISPLAY LIST */
int Viewer_RenderMap(int SingleDLNumber)
{
	if(MapLoaded == false) FileGFXLog = fopen("log.txt", "w");
	
	int DLToRender = 0;
	int DListInfo_CurrentCount_Render = 0;
	TextureInfo_Current = 0;
	
	if((SingleDLNumber == -1)) {
		DLToRender = 0;
		DListInfo_CurrentCount_Render = DListInfo_CurrentCount + 1;
	} else {
		DLToRender = SingleDLNumber;
		DListInfo_CurrentCount_Render = SingleDLNumber + 1;
	}
	
	bool DListHasEnded = false;
	
	if(Renderer_GLDisplayList != 0) {
		glNewList(Renderer_GLDisplayList, GL_COMPILE);
		while (!(DLToRender == DListInfo_CurrentCount_Render)) {
			DLTempPosition = DLists[DLToRender] / 4;
			
			sprintf(GFXLogMsg, "Display List #%d\n", DLToRender + 1);
			HelperFunc_GFXLogMessage(GFXLogMsg);
			
			while (!DListHasEnded) {
				memcpy(&Readout_Current1, &ZMapBuffer[DLTempPosition], 4);
				memcpy(&Readout_Current2, &ZMapBuffer[DLTempPosition + 1], 4);
				
				HelperFunc_SplitCurrentVals(true, false);
				
				switch(Readout_CurrentByte1) {
				case F3DEX2_VTX:
					sprintf(CurrentGFXCmd, "F3DEX2_VTX           ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					Viewer_RenderMap_CMDVertexList();
					break;
				case F3DEX2_TRI1:
					sprintf(CurrentGFXCmd, "F3DEX2_TRI1          ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					Viewer_RenderMap_CMDDrawTri1();
					break;
				case F3DEX2_TRI2:
					sprintf(CurrentGFXCmd, "F3DEX2_TRI2          ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					Viewer_RenderMap_CMDDrawTri2();
					break;
				case F3DEX2_TEXTURE:
					sprintf(CurrentGFXCmd, "F3DEX2_TEXTURE       ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					Viewer_RenderMap_CMDTexture();
					break;
				case G_SETTIMG:
					sprintf(CurrentGFXCmd, "G_SETTIMG            ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					Viewer_RenderMap_CMDSetTImage();
					break;
				case G_SETTILE:
					sprintf(CurrentGFXCmd, "G_SETTILE            ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					Viewer_RenderMap_CMDSetTile();
					break;
				case G_LOADBLOCK:
					sprintf(CurrentGFXCmd, "G_LOADBLOCK          ");
					sprintf(CurrentGFXCmdNote, "<unhandled>");
					HelperFunc_GFXLogCommand();
//					Viewer_RenderMap_CMDLoadBlock();
					break;
				case G_SETTILESIZE:
					sprintf(CurrentGFXCmd, "G_SETTILESIZE        ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					Viewer_RenderMap_CMDSetTileSize();
					break;
				case G_RDPFULLSYNC:
					sprintf(CurrentGFXCmd, "G_RDPFULLSYNC        ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					break;
				case G_RDPTILESYNC:
					sprintf(CurrentGFXCmd, "G_RDPTILESYNC        ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					break;
				case G_RDPPIPESYNC:
					sprintf(CurrentGFXCmd, "G_RDPPIPESYNC        ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					break;
				case G_RDPLOADSYNC:
					sprintf(CurrentGFXCmd, "G_RDPLOADSYNC        ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					break;
				case F3DEX2_GEOMETRYMODE:
					sprintf(CurrentGFXCmd, "F3DEX2_GEOMETRYMODE  ");
					sprintf(CurrentGFXCmdNote, "<partially handled>");
					HelperFunc_GFXLogCommand();
					Viewer_RenderMap_CMDGeometryMode();
					break;
				case F3DEX2_CULLDL:
					sprintf(CurrentGFXCmd, "F3DEX2_CULLDL        ");
					sprintf(CurrentGFXCmdNote, "<unhandled>");
					HelperFunc_GFXLogCommand();
					break;
				case F3DEX2_SETOTHERMODE_H:
					sprintf(CurrentGFXCmd, "F3DEX2_SETOTHERMODE_H");
					sprintf(CurrentGFXCmdNote, "<unhandled>");
					HelperFunc_GFXLogCommand();
					break;
				case F3DEX2_SETOTHERMODE_L:
					sprintf(CurrentGFXCmd, "F3DEX2_SETOTHERMODE_L");
					sprintf(CurrentGFXCmdNote, "<unhandled>");
					HelperFunc_GFXLogCommand();
					break;
				case G_SETFOGCOLOR:
					sprintf(CurrentGFXCmd, "G_SETFOGCOLOR        ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					Viewer_RenderMap_CMDSetFogColor();
					break;
				case F3DEX2_RDPHALF_1:
					sprintf(CurrentGFXCmd, "F3DEX2_RDPHALF_1     ");
					sprintf(CurrentGFXCmdNote, "<broken>");
					HelperFunc_GFXLogCommand();
//					Viewer_RenderMap_CMDRDPHalf1();
					break;
				case F3DEX2_ENDDL:
					sprintf(CurrentGFXCmd, "F3DEX2_ENDDL         ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					DListHasEnded = true;
					break;
				default:
					sprintf(CurrentGFXCmd, "<unknown>            ");
					sprintf(CurrentGFXCmdNote, "");
					HelperFunc_GFXLogCommand();
					break;
				}
				
				if(DListHasEnded == true) HelperFunc_GFXLogMessage("\n");
				
				DLTempPosition+=2;
			}
			DLToRender++;
			DListHasEnded = false;
		}
		glEndList();
		DLToRender = 0;
	}
	
	MapLoaded = true;
	
	return 0;
}

/*	------------------------------------------------------------ */

/* VIEWER_RENDERMAP_CMDVERTEXLIST - F3DEX2_VTX - GET OFFSET, AMOUNT & VERTEX BUFFER POSITION OF VERTICES TO USE */
int Viewer_RenderMap_CMDVertexList()
{
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
		glColor4ub (0xFF, 0xFF, 0xFF, 0xFF);
		
		TempU = (float) CurrentH1_1 * Textures[TextureInfo_Current].S_Scale / 32 / Textures[TextureInfo_Current].WidthRender;
		TempV = (float) CurrentV1_1 * Textures[TextureInfo_Current].T_Scale / 32 / Textures[TextureInfo_Current].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_1, CurrentG1_1, CurrentB1_1, CurrentA1_1); }
		glNormal3f (CurrentR1_1 / 255.0f, CurrentG1_1 / 255.0f, CurrentB1_1 / 255.0f);
		glVertex3d(CurrentX1_1, CurrentY1_1, CurrentZ1_1);
		TempU = (float) CurrentH1_2 * Textures[TextureInfo_Current].S_Scale / 32 / Textures[TextureInfo_Current].WidthRender;
		TempV = (float) CurrentV1_2 * Textures[TextureInfo_Current].T_Scale / 32 / Textures[TextureInfo_Current].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_2, CurrentG1_2, CurrentB1_2, CurrentA1_2); }
		glNormal3f (CurrentR1_2 / 255.0f, CurrentG1_2 / 255.0f, CurrentB1_2 / 255.0f);
		glVertex3d(CurrentX1_2, CurrentY1_2, CurrentZ1_2);
		TempU = (float) CurrentH1_3 * Textures[TextureInfo_Current].S_Scale / 32 / Textures[TextureInfo_Current].WidthRender;
		TempV = (float) CurrentV1_3 * Textures[TextureInfo_Current].T_Scale / 32 / Textures[TextureInfo_Current].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_3, CurrentG1_3, CurrentB1_3, CurrentA1_2); }
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
		glColor4ub (0xFF, 0xFF, 0xFF, 0xFF);
		
		TempU = (float) CurrentH1_1 * Textures[TextureInfo_Current].S_Scale / 32 / Textures[TextureInfo_Current].WidthRender;
		TempV = (float) CurrentV1_1 * Textures[TextureInfo_Current].T_Scale / 32 / Textures[TextureInfo_Current].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_1, CurrentG1_1, CurrentB1_1, CurrentA1_1); }
		glNormal3f (CurrentR1_1 / 255.0f, CurrentG1_1 / 255.0f, CurrentB1_1 / 255.0f);
		glVertex3d(CurrentX1_1, CurrentY1_1, CurrentZ1_1);
		TempU = (float) CurrentH1_2 * Textures[TextureInfo_Current].S_Scale / 32 / Textures[TextureInfo_Current].WidthRender;
		TempV = (float) CurrentV1_2 * Textures[TextureInfo_Current].T_Scale / 32 / Textures[TextureInfo_Current].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_2, CurrentG1_2, CurrentB1_2, CurrentA1_2); }
		glNormal3f (CurrentR1_2 / 255.0f, CurrentG1_2 / 255.0f, CurrentB1_2 / 255.0f);
		glVertex3d(CurrentX1_2, CurrentY1_2, CurrentZ1_2);
		TempU = (float) CurrentH1_3 * Textures[TextureInfo_Current].S_Scale / 32 / Textures[TextureInfo_Current].WidthRender;
		TempV = (float) CurrentV1_3 * Textures[TextureInfo_Current].T_Scale / 32 / Textures[TextureInfo_Current].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_3, CurrentG1_3, CurrentB1_3, CurrentA1_2); }
		glNormal3f (CurrentR1_3 / 255.0f, CurrentG1_3 / 255.0f, CurrentB1_3 / 255.0f);
		glVertex3d(CurrentX1_3, CurrentY1_3, CurrentZ1_3);
		
		TempU = (float) CurrentH2_1 * Textures[TextureInfo_Current].S_Scale / 32 / Textures[TextureInfo_Current].WidthRender;
		TempV = (float) CurrentV2_1 * Textures[TextureInfo_Current].T_Scale / 32 / Textures[TextureInfo_Current].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR2_1, CurrentG2_1, CurrentB2_1, CurrentA2_1); }
		glNormal3f (CurrentR2_1 / 255.0f, CurrentG2_1 / 255.0f, CurrentB2_1 / 255.0f);
		glVertex3d(CurrentX2_1, CurrentY2_1, CurrentZ2_1);
		TempU = (float) CurrentH2_2 * Textures[TextureInfo_Current].S_Scale / 32 / Textures[TextureInfo_Current].WidthRender;
		TempV = (float) CurrentV2_2 * Textures[TextureInfo_Current].T_Scale / 32 / Textures[TextureInfo_Current].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR2_2, CurrentG2_2, CurrentB2_2, CurrentA2_2); } 
		glNormal3f (CurrentR2_2 / 255.0f, CurrentG2_2 / 255.0f, CurrentB2_2 / 255.0f);
		glVertex3d(CurrentX2_2, CurrentY2_2, CurrentZ2_2);
		TempU = (float) CurrentH2_3 * Textures[TextureInfo_Current].S_Scale / 32 / Textures[TextureInfo_Current].WidthRender;
		TempV = (float) CurrentV2_3 * Textures[TextureInfo_Current].T_Scale / 32 / Textures[TextureInfo_Current].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR2_3, CurrentG2_3, CurrentB2_3, CurrentA2_3); }
		glNormal3f (CurrentR2_3 / 255.0f, CurrentG2_3 / 255.0f, CurrentB2_3 / 255.0f);
		glVertex3d(CurrentX2_3, CurrentY2_3, CurrentZ2_3);
	glEnd();
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDTEXTURE - F3DEX2_TEXTURE */
int Viewer_RenderMap_CMDTexture()
{
	Textures[TextureInfo_Current].S_Scale = ((Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6) + 2;
	Textures[TextureInfo_Current].T_Scale = ((Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8) + 2;
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDSETTIMAGE - G_SETTIMG - GET TEXTURE OFFSET AND FORMAT AND STORE FOR FUTURE USE */
int Viewer_RenderMap_CMDSetTImage()
{
	Textures[TextureInfo_Current].DataSource = Readout_CurrentByte5;
	
	Textures[TextureInfo_Current].Offset = Readout_CurrentByte6 << 16;
	Textures[TextureInfo_Current].Offset = Textures[TextureInfo_Current].Offset + (Readout_CurrentByte7 << 8);
	Textures[TextureInfo_Current].Offset = Textures[TextureInfo_Current].Offset + Readout_CurrentByte8;
	
	Textures[TextureInfo_Current].Format_N64 = Readout_CurrentByte2;
	
	Textures[TextureInfo_Current].Format_OGL = GL_RGBA;
	Textures[TextureInfo_Current].Format_OGLPixel = GL_RGBA;
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDSETTILE - G_SETTILE - GET TEXTURE PROPERTIES AND STORE THEM FOR FUTURE USE (MIRROR, ETC.) */
int Viewer_RenderMap_CMDSetTile()
{
	unsigned char TempXParameter = Readout_CurrentByte7 * 0x10;
	
	LineSize = Readout_CurrentByte3 / 2;
	
	switch(Readout_CurrentByte6) {
	case 0x01:
		Textures[TextureInfo_Current].Y_Parameter = 1;
		break;
	case 0x09:
		Textures[TextureInfo_Current].Y_Parameter = 2;
		break;
	case 0x05:
		Textures[TextureInfo_Current].Y_Parameter = 3;
		break;
	default:
		Textures[TextureInfo_Current].Y_Parameter = 1;
		break;
	}
	
	switch(TempXParameter) {
	case 0x00:
		Textures[TextureInfo_Current].X_Parameter = 1;
		break;
	case 0x20:
		Textures[TextureInfo_Current].X_Parameter = 2;
		break;
	case 0x10:
		Textures[TextureInfo_Current].X_Parameter = 3;
		break;
	default:
		Textures[TextureInfo_Current].X_Parameter = 1;
		break;
	}
	
	return 0;
}

/* VIEWER_RENDERMAP_CMDLOADBLOCK - G_LOADBLOCK - UNHANDLED */
int Viewer_RenderMap_CMDLoadBlock()
{
	LBUpper_Y = Readout_CurrentByte2 << 4;
	LBUpper_Y += Readout_CurrentByte3 >> 4;
	LBUpper_X = Readout_CurrentByte3 << 28;
	LBUpper_X >>= 20;
	LBUpper_X += Readout_CurrentByte4;
	
	LowerRight_X = Readout_CurrentByte6 << 4;
	LowerRight_X += Readout_CurrentByte7 >> 4;
	LowerRight_X += 1;
	ScanlineChange_Y = Readout_CurrentByte7 << 28;
	ScanlineChange_Y >>= 20;
	ScanlineChange_Y += Readout_CurrentByte8;
	
//	sprintf(StatusMsg, "UNHANDLED LOADBLOCK!\n\n%02X%02X%02X = \nChange on Y per Scanline: %d\nLower-right X Coord: %d", Readout_CurrentByte6, Readout_CurrentByte7, Readout_CurrentByte8, ScanlineChange_Y, LowerRight_X);
//	MessageBox(hwnd, StatusMsg, "", 0);
	
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
	
	Textures[TextureInfo_Current].Width  = ((LRS - ULS) + 1);// * Textures[TextureInfo_Current].S_Scale;
	Textures[TextureInfo_Current].Height = ((LRT - ULT) + 1);// * Textures[TextureInfo_Current].T_Scale;
	
	if(Textures[TextureInfo_Current].Width > 256) {
		Textures[TextureInfo_Current].WidthRender  = Textures[TextureInfo_Current].Width - 64;
	} else {
		Textures[TextureInfo_Current].WidthRender  = Textures[TextureInfo_Current].Width;
	}
	
	if(Textures[TextureInfo_Current].Height > 256) {
		Textures[TextureInfo_Current].HeightRender = Textures[TextureInfo_Current].Height - 64;
	} else {
		Textures[TextureInfo_Current].HeightRender = Textures[TextureInfo_Current].Height;
	}
	
	Viewer_LoadTexture();
	
	return 0;
}

/* VIEWER_LOADTEXTURE - CALLED FROM VIEWER_RENDERMAP_CMDSETTILESIZE, FETCH THE TEXTURE DATA AND CREATE AN OGL TEXTURE */
GLuint Viewer_LoadTexture()
{
	unsigned long TextureDataSize = (Textures[TextureInfo_Current].Width * Textures[TextureInfo_Current].Height) * 4;
	
	TextureData_OGL = (unsigned char *) malloc ((Textures[TextureInfo_Current].Width * Textures[TextureInfo_Current].Height) * 4);
	TextureData_N64 = (unsigned char *) malloc ((Textures[TextureInfo_Current].Width * Textures[TextureInfo_Current].Height) * 4);
	
	memset(TextureData_OGL, 0x00, TextureDataSize);
	memset(TextureData_N64, 0x00, TextureDataSize);
	
	unsigned char * EmptyTexture;
	EmptyTexture = (unsigned char *) malloc (TextureDataSize);
	memset(EmptyTexture, 0xFF, TextureDataSize);
	
	int i, j, k = 0;
	
	switch(Textures[TextureInfo_Current].Format_N64) {
	/* RGBA FORMAT */
	case 0x00:
	case 0x08:
	case 0x10:
		/* STATUS: seems to be fully correct */
		{
		switch(Textures[TextureInfo_Current].DataSource) {
		case 0x02:
			memcpy(TextureData_N64, &ZSceneBuffer[Textures[TextureInfo_Current].Offset / 4], TextureDataSize);
			break;
		case 0x03:
			memcpy(TextureData_N64, &ZMapBuffer[Textures[TextureInfo_Current].Offset / 4], TextureDataSize);
			break;
		default:
			memcpy(TextureData_N64, EmptyTexture, TextureDataSize);
			break;
		}
		
		unsigned int LoadRGBA_RGBA5551 = 0;
		
		unsigned int LoadRGBA_RExtract = 0;
		unsigned int LoadRGBA_GExtract = 0;
		unsigned int LoadRGBA_BExtract = 0;
		unsigned int LoadRGBA_AExtract = 0;
		
		unsigned int LoadRGBA_InTexturePosition_N64 = 0;
		unsigned int LoadRGBA_InTexturePosition_OGL = 0;
		
		for(j = 0; j < Textures[TextureInfo_Current].Height; j++) {
			for(i = 0; i < Textures[TextureInfo_Current].Width; i++) {
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
		}
		break;
		}
	/* CI FORMAT */
	case 0x40:
	case 0x48:
	case 0x50:
		/* STATUS: unfinished, currently writing semi-garbage data */
		{
		switch(Textures[TextureInfo_Current].DataSource) {
		case 0x02:
			memcpy(TextureData_N64, &ZSceneBuffer[Textures[TextureInfo_Current].Offset / 4], TextureDataSize);
			break;
		case 0x03:
			memcpy(TextureData_N64, &ZMapBuffer[Textures[TextureInfo_Current].Offset / 4], TextureDataSize);
			break;
		default:
			memcpy(TextureData_N64, EmptyTexture, TextureDataSize);
			break;
		}
		
		unsigned char Nibble_1;
		unsigned char Nibble_2;
		
		for(i = 0; i < (Textures[TextureInfo_Current].Width * Textures[TextureInfo_Current].Height); i += 2) {
			Nibble_1 = TextureData_N64[i / 2] >> 4;
			Nibble_2 = TextureData_N64[i / 2] << 4;
			Nibble_2 >>= 4;
			TextureData_OGL[k] = Nibble_1 * 0x88;
			TextureData_OGL[k + 1] = Nibble_1 * 0x88;
			TextureData_OGL[k + 2] = Nibble_1 * 0x88;
			TextureData_OGL[k + 3] = 0xFF;
			TextureData_OGL[k + 4] = Nibble_2 * 0x88;
			TextureData_OGL[k + 5] = Nibble_2 * 0x88;
			TextureData_OGL[k + 6] = Nibble_2 * 0x88;
			TextureData_OGL[k + 7] = 0xFF;
			
			k += 8;
		}
		break;
		}
	/* IA FORMAT */
	case 0x60:
	case 0x68:
	case 0x70:
		/* STATUS: looking good, converting loop might need an overhaul like the RGBA format's */
		{
		switch(Textures[TextureInfo_Current].DataSource) {
		case 0x02:
			memcpy(TextureData_N64, &ZSceneBuffer[Textures[TextureInfo_Current].Offset / 4], TextureDataSize);
			break;
		case 0x03:
			memcpy(TextureData_N64, &ZMapBuffer[Textures[TextureInfo_Current].Offset / 4], TextureDataSize);
			break;
		default:
			memcpy(TextureData_N64, EmptyTexture, TextureDataSize);
			break;
		}
		
		unsigned char Brightness;
		unsigned char Alpha;
		
		for(i = 0; i < (Textures[TextureInfo_Current].Width * Textures[TextureInfo_Current].Height) / 2; i++) {
			Brightness = TextureData_N64[i] / 0x10;
			Alpha = TextureData_N64[i] * 0x10;
			Alpha /= 0x10;
			TextureData_OGL[i * 8] = Brightness * 0x11;
			TextureData_OGL[i * 8 + 1] = Brightness * 0x11;
			TextureData_OGL[i * 8 + 2] = Brightness * 0x11;
			TextureData_OGL[i * 8 + 3] = Alpha * 0x88;
			
			TextureData_OGL[i * 8 + 4] = Brightness * 0x11;
			TextureData_OGL[i * 8 + 5] = Brightness * 0x11;
			TextureData_OGL[i * 8 + 6] = Brightness * 0x11;
			TextureData_OGL[i * 8 + 7] = Alpha * 0x88;
		}
		
		break;
		}
	/* I FORMAT */
	case 0x80:
	case 0x88:
	case 0x90:
		/* STATUS: looking good, minus missing alpha on the pathways, need to check how that works */
		{
		switch(Textures[TextureInfo_Current].DataSource) {
		case 0x02:
			memcpy(TextureData_N64, &ZSceneBuffer[Textures[TextureInfo_Current].Offset / 4], TextureDataSize);
			break;
		case 0x03:
			memcpy(TextureData_N64, &ZMapBuffer[Textures[TextureInfo_Current].Offset / 4], TextureDataSize);
			break;
		default:
			memcpy(TextureData_N64, EmptyTexture, TextureDataSize);
			break;
		}
		
		unsigned char Brightness;
		
		for(i = 0; i < (Textures[TextureInfo_Current].Width * Textures[TextureInfo_Current].Height) / 2; i++) {
			Brightness = TextureData_N64[i];
			Brightness >>= 4;
			TextureData_OGL[i * 8] = Brightness * 0x11;
			TextureData_OGL[i * 8 + 1] = Brightness * 0x11;
			TextureData_OGL[i * 8 + 2] = Brightness * 0x11;
			TextureData_OGL[i * 8 + 3] = 0xFF;
			
			Brightness = TextureData_N64[i];
			Brightness <<= 4;
			Brightness >>= 4;
			TextureData_OGL[i * 8 + 4] = Brightness * 0x11;
			TextureData_OGL[i * 8 + 5] = Brightness * 0x11;
			TextureData_OGL[i * 8 + 6] = Brightness * 0x11;
			TextureData_OGL[i * 8 + 7] = 0xFF;
		}
		break;
		}
	/* FALLBACK - gives us an empty texture */
	default:
		{
		memcpy(TextureData_N64, EmptyTexture, TextureDataSize);
		break;
		}
	}
	
	switch(Textures[TextureInfo_Current].Y_Parameter) {
		case 1:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); break;
		case 2:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); break;
		case 3:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB); break;
		default: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); break;
	}
	
	switch(Textures[TextureInfo_Current].X_Parameter) {
		case 1:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); break;
		case 2:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); break;
		case 3:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB); break;
		default: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); break;
	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, Textures[TextureInfo_Current].Format_OGL, Textures[TextureInfo_Current].WidthRender, Textures[TextureInfo_Current].HeightRender, 0, Textures[TextureInfo_Current].Format_OGLPixel, GL_UNSIGNED_BYTE, TextureData_OGL);
	
	free(TextureData_N64);
	free(TextureData_OGL);
	
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
		
	return 0;
}

int Viewer_RenderMap_CMDSetFogColor()
{
	FogColor[0] = (Readout_CurrentByte5 / 255.0f);
	FogColor[1] = (Readout_CurrentByte6 / 255.0f);
	FogColor[2] = (Readout_CurrentByte7 / 255.0f);
	FogColor[3] = (Readout_CurrentByte8 / 255.0f);
	
	glFogfv(GL_FOG_COLOR, FogColor);
	
	return 0;
}

int Viewer_RenderMap_CMDRDPHalf1()
{
/*	Readout_Current1_Backup = Readout_Current1;
	Readout_Current2_Backup = Readout_Current2;
	Readout_CurrentByte1_Backup = Readout_CurrentByte1;
	Readout_CurrentByte2_Backup = Readout_CurrentByte2;
	Readout_CurrentByte3_Backup = Readout_CurrentByte3;
	Readout_CurrentByte4_Backup = Readout_CurrentByte4;
	Readout_CurrentByte5_Backup = Readout_CurrentByte5;
	Readout_CurrentByte6_Backup = Readout_CurrentByte6;
	Readout_CurrentByte7_Backup = Readout_CurrentByte7;
	Readout_CurrentByte8_Backup = Readout_CurrentByte8;
	*/
	bool Sub_DListHasEnded = false;
	
	Sub_DLTempPosition = Readout_CurrentByte6 << 16;
	Sub_DLTempPosition = Sub_DLTempPosition + (Readout_CurrentByte7 << 8);
	Sub_DLTempPosition = Sub_DLTempPosition + Readout_CurrentByte8;
	
	while (!Sub_DListHasEnded) {
		memcpy(&Readout_Current1, &ZMapBuffer[Sub_DLTempPosition], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[Sub_DLTempPosition + 1], 4);
		
		HelperFunc_SplitCurrentVals(true, true);
		
		switch(Readout_CurrentByte1) {
		case F3DEX2_VTX:
			sprintf(CurrentGFXCmd, "F3DEX2_VTX           ");
			sprintf(CurrentGFXCmdNote, "");
			Viewer_RenderMap_CMDVertexList();
			break;
		case F3DEX2_TRI1:
			sprintf(CurrentGFXCmd, "F3DEX2_TRI1          ");
			sprintf(CurrentGFXCmdNote, "");
			Viewer_RenderMap_CMDDrawTri1();
			break;
		case F3DEX2_TRI2:
			sprintf(CurrentGFXCmd, "F3DEX2_TRI2          ");
			sprintf(CurrentGFXCmdNote, "");
			Viewer_RenderMap_CMDDrawTri2();
			break;
		case F3DEX2_TEXTURE:
			sprintf(CurrentGFXCmd, "F3DEX2_TEXTURE       ");
			sprintf(CurrentGFXCmdNote, "<unhandled>");
			Viewer_RenderMap_CMDTexture();
			break;
		case G_SETTIMG:
			sprintf(CurrentGFXCmd, "G_SETTIMG            ");
			sprintf(CurrentGFXCmdNote, "");
			Viewer_RenderMap_CMDSetTImage();
			break;
		case G_SETTILE:
			sprintf(CurrentGFXCmd, "G_SETTILE            ");
			sprintf(CurrentGFXCmdNote, "");
			Viewer_RenderMap_CMDSetTile();
			break;
		case G_LOADBLOCK:
			sprintf(CurrentGFXCmd, "G_LOADBLOCK          ");
			sprintf(CurrentGFXCmdNote, "<unhandled>");
			Viewer_RenderMap_CMDLoadBlock();
			break;
		case G_SETTILESIZE:
			sprintf(CurrentGFXCmd, "G_SETTILESIZE        ");
			sprintf(CurrentGFXCmdNote, "");
			Viewer_RenderMap_CMDSetTileSize();
			break;
		case G_RDPFULLSYNC:
			sprintf(CurrentGFXCmd, "G_RDPFULLSYNC        ");
			sprintf(CurrentGFXCmdNote, "<unhandled>");
			break;
		case G_RDPTILESYNC:
			sprintf(CurrentGFXCmd, "G_RDPTILESYNC        ");
			sprintf(CurrentGFXCmdNote, "<unhandled>");
			break;
		case G_RDPPIPESYNC:
			sprintf(CurrentGFXCmd, "G_RDPPIPESYNC        ");
			sprintf(CurrentGFXCmdNote, "<unhandled>");
			break;
		case G_RDPLOADSYNC:
			sprintf(CurrentGFXCmd, "G_RDPLOADSYNC        ");
			sprintf(CurrentGFXCmdNote, "<unhandled>");
			break;
		case F3DEX2_GEOMETRYMODE:
			sprintf(CurrentGFXCmd, "F3DEX2_GEOMETRYMODE  ");
			sprintf(CurrentGFXCmdNote, "");
			Viewer_RenderMap_CMDGeometryMode();
			break;
		case F3DEX2_CULLDL:
			sprintf(CurrentGFXCmd, "F3DEX2_CULLDL        ");
			sprintf(CurrentGFXCmdNote, "<unhandled>");
			break;
		case F3DEX2_SETOTHERMODE_H:
			sprintf(CurrentGFXCmd, "F3DEX2_SETOTHERMODE_H");
			sprintf(CurrentGFXCmdNote, "<unhandled>");
			break;
		case F3DEX2_SETOTHERMODE_L:
			sprintf(CurrentGFXCmd, "F3DEX2_SETOTHERMODE_L");
			sprintf(CurrentGFXCmdNote, "<unhandled>");
			break;
		case G_SETFOGCOLOR:
			sprintf(CurrentGFXCmd, "G_SETFOGCOLOR        ");
			sprintf(CurrentGFXCmdNote, "");
			Viewer_RenderMap_CMDSetFogColor();
			break;
		case F3DEX2_ENDDL:
			sprintf(CurrentGFXCmd, "F3DEX2_ENDDL         ");
			sprintf(CurrentGFXCmdNote, "");
			Sub_DListHasEnded = true;
			break;
		default:
			sprintf(CurrentGFXCmd, "<unknown>            ");
			sprintf(CurrentGFXCmdNote, "<unhandled>");
			break;
		}
		
		sprintf(GFXLogMsg, "   - 0x%08X:\t%s\t\t[%02X%02X%02X%02X %02X%02X%02X%02X] %s\n",
			Sub_DLTempPosition * 4, CurrentGFXCmd,
			Readout_CurrentByte1, Readout_CurrentByte2, Readout_CurrentByte3, Readout_CurrentByte4,
			Readout_CurrentByte5, Readout_CurrentByte6, Readout_CurrentByte7, Readout_CurrentByte8,
			CurrentGFXCmdNote);
		HelperFunc_GFXLogMessage(GFXLogMsg);
		
		Sub_DLTempPosition += 2;
	}
	
/*	Readout_Current1 = Readout_Current1_Backup;
	Readout_Current2 = Readout_Current2_Backup;
	Readout_CurrentByte1 = Readout_CurrentByte1_Backup;
	Readout_CurrentByte2 = Readout_CurrentByte2_Backup;
	Readout_CurrentByte3 = Readout_CurrentByte3_Backup;
	Readout_CurrentByte4 = Readout_CurrentByte4_Backup;
	Readout_CurrentByte5 = Readout_CurrentByte5_Backup;
	Readout_CurrentByte6 = Readout_CurrentByte6_Backup;
	Readout_CurrentByte7 = Readout_CurrentByte7_Backup;
	Readout_CurrentByte8 = Readout_CurrentByte8_Backup;
	*/
	return 0;
}

/*	------------------------------------------------------------ */

/* VIEWER_RENDERACTOR - RENDERS AN INFAMOUS 'ACTOR CUBE' AT THE SPECIFIED LOCATION ON THE MAP */
int Viewer_RenderActor(int ActorToRender, GLshort X, GLshort Y, GLshort Z, GLshort X_Rot, GLshort Y_Rot, GLshort Z_Rot, bool IsMapActor)
{
	/* rotation values are being ignored at the moment */
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
		
		glVertex3s( 15 + X, 15 + Y, 15 + Z);   //V2
		glVertex3s( 15 + X,-15 + Y, 15 + Z);   //V1
		glVertex3s( 15 + X,-15 + Y,-15 + Z);   //V3
		glVertex3s( 15 + X, 15 + Y,-15 + Z);   //V4
		
		glVertex3s( 15 + X, 15 + Y,-15 + Z);   //V4
		glVertex3s( 15 + X,-15 + Y,-15 + Z);   //V3
		glVertex3s(-15 + X,-15 + Y,-15 + Z);   //V5
		glVertex3s(-15 + X, 15 + Y,-15 + Z);   //V6
		
		glVertex3s(-15 + X, 15 + Y,-15 + Z);   //V6
		glVertex3s(-15 + X,-15 + Y,-15 + Z);   //V5
		glVertex3s(-15 + X,-15 + Y, 15 + Z);   //V7
		glVertex3s(-15 + X, 15 + Y, 15 + Z);   //V8
		
		glVertex3s(-15 + X, 15 + Y,-15 + Z);   //V6
		glVertex3s(-15 + X, 15 + Y, 15 + Z);   //V8
		glVertex3s( 15 + X, 15 + Y, 15 + Z);   //V2
		glVertex3s( 15 + X, 15 + Y,-15 + Z);   //V4
		
		glVertex3s(-15 + X,-15 + Y, 15 + Z);   //V7
		glVertex3s(-15 + X,-15 + Y,-15 + Z);   //V5
		glVertex3s( 15 + X,-15 + Y,-15 + Z);   //V3
		glVertex3s( 15 + X,-15 + Y, 15 + Z);   //V1
		
		//front
		glVertex3s(-15 + X, 15 + Y, 15 + Z);   //V8
		glVertex3s(-15 + X,-15 + Y, 15 + Z);   //V7
		glVertex3s( 15 + X,-15 + Y, 15 + Z);   //V1
		glVertex3s( 15 + X, 15 + Y, 15 + Z);   //V2
	glEnd();
	
	return 0;
}

/*	------------------------------------------------------------ */

/* HELPERFUNC_SPLITCURRENTVALS - TAKES THE 4 BYTES OF THE LONG VARIABLE(S) USED FOR DATA FETCHING AND SPLITS THEM APART */
void HelperFunc_SplitCurrentVals(bool SplitDual, bool CreateBackup)
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

/* HELPERFUNC_GFXLOGMESSAGE - WRITES GIVEN STRING INTO PREVIOUSLY OPENED LOG FILE */
int HelperFunc_GFXLogMessage(char Message[])
{
	fprintf(FileGFXLog, Message);
	
	return 0;
}

int HelperFunc_GFXLogCommand()
{				
	sprintf(GFXLogMsg, " - 0x%08X:\t%s\t\t[%02X%02X%02X%02X %02X%02X%02X%02X] %s\n",
		DLTempPosition * 4, CurrentGFXCmd,
		Readout_CurrentByte1, Readout_CurrentByte2, Readout_CurrentByte3, Readout_CurrentByte4,
		Readout_CurrentByte5, Readout_CurrentByte6, Readout_CurrentByte7, Readout_CurrentByte8,
		CurrentGFXCmdNote);
	HelperFunc_GFXLogMessage(GFXLogMsg);
	
	return 0;
}

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
	
	glAlphaFunc(GL_GEQUAL, 0.5);
	glEnable(GL_ALPHA_TEST);
	
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_DENSITY, 0.15f);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 1.0f);
	glFogf(GL_FOG_END, 15.0f);
	
	BuildFont();
	
	return true;
}

/* DRAWGLSCENE - DRAW THE CURRENT SCENE USING THE MAP AND ACTOR DATA GATHERED BEFORE */
int DrawGLScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(MapLoaded) {
		HelperFunc_CalculateFPS();
		
		sprintf(Renderer_CoordDisp, "Cam X: %4.2f, Y: %4.2f, Z: %4.2f", CamX, CamY, CamZ);
		SendMessage(hstatus, SB_SETTEXT, 1, (LPARAM)Renderer_CoordDisp);
		
		if(Renderer_EnableMapActors) {
			glLoadIdentity();
			glTranslatef(-0.5f, 0.32f, -1.0f);
			
			if (MapHeader[MapHeader_Current].Actor_Count > 0) {
				sprintf(MapActorMsg, "Map Actor: #%d, Type %04X, Variable %04X, X: %d, Y: %d, Z: %d", ActorInfo_Selected, Actors[ActorInfo_Selected].Number, Actors[ActorInfo_Selected].Variable, Actors[ActorInfo_Selected].X_Position, Actors[ActorInfo_Selected].Y_Position, Actors[ActorInfo_Selected].Z_Position);
			} else {
				sprintf(SceneActorMsg, "No Map Actors found.");
			}
			
			glColor3f(0.0f, 0.0f, 0.0f);
			glRasterPos2f(0.003f, -0.003f);
			glPrint("%s", MapActorMsg);
			
			glColor3f(1.0f, 1.0f, 1.0f);
			glRasterPos2f(0.0f, 0.0f);
			glPrint("%s", MapActorMsg);
		} else {
			//
		}
		
		if(Renderer_EnableSceneActors) {
			glLoadIdentity();
			glTranslatef(-0.5f, 0.32f, -1.0f);
			
			if (SceneHeader[SceneHeader_Current].ScActor_Count > 0) {
				sprintf(SceneActorMsg, "Scene Actor: #%d, Type %04X, Variable %04X, X: %d, Y: %d, Z: %d", ScActorInfo_Selected, ScActors[ScActorInfo_Selected].Number, ScActors[ScActorInfo_Selected].Variable, ScActors[ScActorInfo_Selected].X_Position, ScActors[ScActorInfo_Selected].Y_Position, ScActors[ScActorInfo_Selected].Z_Position);
			} else {
				sprintf(SceneActorMsg, "No Scene Actors found.");
			}
		
			glColor3f(0.0f, 0.0f, 0.0f);
			glRasterPos2f(0.003f, -0.028f);
			glPrint("%s", SceneActorMsg);
			
			glColor3f(1.0f, 1.0f, 1.0f);
			glRasterPos2f(0.0f, -0.025f);
			glPrint("%s", SceneActorMsg);
		} else {
			//
		}
		
		glLoadIdentity();
		
		gluLookAt(CamX, CamY, CamZ, 
				CamX + CamLX, CamY + CamLY, CamZ + CamLZ,
				0.0f, 1.0f, 0.0f);
		
		glScalef(0.005, 0.005, 0.005);
		
		glEnable(GL_TEXTURE_2D);
		glCallList(Renderer_GLDisplayList);
		
		ActorInfo_CurrentCount = 0;
		ScActorInfo_CurrentCount = 0;
		
		if (Renderer_EnableMapActors) {
			if (MapHeader[MapHeader_Current].Actor_Count > 0) {
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_FOG);
				
				while (!(ActorInfo_CurrentCount == MapHeader[MapHeader_Current].Actor_Count)) {
					glEnable(GL_LIGHT1);
					glDisable(GL_LIGHTING);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					Viewer_RenderActor(ActorInfo_CurrentCount, Actors[ActorInfo_CurrentCount].X_Position, Actors[ActorInfo_CurrentCount].Y_Position, Actors[ActorInfo_CurrentCount].Z_Position, Actors[ActorInfo_CurrentCount].X_Rotation, Actors[ActorInfo_CurrentCount].Y_Rotation, Actors[ActorInfo_CurrentCount].Z_Rotation, true);
					glEnable(GL_LIGHTING);
					glDisable(GL_LIGHT1);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glEnable(GL_POLYGON_OFFSET_LINE);
					glPolygonOffset(-1.0f,-1.0f);
					glColor3f(1.0f, 1.0f, 1.0f);
					Viewer_RenderActor(ActorInfo_CurrentCount, Actors[ActorInfo_CurrentCount].X_Position, Actors[ActorInfo_CurrentCount].Y_Position, Actors[ActorInfo_CurrentCount].Z_Position, Actors[ActorInfo_CurrentCount].X_Rotation, Actors[ActorInfo_CurrentCount].Y_Rotation, Actors[ActorInfo_CurrentCount].Z_Rotation ,true);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glEnable(GL_LIGHT1);
					glDisable(GL_LIGHTING);
					ActorInfo_CurrentCount++;
				}
			}
		}
		
		if (Renderer_EnableSceneActors) {
			if (SceneHeader[SceneHeader_Current].ScActor_Count > 0) {
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_FOG);
				
				while (!(ScActorInfo_CurrentCount == SceneHeader[SceneHeader_Current].ScActor_Count)) {
					glEnable(GL_LIGHT1);
					glDisable(GL_LIGHTING);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					Viewer_RenderActor(ScActorInfo_CurrentCount, ScActors[ScActorInfo_CurrentCount].X_Position, ScActors[ScActorInfo_CurrentCount].Y_Position, ScActors[ScActorInfo_CurrentCount].Z_Position, ScActors[ScActorInfo_CurrentCount].X_Rotation, ScActors[ScActorInfo_CurrentCount].Y_Rotation, ScActors[ScActorInfo_CurrentCount].Z_Rotation, false);
					glEnable(GL_LIGHTING);
					glDisable(GL_LIGHT1);
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glEnable(GL_POLYGON_OFFSET_LINE);
					glPolygonOffset(-1.0f,-1.0f);
					glColor3f(1.0f, 1.0f, 1.0f);
					Viewer_RenderActor(ScActorInfo_CurrentCount, ScActors[ScActorInfo_CurrentCount].X_Position, ScActors[ScActorInfo_CurrentCount].Y_Position, ScActors[ScActorInfo_CurrentCount].Z_Position, ScActors[ScActorInfo_CurrentCount].X_Rotation, ScActors[ScActorInfo_CurrentCount].Y_Rotation, ScActors[ScActorInfo_CurrentCount].Z_Rotation, false);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glEnable(GL_LIGHT1);
					glDisable(GL_LIGHTING);
					ScActorInfo_CurrentCount++;
				}
			}
		}
	}
	
	return true;
}

/* RESIZEGLSCENE - RESIZES THE OPENGL RENDERING TARGET ALONG WITH THE MAIN WINDOW */
void ReSizeGLScene(GLsizei width, GLsizei height)
{
	if (height==0)
	{
		height=1;
	}
	
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	OGLTargetWidth = width;
	OGLTargetHeight = height;
}

/* KILLGLTARGET - DESTROYS THE OPENGL RENDERING TARGET FOR PROPER EXITING */
void KillGLTarget(void)
{
	if (hRC)
	{
		if (!wglMakeCurrent(NULL,NULL))
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;
	}
	
	if (hDC_ogl && !ReleaseDC(hwnd,hDC_ogl))
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC_ogl=NULL;
	}
	
	KillFont();
}

/* CREATEGLTARGET - CREATE AN OPENGL RENDERING TARGET WITH THE SPECIFIED PARAMETERS */
BOOL CreateGLTarget(int width, int height, int bits)
{
	GLuint		PixelFormat;
	
	static	PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		0,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	pfd.cColorBits = bits;
	
	if (!(hDC_ogl=GetDC(hogl)))							// Did We Get A Device Context?
	{
		KillGLTarget();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	if (!(PixelFormat=ChoosePixelFormat(hDC_ogl,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLTarget();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	if(!SetPixelFormat(hDC_ogl,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLTarget();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	if (!(hRC=wglCreateContext(hDC_ogl)))				// Are We Able To Get A Rendering Context?
	{
		KillGLTarget();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	if(!wglMakeCurrent(hDC_ogl,hRC))					// Try To Activate The Rendering Context
	{
		KillGLTarget();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLTarget();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	return TRUE;									// Success
}

void BuildFont(void)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping
	
	GLFontBase = glGenLists(96);						// Storage For 96 Characters
	
	font = CreateFont(	-12,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FALSE,							// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Arial");						// Font Name
	
	oldfont = (HFONT)SelectObject(hDC_ogl, font);           // Selects The Font We Want
	wglUseFontBitmaps(hDC_ogl, 32, 96, GLFontBase);			// Builds 96 Characters Starting At Character 32
	SelectObject(hDC_ogl, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
}

void KillFont(void)									// Delete The Font List
{
	glDeleteLists(GLFontBase, 96);						// Delete All 96 Characters
}

void glPrint(const char *fmt, ...)					// Custom GL "Print" Routine
{
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments
	
	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing
	
	va_start(ap, fmt);									// Parses The String For Variables
		vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text
	
	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(GLFontBase - 32);						// Sets The Base Character to 32
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
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
	char			filter[]="Zelda Map files (*.zmap)\0;*.zmap\0";
	OPENFILENAME	ofn;
	int				hh,index;
	
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.hwndOwner=hwnd;
	ofn.lpstrFilter=filter;
	ofn.nFilterIndex=1;
	ofn.lpstrFile=Filename_ZMap;
	ofn.nMaxFile=256;
	ofn.Flags=OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
	GetOpenFileName(&ofn);
	
	return;
}

/* DIALOG_OPENZSCENE - COMMON DIALOG USED FOR SELECTING THE SCENE FILE */
void Dialog_OpenZScene(HWND hwnd)
{
	char			filter[]="Zelda Scene files (*.zscene)\0*.zscene\0";
	OPENFILENAME	ofn;
	int				hh,index;
	
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.hwndOwner=hwnd;
	ofn.lpstrFilter=filter;
	ofn.nFilterIndex=1;
	ofn.lpstrFile=Filename_ZScene;
	ofn.nMaxFile=256;
	ofn.Flags=OFN_EXPLORER|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
	GetOpenFileName(&ofn);
	
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
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
	wincl.lpszMenuName = MAKEINTRESOURCE(IDM_MAINMENU);
	
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
							MapLoaded = false;
							Viewer_RenderMap(DListInfo_DListToRender);
						}
						if(DListInfo_DListToRender == -1) {
							sprintf(StatusMsg, "Display List: rendering all");
						} else {
							sprintf(StatusMsg, "Display List: #%d", DListInfo_DListToRender + 1);
						}
					}
					if (System_KbdKeys[VK_F2]) {
						System_KbdKeys[VK_F2] = false;
						if(!(DListInfo_DListToRender == DListInfo_CurrentCount)) {
							DListInfo_DListToRender++;
							MapLoaded = false;
							Viewer_RenderMap(DListInfo_DListToRender);
						}
						sprintf(StatusMsg, "Display List: #%d", DListInfo_DListToRender + 1);
					}
					if (System_KbdKeys[VK_F3]) {
						System_KbdKeys[VK_F3] = false;
						if(!(MapHeader_TotalCount == 0)) {
							if(!(MapHeader_Current == 0)) {
								MapHeader_Current--;
								Viewer_GetMapHeader(MapHeader_Current);
								Viewer_GetMapActors(MapHeader_Current);
							}
							sprintf(StatusMsg, "Map Header: #%d", MapHeader_Current);
						}
					}
					if (System_KbdKeys[VK_F4]) {
						System_KbdKeys[VK_F4] = false;
						if(!(MapHeader_TotalCount == 0)) {
							if(!(MapHeader_Current == MapHeader_TotalCount - 1)) {
								MapHeader_Current++;
								Viewer_GetMapHeader(MapHeader_Current);
								Viewer_GetMapActors(MapHeader_Current);
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
								Viewer_GetSceneHeader(SceneHeader_Current);
								Viewer_GetSceneActors(SceneHeader_Current);
							}
							sprintf(StatusMsg, "Scene Header: #%d", SceneHeader_Current);
						}
					}
					if (System_KbdKeys[VK_MULTIPLY]) {
						System_KbdKeys[VK_MULTIPLY] = false;
						if(!(SceneHeader_TotalCount == 0)) {
							if(!(SceneHeader_Current == SceneHeader_TotalCount - 1)) {
								SceneHeader_Current++;
								Viewer_GetSceneHeader(SceneHeader_Current);
								Viewer_GetSceneActors(SceneHeader_Current);
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
						GLUTCamera_Orientation(CamAngleX, CamAngleY);
					}
					if (System_KbdKeys[VK_RIGHT]) {
						CamAngleX += 0.02f;
						GLUTCamera_Orientation(CamAngleX, CamAngleY);
					}
					
					if (System_KbdKeys[VK_UP]) {
						CamAngleY += 0.01f;
						GLUTCamera_Orientation(CamAngleX, CamAngleY);
					}
					if (System_KbdKeys[VK_DOWN]) {
						CamAngleY -= 0.01f;
						GLUTCamera_Orientation(CamAngleX, CamAngleY);
					}
					
					if (System_KbdKeys[VK_TAB]) {
						System_KbdKeys[VK_TAB] = false;
						CamAngleX = 0.0f, CamAngleY = 0.0f;
						CamX = 0.0f, CamY = 0.0f, CamZ = 5.0f;
						CamLX = 0.0f, CamLY = 0.0f, CamLZ = -1.0f;
					}
					
					if (System_KbdKeys['1']) {
						DListInfo_DListToRender = 28;
						MapLoaded = false;
						Viewer_RenderMap(DListInfo_DListToRender);
					}
					
					SendMessage(hstatus, SB_SETTEXT, 1, (LPARAM)Renderer_CoordDisp);
					SendMessage(hstatus, SB_SETTEXT, 2, (LPARAM)StatusMsg);
					
					fclose(FileGFXLog);
				}
			}
			
			DrawGLScene();
			SwapBuffers(hDC_ogl);
		}
	}
	KillGLTarget();
	DestroyWindow(hwnd);
	
	return (messages.wParam);
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_ACTIVATE:
			if (!HIWORD(wParam)) {
				WndActive = true;
			} else {
				WndActive = false;
			}
			break;
			
		case WM_SIZE: ;
			HWND hogl;
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			hogl = GetDlgItem(hwnd, IDC_MAIN_OPENGL);
			SetWindowPos(hogl, NULL, 0, 0, rcClient.right, rcClient.bottom - 25, SWP_NOZORDER);
			
			RECT rcStatus;
			GetDlgItem(hwnd, IDC_MAIN_STATUSBAR);
			SendMessage(hstatus, WM_SIZE, 0, 0);
			GetWindowRect(hstatus, &rcStatus);
			StatusBarHeight = rcStatus.bottom - rcStatus.top;
			
			ReSizeGLScene(rcClient.right, rcClient.bottom);
			DrawGLScene();
			SwapBuffers(hDC_ogl);
			
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDM_FILE_OPEN:
					Dialog_OpenZMap(hwnd);
					Dialog_OpenZScene(hwnd);
					Viewer_Initialize();
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
				case IDM_HELP_ABOUT: ;
					char AboutMsg[256] = "";
					sprintf(AboutMsg, "%s %s (Build '%s') - OpenGL Zelda Map Viewer\n\nWritten in October 2008 by xdaniel\nhttp://magicstone.de/dzd/", AppTitle, AppVersion, AppBuildName);
					MessageBox(hwnd, AboutMsg, "About", MB_OK | MB_ICONINFORMATION);
					break;
			}
			break;
			
		case WM_CLOSE:
			ExitProgram = true;
			break;
			
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
			
		case WM_KEYDOWN:
			System_KbdKeys[wParam] = true;
			break;
			
		case WM_KEYUP:
			System_KbdKeys[wParam] = false;
			break;
			
		case WM_LBUTTONDOWN:
			MouseButtonDown = true;
			MouseCenterX = (signed int)LOWORD(lParam);
			MouseCenterY = (signed int)HIWORD(lParam);
			break;
			
		case WM_LBUTTONUP:
			MouseButtonDown = false;
			break;
			
		case WM_MOUSEMOVE:
			if((MouseButtonDown) && (WndActive)) {
				MousePosX = (signed int)LOWORD(lParam);
				MousePosY = (signed int)HIWORD(lParam);
				Camera_MouseMove(MousePosX, MousePosY);
				GLUTCamera_Orientation(CamAngleX, CamAngleY);
			}
			break;
			
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
	
	return 0;
}

/*	------------------------------------------------------------ */
