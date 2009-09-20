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

PFNGLMULTITEXCOORD1FARBPROC			glMultiTexCoord1fARB			= NULL;
PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB			= NULL;
PFNGLMULTITEXCOORD3FARBPROC			glMultiTexCoord3fARB			= NULL;
PFNGLMULTITEXCOORD4FARBPROC			glMultiTexCoord4fARB			= NULL;
PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB				= NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB		= NULL;

PFNGLGENPROGRAMSARBPROC				glGenProgramsARB				= NULL;
PFNGLBINDPROGRAMARBPROC				glBindProgramARB				= NULL;
PFNGLDELETEPROGRAMSARBPROC			glDeleteProgramsARB				= NULL;
PFNGLPROGRAMSTRINGARBPROC			glProgramStringARB				= NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC	glProgramEnvParameter4fARB		= NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC	glProgramLocalParameter4fARB	= NULL;;

/*	------------------------------------------------------------
	VARIABLES
	------------------------------------------------------------ */

/* WINDOW HANDLING VARIABLES, ETC. */
HWND			hwnd = NULL;
HMENU			hmenu = NULL;
HWND			hogl = NULL;
HWND			hstatus = NULL;
HWND			hlvlcombo = NULL;

HDC				hDC_ogl = NULL;
HGLRC			hRC = NULL;
HINSTANCE		hInstance;

char			szClassName[] = "OZMAVClass";

/* GENERAL GLOBAL PROGRAM VARIABLES */
bool			System_KbdKeys[256];

char			AppTitle[32] = "OZMAV";
char			AppVersion[32] = "V0.75";
char			AppBuildName[64] = "~type KUMI~";
char			AppPath[512] = "";
char			INIPath[512] = "";
char			WindowTitle[256] = "";
char			StatusMsg[256] = "";
char			ErrorMsg[8192] = "";

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
char			WavefrontObjColMsg[1024] = "";

bool			GFXLogOpened = false;

bool			WavefrontObjOpened = false;
bool			WavefrontMtlOpened = false;
bool			WavefrontObjColOpened = false;

unsigned int	WavefrontObjVertCount = 0;
unsigned int	WavefrontObjVertCount_Previous = 0;

unsigned int	WavefrontObjMaterialCnt = 0;

unsigned int	WavefrontObjColVertCount = 0;
unsigned int	WavefrontObjColVertCount_Previous = 0;

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

bool			GetDLFromZMapScene = true;

unsigned int	Scene_Start = 0x00;
char			Scene_Name[256];

unsigned int	Map_Start[256];

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
FILE			* FileCombinerLog = NULL;
FILE			* FileWavefrontObj = NULL;
FILE			* FileWavefrontMtl = NULL;
FILE			* FileWavefrontObjCol = NULL;

int				GameMode = 0;				/* 0 = OoT, 1 = MM */

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
unsigned long	Readout_PrevGFXCommand1 = 0;

/* F3DEX2 DISPLAY LIST HANDLING VARIABLES */
unsigned long	DLists[256][2048];
signed long		DListInfo_CurrentCount[256];
signed long		DListInfo_TotalCount = 0;
signed long		DListInfo_DListToRender = 0;
unsigned long	DLTempPosition = 0;

unsigned int	DLToRender = 0;
bool			DListHasEnded = false;

unsigned long	Storage_RDPHalf1 = 0;
unsigned long	Storage_RDPHalf2 = 0;

GLfloat			Matrix[4][4];
GLfloat			Matrix_Stack[32][4][4];
GLfloat			ProjMatrix[4][4];
int				MVMatrixCount = 0;

unsigned long	N64_GeometryMode = 0x00;

/* F3DEX2 TEXTURE HANDLING VARIABLES */
unsigned char	* TextureData_OGL = NULL;
unsigned char	* TextureData_N64 = NULL;

unsigned char	* PaletteData = NULL;

unsigned long	TexCachePosition = 0;
unsigned long	TotalTexCount = 0;

/* Combiner variables */
unsigned int	Combine0 = 0, Combine1 = 0;
int				cA[2], cB[2], cC[2], cD[2], aA[2], aB[2], aC[2], aD[2];
int				VertProg = 0;

unsigned int	FPCachePosition = 0;

bool			RDPOtherMode_ForceBlender = false;
unsigned short	RDPOtherMode_BlendMode = 0x00;

/* ZELDA ROM HANDLING VARIABLES */
unsigned long	ROM_SceneTableOffset = 0x00;
unsigned int	ROM_SceneToLoad = 0x00;
unsigned int	ROM_MaxSceneCount = 0x00;
unsigned int	ROM_SceneEntryLength = 0x00;

unsigned long	ROM_ObjectTableOffset = 0x00;
unsigned long	ROM_ActorTableOffset = 0x00;

int				ROM_CurrentMap = 0;
int				ROM_CurrentMap_Temp = 0;

int				DListParser_CurrentMap = 0;

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

int				DoorInfo_CurrentCount = 0;
int				DoorInfo_Selected = 0;

/* GENERAL RENDERER VARIABLES */
GLuint			Renderer_GLDisplayList = 0;
GLuint			Renderer_GLDisplayList_Current = 0;
GLuint			Renderer_GLDisplayList_Total = 0;

GLuint			Renderer_GLTexture;
int				CurrentTextureID = 0;
bool			IsMultiTexture = false;

GLuint			TempGLTexture = 0;

GLuint			Renderer_FilteringMode_Min = GL_LINEAR;
GLuint			Renderer_FilteringMode_Mag = GL_LINEAR;

DWORD			Renderer_LastFPS = 0;
int				Renderer_FPS, Renderer_FrameNo = 0;
char			Renderer_FPSMessage[32] = "";

char			Renderer_CoordDisp[256] = "";

