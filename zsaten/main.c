#include "globals.h"

// ----------------------------------------

vProgramStruct vProgram;
vCameraStruct vCamera;
vCurrentActorStruct vCurrentActor;

vGameROMStruct vGameROM;
vZeldaInfoStruct vZeldaInfo;
vActorStruct vActors[768];
vObjectStruct vObjects[768];

vRGBAStruct vBoneColorFactor;

#include "dialog.h"

// ----------------------------------------

__RAM RAM[MAX_SEGMENTS];

// ----------------------------------------

void aboutProgram(unsigned char * Ptr)
{
	vProgram.guiHandleAbout = MSK_Dialog(&dlgAbout);
}

void programOptions(unsigned char * Ptr)
{
	vProgram.guiHandleOptions = MSK_Dialog(&dlgOptions);
}

void setActorNumber(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: No parameter specified!\n");
	} else {
		int Var = 0;
		sscanf((char*)Ptr+1, "%d", &Var);
		if(Var >= 0 && Var < vZeldaInfo.actorCount) {
			vCurrentActor.actorNumber = Var;
			initActorParsing(-1);
		} else {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Invalid actor number!\n");
		}
	}
}

void loadObjectName(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: No parameter specified!\n");
	} else {
		char ObjFilename[MAX_PATH];
		strcpy(ObjFilename, (char*)Ptr+1);

		DMA ObjFile = zl_DMAGetFileByFilename(ObjFilename);
		if(ObjFile.ID != -1) {
			initActorParsing(ObjFile.ID);
		} else {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Object '%s' not found!\n", ObjFilename);
		}
	}
}

void loadObjectAnim(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: No parameter specified!\n");
	} else {
		char AnimFilename[MAX_PATH];
		strcpy(AnimFilename, (char*)Ptr+1);

		DMA AnimFile = zl_DMAGetFileByFilename(AnimFilename);
		if(AnimFile.ID != -1) {
			if((AnimFile.PStart != 0) && (AnimFile.PEnd != 0)) {
				strcpy(vCurrentActor.eaName, AnimFile.Filename);

				// arbitrary segment number
				RDP_ClearSegment(0x01);

				RAM[0x01].Size = AnimFile.PEnd - AnimFile.PStart;
				RAM[0x01].Data = zl_DMAToBuffer(AnimFile);
				RAM[0x01].IsSet = true;

				initActorParsing(-2);
			}
		} else {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Animation file '%s' not found!\n", AnimFilename);
		}
	}
}

// ----------------------------------------

int main(int argc, char **argv)
{
	if(argc != 2) {
		printf("Syntax: %s <ROM filename>\n", getFilename(argv[0]));
		return EXIT_FAILURE;
	}

	char temp[BUFSIZ];

//	getcwd(vProgram.appPath, MAX_PATH);
	getFilePath(argv[0], vProgram.appPath);

	sprintf(vProgram.wndTitle, APP_TITLE" "APP_VERSION" (build "__DATE__" "__TIME__")");

	MSK_Init(vProgram.wndTitle);
	sprintf(temp, "%s%clog.txt", vProgram.appPath, FILESEP);
	MSK_InitLogging(temp);
	MSK_SetValidCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789.,:\\/\"-()[]_!");
	MSK_AddCommand("actor", "Jump to actor number (0-x)", setActorNumber);
	MSK_AddCommand("loadobject", "Load object by filename", loadObjectName);
	MSK_AddCommand("loadanim", "Load animations by filename", loadObjectAnim);
	MSK_AddCommand("options", "Change program options", programOptions);
	MSK_AddCommand("about", "About this program", aboutProgram);

	#ifdef WIN32
	dbgprintf(0, MSK_COLORTYPE_INFO, APP_TITLE" launched, running on 32/64-bit Windows...\n");
	#else
	dbgprintf(0, MSK_COLORTYPE_INFO, APP_TITLE" launched, running on non-Windows OS...\n");
	#endif
	dbgprintf(0, MSK_COLORTYPE_INFO, "\n");

	vProgram.windowWidth = WINDOW_WIDTH;
	vProgram.windowHeight = WINDOW_HEIGHT;
	if(oz_InitProgram(APP_TITLE, vProgram.windowWidth, vProgram.windowHeight)) return EXIT_FAILURE;

	sprintf(temp, "%s%cdata%cfont.bmp", vProgram.appPath, FILESEP, FILESEP);
	if(hud_Init(temp)) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Failed to initialize HUD system!\n");
		die(EXIT_FAILURE);
	}

	gl_CreateViewerDLists();

	RDP_SetupOpenGL();

	RDP_InitParser(F3DEX2);
	RDP_SetRendererOptions(BRDP_TEXTURES | BRDP_COMBINER);

	vProgram.enableTextures = true;
	vProgram.enableWireframe = false;

	vProgram.enableHUD = true;

	vCurrentActor.actorNumber = 2;//467;

	vProgram.debugLevel = 0;

	sprintf(temp, "%s%c%s", vProgram.appPath, FILESEP, argv[1]);
	zl_Init(temp);

	initActorParsing(-1);

	ca_Reset();

	vProgram.isRunning = true;

	while(vProgram.isRunning) {
		// let the API do whatever it needs to
		switch(oz_APIMain()) {
			// API's done...
			case EXIT_SUCCESS: {
				// all clear, let MISAKA do her stuff
				vProgram.isRunning = MSK_DoEvents();

				if(ReturnVal.Handle == vProgram.guiHandleOptions) {
					// -> button "OK"
					if(ReturnVal.s8 == 5) {
						unsigned char Options = 0;
						if(vProgram.enableTextures) Options |= BRDP_TEXTURES;
						if(vProgram.enableWireframe) Options |= BRDP_WIREFRAME;
						RDP_SetRendererOptions(Options);
						RDP_ClearStructures(true);
						RDP_ClearTextures();
					}
				}

				if(vProgram.animPlay) {
					if(vProgram.animWait++ == vProgram.animDelay) {
						vCurrentActor.frameCurrent++;
						vProgram.animWait = 0;
					}
					if(vCurrentActor.frameCurrent >= vCurrentActor.frameTotal) vCurrentActor.frameCurrent = 0;
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

inline void dbgprintf(int Level, int Type, char * Format, ...)
{
	if(vProgram.debugLevel >= Level) {
		char Text[256];
		va_list argp;

		if(Format == NULL) return;

		va_start(argp, Format);
		vsprintf(Text, Format, argp);
		va_end(argp);

		MSK_ConsolePrint(Type, Text);
	}
}

void die(int Code)
{
	if(vProgram.tempString != NULL) free(vProgram.tempString);

	hud_KillFont();

	zl_DeInit();

	dbgprintf(0, MSK_COLORTYPE_INFO, "\n");

	if(Code == EXIT_SUCCESS)
		dbgprintf(0, MSK_COLORTYPE_INFO, "Program terminated normally.\n");
	else
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Program terminated abnormally, error code %i.\n", Code);

	dbgprintf(0, MSK_COLORTYPE_INFO, "\n");
	dbgprintf(0, MSK_COLORTYPE_INFO, "Press any key to continue...\n");

	MSK_DoEvents();

	while(!getch());
	MSK_Exit();

	exit(Code);
}
