#include "globals.h"

// ----------------------------------------

char AppPath[MAX_PATH];
#ifdef WIN32
char Sep = '\\';
#else
char Sep = '/';
#endif

char ProgTitle[256];

int DMAFileCount;

struct __ROM ROM;
struct __DMA DMA[256];

__GameVer ThisGame;
bool IsROMLoaded = FALSE;

// ----------------------------------------

void fn_GetPath(unsigned char * FullPath, unsigned char * Target)
{
	char Temp[MAX_PATH];
	strcpy(Temp, FullPath);
	char * Ptr;
	if((Ptr = strrchr(Temp, Sep))) {
		Ptr++;
		*Ptr = '\0';
		strcpy(Target, Temp);
	} else {
		sprintf(Target, ".%c", Sep);
	}
}

void fn_GetFilename(unsigned char * FullPath)
{
	char * Ptr;
	if((Ptr = strrchr(FullPath, Sep))) {
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
			{ MSK_UI_DLGOBJ_LABEL,		1,	1,	-1,	ProgTitle,														NULL },
			{ MSK_UI_DLGOBJ_LABEL,		2,	1,	-1,	"Written in 2010 by xdaniel",									NULL },
			{ MSK_UI_DLGOBJ_LABEL,		4,	1,	-1,	"Using PDCurses and libMISAKA Console & UI Library",			NULL },
			{ MSK_UI_DLGOBJ_LABEL,		5,	1,	-1,	"libMISAKA - Written in 2010 by xdaniel",						NULL },
			{ MSK_UI_DLGOBJ_LINE,		7,	1,	-1,	"-1",															NULL },
			{ MSK_UI_DLGOBJ_BUTTON,		-1,	-1,	0,	"OK|1",															NULL }
		}
	};

	ReturnVal.s16 = MSK_Dialog(&DlgAbout);
}

int main(int argc, char *argv[])
{
	// init MISAKA
	sprintf(ProgTitle, APPTITLE" "VERSION" (build "__DATE__" "__TIME__")");
	MSK_Init(ProgTitle);
	MSK_SetValidCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789.\\/\"-()[]");
	MSK_AddCommand("about", "About this program", fn_About);
	MSK_AddCommand("loadrom", "Load a ROM to used", sf_LoadROM);
	MSK_AddCommand("listfiles", "List all files found in DMA table", sf_ListDMATable);
	MSK_AddCommand("extractfiles", "Extract all files found in DMA table", sf_ExtractFiles);
	MSK_AddCommand("expandrom", "Create expanded ROM (argument: Filename)", sf_CreateExpandedROM);
	MSK_AddCommand("createrom", "Create ROM from extracted files (argument: Layout file)", sf_CreateFreshROM);
	MSK_AddCommand("fixcrc", "Fix ROM's CRC checksum", sf_FixCrc);
	MSK_AddCommand("saverom", "Save any changes to ROM loaded by loadrom", sf_SaveROM);

	MSK_ConsolePrint(MSK_COLORTYPE_INFO, "\n%s launched...\n\n", APPTITLE);

	fn_GetPath(argv[0], AppPath);

	// check arguments
	if(argc > 2) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Too many arguments specified!\n");
		die(EXIT_FAILURE);
	}

	if(argc == 2) {
		fn_GetFilename(argv[1]);
		IsROMLoaded = sf_Init();
	}
	if(!IsROMLoaded) MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "- No ROM loaded!\n\n");

	// while MISAKA says it's okay, do stuff
	while(MSK_DoEvents()) {
		// if any function reports failure, die with that error code
		if(ReturnVal.s8) die(ReturnVal.s8);

		ReturnVal.s8 = EXIT_SUCCESS;
	}

	// MISAKA's done, so die
	die(EXIT_SUCCESS);

	return 0;
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
