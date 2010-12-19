// Project DLViewer
// December 2010 by xdaniel

// ----------------------------------------

#include "globals.h"

// ----------------------------------------

struct __zProgram zProgram;
struct __zOptions zOptions;

struct __zCamera zCamera;

struct __zHUD zHUD;

// ----------------------------------------

__RAM RAM[MAX_SEGMENTS];
__RDRAM RDRAM;

// ----------------------------------------

void die(int Code);

// ----------------------------------------

int main(int argc, char * argv[])
{
	char Temp[MAX_PATH];

	sprintf(zProgram.Title,
		APPTITLE" "VERSION" (build "__DATE__" "__TIME__
		#ifdef DEBUG
			" / Debug"
		#endif
		")");

	// get working directory from executable path
	GetFilePath(argv[0], zProgram.AppPath);

	// check for --help and --about options
	if(argc > 1) {
		if(!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
			char Temp[MAX_PATH];
			GetFileName(argv[0], Temp);
			printf("Prototype:\n %s [options]\n\nOptions:\n -r PATH\tPath to Ocarina of Time or Majora's Mask ROM to load\n -s SCENE\tInitial scene number to load, in decimal\n -d LEVEL\tLevel of debugging messages shown, between 0 and 3\n -a, --about\tDisplay program information\n -h, --help\tThis message\n", Temp);
			return EXIT_SUCCESS;
		}

		if(!strcmp(argv[1], "--about") || !strcmp(argv[1], "-a")) {
			printf("%s", zProgram.Title);
			#ifdef DEBUG
			printf(" (Debug build)");
			#endif
			printf("\n");
			return EXIT_SUCCESS;
		}
	}

	// init libMISAKA
	MSK_Init(zProgram.Title);
	sprintf(Temp, "%s//log.txt", zProgram.AppPath);
	MSK_InitLogging(Temp);
	MSK_SetValidCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789.,:\\/\"-()[]_!");
	cn_InitCommands();

	#ifdef WIN32
	dbgprintf(0, MSK_COLORTYPE_INFO, APPTITLE" launched, running on 32/64-bit Windows...\n");
	#else
	dbgprintf(0, MSK_COLORTYPE_INFO, APPTITLE" launched, running on non-Windows OS...\n");
	#endif
	dbgprintf(0, MSK_COLORTYPE_INFO, "\n");

	// init API via OZ wrapper
	zProgram.WindowWidth = WINDOW_WIDTH;
	zProgram.WindowHeight = WINDOW_HEIGHT;
	if(oz_InitProgram(zProgram.Title, zProgram.WindowWidth, zProgram.WindowHeight)) die(EXIT_FAILURE);

	// init HUD system
	if(hud_Init()) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Failed to initialize HUD system!\n");
		die(EXIT_FAILURE);
	} else {
		hudMenu_Init();
	}

	// init OpenGL & renderer
	RDP_SetupOpenGL();
	RDP_SetRendererOptions(BRDP_TEXTURES | BRDP_COMBINER | BRDP_TEXCRC/* | BRDP_WIREFRAME*/);
	dl_ViewerInit(F3DEX2);

	gl_SetupScene3D(zProgram.WindowWidth, zProgram.WindowHeight);

	// init viewer
	gl_CreateViewerDLists();

	zProgram.ScaleFactor = 0.1f;
	zProgram.DListCount = -1;
	zProgram.DListSel = -1;

	zOptions.EnableHUD = true;
	zOptions.EnableGrid = true;

	ca_Reset();

	// get the program to run
	zProgram.IsRunning = true;

	while(zProgram.IsRunning) {
		// let the API do whatever it needs to
		switch(oz_APIMain()) {
			// API's done...
			case EXIT_SUCCESS: {
				// all clear, let MISAKA do her stuff
				zProgram.IsRunning = MSK_DoEvents();

				// let OpenGL do the rendering
				gl_DrawScene();
				if(gl_FinishScene()) die(EXIT_FAILURE);

				// calculate camera speed
				zCamera.CamSpeed = ScaleRange(zProgram.ScaleFactor, 0.0005f, 1.0f, 1.0f, 10.0f) * 7.5f;

				// FPS calculation!
				zProgram.Frames++;
				if((clock() - zProgram.LastTime) >= 1000) {
					zProgram.LastTime = clock();
					zProgram.LastFPS = zProgram.Frames;
					zProgram.Frames = 0;
				}
				break;
			}

			// API's not done, so do nothing here...
			case -1: {
				break; }

			// ouch, something bad happened with the API, terminating now...
			case EXIT_FAILURE: {
				die(EXIT_FAILURE); }
		}
	}

	// trying to do a clean exit with the API
	if(oz_ExitProgram()) die(EXIT_FAILURE);

	// now die
	die(EXIT_SUCCESS);

	return EXIT_FAILURE;
}

