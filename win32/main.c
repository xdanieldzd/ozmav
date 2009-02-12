/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	main.c - Windows stuff, basic initialization + camera
	------------------------------------------------------------ */

#include "../globals.h"

/*	------------------------------------------------------------
	SYSTEM FUNCTIONS - OPENGL & WINDOWS
	------------------------------------------------------------ */

int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

PFNGLMULTITEXCOORD1FARBPROC			glMultiTexCoord1fARB		= NULL;
PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB		= NULL;
PFNGLMULTITEXCOORD3FARBPROC			glMultiTexCoord3fARB		= NULL;
PFNGLMULTITEXCOORD4FARBPROC			glMultiTexCoord4fARB		= NULL;
PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB			= NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB	= NULL;

PFNGLGENPROGRAMSARBPROC				glGenProgramsARB			= NULL;
PFNGLBINDPROGRAMARBPROC				glBindProgramARB			= NULL;
PFNGLDELETEPROGRAMSARBPROC			glDeleteProgramsARB			= NULL;
PFNGLPROGRAMSTRINGARBPROC			glProgramStringARB			= NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC	glProgramEnvParameter4fARB = NULL;

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

char			szClassName[] = "OZMAVClass";

/* GENERAL GLOBAL PROGRAM VARIABLES */
bool			System_KbdKeys[256];

char			AppTitle[256] = "OZMAV";
char			AppVersion[256] = "V0.6b";
char			AppBuildName[256] = "interrupted stability";
char			AppPath[512] = "";
char			INIPath[512] = "";
char			WindowTitle[256] = "";
char			StatusMsg[256] = "";
char			ErrorMsg[1024] = "";

char			MapActorMsg[256] = "";
char			SceneActorMsg[256] = "";

bool			AreaLoaded = false;
bool			WndActive = true;
bool			ExitProgram = false;

char			CurrentGFXCmd[256] = "";
char			CurrentGFXCmdNote[256] = "";
char			GFXLogMsg[1024] = "";
char			SystemLogMsg[1024] = "";
char			WavefrontObjMsg[1024] = "";
char			WavefrontMtlMsg[1024] = "";

bool			GFXLogOpened = false;

bool			WavefrontObjOpened = false;
bool			WavefrontMtlOpened = false;

unsigned int	WavefrontObjVertCount = 0;
unsigned int	WavefrontObjVertCount_Previous = 0;

/* CAMERA / VIEWPOINT VARIABLES */
float			CamAngleX = 0, CamAngleY = 0;
float			CamX = 0, CamY = 0, CamZ = 0;
float			CamLX = 0, CamLY = 0, CamLZ = 0;

int				MousePosX = 0, MousePosY = 0;
int				MouseCenterX = 0, MouseCenterY = 0;

bool			MouseButtonDown = false;

/* FILE HANDLING VARIABLES */
FILE			* FileROM = NULL;

unsigned int	* ROMBuffer;
unsigned int	* ZMapBuffer[256];
unsigned int	* ZSceneBuffer;
unsigned int	* GameplayKeepBuffer;
unsigned int	* GameplayFDKeepBuffer;

unsigned int	* TempActorBuffer;
unsigned int	* TempObjectBuffer;
bool			TempActorBuffer_Allocated;
bool			TempObjectBuffer_Allocated;

unsigned int	Debug_MallocOperations;
unsigned int	Debug_FreeOperations;

bool			GetDLFromZMapScene;

unsigned long	ROMFilesize = 0;
unsigned long	ZMapFilesize[256];
unsigned long	ZSceneFilesize = 0;
unsigned long	GameplayKeepFilesize = 0;
unsigned long	GameplayFDKeepFilesize = 0;

unsigned long	CurrentObject_Length;
unsigned int	ObjectID;

char			Filename_ROM[256] = "";

bool			ROMExists = false;

FILE			* FileGFXLog = NULL;
FILE			* FileSystemLog = NULL;
FILE			* FileWavefrontObj = NULL;
FILE			* FileWavefrontMtl = NULL;

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
unsigned long	DLists[256][2048];
signed long		DListInfo_CurrentCount[256];
signed long		DListInfo_TotalCount = 0;
signed long		DListInfo_DListToRender = 0;
unsigned long	DLTempPosition = 0;

unsigned int	DLToRender = 0;
bool			DListHasEnded = false;

bool			SubDLCall = false;

