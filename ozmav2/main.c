// Project "OZMAV2"
// February/March 2010 by xdaniel & contributors

// ----------------------------------------

#include "globals.h"

#include "confunc.h"

// ----------------------------------------

#ifdef WIN32
PFNGLMULTITEXCOORD1FARBPROC			glMultiTexCoord1fARB = NULL;
PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB = NULL;
PFNGLMULTITEXCOORD3FARBPROC			glMultiTexCoord3fARB = NULL;
PFNGLMULTITEXCOORD4FARBPROC			glMultiTexCoord4fARB = NULL;
PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB = NULL;
#endif

PFNGLGENPROGRAMSARBPROC				glGenProgramsARB = NULL;
PFNGLBINDPROGRAMARBPROC				glBindProgramARB = NULL;
PFNGLDELETEPROGRAMSARBPROC			glDeleteProgramsARB = NULL;
PFNGLPROGRAMSTRINGARBPROC			glProgramStringARB = NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC	glProgramEnvParameter4fARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC	glProgramLocalParameter4fARB = NULL;

// ----------------------------------------

struct __zProgram zProgram;
struct __zOpenGL zOpenGL;
struct __zOptions zOptions;

struct __zROM zROM;
struct __zGame zGame;
struct __zRAM zRAM[64];
struct __zHeader zSHeader[256];
struct __zHeader zMHeader[256][256];

struct __zGfx zGfx;
struct __zPalette zPalette[256];

struct __zVertex zVertex[32];

struct __zTexture zTexture[2];

struct __zObject zObject[1024];
struct __zActor zActor[1024];
struct __zMapActor zLink[256];
struct __zMapActor zMapActor[256][256];
struct __zDoor zDoor[256];

struct __zFragmentCache zFragmentCache[CACHE_FRAGMENT];
struct __zTextureCache zTextureCache[CACHE_TEXTURES];

struct __zCamera zCamera;

// ----------------------------------------

void die(int Code);

// ----------------------------------------