// ----------------------------------------

int DoMainKbdInput()
{
	if(RDRAM.IsSet == false) {
		if(zProgram.Key[KEY_CAMERA_UP]) ca_Movement(false, zCamera.CamSpeed);
		if(zProgram.Key[KEY_CAMERA_DOWN]) ca_Movement(false, -zCamera.CamSpeed);
		if(zProgram.Key[KEY_CAMERA_LEFT]) ca_Movement(true, -zCamera.CamSpeed);
		if(zProgram.Key[KEY_CAMERA_RIGHT]) ca_Movement(true, zCamera.CamSpeed);
	}

	if(zProgram.Key[KEY_GUI_TOGGLEHUD]) {
		zOptions.EnableHUD ^= 1;
		zProgram.Key[KEY_GUI_TOGGLEHUD] = false;
	}

	if(zProgram.Key[KEY_GUI_TOGGLEGRID]) {
		zOptions.EnableGrid ^= 1;
		zProgram.Key[KEY_GUI_TOGGLEGRID] = false;
	}

	if(zProgram.Key[KEY_DLIST_NEXTLIST] && zProgram.DListCount >= 0) {
		if((zProgram.DListSel < zProgram.DListCount) && (zProgram.DListCount != 0)) {
			zProgram.DListSel++;
		}
		zProgram.Key[KEY_DLIST_NEXTLIST] = false;
	}

	if(zProgram.Key[KEY_DLIST_PREVLIST] && zProgram.DListCount >= 0) {
		if((zProgram.DListSel >= 0) && (zProgram.DListCount != 0)) {
			zProgram.DListSel--;
		}
		zProgram.Key[KEY_DLIST_PREVLIST] = false;
	}

	return EXIT_SUCCESS;
}

// ----------------------------------------

float ScaleRange(float in, float oldMin, float oldMax, float newMin, float newMax)
{
	return (in / ((oldMax - oldMin) / (newMax - newMin))) + newMin;
}

void GetFilePath(char * FullPath, char * Target)
{
	char Temp[MAX_PATH];
	strcpy(Temp, FullPath);
	char * Ptr;
	if((Ptr = strrchr(Temp, '\\')) || (Ptr = strrchr(Temp, '/'))) {
		Ptr++;
		*Ptr = '\0';
		strcpy(Target, Temp);
	} else {
		sprintf(Target, ".%c", FILESEP);
	}
}

void GetFileName(char * FullPath, char * Target)
{
	char * Ptr;
	if((Ptr = strrchr(FullPath, FILESEP))) {
		strcpy(Target, Ptr+1);
	} else {
		strcpy(Target, FullPath);
	}
}

inline void dbgprintf(int Level, int Type, char * Format, ...)
{
	if(zOptions.DebugLevel >= Level) {
		char Text[256];
		va_list argp;

		if(Format == NULL) return;

		va_start(argp, Format);
		vsprintf(Text, Format, argp);
		va_end(argp);

		MSK_ConsolePrint(Type, Text);
	}
}

// ----------------------------------------

void die(int Code)
{
	// clear out HUD stuff
	hud_KillFont();

	dbgprintf(0, MSK_COLORTYPE_INFO, "\n");

	// determine if program exited normally or not
	if(Code == EXIT_SUCCESS)
		dbgprintf(0, MSK_COLORTYPE_INFO, "Program terminated normally.\n");
	else
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Program terminated abnormally, error code %i.\n", Code);

	dbgprintf(0, MSK_COLORTYPE_INFO, "\n");
	dbgprintf(0, MSK_COLORTYPE_INFO, "Press any key to continue...\n");

	// one more MISAKA update
	MSK_DoEvents();

	// wait for keypress, then shut down MISAKA and exit
	while(!getch());
	MSK_Exit();

	exit(Code);
}
