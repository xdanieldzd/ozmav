// Project "OZMAV2"
// February/March 2010 by xdaniel & contributors

// ----------------------------------------

#include "globals.h"

#include "confunc.h"

// ----------------------------------------

struct __zProgram zProgram;
struct __zOptions zOptions;

struct __zROM zROM;
struct __zGame zGame;
struct __zHeader zSHeader[256];
struct __zHeader zMHeader[256][256];

struct __zGfx zGfx;

struct __zObject zObject[1024];
struct __zActor zActor[1024];
struct __zMapActor zLink[256];
struct __zMapActor zMapActor[256][256];
struct __zDoor zDoor[256];

struct __zCamera zCamera;

struct __zHUD zHUD;

// ----------------------------------------

__RAM RAM[MAX_SEGMENTS];

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
	MSK_SetValidCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789.,:\\/\"-()[]_!");
	MSK_AddCommand("loadrom", "Load ROM file to use", cn_Cmd_LoadROM);
	MSK_AddCommand("saverom", "Save eventual changes to current ROM", cn_Cmd_SaveROM);
	MSK_AddCommand("rominfo", "Show ROM information", cn_Cmd_ShowROMInfo);
	MSK_AddCommand("loadscene", "Load specific Scene (0-x)", cn_Cmd_LoadScene);
	MSK_AddCommand("modifyactor", "Modify specific Actor (0-x)", cn_Cmd_ModifyActor);
	MSK_AddCommand("saveactors", "Save the current Map's Actors", cn_Cmd_SaveActors);
	MSK_AddCommand("dumpobj", "Dump Scene to .obj file", cn_Cmd_DumpObj);
	MSK_AddCommand("settexture", "Disable/enable texturing (0-1)", cn_Cmd_SetTexture);
	MSK_AddCommand("setcombiner", "Disable/enable combiner (0-1)", cn_Cmd_SetCombiner);
	MSK_AddCommand("setactor", "Disable/enable Actor rendering (0-1)", cn_Cmd_SetActorRendering);
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
	zProgram.WindowWidth = WINDOW_WIDTH;
	zProgram.WindowHeight = WINDOW_HEIGHT;
	if(oz_InitProgram(APPTITLE, zProgram.WindowWidth, zProgram.WindowHeight)) die(EXIT_FAILURE);

	// init HUD system
	if(hud_Init()) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Failed to initialize HUD system!\n");
		die(EXIT_FAILURE);
	} else {
		hudMenu_Init();
	}

	// create folder for .obj & texture dumps
	sprintf(Temp, "%s//dump", zProgram.AppPath);
	oz_CreateFolder(Temp);

	// create folder for file extraction
	sprintf(Temp, "%s//extr", zProgram.AppPath);
	oz_CreateFolder(Temp);

	// init OpenGL & renderer
	RDP_SetupOpenGL();
	RDP_SetRendererOptions(BRDP_TEXTURES | BRDP_COMBINER/* | BRDP_WIREFRAME*/);

	gl_SetupScene3D(zProgram.WindowWidth, zProgram.WindowHeight);

	gl_CreateViewerDLists();

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

	zProgram.MouseMode = 0;

	// get the program to run
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
						unsigned char Options = 0;
						if(zOptions.EnableTextures) Options |= BRDP_TEXTURES;
						if(zOptions.EnableCombiner) Options |= BRDP_COMBINER;
						RDP_SetRendererOptions(Options);

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

				// handling Modify Actor dialog
				if(ReturnVal.Handle == zProgram.HandleModifyActor) {
					// -> button "OK"
					if(ReturnVal.s8 == 8) {
						if((!zGame.IsCompressed) && (!zGame.GameType)) {
							zActor[zMapActor[zOptions.MapToRender][zOptions.SelectedActor].Number].IsSet = false;
							if(glIsList(zMapActor[zOptions.MapToRender][zOptions.SelectedActor].GLDList)) glDeleteLists(zMapActor[zOptions.MapToRender][zOptions.SelectedActor].GLDList, 1);
							zl_ProcessActor(zOptions.MapToRender, zOptions.SelectedActor, 0);
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

int DoMainKbdInput()
{
	if(zProgram.Key[KEY_CAMERA_UP_FAST]) ca_Movement(false, 6.0f);
	if(zProgram.Key[KEY_CAMERA_DOWN_FAST]) ca_Movement(false, -6.0f);
	if(zProgram.Key[KEY_CAMERA_LEFT_FAST]) ca_Movement(true, -6.0f);
	if(zProgram.Key[KEY_CAMERA_RIGHT_FAST]) ca_Movement(true, 6.0f);

	if(zProgram.Key[KEY_CAMERA_UP_SLOW]) ca_Movement(false, 1.0f);
	if(zProgram.Key[KEY_CAMERA_DOWN_SLOW]) ca_Movement(false, -1.0f);
	if(zProgram.Key[KEY_CAMERA_LEFT_SLOW]) ca_Movement(true, -1.0f);
	if(zProgram.Key[KEY_CAMERA_RIGHT_SLOW]) ca_Movement(true, 1.0f);

	if(zOptions.SelectedActor == -1) {
		if(zProgram.Key[KEY_SCENE_PREVIOUS] && zOptions.SceneNo > 0) {
			zOptions.SceneNo--;
			if(zl_LoadScene(zOptions.SceneNo)) return EXIT_FAILURE;
			zProgram.Key[KEY_SCENE_PREVIOUS] = false;
		}

		if(zProgram.Key[KEY_SCENE_NEXT] && zOptions.SceneNo < zGame.SceneCount) {
			zOptions.SceneNo++;
			if(zl_LoadScene(zOptions.SceneNo)) return EXIT_FAILURE;
			zProgram.Key[KEY_SCENE_NEXT] = false;
		}

		if(zProgram.Key[KEY_MAP_PREVIOUS] && zOptions.MapToRender > -1) {
			if(zSHeader[0].MapCount > 1) {
				zOptions.MapToRender--;
				zOptions.SelectedActor = -1;
				zOptions.SelectedActorMap = zOptions.MapToRender;
			} else {
				zOptions.MapToRender = 0;
			}
			zProgram.Key[KEY_MAP_PREVIOUS] = false;
		}

		if(zProgram.Key[KEY_MAP_NEXT] && zOptions.MapToRender < zSHeader[0].MapCount - 1) {
			zOptions.MapToRender++;
			zOptions.SelectedActor = -1;
			zOptions.SelectedActorMap = zOptions.MapToRender;
			zProgram.Key[KEY_MAP_NEXT] = false;
		}
	} else {
		__zHUDMenuEntry ActorMenu[] = {
			{ "Name", NULL, -1, -1 },
			{ "---", NULL, -1, -1 },
			{ "Settings", NULL, -1, -1 },
			{ "Number", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Number, 2, 3 },
			{ "Variable", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Var, 2, 3 },
			{ "Position", NULL, -1, -1 },
			{ "X", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.X, 2, 0 },
			{ "Y", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.Y, 2, 0 },
			{ "Z", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.Z, 2, 0 },
			{ "Rotation", NULL, -1, -1 },
			{ "X", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Rot.X, 2, 0 },
			{ "Y", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Rot.Y, 2, 0 },
			{ "Z", (short*)&zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Rot.Z, 2, 0 }
		};

		hudMenu_HandleInput(ActorMenu, ArraySize(ActorMenu));

		if(zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Number > zGame.ActorCount)
			zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Number = zGame.ActorCount;

		if(zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Number == 0)
			zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Number = 1;

		if((!zGame.IsCompressed) && (!zGame.GameType)) zl_ProcessActor(zOptions.SelectedActorMap, zOptions.SelectedActor, 0);
	}

	if(zProgram.Key[KEY_MOUSE_MODESWITCH]) {
		zProgram.MouseMode++;
		if(zProgram.MouseMode == 6) zProgram.MouseMode = 0;
		sprintf(zProgram.WndTitle, "%s - %s - ", APPTITLE, zGame.TitleText);
		switch(zProgram.MouseMode) {
			case 0: sprintf(zProgram.WndTitle, "%sCamera Mode", zProgram.WndTitle); break;
			case 1: sprintf(zProgram.WndTitle, "%sActor Mode (X/Y)", zProgram.WndTitle); break;
			case 2: sprintf(zProgram.WndTitle, "%sActor Mode (X/Z)", zProgram.WndTitle); break;
			case 3: sprintf(zProgram.WndTitle, "%sActor Mode (X)", zProgram.WndTitle); break;
			case 4: sprintf(zProgram.WndTitle, "%sActor Mode (Y)", zProgram.WndTitle); break;
			case 5: sprintf(zProgram.WndTitle, "%sActor Mode (Z)", zProgram.WndTitle); break;
		}
		oz_SetWindowTitle(zProgram.WndTitle);
		zProgram.Key[KEY_MOUSE_MODESWITCH] = false;
	}
	if(zProgram.Key[KEY_ACTOR_PREVIOUS] && zOptions.SelectedActor > -1) {
		zOptions.SelectedActor--;
		zProgram.Key[KEY_ACTOR_PREVIOUS] = false;
	}

	if(zProgram.Key[KEY_ACTOR_NEXT] && zOptions.SelectedActor < zMHeader[0][zOptions.MapToRender].ActorCount - 1) {
		zOptions.SelectedActor++;
		zProgram.Key[KEY_ACTOR_NEXT] = false;
	}

	return EXIT_SUCCESS;
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
	// clear out HUD stuff
	hud_KillFont();

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