int main(int argc, char * argv[])
{
	char Temp[MAX_PATH];
//	debugLog = false;

	sprintf(zProgram.Title, APPTITLE" "VERSION" (build "__DATE__" "__TIME__")");
	sprintf(zGame.TitleText, "No ROM loaded");

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
	MSK_SetValidCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789.,\\/\"-()[]_!");
	MSK_AddCommand("loadrom", "Load ROM file to use", cn_Cmd_LoadROM);
	MSK_AddCommand("loadscene", "Load specific Scene (0-x)", cn_Cmd_LoadScene);
	MSK_AddCommand("dumpobj", "Dump Scene to .obj file", cn_Cmd_DumpObj);
	MSK_AddCommand("settexture", "Disable/enable texturing (0-1)", cn_Cmd_SetTexture);
	MSK_AddCommand("setcombiner", "Disable/enable combiner (0-1)", cn_Cmd_SetCombiner);
	MSK_AddCommand("resetcam", "Reset camera to initial position", cn_Cmd_ResetCam);
	MSK_AddCommand("setdebug", "Set debug level (0-3)", cn_Cmd_SetDebug);
	MSK_AddCommand("list", "List all files in ROM", cn_Cmd_ListFiles);
	MSK_AddCommand("extract", "Extract all files from ROM", cn_Cmd_ExtractFiles);
	MSK_AddCommand("options", "Show options dialog", cn_Cmd_Options);
	MSK_AddCommand("about", "About the program", cn_Cmd_About);

	#ifdef WIN32
	dbgprintf(0, MSK_COLORTYPE_INFO, APPTITLE" launched, running on 32/64-bit Windows...\n");
	#else
	dbgprintf(0, MSK_COLORTYPE_INFO, APPTITLE" launched, running on non-Windows OS...\n");
	#endif
	dbgprintf(0, MSK_COLORTYPE_INFO, "\n");

	// init API via OZ wrapper
	if(oz_InitProgram(APPTITLE, WINDOW_WIDTH, WINDOW_HEIGHT)) die(EXIT_FAILURE);

	// create folder for .obj & texture dumps
	sprintf(Temp, "%s//dump", zProgram.AppPath);
	oz_CreateFolder(Temp);

	// create folder for file extraction
	sprintf(Temp, "%s//extr", zProgram.AppPath);
	oz_CreateFolder(Temp);

	// init OpenGL & renderer
	gl_InitExtensions();
	gl_InitRenderer();

	gl_ResizeScene(WINDOW_WIDTH, WINDOW_HEIGHT);

	// used to delay ROM loading until after reading eventual scene number and debug level arguments
	bool LoadNow = false;

	if(argc == 2) {
		// if exactly one argument, assume that it's the ROM (for drag-and-drop of ROM file onto executable)
		strcpy(Temp, argv[1]);
		LoadNow = true;
	} else if(argc > 1) {
		int curr_arg = 1;
		while(curr_arg < argc) {
			if(!strcmp(argv[curr_arg], "-r")) {
				// load ROM
				strcpy(Temp, argv[curr_arg + 1]);
				LoadNow = true;
				curr_arg++;
			} else
			if(!strcmp(argv[curr_arg], "-s")) {
				// set initial scene number
				sscanf(argv[curr_arg + 1], "%i", &zOptions.SceneNo);
				curr_arg++;
			} else
			if(!strcmp(argv[curr_arg], "-d")) {
				// set debug level
				sscanf(argv[curr_arg + 1], "%i", &zOptions.DebugLevel);
				curr_arg++;
			} else
			if(!strcmp(argv[curr_arg], "-l")) {
				//debug log
//				debugLog=1;
//				dbglogf = fopen(argv[++curr_arg],"w");
			}
			curr_arg++;
		}
	}

	// now try loading the ROM
	if(LoadNow) zROM.IsROMLoaded = zl_Init(Temp);
	if(!zROM.IsROMLoaded) dbgprintf(0, MSK_COLORTYPE_WARNING, "- No ROM loaded!\n\n");

	char WndTitle[256];
	sprintf(WndTitle, "%s - %s", APPTITLE, zGame.TitleText);
	oz_SetWindowTitle(WndTitle);

	zProgram.IsRunning = true;

	while(zProgram.IsRunning) {
		// let the API do whatever it needs to
		switch(oz_APIMain()) {
			// API's done...
			case EXIT_SUCCESS: {
				// all clear, let MISAKA do her stuff
				zProgram.IsRunning = MSK_DoEvents();

				// handling Options dialog
				if(ReturnVal.Handle == zProgram.HandleOptions) {
					// -> button "Dump .obj"
					if(ReturnVal.s8 == 3) {
						cn_Cmd_DumpObj(NULL);
					}

					// -> button "OK"
					if(ReturnVal.s8 == 4) {
						int LastDbg = zOptions.DebugLevel;
						zOptions.DebugLevel = 0;
						struct __zCamera TempCam = zCamera;
						if(zl_LoadScene(zOptions.SceneNo)) dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Fatal error!\n");
						zOptions.DebugLevel = LastDbg;
						zCamera = TempCam;
						dbgprintf(0, MSK_COLORTYPE_OKAY, "> Options have been changed.\n");
					}
				}

				// handling Load Scene dialog
				if(ReturnVal.Handle == zProgram.HandleLoadScene) {
					// -> button "OK"
					if(ReturnVal.s8 == 1) {
						if(zl_LoadScene(zOptions.SceneNo)) {
							dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Failed to load Scene #%i!\n", zOptions.SceneNo);
						}
					}
				}

				// let OpenGL do the rendering
				gl_DrawScene();
				if(gl_FinishScene()) die(EXIT_FAILURE);

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

void GetFilePath(char * FullPath, char * Target)
{
	char Temp[MAX_PATH];
	strcpy(Temp, FullPath);
	char * Ptr;
	if((Ptr = strrchr(Temp, FILESEP))) {
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
/*
		if (debugLog){
			fprintf(dbglogf, "%s%s", Text, (Text[strlen(Text)-1] == '\n') ? " " : "\n" );
			fflush(dbglogf);
		}
*/
		MSK_ConsolePrint(Type, Text);
	}
}

// ----------------------------------------

void die(int Code)
{
	// clear out Zelda stuff
	zl_DeInit();

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
