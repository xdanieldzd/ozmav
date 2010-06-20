#include "globals.h"

// ----------------------------------------

struct __Program Program;
struct __OpenGL OpenGL;
struct __ROM ROM;
struct __RAM RAM[64];
struct __DMA DMA[256];
struct __Vertex Vertex[32];
struct __Palette Palette[256];
struct __Texture Texture[2];
struct __Gfx Gfx;
struct __FragmentCache FragmentCache[256];
struct __TextureCache TextureCache[2048];
struct __Camera Camera;
struct __Viewer Viewer;
struct __Object Object[2048];

__GameVer ThisGame;

// ----------------------------------------

void fn_GetPath(unsigned char * FullPath, unsigned char * Target)
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

void fn_GetFilename(unsigned char * FullPath)
{
	char * Ptr;
	if((Ptr = strrchr(FullPath, FILESEP))) {
		strcpy(ROM.Filename, Ptr+1);
	} else {
		strcpy(ROM.Filename, FullPath);
	}
	strcpy(ROM.Filepath, FullPath);
}

void fn_About(unsigned char * Ptr)
{
	static __MSK_UI_Dialog DlgAbout =
	{
		"About", 12, 62,
		{
			{ MSK_UI_DLGOBJ_LABEL,		1,	1,	-1,	Program.Title,													NULL },
			{ MSK_UI_DLGOBJ_LABEL,		2,	1,	-1,	"Written in 2010 by xdaniel",									NULL },
			{ MSK_UI_DLGOBJ_LABEL,		4,	1,	-1,	"Using PDCurses and libMISAKA Console & UI Library",			NULL },
			{ MSK_UI_DLGOBJ_LABEL,		5,	1,	-1,	"libMISAKA - Written in 2010 by xdaniel",						NULL },
			{ MSK_UI_DLGOBJ_LINE,		7,	1,	-1,	"-1",															NULL },
			{ MSK_UI_DLGOBJ_BUTTON,		-1,	-1,	0,	"OK|1",															NULL }
		}
	};

	Program.HandleAbout = MSK_Dialog(&DlgAbout);
}

int main(int argc, char *argv[])
{
	// init MISAKA
	sprintf(Program.Title, APPTITLE" "VERSION" (build "__DATE__" "__TIME__")");
	MSK_Init(Program.Title);
	MSK_SetValidCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789.\\/\"-()[]");
	MSK_AddCommand("about", "About this program", fn_About);
	MSK_AddCommand("loadrom", "Load a ROM file to use", sf_LoadROM);
	MSK_AddCommand("listfiles", "List all files found in DMA table", sf_ListDMATable);
	MSK_AddCommand("extractfiles", "Extract all files found in DMA table", sf_ExtractFiles);
	MSK_AddCommand("expandrom", "Create expanded ROM (argument: Filename)", sf_CreateExpandedROM);
	MSK_AddCommand("createrom", "Create ROM from extracted files (argument: Layout file)", sf_CreateFreshROM);
	MSK_AddCommand("fixcrc", "Fix ROM's CRC checksum", sf_FixCrc);
	MSK_AddCommand("saverom", "Save any changes to ROM loaded by loadrom", sf_SaveROM);
	MSK_AddCommand("viewer", "Enable level viewer", sv_EnableViewer);

	MSK_ConsolePrint(MSK_COLORTYPE_INFO, "\n%s launched...\n\n", APPTITLE);

	fn_GetPath(argv[0], Program.AppPath);

	// check arguments
	if(argc > 2) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Too many arguments specified!\n");
		die(EXIT_FAILURE);
	}

	if(argc == 2) {
		fn_GetFilename(argv[1]);
		Program.IsROMLoaded = sf_Init();
	}
	if(!Program.IsROMLoaded) MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "- No ROM loaded!\n\n");

	// init API via OZ wrapper
	if(oz_InitProgram(APPTITLE, WINDOW_WIDTH, WINDOW_HEIGHT)) die(EXIT_FAILURE);

	// init OpenGL & renderer
	gl_InitExtensions();
	gl_InitRenderer();
	gl_ResizeScene(WINDOW_WIDTH, WINDOW_HEIGHT);

	Program.IsRunning = true;

	Program.IsViewerEnabled = false;

	while(Program.IsRunning) {
		// let the API do whatever it needs to
		switch(oz_APIMain()) {
			// API's done...
			case EXIT_SUCCESS: {
				// all clear, let MISAKA do her stuff
				Program.IsRunning = MSK_DoEvents();

				// if the viewer's enabled...
				if(Program.IsViewerEnabled) {
					// let OpenGL do the rendering
					gl_DrawScene();
					if(gl_FinishScene()) die(EXIT_FAILURE);
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

void die(int Code)
{
	MSK_ConsolePrint(MSK_COLORTYPE_INFO, "\n");

	// determine if program exited normally or not
	if(Code == EXIT_SUCCESS)
		MSK_ConsolePrint(MSK_COLORTYPE_INFO, "Program terminated normally.\n");
	else
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "Program terminated abnormally, error code %i.\n", Code);

	MSK_ConsolePrint(MSK_COLORTYPE_INFO, "\n");
	MSK_ConsolePrint(MSK_COLORTYPE_INFO, "Press any key to continue...\n");

	// one more MISAKA update
	MSK_DoEvents();

	// wait for keypress, then shut down MISAKA and exit
	while(!getch());

	free(ROM.Data);
	MSK_Exit();

	exit(Code);
}
