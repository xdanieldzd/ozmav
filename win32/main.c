/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written in October/November 2008 by xdaniel & contributors
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

char			szClassName[] = "OZMAVClass";

/* GENERAL GLOBAL PROGRAM VARIABLES */
bool			System_KbdKeys[256];

char			AppTitle[256] = "OZMAV";
char			AppVersion[256] = "V0.6";
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
char			GFXLogMsg[256] = "";
char			SystemLogMsg[256] = "";

bool			GFXLogOpened = false;

/* CAMERA / VIEWPOINT VARIABLES */
float			CamAngleX = 0, CamAngleY = 0;
float			CamX = 0, CamY = 0, CamZ = 0;
float			CamLX = 0, CamLY = 0, CamLZ = 0;

int				MousePosX = 0, MousePosY = 0;
int				MouseCenterX = 0, MouseCenterY = 0;

bool			MouseButtonDown = false;

/* FILE HANDLING VARIABLES */
FILE			* FileROM = NULL;
FILE			* FileZMap = NULL;
FILE			* FileZScene = NULL;
FILE			* FileGameplayKeep = NULL;
FILE			* FileGameplayFDKeep = NULL;

unsigned int	* ROMBuffer;
unsigned int	* ZMapBuffer[256];
unsigned int	* ZSceneBuffer;
unsigned int	* GameplayKeepBuffer;
unsigned int	* GameplayFDKeepBuffer;

unsigned long	ROMFilesize = 0;
unsigned long	ZMapFilesize[256];
unsigned long	ZSceneFilesize = 0;
unsigned long	GameplayKeepFilesize = 0;
unsigned long	GameplayFDKeepFilesize = 0;

char			Filename_ROM[256] = "";
char			Filename_ZMap[256] = "";
char			Filename_ZScene[256] = "";
char			Filename_GameplayKeep[256] = "";
char			Filename_GameplayFDKeep[256] = "";

bool			ROMExists = false;
bool			ZMapExists = false;
bool			ZSceneExists = false;

FILE			* FileGFXLog = NULL;
FILE			* FileSystemLog = NULL;

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

bool			IsMultitex = false;
unsigned int	MTexScaler = 1;

/* ZELDA ROM HANDLING VARIABLES */
unsigned long	ROM_SceneTableOffset = 0x00;
unsigned int	ROM_SceneToLoad = 0x00;
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

bool			Renderer_EnableCombiner = false;

/* OPENGL EXTENSION VARIABLES */
char			* GLExtension_List;
bool			GLExtension_MultiTexture = false;
bool			GLExtension_TextureMirror = false;
bool			GLExtension_AnisoFilter = false;
char			GLExtensionsSupported[256] = "";

bool			GLExtensionsUnsupported = false;
char			GLExtensionsErrorMsg[512] = "";

/* N64 BLENDING & COMBINER SIMULATION VARIABLES */
unsigned int	RDPCycleMode = 0;

unsigned long	Blender_Cycle1 = 0x00;
unsigned long	Blender_Cycle2 = 0x00;

unsigned int	Combiner_TextureMode = 0;
unsigned int	Combiner_Texture = 0;
unsigned int	Combiner_AlphaMode = 0;
unsigned int	Combiner_AlphaCycles = 1;

unsigned long	Combiner_Cycle1 = 0x00;
unsigned long	Combiner_Cycle2 = 0x00;

GLenum			Combiner_Color_A0 = 0x00;
GLenum			Combiner_Color_B0 = 0x00;
GLenum			Combiner_Color_C0 = 0x00;
GLenum			Combiner_Color_D0 = 0x00;
GLenum			Combiner_Alpha_A0 = 0x00;
GLenum			Combiner_Alpha_B0 = 0x00;
GLenum			Combiner_Alpha_C0 = 0x00;
GLenum			Combiner_Alpha_D0 = 0x00;

GLenum			Combiner_Color_A1 = 0x00;
GLenum			Combiner_Color_B1 = 0x00;
GLenum			Combiner_Color_C1 = 0x00;
GLenum			Combiner_Color_D1 = 0x00;
GLenum			Combiner_Alpha_A1 = 0x00;
GLenum			Combiner_Alpha_B1 = 0x00;
GLenum			Combiner_Alpha_C1 = 0x00;
GLenum			Combiner_Alpha_D1 = 0x00;

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
struct Texture_Struct Texture[1];
/* CI TEXTURE PALETTE STRUCTURE */
struct Palette_Struct Palette[512];

/*	------------------------------------------------------------ */

