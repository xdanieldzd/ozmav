// Project "OZMAV2"
// February/March 2010 by xdaniel

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

struct __zFragmentCache zFragmentCache[256];
struct __zTextureCache zTextureCache[512];

struct __zCamera zCamera;

// ----------------------------------------

void die(int Code);

// ----------------------------------------

int main(int argc, char * argv[])
{
	sprintf(zProgram.Title, "%s %s (build %s %s)", APPTITLE, VERSION, __DATE__, __TIME__);
	MSK_Init(zProgram.Title);
	MSK_SetValidCharacters("abcdefghijklmnopqrstuvwxyz 0123456789.\\/\"-");
	MSK_AddCommand("loadscene", "Load specific Scene (0-x)", cn_Cmd_LoadScene);
	MSK_AddCommand("dumpobj", "Dump Scene to .obj file", cn_Cmd_DumpObj);
	MSK_AddCommand("settexture", "Disable/enable texturing (0-1)", cn_Cmd_SetTexture);
	MSK_AddCommand("setcombiner", "Disable/enable combiner (0-1)", cn_Cmd_SetCombiner);
	MSK_AddCommand("resetcam", "Reset camera to initial position", cn_Cmd_ResetCam);
	MSK_AddCommand("setdebug", "Set debug level (0-3)", cn_Cmd_SetDebug);
	MSK_AddCommand("extract", "Extract all files from ROM", cn_Cmd_ExtractFiles);
	MSK_AddCommand("options", "Show options dialog", cn_Cmd_Options);
	MSK_AddCommand("about", "About the program", cn_Cmd_About);

	#ifdef WIN32
	dbgprintf(0, MSK_COLORTYPE_INFO, "Running on 32/64-bit Windows...\n");
	dbgprintf(0, MSK_COLORTYPE_INFO, "\n");
	#else
	dbgprintf(0, MSK_COLORTYPE_INFO, "Running on non-Windows OS...\n");
	dbgprintf(0, MSK_COLORTYPE_INFO, "\n");
	#endif

	if(argc < 2) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Invalid arguments specified\n");
		die(EXIT_FAILURE);
	}

	if(oz_InitProgram(APPTITLE, WINDOW_WIDTH, WINDOW_HEIGHT)) die(EXIT_FAILURE);

	oz_CreateFolder(".//dump");

	gl_InitExtensions();
	gl_InitRenderer();

	gl_ResizeScene(WINDOW_WIDTH, WINDOW_HEIGHT);

	if(argc > 2) sscanf(argv[2], "%i", &zOptions.SceneNo);

	if(argc > 3) {
		int curr_arg = 3;
		while(curr_arg < argc) {
			if(!strcmp(argv[curr_arg], "-d")) {
				sscanf(argv[curr_arg + 1], "%i", &zOptions.DebugLevel);
				curr_arg++;
			}

/*			else if(!strcmp(argv[curr_arg], "-l")) {
				FILE * fp;
				if((fp = freopen("log.txt", "w", stdout)) == NULL) dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not create log file!\n");
			}
*/
/*			else if(!strcmp(argv[curr_arg], "-w")) {
				zOptions.DumpModel = true;
			}
*/
			curr_arg++;
		}
	}

	if(zl_Init(argv[1])) die(EXIT_FAILURE);

	char WndTitle[256];
	sprintf(WndTitle, "%s - %s", APPTITLE, zGame.TitleText);
	oz_SetWindowTitle(WndTitle);

	zProgram.IsRunning = true;

	while(zProgram.IsRunning) {
		switch(oz_APIMain()) {
			case EXIT_SUCCESS: {
				// all clear so do your stuff
				zProgram.IsRunning = MSK_DoEvents();

				// options dialog
				if(ReturnVal.Handle == zProgram.HandleOptions) {
					// -> button Dump .obj
					if(ReturnVal.s8 == 3) {
						cn_Cmd_DumpObj(NULL);
					}

					// -> button OK
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

				// loadscene dialog
				if(ReturnVal.Handle == zProgram.HandleLoadScene) {
					// -> button Ok
					if(ReturnVal.s8 == 1) {
						if(zl_LoadScene(zOptions.SceneNo)) {
							dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Failed to load Scene #%i!\n", zOptions.SceneNo);
						}
					}
				}

				gl_DrawScene();
				if(gl_FinishScene()) die(EXIT_FAILURE);

				break;
			}

			case -1: {
				// let the api do its shit
				break; }

			case EXIT_FAILURE: {
				// error!
				die(EXIT_FAILURE); }
			}
	}

	zl_DeInit();

	if(oz_ExitProgram()) die(EXIT_FAILURE);

	die(EXIT_SUCCESS);

	return EXIT_FAILURE;
}

// ----------------------------------------

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
	dbgprintf(0, MSK_COLORTYPE_INFO, "\n");

	if(Code == EXIT_SUCCESS)
		dbgprintf(0, MSK_COLORTYPE_INFO, "Program terminated normally.\n");
	else
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Program terminated abnormally, error code %i.\n", Code);

	dbgprintf(0, MSK_COLORTYPE_INFO, "\n");
	dbgprintf(0, MSK_COLORTYPE_INFO, "Press any key to continue...\n");

	while(!getch()) { }

	MSK_Exit();

	exit(Code);
}