GLfloat			LightAmbient[]=  {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat			LightDiffuse[]=  {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat			LightSpecular[]=  {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat			LightPosition[]= {1.0f, 1.0f, 1.0f, 1.0f};

float			FogColor[]=   {0.75f, 0.75f, 0.75f, 1.0f};
float			BlendColor[]= {0.0f, 0.0f, 0.0f, 0.3f};
float			PrimColor[]=  {0.0f, 0.0f, 0.0f, 0.0f};
float			EnvColor[]=   {0.0f, 0.0f, 0.0f, 0.0f};

char			ShaderString[16384] = "";

bool			Renderer_EnableMapActors = true;
bool			Renderer_EnableSceneActors = true;
bool			Renderer_EnableDoors = true;

bool			Renderer_EnableMap = true;
bool			Renderer_EnableCollision = true;
GLfloat			Renderer_CollisionAlpha = 0.3f;

bool			Renderer_EnableWavefrontDump = false;

bool			Renderer_EnableFog = true;

bool			Renderer_EnableWireframe = false;

bool			Renderer_EnableFragShader = true;

int				CurrentEnvSetting = 0;

/* OPENGL EXTENSION VARIABLES */
char			* GLExtension_List;
bool			GLExtension_MultiTexture = false;
bool			GLExtension_TextureMirror = false;
bool			GLExtension_AnisoFilter = false;
bool			GLExtension_VertFragProgram = false;
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

struct EnvSetting_Struct EnvSetting[256];

/* ZELDA MAP & SCENE ACTOR DATA STRUCTURES */
struct Actors_Struct Actors[256][1024];
struct ScActors_Struct ScActors[1024];
struct Door_Struct Doors[1024];

/* F3DEX2 VERTEX DATA STRUCTURE */
struct Vertex_Struct Vertex[4096];
/* F3DEX2 TEXTURE DATA STRUCTURE */
struct Texture_Struct Texture[2];
/* CI TEXTURE PALETTE STRUCTURE */
struct Palette_Struct Palette[512];
/* PSEUDO TEXTURE CACHE STRUCTURE */
struct CurrentTextures_Struct CurrentTextures[1024];

struct ObjectActorTable_Struct ObjectTable[8192];
struct ObjectActorTable_Struct ActorTable[8192];

struct Vertex_Struct CollisionVertex[8192];

struct FPCache_Struct FPCache[256];

/*	------------------------------------------------------------ */

int Viewer_Initialize()
{
	Viewer_ResetVariables();

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

	if(Viewer_LoadAreaData() == -1) {
		MessageBox(hwnd, "Error while loading level!", "Error", MB_OK | MB_ICONEXCLAMATION);
		AreaLoaded = true;
		sprintf(StatusMsg, "Level: 0x%02X", ROM_SceneToLoad);
	} else {
		Viewer_RenderMapRefresh();

		EnableMenuItem(hmenu, IDM_LEVEL_PREVLEVEL, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_LEVEL_NEXTLEVEL, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_MAP_PREVHEADER, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_MAP_NEXTHEADER, MF_BYCOMMAND | MF_ENABLED);
	//	EnableMenuItem(hmenu, IDM_SCENE_PREVHEADER, MF_BYCOMMAND | MF_ENABLED);
	//	EnableMenuItem(hmenu, IDM_SCENE_NEXTHEADER, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_ACTORS_MAPRENDER, MF_BYCOMMAND | MF_ENABLED);
	//	EnableMenuItem(hmenu, IDM_ACTORS_SELECTPREV, MF_BYCOMMAND | MF_ENABLED);
	//	EnableMenuItem(hmenu, IDM_ACTORS_SELECTNEXT, MF_BYCOMMAND | MF_ENABLED);
	//	EnableMenuItem(hmenu, IDM_ACTORS_JUMPFIRST, MF_BYCOMMAND | MF_ENABLED);
	//	EnableMenuItem(hmenu, IDM_ACTORS_JUMPLAST, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_ACTORS_SCENERENDER, MF_BYCOMMAND | MF_ENABLED);
	//	EnableMenuItem(hmenu, IDM_ACTORS_SCENEPREV, MF_BYCOMMAND | MF_ENABLED);
	//	EnableMenuItem(hmenu, IDM_ACTORS_SCENENEXT, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_ACTORS_DOORRENDER, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_OPTIONS_RESETCAMCOORDS, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_OPTIONS_FILTERNEAREST, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_OPTIONS_FILTERLINEAR, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_OPTIONS_FILTERMIPMAP, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_OPTIONS_RENDERMAPS, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_OPTIONS_RENDERCOLLISION, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_OPTIONS_WAVEFRONT, MF_BYCOMMAND | MF_ENABLED);
	//	EnableMenuItem(hmenu, IDM_OPTIONS_FRAGSHADER, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_OPTIONS_RENDERFOG, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hmenu, IDM_OPTIONS_WIREFRAME, MF_BYCOMMAND | MF_ENABLED);
	}

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

int Viewer_GetGameVersion()
{
	/* ---- GET GAME VERSION ---- */

	unsigned long VersionCheck = ROMBuffer[4];
	char Temp[1024];

	bool CheckOkay = true;

	if(ROMFilesize < 0x04000000) CheckOkay = false;

	switch(VersionCheck) {
		/* OCARINA OF TIME VERSIONS */
		case 0x7B2E5293:
			/* oot us/jpn 1.0 !decompressed! */
			ROM_SceneTableOffset = 0xB71440;
			ROM_MaxSceneCount = 0x64;
			ROM_SceneEntryLength = 0x14;
			GameMode = 0;
			break;
		case 0x93C34093:
			/* oot us/jpn 1.1 !decompressed! */
			ROM_SceneTableOffset = 0xB71600;
			ROM_MaxSceneCount = 0x64;
			ROM_SceneEntryLength = 0x14;
			GameMode = 0;
			break;
		case 0x2AF1902D:
			/* oot us/jpn 1.2 !decompressed! */
			ROM_SceneTableOffset = 0xB71450;
			ROM_MaxSceneCount = 0x64;
			ROM_SceneEntryLength = 0x14;
			GameMode = 0;
			break;

		case 0xB5539DEE:
			/* oot pal 1.0 !decompressed! */
			ROM_SceneTableOffset = 0xB70D60;
			ROM_MaxSceneCount = 0x64;
			ROM_SceneEntryLength = 0x14;
			GameMode = 0;
			break;
		case 0x61D055DC:
			/* oot pal 1.1 !decompressed! */
			ROM_SceneTableOffset = 0xB70DA0;
			ROM_MaxSceneCount = 0x64;
			ROM_SceneEntryLength = 0x14;
			GameMode = 0;
			break;

		case 0xF6187D91:
			/* mq debug rom */
			ROM_SceneTableOffset = 0xBA0BB0;
			ROM_MaxSceneCount = 0x6D;
			ROM_SceneEntryLength = 0x14;
			GameMode = 0;
			break;

		/* MAJORA'S MASK VERSIONS */
		case 0x0D52D049:
			/* mm jpn 1.0 !decompressed! */
			ROM_SceneTableOffset = 0xC76510;
			ROM_MaxSceneCount = 0x70;
			ROM_SceneEntryLength = 0x10;
			GameMode = 1;
			break;
		case 0x2719B294:
			/* mm jpn 1.1 !decompressed! */
			ROM_SceneTableOffset = 0xC767E0;
			ROM_MaxSceneCount = 0x70;
			ROM_SceneEntryLength = 0x10;
			GameMode = 1;
			break;

		case 0x88989821:
			/* mm us 1.0 !decompressed! */
			ROM_SceneTableOffset = 0xC5A1E0;
			ROM_MaxSceneCount = 0x70;
			ROM_SceneEntryLength = 0x10;
			GameMode = 1;
			break;

		case 0x695908CB:
			/* mm pal 1.0 !decompressed! */
			ROM_SceneTableOffset = 0xDA8860;
			ROM_MaxSceneCount = 0x70;
			ROM_SceneEntryLength = 0x10;
			GameMode = 1;
			break;

		case 0xE897A94F:
			/* mm us demo !decompressed! */
			ROM_SceneTableOffset = 0xC5A3E0;
			ROM_MaxSceneCount = 0x70;
			ROM_SceneEntryLength = 0x10;
			GameMode = 1;
			break;

		default: {
			/* unknown rom */
			CheckOkay = false;
			GameMode = 0;
			break; }
	}

	if(CheckOkay == false) {
		sprintf(Temp, "ROM couldn't be recognized!\n\n(Unknown: %08X)", (unsigned int)VersionCheck);
		MessageBox(hwnd, Temp, "Error", MB_OK | MB_ICONEXCLAMATION);
		GameMode = 0;
		return -1;
	}

	return 0;
}

int Viewer_InitLevelSelector()
{
	int i = 0; char Temp[256];
	/* del list */
	for(i = 0; i < ROM_MaxSceneCount + 1; i++) {
		SendMessage(GetDlgItem(hwnd, IDC_MAIN_LEVELCOMBO), CB_DELETESTRING, (WPARAM)NULL, (LPARAM)NULL);
	}
	/* make list + select current */
	for(i = 0; i < ROM_MaxSceneCount + 1; i++) {
		sprintf(Temp, "0x%02X", i);
		SendMessage(GetDlgItem(hwnd, IDC_MAIN_LEVELCOMBO), CB_ADDSTRING, (WPARAM)NULL, (LPARAM)Temp);
		if(ROM_SceneToLoad == i) SendMessage(GetDlgItem(hwnd, IDC_MAIN_LEVELCOMBO), CB_SETCURSEL, (WPARAM)i, (LPARAM)NULL);
	}

	return 0;
}

int Viewer_LoadAreaData()
{
	char Temp[1024];
	sprintf(Temp, "%s\\log.txt", AppPath);
	FileSystemLog = fopen(Temp, "w");

	AreaLoaded = false;

	sprintf(StatusMsg, "Loading level...");
	SendMessage(hstatus, SB_SETTEXT, 2, (LPARAM)StatusMsg);

//	if(GameplayKeepBuffer != NULL) free(GameplayKeepBuffer);
//	if(GameplayFDKeepBuffer != NULL) free(GameplayFDKeepBuffer);

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

	CurrentEnvSetting = 0;

	ROM_CurrentMap = -1;
	ROM_CurrentMap_Temp = 0;

	memset(MapHeader, 0x00, sizeof(MapHeader));
	memset(SceneHeader, 0x00, sizeof(SceneHeader));
	memset(Actors, 0x00, sizeof(Actors));
	memset(ScActors, 0x00, sizeof(ScActors));
	memset(Vertex, 0x00, sizeof(Vertex));

	PaletteData = (unsigned char *) malloc (1024);
	memset(PaletteData, 0x00, sizeof(PaletteData));

	EnvColor[0] = 0.5f; EnvColor[1] = 0.5f; EnvColor[2] = 0.5f; EnvColor[3] = 0.5f;
	PrimColor[0] = 0.5f; PrimColor[1] = 0.5f; PrimColor[2] = 0.5f; PrimColor[3] = 0.5f;

	if(GLExtension_VertFragProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, EnvColor[0], EnvColor[1], EnvColor[2], EnvColor[3]);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);

		glDisable(GL_FRAGMENT_PROGRAM_ARB); glDisable(GL_VERTEX_PROGRAM_ARB);
	}

	CurrentTextureID = 0;

	TexCachePosition = 0;

	if(Viewer_GetGameVersion() == -1) return -1;

	if(ROM_SceneToLoad > ROM_MaxSceneCount) ROM_SceneToLoad = ROM_MaxSceneCount;

	Viewer_InitLevelSelector();

	/* ---- LOAD GAMEPLAY DATA FILES ---- */

	unsigned long TempOffset = 0;
/*	TempOffset = 0xF5E000;
	GameplayKeepFilesize = 0x0567B0;
	GameplayKeepBuffer = (unsigned int*) malloc (sizeof(int) * GameplayKeepFilesize);
	memcpy(GameplayKeepBuffer, &ROMBuffer[TempOffset / 4], GameplayKeepFilesize);

	TempOffset = 0xFC3000;
	GameplayFDKeepFilesize = 0x017AF0;
	GameplayFDKeepBuffer = (unsigned int*) malloc (sizeof(int) * GameplayFDKeepFilesize);
	memcpy(GameplayFDKeepBuffer, &ROMBuffer[TempOffset / 4], GameplayFDKeepFilesize);
*/
	/* ---- LOAD SCENE DATA ---- */

	/* get current scene's offset */

	TempOffset = (ROM_SceneTableOffset / 4) + (ROM_SceneToLoad * ROM_SceneEntryLength) / 4;

	memcpy(&Readout_Current1, &ROMBuffer[TempOffset], 4);
	memcpy(&Readout_Current2, &ROMBuffer[TempOffset + 1], 4);
	Helper_SplitCurrentVals(true);

	// MM: if list entry is empty, return - used for non-jpn games, jpn default to testmap instead of 0
	if((Readout_Current1 == 0x00) || (Readout_Current2 == 0x00)) {
		return -1;
	}

	Scene_Start = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned long Scene_End = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
	unsigned long Scene_Length = Scene_End - Scene_Start;

	ZSceneFilesize = Scene_Length;

	Zelda_GetSceneName();

	/* copy current scene into buffer */
	ZSceneBuffer = (unsigned int*) malloc (sizeof(int) * Scene_Length);
	memcpy(ZSceneBuffer, &ROMBuffer[Scene_Start / 4], Scene_Length);

	/* check for multiple scene headers */
	memcpy(&Readout_Current1, &ZSceneBuffer[0], 4);
	memcpy(&Readout_Current2, &ZSceneBuffer[0 + 1], 4);
	Helper_SplitCurrentVals(true);

	if((Readout_CurrentByte1 == 0x18)) {
		SceneHeader_MultiHeaderMap = true;
		unsigned long SceneHeaderListPos = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
		Zelda_GetSceneHeaderList(SceneHeaderListPos);
	} else {
		SceneHeader_TotalCount = 0;
	}

	Zelda_GetSceneHeader(SceneHeader_Current);
	Zelda_GetSceneActors(SceneHeader_Current);
	Zelda_GetDoorData(SceneHeader_Current);
	Zelda_GetEnvironmentSettings(SceneHeader_Current);

	/* ---- LOAD MAP DATA ---- */

	for(i = 0; i < (SceneHeader[SceneHeader_Current].Map_Count); i++) {
		memcpy(&Readout_Current1, &ZSceneBuffer[((SceneHeader[SceneHeader_Current].Map_ListOffset + (i * 0x08)) / 4)], 4);
		memcpy(&Readout_Current2, &ZSceneBuffer[((SceneHeader[SceneHeader_Current].Map_ListOffset + (i * 0x08)) / 4) + 1], 4);
		Helper_SplitCurrentVals(true);

		Map_Start[i] = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
		unsigned long Map_End = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
		unsigned long Map_Length = Map_End - Map_Start[i];

		ZMapFilesize[i] = Map_Length;

		ZMapBuffer[i] = (unsigned int*) malloc (sizeof(int) * Map_Length);
		memcpy(ZMapBuffer[i], &ROMBuffer[Map_Start[i] / 4], Map_Length);

		memcpy(&Readout_Current1, &ZMapBuffer[i][0], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[i][1], 4);
		Helper_SplitCurrentVals(true);

		if((Readout_CurrentByte1 == 0x08) || (Readout_CurrentByte1 == 0x16) || (Readout_CurrentByte1 == 0x18)) {
			if((Readout_CurrentByte1 == 0x18)) {
				MapHeader_MultiHeaderMap = true;
				unsigned long MapHeaderListPos = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
				Zelda_GetMapHeaderList(MapHeaderListPos, i);
			} else {
				MapHeader_TotalCount = 0;
			}

			Zelda_GetMapHeader(MapHeader_Current, i);
			Zelda_GetMapActors(MapHeader_Current, i);
		} else {
			MessageBox(hwnd, "Invalid or non-standard map header!", "Error", MB_OK | MB_ICONEXCLAMATION);
		}

		Zelda_GetMapDisplayLists(ZMapFilesize[i], i);
	}
/*
	sprintf(Temp, "%s\\temp.zmap", AppPath);
	FILE * TempFile = fopen(Temp, "w");
	fwrite(ZMapBuffer[0], ZMapFilesize[0], 1, TempFile);
	fclose(TempFile);
	sprintf(Temp, "%s\\temp.zscene", AppPath);
	TempFile = fopen(Temp, "w");
	fwrite(ZSceneBuffer, ZSceneFilesize, 1, TempFile);
	fclose(TempFile);

	sprintf(Temp, "Scene start: %08X, Map 0 start: %08X", Scene_Start, Map_Start[0]);
	MessageBox(hwnd, Temp, "", 0);
*/
	/* ---- LOAD OBJECT DATA ---- */

/*	ROM_ObjectTableOffset = 0xB9E6C8;

	for(i = 0; i < (0x192 * 2); i+=2) {
		memcpy(&Readout_Current1, &ROMBuffer[(ROM_ObjectTableOffset / 4) + i], 4);
		memcpy(&Readout_Current2, &ROMBuffer[(ROM_ObjectTableOffset / 4) + i + 1], 4);
		Helper_SplitCurrentVals(true);

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
*/
	/* ---- LOAD ACTOR DATA ---- */
/*
	ROM_ActorTableOffset = 0xB8D440;

	for(i = 0; i < (0x1D7 * 8); i+=8) {
		memcpy(&Readout_Current1, &ROMBuffer[(ROM_ActorTableOffset / 4) + i], 4);
		memcpy(&Readout_Current2, &ROMBuffer[(ROM_ActorTableOffset / 4) + i + 1], 4);
		Helper_SplitCurrentVals(true);

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
*/
	CamAngleX = 0.0f, CamAngleY = 0.0f;
	CamX = 0.0f, CamY = 0.0f, CamZ = 5.0f;
	CamLX = 0.0f, CamLY = 0.0f, CamLZ = -1.0f;
/*
	CamAngleX = 0.0f, CamAngleY = -0.5f;
	CamX = 0.0f, CamY = 2.0f, CamZ = 12.5f;
	CamLX = 0.0f, CamLY = 0.0f, CamLZ = -1.0f;
*/
	memset(CurrentGFXCmd, 0x00, sizeof(CurrentGFXCmd));
	memset(CurrentGFXCmdNote, 0x00, sizeof(CurrentGFXCmdNote));
	memset(GFXLogMsg, 0x00, sizeof(GFXLogMsg));
	memset(SystemLogMsg, 0x00, sizeof(SystemLogMsg));

	sprintf(StatusMsg, "Level: 0x%02X", ROM_SceneToLoad);

	sprintf(WindowTitle, "%s %s - %s", AppTitle, AppVersion, Scene_Name);
	SetWindowText(hwnd, WindowTitle);

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
	OGL_DrawScene();
	Viewer_RenderMap();

//	if(ROMBuffer != NULL) free(ROMBuffer);

	return 0;
}

/*	------------------------------------------------------------ */

void GLUTCamera_Orientation(float ang, float ang2)
{
	CamLX = sin(ang);
	CamLY = ang2;
	CamLZ = -cos(ang);
}

void GLUTCamera_Movement(int direction, bool strafe)
{
	if(!strafe) {
		CamX = CamX + direction * (CamLX) * 0.025f;
		CamY = CamY + direction * (CamLY) * 0.025f;
		CamZ = CamZ + direction * (CamLZ) * 0.025f;
	} else {
		/* insert code for strafing here */
	}
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

int Viewer_SelectActor(int MouseX, int MouseY)
{
	/* not working very well at all... doesn't even pick up actor cubes correctly, no way i can get this to select vertices */

	/* translate mouse click pos into 3d pos in ogl space */
	int Viewport[4];
	double MvMatrix[16];
	double ProjMatrix[16];
	double PosX, PosY, PosZ;
	float InWindowX, InWindowY, InWindowZ;

	glGetIntegerv(GL_VIEWPORT, Viewport);

	InWindowX = MouseX;
	InWindowY = (Viewport[3] - MouseY);
	glReadPixels(MouseX, (int)InWindowY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &InWindowZ);

	glGetDoublev(GL_MODELVIEW_MATRIX, MvMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, ProjMatrix);
	gluUnProject(InWindowX, InWindowY, InWindowZ, MvMatrix, ProjMatrix, Viewport, &PosX, &PosY, &PosZ);

	/* prepare variables */
	int i = 0, j = 0;
	int Type = 0;		/* 0 = map, 1 = scene, 2 = door/rc */
	int MapNo = 0; int SelActorNo = -1;
	int MapFrom = 0; int MapTo = 0;

	/* pickup hits +/- actor center position, otherwise detection is even worse than it is.
	   - value too small, and you need to position the cam really close to the actor
	   - value too high, and adjacent actors can't be separated properly */
	float HitRadiusX = 30.0f;	/* 2x actual size of actor cubes */
	float HitRadiusY = 30.0f;
	float HitRadiusZ = 30.0f;

	if(ROM_CurrentMap == -1) {
		/* when all maps are rendered, check actors of each one */
		MapFrom = 0;
		MapTo = SceneHeader[SceneHeader_Current].Map_Count;
	} else {
		/* else only check actors of selected one */
		MapFrom = ROM_CurrentMap;
		MapTo = MapFrom + 1;
	}

	/* scan loop: compare 3d pos of all map actors with 3d pos of last right-click */
	for(j = MapFrom; j < MapTo; j++) {
		MapNo = j;
		for(i = 0; i < MapHeader[MapNo][MapHeader_Current].Actor_Count; i++) {
			float AX = (float)Actors[MapNo][i].X_Position;
			float AY = (float)Actors[MapNo][i].Y_Position;
			float AZ = (float)Actors[MapNo][i].Z_Position;
			if((AX < (PosX + HitRadiusX)) && (AX > (PosX - HitRadiusX)) &&
				(AY < (PosY + HitRadiusY)) && (AY > (PosY - HitRadiusY)) &&
				(AZ < (PosZ + HitRadiusZ)) && (AZ > (PosZ - HitRadiusZ))) {
				if(Renderer_EnableMapActors) SelActorNo = i;
			}
			/* if hit found, breakout */
			if(SelActorNo != -1) break;
		}
		/* if hit found, breakout */
		if(SelActorNo != -1) break;
	}

	if(SelActorNo == -1) {
		/* next scan loop - scene actors */
		Type = 1;
		for(i = 0; i < SceneHeader[SceneHeader_Current].ScActor_Count; i++) {
			float AX = (float)ScActors[i].X_Position;
			float AY = (float)ScActors[i].Y_Position;
			float AZ = (float)ScActors[i].Z_Position;
			if((AX < (PosX + HitRadiusX)) && (AX > (PosX - HitRadiusX)) &&
				(AY < (PosY + HitRadiusY)) && (AY > (PosY - HitRadiusY)) &&
				(AZ < (PosZ + HitRadiusZ)) && (AZ > (PosZ - HitRadiusZ))) {
				if(Renderer_EnableSceneActors) SelActorNo = i;
			}
			/* if hit found, breakout */
			if(SelActorNo != -1) break;
		}
	}

	if(SelActorNo == -1) {
		/* next scan loop - doors/room-changers */
		Type = 2;
		for(i = 0; i < SceneHeader[SceneHeader_Current].Door_Count; i++) {
			float AX = (float)Doors[i].X_Position;
			float AY = (float)Doors[i].Y_Position;
			float AZ = (float)Doors[i].Z_Position;
			if((AX < (PosX + HitRadiusX)) && (AX > (PosX - HitRadiusX)) &&
				(AY < (PosY + HitRadiusY)) && (AY > (PosY - HitRadiusY)) &&
				(AZ < (PosZ + HitRadiusZ)) && (AZ > (PosZ - HitRadiusZ))) {
				if(Renderer_EnableDoors) SelActorNo = i;
			}
			/* if hit found, breakout */
			if(SelActorNo != -1) break;
		}
	}

	/* if still no hit found, return */
	if(SelActorNo == -1) return 0;

	/* otherwise, show actor data message */
	char Title[256]; char Message[256];
	if(Type == 0) {
		/* map */
		sprintf(Title, "Map #%d, Actor #%d\n", MapNo, SelActorNo);
		sprintf(Message, "Number: 0x%04X, Variable: 0x%04X\n"
			"X Position: %4.2f, Y Position: %4.2f, Z Position: %4.2f\n"
			"X Rotation: %4.2f, Y Rotation: %4.2f, Z Rotation: %4.2f\n"
			/*"(Click Pos: %4.2f, %4.2f, %4.2f)"*/,
			Actors[MapNo][SelActorNo].Number, Actors[MapNo][SelActorNo].Variable,
			(float)Actors[MapNo][SelActorNo].X_Position, (float)Actors[MapNo][SelActorNo].Y_Position, (float)Actors[MapNo][SelActorNo].Z_Position,
			(float)Actors[MapNo][SelActorNo].X_Rotation, (float)Actors[MapNo][SelActorNo].Y_Rotation, (float)Actors[MapNo][SelActorNo].Z_Rotation
			/*, PosX, PosY, PosZ*/);
	} else if(Type == 1) {
		/* scene */
		sprintf(Title, "Scene Actor #%d\n", SelActorNo);
		sprintf(Message, "Number: 0x%04X, Variable: 0x%04X\n"
			"X Position: %4.2f, Y Position: %4.2f, Z Position: %4.2f\n"
			"X Rotation: %4.2f, Y Rotation: %4.2f, Z Rotation: %4.2f\n"
			/*"(Click Pos: %4.2f, %4.2f, %4.2f)"*/,
			ScActors[SelActorNo].Number, ScActors[SelActorNo].Variable,
			(float)ScActors[SelActorNo].X_Position, (float)ScActors[SelActorNo].Y_Position, (float)ScActors[SelActorNo].Z_Position,
			(float)ScActors[SelActorNo].X_Rotation, (float)ScActors[SelActorNo].Y_Rotation, (float)ScActors[SelActorNo].Z_Rotation
			/*, PosX, PosY, PosZ*/);
	} else if(Type == 2) {
		/* door/rc */
		sprintf(Title, "Room-changer #%d\n", SelActorNo);
		sprintf(Message, "Number: 0x%04X, Variable: 0x%04X\n"
			"X Position: %4.2f, Y Position: %4.2f, Z Position: %4.2f\n"
			"Y Rotation: %4.2f\n"
			"Unknown1: 0x%04X, Unknown2: 0x%04X\n"
			/*"(Click Pos: %4.2f, %4.2f, %4.2f)"*/,
			Doors[SelActorNo].Number, Doors[SelActorNo].Variable,
			(float)Doors[SelActorNo].X_Position, (float)Doors[SelActorNo].Y_Position, (float)Doors[SelActorNo].Z_Position,
			(float)Doors[SelActorNo].Y_Rotation,
			Doors[SelActorNo].Unknown1, Doors[SelActorNo].Unknown2
			/*, PosX, PosY, PosZ*/);
	}
	MessageBox(hwnd, Message, Title, MB_OK | MB_ICONINFORMATION);

	return 0;
}

/*	------------------------------------------------------------ */

int CheckUncheckMenu(unsigned int MenuItem, int Checked)
{
	CheckMenuItem(hmenu, MenuItem, MF_BYCOMMAND | (Checked ? MF_CHECKED : MF_UNCHECKED));

	return 0;
}

int CheckUncheckMenu_Filters()
{
	switch(Renderer_FilteringMode_Min) {
		case GL_NEAREST: {
			CheckUncheckMenu(IDM_OPTIONS_FILTERNEAREST, 1);
			CheckUncheckMenu(IDM_OPTIONS_FILTERLINEAR, 0);
			CheckUncheckMenu(IDM_OPTIONS_FILTERMIPMAP, 0);
			break; }
		case GL_LINEAR: {
			CheckUncheckMenu(IDM_OPTIONS_FILTERNEAREST, 0);
			CheckUncheckMenu(IDM_OPTIONS_FILTERLINEAR, 1);
			CheckUncheckMenu(IDM_OPTIONS_FILTERMIPMAP, 0);
			break; }
		case GL_LINEAR_MIPMAP_LINEAR: {
			CheckUncheckMenu(IDM_OPTIONS_FILTERNEAREST, 0);
			CheckUncheckMenu(IDM_OPTIONS_FILTERLINEAR, 0);
			CheckUncheckMenu(IDM_OPTIONS_FILTERMIPMAP, 1);
			break; }
	}

	return 0;
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
/*
	hlvlcombo = CreateWindowEx(
			0,
			"COMBOBOX",
			"",
			CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
			0,
			0,
			180,
			200,
			hwnd,
			(HMENU)IDC_MAIN_LEVELCOMBO,
			hThisInstance,
			NULL);
	HFONT ComboFont = CreateFont(8, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
	SendMessage(hlvlcombo, WM_SETFONT, (WPARAM)ComboFont, (LPARAM)NULL);
*/
	sprintf(WindowTitle, "%s %s", AppTitle, AppVersion);
	SetWindowText(hwnd, WindowTitle);

	GetModuleFileName(NULL, AppPath, sizeof(AppPath) - 1);

	char *AppPathTemp = strrchr(AppPath, '\\');
	if(AppPathTemp) ++AppPathTemp; if(AppPathTemp) *AppPathTemp = 0;
	sprintf(INIPath, "%s\\ozmav.ini", AppPath);

	GetPrivateProfileString("Viewer", "LastROM", "", Filename_ROM, sizeof(Filename_ROM), INIPath);
	ROM_SceneToLoad = GetPrivateProfileInt("Viewer", "LastScene", 0x00, INIPath);

	Renderer_EnableMap = GetPrivateProfileInt("Viewer", "RenderMaps", true, INIPath);
	Renderer_EnableCollision = GetPrivateProfileInt("Viewer", "RenderCollision", false, INIPath);
	Renderer_EnableMapActors = GetPrivateProfileInt("Viewer", "RenderMapActors", true, INIPath);
	Renderer_EnableSceneActors = GetPrivateProfileInt("Viewer", "RenderSceneActors", true, INIPath);
	Renderer_EnableDoors = GetPrivateProfileInt("Viewer", "RenderDoors", true, INIPath);
	Renderer_EnableFog = GetPrivateProfileInt("Viewer", "RenderFog", true, INIPath);
	Renderer_EnableWireframe = GetPrivateProfileInt("Viewer", "RenderWireframe", false, INIPath);

	Renderer_FilteringMode_Min = GetPrivateProfileInt("Viewer", "TexFilterMin", GL_LINEAR_MIPMAP_LINEAR, INIPath);
	Renderer_FilteringMode_Mag = GetPrivateProfileInt("Viewer", "TexFilterMag", GL_LINEAR, INIPath);

	Renderer_EnableWavefrontDump = GetPrivateProfileInt("Viewer", "DumpToWavefrontModel", false, INIPath);

	Renderer_EnableFragShader = GetPrivateProfileInt("Viewer", "UseFragShader", true, INIPath);

	CheckUncheckMenu(IDM_ACTORS_MAPRENDER, Renderer_EnableMapActors);
	CheckUncheckMenu(IDM_ACTORS_SCENERENDER, Renderer_EnableSceneActors);
	CheckUncheckMenu(IDM_ACTORS_DOORRENDER, Renderer_EnableDoors);
	CheckUncheckMenu(IDM_OPTIONS_RENDERMAPS, Renderer_EnableMap);
	CheckUncheckMenu(IDM_OPTIONS_RENDERCOLLISION, Renderer_EnableCollision);
	CheckUncheckMenu(IDM_OPTIONS_WAVEFRONT, Renderer_EnableWavefrontDump);
	//CheckUncheckMenu(IDM_OPTIONS_FRAGSHADER, Renderer_EnableFragShader);
	CheckUncheckMenu(IDM_OPTIONS_RENDERFOG, Renderer_EnableFog);
	CheckUncheckMenu(IDM_OPTIONS_WIREFRAME, Renderer_EnableWireframe);

	CheckUncheckMenu_Filters();

	char Temp[1024];
	sprintf(Temp, "%s\\dump", AppPath);
	CreateDirectory(Temp, NULL);

	if (!OGL_CreateTarget(640,480,16)) return 0;

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
							if(Viewer_LoadAreaData() == -1) {
								MessageBox(hwnd, "Error while loading level!", "Error", MB_OK | MB_ICONEXCLAMATION);
								AreaLoaded = true;
							} else {
								Viewer_RenderMapRefresh();
							}
						}
						sprintf(StatusMsg, "Level: 0x%02X", ROM_SceneToLoad);
					}
					if (System_KbdKeys[VK_F2]) {
						System_KbdKeys[VK_F2] = false;
						if(!(ROM_SceneToLoad == ROM_MaxSceneCount)) {
							ROM_SceneToLoad++;
							Viewer_ResetVariables();
							if(Viewer_LoadAreaData() == -1) {
								MessageBox(hwnd, "Error while loading level!", "Error", MB_OK | MB_ICONEXCLAMATION);
								AreaLoaded = true;
							} else {
								Viewer_RenderMapRefresh();
							}
						}
						sprintf(StatusMsg, "Level: 0x%02X", ROM_SceneToLoad);
					}
					if (System_KbdKeys[VK_F3]) {
						System_KbdKeys[VK_F3] = false;
						if(!(ROM_CurrentMap == -1)) {
							ROM_CurrentMap--;
						}
						sprintf(StatusMsg, "Map: 0x%02X", ROM_CurrentMap);
						if(ROM_CurrentMap == -1) sprintf(StatusMsg, "Map: rendering all");
					}
					if (System_KbdKeys[VK_F4]) {
						System_KbdKeys[VK_F4] = false;
						if(!(ROM_CurrentMap == SceneHeader[SceneHeader_Current].Map_Count - 1)) {
							ROM_CurrentMap++;
						}
						sprintf(StatusMsg, "Map: 0x%02X", ROM_CurrentMap);
					}

					if (System_KbdKeys[VK_F5]) {
						System_KbdKeys[VK_F5] = false;
						if(!(CurrentEnvSetting == 0)) {
							CurrentEnvSetting--;
							Zelda_SelectEnvSettings();
						}
						sprintf(StatusMsg, "Environment: 0x%02X", CurrentEnvSetting);
					}
					if (System_KbdKeys[VK_F6]) {
						System_KbdKeys[VK_F6] = false;
						if(!(CurrentEnvSetting == SceneHeader[SceneHeader_Current].EnvSetting_Count - 1)) {
							CurrentEnvSetting++;
							Zelda_SelectEnvSettings();
						}
						sprintf(StatusMsg, "Environment: 0x%02X", CurrentEnvSetting);
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
								char Temp[1024];
								sprintf(Temp, "%s\\log.txt", AppPath);
								FileSystemLog = fopen(Temp, "w");
								Zelda_GetSceneHeader(SceneHeader_Current);
								Zelda_GetSceneActors(SceneHeader_Current);
								Zelda_GetDoorData(SceneHeader_Current);
								Zelda_GetEnvironmentSettings(SceneHeader_Current);
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
								char Temp[1024];
								sprintf(Temp, "%s\\log.txt", AppPath);
								FileSystemLog = fopen(Temp, "w");
								Zelda_GetSceneHeader(SceneHeader_Current);
								Zelda_GetSceneActors(SceneHeader_Current);
								Zelda_GetDoorData(SceneHeader_Current);
								Zelda_GetEnvironmentSettings(SceneHeader_Current);
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
							GLUTCamera_Movement(6, false);
						} else if(System_KbdKeys[VK_CONTROL]) {
							GLUTCamera_Movement(2, false);
						} else {
							GLUTCamera_Movement(4, false);
						}
					}
					if (System_KbdKeys['S']) {
						if(System_KbdKeys[VK_SHIFT]) {
							GLUTCamera_Movement(-6, false);
						} else if(System_KbdKeys[VK_CONTROL]) {
							GLUTCamera_Movement(-2, false);
						} else {
							GLUTCamera_Movement(-4, false);
						}
					}

					if (System_KbdKeys['A']) {
						if(System_KbdKeys[VK_SHIFT]) {
							GLUTCamera_Movement(-6, true);
						} else if(System_KbdKeys[VK_CONTROL]) {
							GLUTCamera_Movement(-2, true);
						} else {
							GLUTCamera_Movement(-4, true);
						}
					}

					if (System_KbdKeys[VK_LEFT]) {
						if(System_KbdKeys[VK_SHIFT]) {
							CamAngleX -= 0.0375;
						} else if(System_KbdKeys[VK_CONTROL]) {
							CamAngleX -= 0.0125f;
						} else {
							CamAngleX -= 0.025f;
						}
					}
					if (System_KbdKeys[VK_RIGHT]) {
						if(System_KbdKeys[VK_SHIFT]) {
							CamAngleX += 0.0375;
						} else if(System_KbdKeys[VK_CONTROL]) {
							CamAngleX += 0.0125f;
						} else {
							CamAngleX += 0.025f;
						}
					}

					if (System_KbdKeys[VK_UP]) {
						if(System_KbdKeys[VK_SHIFT]) {
							CamAngleY += 0.020f;
						} else if(System_KbdKeys[VK_CONTROL]) {
							CamAngleY += 0.010f;
						} else {
							CamAngleY += 0.015f;
						}
					}
					if (System_KbdKeys[VK_DOWN]) {
						if(System_KbdKeys[VK_SHIFT]) {
							CamAngleY -= 0.020f;
						} else if(System_KbdKeys[VK_CONTROL]) {
							CamAngleY -= 0.010f;
						} else {
							CamAngleY -= 0.015f;
						}
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

			OGL_DrawScene();
			SwapBuffers(hDC_ogl);
		}
	}

	WritePrivateProfileString("Viewer", "LastROM", Filename_ROM, INIPath);
	char TempStr[256];
	sprintf(TempStr, "%d", ROM_SceneToLoad);
	WritePrivateProfileString("Viewer", "LastScene", TempStr, INIPath);

	sprintf(TempStr, "%d", Renderer_EnableMap);
	WritePrivateProfileString("Viewer", "RenderMaps", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableCollision);
	WritePrivateProfileString("Viewer", "RenderCollision", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableMapActors);
	WritePrivateProfileString("Viewer", "RenderMapActors", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableSceneActors);
	WritePrivateProfileString("Viewer", "RenderSceneActors", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableDoors);
	WritePrivateProfileString("Viewer", "RenderDoors", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableFog);
	WritePrivateProfileString("Viewer", "RenderFog", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_EnableWireframe);
	WritePrivateProfileString("Viewer", "RenderWireframe", TempStr, INIPath);

	sprintf(TempStr, "%d", Renderer_FilteringMode_Min);
	WritePrivateProfileString("Viewer", "TexFilterMin", TempStr, INIPath);
	sprintf(TempStr, "%d", Renderer_FilteringMode_Mag);
	WritePrivateProfileString("Viewer", "TexFilterMag", TempStr, INIPath);

	sprintf(TempStr, "%d", Renderer_EnableWavefrontDump);
	WritePrivateProfileString("Viewer", "DumpToWavefrontModel", TempStr, INIPath);

	sprintf(TempStr, "%d", Renderer_EnableFragShader);
	WritePrivateProfileString("Viewer", "UseFragShader", TempStr, INIPath);

	OGL_KillTarget();
	DestroyWindow(hwnd);

	return (messages.wParam);
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_ACTIVATE: {
			if(!HIWORD(wParam)) {
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
			//SetWindowPos(hogl, NULL, 0, 20, rcClient.right, rcClient.bottom - 40, SWP_NOZORDER);
			SetWindowPos(hogl, NULL, 0, 0, rcClient.right, rcClient.bottom - 20, SWP_NOZORDER);

			RECT rcStatus;
			GetDlgItem(hwnd, IDC_MAIN_STATUSBAR);
			SendMessage(hstatus, WM_SIZE, 0, 0);
			GetWindowRect(hstatus, &rcStatus);

			OGL_ResizeScene(rcClient.right, rcClient.bottom);
			OGL_DrawScene();
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
//					System_KbdKeys[VK_DIVIDE] = true;
					break;
				case IDM_SCENE_NEXTHEADER:
//					System_KbdKeys[VK_MULTIPLY] = true;
					break;
				case IDM_ACTORS_MAPRENDER:
					Renderer_EnableMapActors = !Renderer_EnableMapActors;
					CheckUncheckMenu(IDM_ACTORS_MAPRENDER, Renderer_EnableMapActors);
					break;
/*				case IDM_ACTORS_SELECTPREV:
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
*/				case IDM_ACTORS_SCENERENDER:
					Renderer_EnableSceneActors = !Renderer_EnableSceneActors;
					CheckUncheckMenu(IDM_ACTORS_SCENERENDER, Renderer_EnableSceneActors);
					break;
/*				case IDM_ACTORS_SCENEPREV:
					System_KbdKeys[VK_SUBTRACT] = true;
					break;
				case IDM_ACTORS_SCENENEXT:
					System_KbdKeys[VK_ADD] = true;
					break;
*/				case IDM_ACTORS_DOORRENDER:
					Renderer_EnableDoors = !Renderer_EnableDoors;
					CheckUncheckMenu(IDM_ACTORS_DOORRENDER, Renderer_EnableDoors);
					break;
				case IDM_OPTIONS_RENDERFOG:
					Renderer_EnableFog = !Renderer_EnableFog;
					CheckUncheckMenu(IDM_OPTIONS_RENDERFOG, Renderer_EnableFog);
					Viewer_RenderMap();
					break;
				case IDM_OPTIONS_RESETCAMCOORDS:
					System_KbdKeys[VK_TAB] = true;
					break;
				case IDM_OPTIONS_FILTERNEAREST:
				case IDM_OPTIONS_FILTERLINEAR:
				case IDM_OPTIONS_FILTERMIPMAP:
					/* shit */
					switch(LOWORD(wParam)) {
						case IDM_OPTIONS_FILTERNEAREST: {
							/* nearest */
							Renderer_FilteringMode_Min = GL_NEAREST;
							Renderer_FilteringMode_Mag = GL_NEAREST;
							break; }
						case IDM_OPTIONS_FILTERLINEAR: {
							/* linear */
							Renderer_FilteringMode_Min = GL_LINEAR;
							Renderer_FilteringMode_Mag = GL_LINEAR;
							break; }
						case IDM_OPTIONS_FILTERMIPMAP: {
							/* mipmap */
							Renderer_FilteringMode_Min = GL_LINEAR_MIPMAP_LINEAR;
							Renderer_FilteringMode_Mag = GL_LINEAR;
							break; }
					}
					CheckUncheckMenu_Filters();
					Viewer_RenderMap();
					break;
				case IDM_OPTIONS_RENDERMAPS:
					Renderer_EnableMap = !Renderer_EnableMap;
					CheckUncheckMenu(IDM_OPTIONS_RENDERMAPS, Renderer_EnableMap);
					break;
				case IDM_OPTIONS_RENDERCOLLISION:
					Renderer_EnableCollision = !Renderer_EnableCollision;
					CheckUncheckMenu(IDM_OPTIONS_RENDERCOLLISION, Renderer_EnableCollision);
					break;
				case IDM_OPTIONS_WAVEFRONT:
					Renderer_EnableWavefrontDump = !Renderer_EnableWavefrontDump;
					CheckUncheckMenu(IDM_OPTIONS_WAVEFRONT, Renderer_EnableWavefrontDump);
					if(Renderer_EnableWavefrontDump) Viewer_RenderMap();
					break;
/*				case IDM_OPTIONS_FRAGSHADER:
					Renderer_EnableFragShader = !Renderer_EnableFragShader;
					CheckUncheckMenu(IDM_OPTIONS_FRAGSHADER, Renderer_EnableFragShader);
					Viewer_RenderMap();
					break;*/
				case IDM_OPTIONS_WIREFRAME:
					Renderer_EnableWireframe = !Renderer_EnableWireframe;
					CheckUncheckMenu(IDM_OPTIONS_WIREFRAME, Renderer_EnableWireframe);
					Viewer_RenderMap();
					break;
				case IDM_HELP_CONTROLS: {
					char CtrlMsg[1024] = "";
					sprintf(CtrlMsg,
						"Controls:\n"
						"\n"
						"Left mouse click + movement: Rotate camera\n"
						"Right mouse click on actor: show actor properties\n"
						"W/S: move camera forward/backward\n"
						"Cursor keys: rotate camera\n"
						"F1/F2: load previous/next scene from ROM\n"
						"F3/F4: if scene has multiple maps, select which map to render\n"
						"F5/F6: select which environment setting to use (defaults to 0x01, generally daytime)\n"
						"Numpad / and *: select scene header to use\n"
						);
					MessageBox(hwnd, CtrlMsg, "Controls", MB_OK | MB_ICONINFORMATION);
					break; }
				case IDM_HELP_ABOUT: {
					sprintf(GLExtensionsSupported, "OpenGL extensions supported and used:\n");
					if(GLExtension_MultiTexture) sprintf(GLExtensionsSupported, "%sGL_ARB_multitexture\n", GLExtensionsSupported);
					if(GLExtension_TextureMirror) sprintf(GLExtensionsSupported, "%sGL_ARB_texture_mirrored_repeat\n", GLExtensionsSupported);
					if(GLExtension_AnisoFilter) sprintf(GLExtensionsSupported, "%sGL_EXT_texture_filter_anisotropic\n", GLExtensionsSupported);
					if(GLExtension_VertFragProgram) sprintf(GLExtensionsSupported, "%sGL_ARB_fragment_program\nGL_ARB_vertex_program\n", GLExtensionsSupported);

					char AboutMsg[256] = "";
					sprintf(AboutMsg, "%s %s (Build '%s') - OpenGL Zelda Map Viewer\n\n"
						"Written 2008/2009 by xdaniel & contributors\n"
						"http://ozmav.googlecode.com/\n\n"
						"%s", AppTitle, AppVersion, AppBuildName, GLExtensionsSupported);
					MessageBox(hwnd, AboutMsg, "About", MB_OK | MB_ICONINFORMATION);
					break; }

//				case IDC_MAIN_LEVELCOMBO: {
//					ROM_SceneToLoad = SendMessage(GetDlgItem(hwnd, IDC_MAIN_LEVELCOMBO), CB_GETCURSEL, (WPARAM)NULL, (LPARAM)NULL);
//					SendMessage(hwnd, WM_SETFOCUS, (WPARAM)GetDlgItem(hwnd, IDC_MAIN_OPENGL), (LPARAM)NULL);
//					Viewer_LoadAreaData();
//					break; }
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
		case WM_RBUTTONDOWN: {
			Viewer_SelectActor((signed int)LOWORD(lParam), (signed int)HIWORD(lParam));
			break;}

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
		case WM_KILLFOCUS: {
			/* fix for ghost keypresses when leaving window */
			MouseButtonDown = false;
			memset(System_KbdKeys, false, sizeof(System_KbdKeys));
			break;
		}
		default: return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

/*	------------------------------------------------------------ */