/* VIEWER_INITIALIZE - CALLED AFTER SELECTING THE MAP AND SCENE FILES */
int Viewer_Initialize()
{
	/* FREE PREVIOUSLY ALLOCATED MEMORY */
	int i;
	for(i = 0; i < 128; i++) {
		if(ZMapBuffer[i] != NULL) free(ZMapBuffer[i]);
	}
	if(ROMBuffer != NULL) free(ROMBuffer);
	if(ZSceneBuffer != NULL) free(ZSceneBuffer);
//	if(GameplayKeepBuffer != NULL) free(GameplayKeepBuffer);
//	if(GameplayFDKeepBuffer != NULL) free(GameplayFDKeepBuffer);
	if(PaletteData != NULL) free(PaletteData);

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
		return 0;
	}

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

	sprintf(WindowTitle, "%s %s - %s", AppTitle, AppVersion, Filename_ROM);
	SetWindowText(hwnd, WindowTitle);

	return 0;
}

int Viewer_LoadAreaData()
{
	FileSystemLog = fopen("log.txt", "w");

	AreaLoaded = false;

	int i;

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

	/* get current scene's offset */
	unsigned long TempOffset = (ROM_SceneTableOffset / 4) + (ROM_SceneToLoad * 0x14) / 4;
	memcpy(&Readout_Current1, &ROMBuffer[TempOffset], 4);
	memcpy(&Readout_Current2, &ROMBuffer[TempOffset + 1], 4);
	HelperFunc_SplitCurrentVals(true);

	unsigned long Scene_Start = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned long Scene_End = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
	unsigned long Scene_Length = Scene_End - Scene_Start;

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
		}

		Viewer_GetMapHeader(MapHeader_Current);
		Viewer_GetMapActors(MapHeader_Current);
		Viewer_GetDisplayLists(ZMapFilesize[i]);
	}

	CamAngleX = 0.0f, CamAngleY = 0.0f;
	CamX = 0.0f, CamY = 0.0f, CamZ = 5.0f;
	CamLX = 0.0f, CamLY = 0.0f, CamLZ = -1.0f;

	memset(CurrentGFXCmd, 0x00, sizeof(CurrentGFXCmd));
	memset(CurrentGFXCmdNote, 0x00, sizeof(CurrentGFXCmdNote));
	memset(GFXLogMsg, 0x00, sizeof(GFXLogMsg));
	memset(SystemLogMsg, 0x00, sizeof(SystemLogMsg));

	sprintf(StatusMsg, "Map loaded successfully!");

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

	GetPrivateProfileString("Viewer", "LastROM", "", Filename_ROM, sizeof(Filename_ROM), INIPath);
	ROM_SceneTableOffset = GetPrivateProfileInt("Viewer", "SceneTable", 0xBA0BB0, INIPath);
	ROM_SceneToLoad = GetPrivateProfileInt("Viewer", "LastScene", 0x00, INIPath);
	Renderer_FilteringMode_Min = GetPrivateProfileInt("Viewer", "TexFilterMin", GL_LINEAR_MIPMAP_LINEAR, INIPath);
	Renderer_FilteringMode_Mag = GetPrivateProfileInt("Viewer", "TexFilterMag", GL_LINEAR, INIPath);
	Renderer_EnableMapActors = GetPrivateProfileInt("Viewer", "RenderMapActors", true, INIPath);
	Renderer_EnableSceneActors = GetPrivateProfileInt("Viewer", "RenderSceneActors", false, INIPath);
	Renderer_EnableCombiner = GetPrivateProfileInt("Viewer", "EnableCombiner", false, INIPath);

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
							if(ROMExists) Viewer_LoadAreaData(); Viewer_RenderMap();
						}
						sprintf(StatusMsg, "Rendering level 0x%02X", ROM_SceneToLoad);
					}
					if (System_KbdKeys[VK_F2]) {
						System_KbdKeys[VK_F2] = false;
						if(!(ROM_SceneToLoad == 0x6D)) {
							ROM_SceneToLoad++;
							if(ROMExists) Viewer_LoadAreaData();
						}
						sprintf(StatusMsg, "Rendering level 0x%02X", ROM_SceneToLoad);
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
	sprintf(TempStr, "%d", Renderer_EnableCombiner);
	WritePrivateProfileString("Viewer", "EnableCombiner", TempStr, INIPath);

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
					ROMExists = false;

					Dialog_OpenROM(hwnd);
					if(ROMExists) Viewer_Initialize();
/*					ZMapExists = false;
					ZSceneExists = false;

					Dialog_OpenZMap(hwnd);

					if(ZMapExists) Dialog_OpenZScene(hwnd);
					if(ZSceneExists) Viewer_Initialize();
*/					break;
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
					if(Renderer_EnableCombiner) {
						Renderer_EnableCombiner = false;
					} else {
						Renderer_EnableCombiner = true;
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
