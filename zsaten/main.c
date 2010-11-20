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

void listSegmentUsage(unsigned char * Ptr)
{
	dbgprintf(0, MSK_COLORTYPE_OKAY, "\nCurrent RAM segments:\n");

	int i = 0;
	for(i = 0; i < MAX_SEGMENTS; i++) {
		if(RAM[i].IsSet) {
			dbgprintf(0, MSK_COLORTYPE_OKAY, " - Segment 0x%02X -> Size: 0x%06X bytes, ROM source offset: 0x%08X (%s)\n",
				i, RAM[i].Size, RAM[i].SourceOffset,
					(RAM[i].SourceCompType ? (RAM[i].SourceCompType == 1 ? "Yaz0" : "MIO0") : ("raw"))
			);
		} else {
			dbgprintf(0, MSK_COLORTYPE_WARNING, " - Segment 0x%02X -> Segment not set.\n", i);
		}
	}
}

void printActorData(unsigned char * Ptr)
{
	dbgprintf(0, MSK_COLORTYPE_OKAY, "\nActor #0x%04X:\n", vCurrentActor.actorNumber);

	int i = 0;

	if(vCurrentActor.useActorOvl) {
		unsigned short ActorNumber = vCurrentActor.actorNumber;
		unsigned short ObjectNumber = vActors[ActorNumber].ObjectNumber;
		unsigned short AltObjectNumber = vActors[ActorNumber].AltObjectNumber;

		if(!vActors[ActorNumber].isValid) {
			dbgprintf(0, MSK_COLORTYPE_WARNING, " - Invalid Actor data!\n");
			return;
		}
		dbgprintf(0, MSK_COLORTYPE_INFO, " - Actor 0x%04X '%s', %i bytes\n",
			ActorNumber,
			(strcmp(vActors[ActorNumber].ActorName, "\0") ? vActors[ActorNumber].ActorName : "unknown"),
			vActors[ActorNumber].ActorSize);
		if(ObjectNumber && vObjects[ObjectNumber].isValid) {
			dbgprintf(0, MSK_COLORTYPE_INFO, "  - Object 0x%04X '%s', %i bytes, segment 0x%02X\n",
				ObjectNumber,
				(strcmp(vObjects[ObjectNumber].ObjectName, "\0") ? vObjects[ObjectNumber].ObjectName : "unknown"),
				vObjects[ObjectNumber].ObjectSize,
				vObjects[ObjectNumber].ObjectSegment);
			if(AltObjectNumber && vObjects[AltObjectNumber].isValid) {
				dbgprintf(0, MSK_COLORTYPE_INFO, "  - Alt. Object 0x%04X '%s', %i bytes, segment 0x%02X\n",
					ObjectNumber,
					(strcmp(vObjects[AltObjectNumber].ObjectName, "\0") ? vObjects[AltObjectNumber].ObjectName : "unknown"),
					vObjects[AltObjectNumber].ObjectSize,
					vObjects[AltObjectNumber].ObjectSegment);
			}
		}

		if(vCurrentActor.offsetDList != 0) {
			dbgprintf(0, MSK_COLORTYPE_INFO, " - Display List at offset 0x%08X\n", vCurrentActor.offsetDList);
		} else {
			dbgprintf(0, MSK_COLORTYPE_WARNING, " - No Display List found!\n");
		}

		dbgprintf(0, MSK_COLORTYPE_INFO, " - Bone structures:\n");
		for(i = 0; i < vCurrentActor.boneSetupTotal + 1; i++) {
			dbgprintf(0, MSK_COLORTYPE_INFO, "  - Structure #%i, offset 0x%08X\n", i + 1, vCurrentActor.offsetBoneSetup[i]);
		}
		if(i == 0) dbgprintf(0, MSK_COLORTYPE_WARNING, "  - No structures found!\n");

		dbgprintf(0, MSK_COLORTYPE_INFO, " - Animations:\n");
		if(vCurrentActor.isLink)
		{
			for(i = 0; i < vCurrentActor.animTotal; i++) {
				dbgprintf(0, MSK_COLORTYPE_INFO, "  - Animation #%i, offset 0x%08X, %i frames\n", i + 1, vCurrentActor.offsetAnims[i], vCurrentActor.animFrames[i]);
			}
		}else{
			for(i = 0; i < vCurrentActor.animTotal + 1; i++) {
				dbgprintf(0, MSK_COLORTYPE_INFO, "  - Animation #%i, offset 0x%08X\n", i + 1, vCurrentActor.offsetAnims[i]);
			}
		}
		if(i == 0) dbgprintf(0, MSK_COLORTYPE_WARNING, "  - No animations found!\n");
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
	MSK_AddCommand("listsegs", "List currently set segments", listSegmentUsage);
	MSK_AddCommand("printdata", "Print current actor data", printActorData);
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
	if(oz_InitProgram(vProgram.wndTitle, vProgram.windowWidth, vProgram.windowHeight)) return EXIT_FAILURE;

	sprintf(temp, "%s%cdata%cfont.bmp", vProgram.appPath, FILESEP, FILESEP);
	if(hud_Init((unsigned char*)temp)) {
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

	vCurrentActor.actorNumber = 0;//x1c6;//467;

	vProgram.debugLevel = 0;

	zl_Init(argv[1]);

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

				static float lastTime = 0.0f;
				float startTime = clock() * 0.001f;

				gl_DrawScene();

				if(((float)startTime - lastTime) > 1.0f / vProgram.targetFPS) {
					lastTime = startTime;
					if(vProgram.animPlay) {
						vCurrentActor.frameCurrent++;
						if(vCurrentActor.frameCurrent > vCurrentActor.frameTotal) vCurrentActor.frameCurrent = 0;
					}
				}

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
	/* define getch() */
	int getch(void);

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