/* F3DZEX TEXTURE HANDLING VARIABLES */
unsigned char	* TextureData_OGL = NULL;
unsigned char	* TextureData_N64 = NULL;

unsigned char	* PaletteData = NULL;

unsigned long	TexCachePosition = 0;
unsigned long	TotalTexCount = 0;

/* Combiner variables */
unsigned int fragProg = 0;
int COMBINE0 = 0,COMBINE1 = 0;
int cA0 = 0,cB0 = 0,cC0 = 0,cD0 = 0,aA0 = 0,aB0 = 0,aC0 = 0,aD0 = 0;
int cA1 = 0,cB1 = 0,cC1 = 0,cD1 = 0,aA1 = 0,aB1 = 0,aC1 = 0,aD1 = 0;

/* ZELDA ROM HANDLING VARIABLES */
unsigned long	ROM_SceneTableOffset = 0x00;
unsigned int	ROM_SceneToLoad = 0x00;

unsigned long	ROM_ObjectTableOffset = 0x00;
unsigned long	ROM_ActorTableOffset = 0x00;

unsigned int	ROM_CurrentMap = 0;

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
int				ActorInfo_CurrentCount[256];
int				ActorInfo_Selected = 0;

int				ScActorInfo_CurrentCount = 0;
int				ScActorInfo_Selected = 0;

/* GENERAL RENDERER VARIABLES */
GLuint			Renderer_GLDisplayList = 0;
GLuint			Renderer_GLDisplayList_Current = 0;
GLuint			Renderer_GLDisplayList_Total = 0;

GLuint			Renderer_GLTexture = 0;

GLuint			TempGLTexture = 0;

GLuint			Renderer_FilteringMode_Min = GL_LINEAR;
GLuint			Renderer_FilteringMode_Mag = GL_LINEAR;

DWORD			Renderer_LastFPS = 0;
int				Renderer_FPS, Renderer_FrameNo = 0;
char			Renderer_FPSMessage[32] = "";

char			Renderer_CoordDisp[256] = "";

bool			Renderer_EnableLighting = true;

GLfloat			LightAmbient[]=  {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat			LightDiffuse[]=  {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat			LightPosition[]= {1.0f, 1.0f, 1.0f, 1.0f};

GLfloat			FogColor[]=  {0.0f, 0.0f, 0.0f, 0.5f};
GLfloat			PrimColor[]= {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat         EnvColor[]=  {0.0f, 0.0f, 0.0f, 1.0f};

bool			Renderer_EnableMapActors = true;
bool			Renderer_EnableSceneActors = true;

bool			Renderer_EnableMap = true;
bool			Renderer_EnableCollision = true;
GLfloat			Renderer_CollisionAlpha = 0.3f;

/* OPENGL EXTENSION VARIABLES */
char			* GLExtension_List;
bool			GLExtension_MultiTexture = false;
bool			GLExtension_TextureMirror = false;
bool			GLExtension_AnisoFilter = false;
bool			GLExtension_FragmentProgram = false;
char			GLExtensionsSupported[256] = "";

bool			GLExtensionsUnsupported = false;
char			GLExtensionsErrorMsg[512] = "";

/* N64 BLENDING & COMBINER SIMULATION VARIABLES */
unsigned int	RDPCycleMode = 0;

unsigned long	Blender_Cycle1 = 0x00;
unsigned long	Blender_Cycle2 = 0x00;

/*	------------------------------------------------------------
	STRUCTURES
	------------------------------------------------------------ */

/* ZELDA MAP & SCENE HEADER STRUCTURES */
struct MapHeader_Struct MapHeader[256][256];
struct SceneHeader_Struct SceneHeader[256];

/* ZELDA MAP & SCENE ACTOR DATA STRUCTURES */
struct Actors_Struct Actors[256][1024];
struct ScActors_Struct ScActors[1024];

/* F3DZEX VERTEX DATA STRUCTURE */
struct Vertex_Struct Vertex[4096];
/* F3DZEX TEXTURE DATA STRUCTURE */
struct Texture_Struct Texture[0];
/* CI TEXTURE PALETTE STRUCTURE */
struct Palette_Struct Palette[512];
/* PSEUDO TEXTURE CACHE STRUCTURE */
struct CurrentTextures_Struct CurrentTextures[1024];

struct ObjectActorTable_Struct ObjectTable[8192];
struct ObjectActorTable_Struct ActorTable[8192];

struct Vertex_Struct CollisionVertex[8192];

/*	------------------------------------------------------------ */

int Viewer_Initialize()
{
	Viewer_ResetVariables();
	Viewer_LoadAreaData();
	Viewer_RenderMapRefresh();

	EnableMenuItem(hmenu, IDM_LEVEL_PREVLEVEL, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_LEVEL_NEXTLEVEL, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_MAP_PREVHEADER, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_MAP_NEXTHEADER, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_SCENE_PREVHEADER, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_SCENE_NEXTHEADER, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_ACTORS_MAPRENDER, MF_BYCOMMAND | MF_ENABLED);
//	EnableMenuItem(hmenu, IDM_ACTORS_SELECTPREV, MF_BYCOMMAND | MF_ENABLED);
//	EnableMenuItem(hmenu, IDM_ACTORS_SELECTNEXT, MF_BYCOMMAND | MF_ENABLED);
//	EnableMenuItem(hmenu, IDM_ACTORS_JUMPFIRST, MF_BYCOMMAND | MF_ENABLED);
//	EnableMenuItem(hmenu, IDM_ACTORS_JUMPLAST, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_ACTORS_SCENERENDER, MF_BYCOMMAND | MF_ENABLED);
//	EnableMenuItem(hmenu, IDM_ACTORS_SCENEPREV, MF_BYCOMMAND | MF_ENABLED);
//	EnableMenuItem(hmenu, IDM_ACTORS_SCENENEXT, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_RESETCAMCOORDS, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_FILTERNEAREST, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_FILTERLINEAR, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_FILTERMIPMAP, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_RENDERMAPS, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(hmenu, IDM_OPTIONS_RENDERCOLLISION, MF_BYCOMMAND | MF_ENABLED);

	sprintf(WindowTitle, "%s %s - %s", AppTitle, AppVersion, Filename_ROM);
	SetWindowText(hwnd, WindowTitle);

	return 0;
}

int Viewer_ResetVariables()
{
	/* resets some variables to fix problems when switching maps
	   (ex. depth test not showing up anymore after viewing other maps) */
	MapHeader_MultiHeaderMap = false;
	MapHeader_Current = 0;
	MapHeader_TotalCount = 0;
	memset(MapHeader_List, 0x00, sizeof(MapHeader_List));
	MapHeader_CurrentPosInList = 0;

	SceneHeader_MultiHeaderMap = false;
	SceneHeader_Current = 0;
	SceneHeader_TotalCount = 0;
	memset(SceneHeader_List, 0x00, sizeof(SceneHeader_List));
	SceneHeader_CurrentPosInList = 0;

	return 0;
}

int Viewer_LoadAreaData()
{
	FileSystemLog = fopen("log.txt", "w");

	AreaLoaded = false;

	sprintf(StatusMsg, "Loading level...");
	SendMessage(hstatus, SB_SETTEXT, 2, (LPARAM)StatusMsg);

	/* OPEN FILE */
	FileROM = fopen(Filename_ROM, "r+b");
	/* GET FILESIZE */
	size_t Result;
	fseek(FileROM, 0, SEEK_END);
	ROMFilesize = ftell(FileROM);
	rewind(FileROM);
	/* LOAD FILE INTO BUFFER */
	ROMBuffer = (unsigned int*) malloc (sizeof(int) * ROMFilesize);
	Result = fread(ROMBuffer, 1, ROMFilesize, FileROM);
	/* CLOSE FILE */
	fclose(FileROM);

	free(GameplayKeepBuffer);
	free(GameplayFDKeepBuffer);

	int i = 0;

	for(i = 0; i < (SceneHeader[SceneHeader_Current].Map_Count); i++) {
		if(ZMapBuffer[i] != NULL) free(ZMapBuffer[i]);
	}
	if(ZSceneBuffer != NULL) free(ZSceneBuffer);
	if(PaletteData != NULL) free(PaletteData);

	for(i = 0; i < 1025; i++) {
		if(glIsTexture(CurrentTextures[i].GLTextureID) == GL_TRUE) {
			glDeleteTextures(1, &CurrentTextures[i].GLTextureID);
		}
	}
	memset(CurrentTextures, 0x00, sizeof(CurrentTextures));

	MapHeader_Current = 0;
	SceneHeader_Current = 0;
	DListInfo_DListToRender = -1;
	ActorInfo_Selected = 0;

	ROM_CurrentMap = 0;

	Renderer_EnableLighting = true;

	memset(MapHeader, 0x00, sizeof(MapHeader));
	memset(SceneHeader, 0x00, sizeof(SceneHeader));
	memset(Actors, 0x00, sizeof(Actors));
	memset(ScActors, 0x00, sizeof(ScActors));
	memset(Vertex, 0x00, sizeof(Vertex));

	PaletteData = (unsigned char *) malloc (1024);
	memset(PaletteData, 0x00, sizeof(PaletteData));

	/* reset texture struct to prevent texture loader from going out of whack when there aren't any on a map (ex. sasatest) */
	Texture[0].Height = 0x00, Texture[0].HeightRender = 0x00, Texture[0].Width = 0x00, Texture[0].WidthRender = 0x00;
	Texture[0].DataSource = 0x00, Texture[0].PalDataSource = 0x00, Texture[0].Offset = 0x00, Texture[0].PalOffset = 0x00;
	Texture[0].Format_N64 = 0x00, Texture[0].Format_OGL = 0x00, Texture[0].Format_OGLPixel = 0x00;
	Texture[0].Y_Parameter = 0x00, Texture[0].X_Parameter = 0x00, Texture[0].S_Scale = 0x00, Texture[0].T_Scale = 0x00;
	Texture[0].LineSize = 0x00, Texture[0].Palette = 0x00;

	TexCachePosition = 0;

	/* ---- LOAD GAMEPLAY DATA FILES ---- */

	unsigned long TempOffset = 0;
	TempOffset = 0xF5E000;
	GameplayKeepFilesize = 0x0567B0;
	GameplayKeepBuffer = (unsigned int*) malloc (sizeof(int) * GameplayKeepFilesize);
	memcpy(GameplayKeepBuffer, &ROMBuffer[TempOffset / 4], GameplayKeepFilesize);

	TempOffset = 0xFC3000;
	GameplayFDKeepFilesize = 0x017AF0;
	GameplayFDKeepBuffer = (unsigned int*) malloc (sizeof(int) * GameplayFDKeepFilesize);
	memcpy(GameplayFDKeepBuffer, &ROMBuffer[TempOffset / 4], GameplayFDKeepFilesize);

	/* ---- LOAD SCENE DATA ---- */

	/* get current scene's offset */
	TempOffset = (ROM_SceneTableOffset / 4) + (ROM_SceneToLoad * 0x14) / 4;
	memcpy(&Readout_Current1, &ROMBuffer[TempOffset], 4);
	memcpy(&Readout_Current2, &ROMBuffer[TempOffset + 1], 4);
	HelperFunc_SplitCurrentVals(true);

	unsigned long Scene_Start = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned long Scene_End = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
	unsigned long Scene_Length = Scene_End - Scene_Start;

	ZSceneFilesize = Scene_Length;

	/* copy current scene into buffer */
	ZSceneBuffer = (unsigned int*) malloc (sizeof(int) * Scene_Length);
	memcpy(ZSceneBuffer, &ROMBuffer[Scene_Start / 4], Scene_Length);

	/* check for multiple scene headers */
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

	Viewer_GetSceneHeader(SceneHeader_Current);
	Viewer_GetSceneActors(SceneHeader_Current);
//	Viewer_GetMapCollision(SceneHeader_Current);			/* do when rendering map */

	/* ---- LOAD MAP DATA ---- */

	for(i = 0; i < (SceneHeader[SceneHeader_Current].Map_Count); i++) {
		ROM_CurrentMap = i;

		memcpy(&Readout_Current1, &ZSceneBuffer[((SceneHeader[SceneHeader_Current].Map_ListOffset + (i * 0x08)) / 4)], 4);
		memcpy(&Readout_Current2, &ZSceneBuffer[((SceneHeader[SceneHeader_Current].Map_ListOffset + (i * 0x08)) / 4) + 1], 4);
		HelperFunc_SplitCurrentVals(true);

		unsigned long Map_Start = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
		unsigned long Map_End = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
		unsigned long Map_Length = Map_End - Map_Start;

		ZMapFilesize[i] = Map_Length;

		ZMapBuffer[i] = (unsigned int*) malloc (sizeof(int) * Map_Length);
		memcpy(ZMapBuffer[i], &ROMBuffer[Map_Start / 4], Map_Length);

		memcpy(&Readout_Current1, &ZMapBuffer[i][0], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[i][1], 4);
		HelperFunc_SplitCurrentVals(true);

		if((Readout_CurrentByte1 == 0x08) || (Readout_CurrentByte1 == 0x16) || (Readout_CurrentByte1 == 0x18)) {
			if((Readout_CurrentByte1 == 0x18)) {
				MapHeader_MultiHeaderMap = true;
				unsigned long MapHeaderListPos = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
				Viewer_GetMapHeaderList(MapHeaderListPos);
			} else {
				MapHeader_TotalCount = 0;
			}

			Viewer_GetMapHeader(MapHeader_Current);
			Viewer_GetMapActors(MapHeader_Current);
		} else {
			MessageBox(hwnd, "Invalid or non-standard map header!", "Error", MB_OK | MB_ICONEXCLAMATION);
		}

		Viewer_GetMapDisplayLists(ZMapFilesize[i]);
	}

	/* ---- LOAD OBJECT DATA ---- */

	ROM_ObjectTableOffset = 0xB9E6C8;

	for(i = 0; i < (0x192 * 2); i+=2) {
		memcpy(&Readout_Current1, &ROMBuffer[(ROM_ObjectTableOffset / 4) + i], 4);
		memcpy(&Readout_Current2, &ROMBuffer[(ROM_ObjectTableOffset / 4) + i + 1], 4);
		HelperFunc_SplitCurrentVals(true);

		ObjectTable[i / 2].StartOffset = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
		ObjectTable[i / 2].EndOffset = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

		if((ObjectTable[i / 2].StartOffset == 0x00) && (ObjectTable[i / 2].EndOffset == 0x00)) {
			ObjectTable[i / 2].Valid = false;
		} else {
			ObjectTable[i / 2].Valid = true;
		}

//		sprintf(ErrorMsg, "%04X: %08X -> %08X", i / 2, ObjectTable[i / 2].StartOffset, ObjectTable[i / 2].EndOffset);
//		MessageBox(hwnd, ErrorMsg, "", 0);
	}

	/* ---- LOAD ACTOR DATA ---- */

	ROM_ActorTableOffset = 0xB8D440;

	for(i = 0; i < (0x1D7 * 8); i+=8) {
		memcpy(&Readout_Current1, &ROMBuffer[(ROM_ActorTableOffset / 4) + i], 4);
		memcpy(&Readout_Current2, &ROMBuffer[(ROM_ActorTableOffset / 4) + i + 1], 4);
		HelperFunc_SplitCurrentVals(true);

		ActorTable[i / 8].StartOffset = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
		ActorTable[i / 8].EndOffset = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

		if((ActorTable[i / 8].StartOffset == 0x00) && (ActorTable[i / 8].EndOffset == 0x00)) {
			ActorTable[i / 8].Valid = false;
		} else {
			ActorTable[i / 8].Valid = true;
		}

//		sprintf(ErrorMsg, "%04X: %08X -> %08X (valid: %d)", i / 8, ActorTable[i / 8].StartOffset, ActorTable[i / 8].EndOffset, ActorTable[i / 8].Valid);
//		MessageBox(hwnd, ErrorMsg, "", 0);
	}

	CamAngleX = 0.0f, CamAngleY = 0.0f;
	CamX = 0.0f, CamY = 0.0f, CamZ = 5.0f;
	CamLX = 0.0f, CamLY = 0.0f, CamLZ = -1.0f;

	memset(CurrentGFXCmd, 0x00, sizeof(CurrentGFXCmd));
	memset(CurrentGFXCmdNote, 0x00, sizeof(CurrentGFXCmdNote));
	memset(GFXLogMsg, 0x00, sizeof(GFXLogMsg));
	memset(SystemLogMsg, 0x00, sizeof(SystemLogMsg));

//	sprintf(StatusMsg, "Level loaded successfully!");
	sprintf(StatusMsg, "Level: 0x%02X", ROM_SceneToLoad);

	if(ROMBuffer != NULL) free(ROMBuffer);

	return 0;
}

int Viewer_RenderMapRefresh()
{
	/* rendering speed workaround: i don't know why it works (related to texture
	   handling most likely), only that it does!

	   basically, we create the whole ogl dlist stuff once (rendermap), then
	   draw the whole thing once (drawglscene) and then recreate the ogl dlists
	   again - instant speedup! the level loading process now takes roughly
	   double the time, but with the spirit temple rendering at ~65 fps instead
	   of a mere 28 or so (depending where the camera's at), i guess that
	   tradeoff isn't too bad :) */

	Viewer_RenderMap();
	DrawGLScene();
	Viewer_RenderMap();

	return 0;
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
	CamX = CamX + direction * (CamLX) * 0.025f;
	CamY = CamY + direction * (CamLY) * 0.025f;
	CamZ = CamZ + direction * (CamLZ) * 0.025f;
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

void Dialog_OpenROM(HWND hwnd)
{
	char			filter[] = "Nintendo 64 ROMs (*.z64)\0;*.z64\0";
	OPENFILENAME	ofn;

	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= hwnd;
	ofn.lpstrFilter		= filter;
	ofn.nFilterIndex	= 1;
	ofn.lpstrFile		= Filename_ROM;
	ofn.nMaxFile		= 256;
	ofn.Flags			= OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if(GetOpenFileName(&ofn)) ROMExists = true;

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

	int hstatuswidths[] = {50, 220, -1};
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

	sprintf(WindowTitle, "%s %s", AppTitle, AppVersion);
	SetWindowText(hwnd, WindowTitle);

	GetModuleFileName(NULL, AppPath, sizeof(AppPath) - 1);

	char *AppPathTemp = strrchr(AppPath, '\\');
	if(AppPathTemp) ++AppPathTemp; if(AppPathTemp) *AppPathTemp = 0;
	sprintf(INIPath, "%s\\ozmav.ini", AppPath);

	GetPrivateProfileString("Viewer", "LastROM", "", Filename_ROM, sizeof(Filename_ROM), INIPath);
	ROM_SceneTableOffset = GetPrivateProfileInt("Viewer", "SceneTable", 0xBA0BB0, INIPath);
	ROM_SceneToLoad = GetPrivateProfileInt("Viewer", "LastScene", 0x00, INIPath);
	Renderer_FilteringMode_Min = GetPrivateProfileInt("Viewer", "TexFilterMin", GL_LINEAR_MIPMAP_LINEAR, INIPath);
	Renderer_FilteringMode_Mag = GetPrivateProfileInt("Viewer", "TexFilterMag", GL_LINEAR, INIPath);
	Renderer_EnableMapActors = GetPrivateProfileInt("Viewer", "RenderMapActors", true, INIPath);
	Renderer_EnableSceneActors = GetPrivateProfileInt("Viewer", "RenderSceneActors", false, INIPath);
	Renderer_EnableMap = GetPrivateProfileInt("Viewer", "RenderMaps", true, INIPath);
	Renderer_EnableCollision = GetPrivateProfileInt("Viewer", "RenderCollision", true, INIPath);

	if (!CreateGLTarget(640,480,16)) return 0;

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

				if (AreaLoaded) {
					if (System_KbdKeys[VK_F1]) {
						System_KbdKeys[VK_F1] = false;
						if(!(ROM_SceneToLoad == 0)) {
							ROM_SceneToLoad--;
							Viewer_ResetVariables();
							Viewer_LoadAreaData();
							Viewer_RenderMapRefresh();
						}
						sprintf(StatusMsg, "Level: 0x%02X", ROM_SceneToLoad);
					}
					if (System_KbdKeys[VK_F2]) {
						System_KbdKeys[VK_F2] = false;
						if(!(ROM_SceneToLoad == 0x6D)) {
							ROM_SceneToLoad++;
							Viewer_ResetVariables();
							Viewer_LoadAreaData();
							Viewer_RenderMapRefresh();
						}
						sprintf(StatusMsg, "Level: 0x%02X", ROM_SceneToLoad);
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
/*					if (System_KbdKeys[VK_F5]) {
						System_KbdKeys[VK_F5] = false;
						if(!(ActorInfo_Selected == 0)) {
							ActorInfo_Selected--;
						}
					}
					if (System_KbdKeys[VK_F6]) {
						System_KbdKeys[VK_F6] = false;
						if(!(ActorInfo_Selected == MapHeader[ROM_CurrentMap][MapHeader_Current].Actor_Count - 1)) {
							ActorInfo_Selected++;
						}
					}
					if (System_KbdKeys[VK_F7]) {
						System_KbdKeys[VK_F7] = false;
						ActorInfo_Selected = 0;
					}
					if (System_KbdKeys[VK_F8]) {
						System_KbdKeys[VK_F8] = false;
						ActorInfo_Selected = MapHeader[ROM_CurrentMap][MapHeader_Current].Actor_Count - 1;
					}
*/
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
/*					if (System_KbdKeys[VK_SUBTRACT]) {
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
*/
					if (System_KbdKeys['W']) {
						if(System_KbdKeys[VK_SHIFT]) {
							GLUTCamera_Movement(6);
						} else if(System_KbdKeys[VK_CONTROL]) {
							GLUTCamera_Movement(1);
						} else {
							GLUTCamera_Movement(3);
						}
					}
					if (System_KbdKeys['S']) {
						if(System_KbdKeys[VK_SHIFT]) {
							GLUTCamera_Movement(-6);
						} else if(System_KbdKeys[VK_CONTROL]) {
							GLUTCamera_Movement(-1);
						} else {
							GLUTCamera_Movement(-3);
						}
					}
					if (System_KbdKeys[VK_LEFT]) {
						CamAngleX -= 0.025f;
					}
					if (System_KbdKeys[VK_RIGHT]) {
						CamAngleX += 0.025f;
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

	WritePrivateProfileString("Viewer", "LastROM", Filename_ROM, INIPath);
	char TempStr[256];

	sprintf(TempStr, "%d", ROM_SceneTableOffset);
	WritePrivateProfileString("Viewer", "SceneTable", TempStr, INIPath);
	sprintf(TempStr, "%d", ROM_SceneToLoad);
	WritePrivateProfileString("Viewer", "LastScene", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_FilteringMode_Min);
	WritePrivateProfileString("Viewer", "TexFilterMin", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_FilteringMode_Mag);
	WritePrivateProfileString("Viewer", "TexFilterMag", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableMapActors);
	WritePrivateProfileString("Viewer", "RenderMapActors", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableSceneActors);
	WritePrivateProfileString("Viewer", "RenderSceneActors", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableMap);
	WritePrivateProfileString("Viewer", "RenderMaps", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableCollision);
	WritePrivateProfileString("Viewer", "RenderCollision", TempStr, INIPath);

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
			SetWindowPos(hogl, NULL, 0, 0, rcClient.right, rcClient.bottom - 20, SWP_NOZORDER);

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
					ROMExists = false;

					Dialog_OpenROM(hwnd);
					if(ROMExists) Viewer_Initialize();

					break;
				case IDM_FILE_SAVE:
					break;
				case IDM_FILE_EXIT:
					ExitProgram = true;
					break;
				case IDM_LEVEL_PREVLEVEL:
					System_KbdKeys[VK_F1] = true;
					break;
				case IDM_LEVEL_NEXTLEVEL:
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
				case IDM_OPTIONS_RESETCAMCOORDS:
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
				case IDM_OPTIONS_RENDERMAPS:
					if(!Renderer_EnableMap) {
						Renderer_EnableMap = true;
					} else {
						Renderer_EnableMap = false;
					}
					break;
				case IDM_OPTIONS_RENDERCOLLISION:
					if(!Renderer_EnableCollision) {
						Renderer_EnableCollision = true;
					} else {
						Renderer_EnableCollision = false;
					}
					break;
				case IDM_HELP_ABOUT: ;
					sprintf(GLExtensionsSupported, "OpenGL extensions supported and used:\n");
					if(GLExtension_MultiTexture) sprintf(GLExtensionsSupported, "%sGL_ARB_multitexture\n", GLExtensionsSupported);
					if(GLExtension_TextureMirror) sprintf(GLExtensionsSupported, "%sGL_ARB_texture_mirrored_repeat\n", GLExtensionsSupported);
					if(GLExtension_AnisoFilter) sprintf(GLExtensionsSupported, "%sGL_EXT_texture_filter_anisotropic\n", GLExtensionsSupported);

					char AboutMsg[256] = "";
					sprintf(AboutMsg, "%s %s (Build '%s') - OpenGL Zelda Map Viewer\n\nWritten 2008/2009 by xdaniel & contributors\nhttp://ozmav.googlecode.com/\n\n%s", AppTitle, AppVersion, AppBuildName, GLExtensionsSupported);
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
