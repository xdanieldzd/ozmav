#include "globals.h"
#include "dialog.h"

void cn_Cmd_LoadROM(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No file specified!\n");
	} else {
		char Path[MAX_PATH];
		strcpy(Path, zProgram.AppPath);
		strcat(Path, (char*)Ptr+1);
		zOptions.SceneNo = 0;
		zROM.IsROMLoaded = zl_Init(Path);
	}
}

void cn_Cmd_LoadScene(unsigned char * Ptr)
{
	if(!zROM.IsROMLoaded) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No ROM loaded!\n");
		return;
	}

	if(Ptr == NULL) {
		char * SceneSelParam = (char*)malloc(sizeof(char) * 256);
		sprintf(SceneSelParam, "Scene ID|%i|1|0|0", zGame.SceneCount + 1);

		__MSK_UI_Dialog DlgLoadScene =
		{
			"Load Scene", 10, 40,
			{
				{ MSK_UI_DLGOBJ_LABEL,     1,  1,  -1, "Select which Scene to load:", NULL },
				{ MSK_UI_DLGOBJ_NUMBERSEL, 3,  1,  0,  (char*)SceneSelParam,          (short*)&zOptions.SceneNo },
				{ MSK_UI_DLGOBJ_LINE,      5,  1,  -1, "-1",                          NULL },
				{ MSK_UI_DLGOBJ_BUTTON,    -1, -1, 1,  "OK|1",                        NULL }
			}
		};

		zProgram.HandleLoadScene = MSK_Dialog(&DlgLoadScene);
	} else {
		int Var = 0;
		sscanf((char*)Ptr+1, "%d", &Var);
		zOptions.SceneNo = Var;
		if(zl_LoadScene(zOptions.SceneNo)) {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Failed to load Scene #%i!\n", zOptions.SceneNo);
		}
	}
}

void cn_Cmd_DumpObj(unsigned char * Ptr)
{
	if(!zROM.IsROMLoaded) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No ROM loaded!\n");
		return;
	}

	int LastDbg = zOptions.DebugLevel;

	zOptions.DebugLevel = 0;
	struct __zCamera TempCam = zCamera;

	// tell OZMAV2 that we're dumping the level model
	zOptions.DumpModel = true;

	// create folders
	char Temp[MAX_PATH];
	sprintf(Temp, "%s//dump//%s", zProgram.AppPath, zGame.TitleText);
	oz_CreateFolder(Temp);
	sprintf(Temp, "%s//dump//%s//scene_%i", zProgram.AppPath, zGame.TitleText, zOptions.SceneNo);
	oz_CreateFolder(Temp);

	// initialize dumper
	RDP_Dump_InitModelDumping(Temp, "model.obj", "material.mtl");
	// reload the scene - without rendering the actors! - to dump the map(s)
	if(zl_LoadScene(zOptions.SceneNo)) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Fatal error!\n");
	}
	// stop the dumper
	RDP_Dump_StopModelDumping();

	// reset the dumping option
	zOptions.DumpModel = false;

	// reload the scene again, this time also rendering the actors again
	if(zl_LoadScene(zOptions.SceneNo)) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Fatal error!\n");
	} else {
		dbgprintf(0, MSK_COLORTYPE_OKAY, "> Model has been dumped.\n");
	}

	zOptions.DebugLevel = LastDbg;
	zCamera = TempCam;
}

void cn_Cmd_SetTexture(unsigned char * Ptr)
{
	if(!zROM.IsROMLoaded) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No ROM loaded!\n");
		return;
	}

	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: No parameter specified!\n");
	} else {
		int Var = 0;
		sscanf((char*)Ptr+1, "%d", &Var);
		zOptions.EnableTextures = Var;
		unsigned char Options = 0;
		if(zOptions.EnableTextures) Options |= BRDP_TEXTURES;
		if(zOptions.EnableCombiner) Options |= BRDP_COMBINER;
		RDP_SetRendererOptions(Options);
		dbgprintf(0, MSK_COLORTYPE_OKAY, "> Texturing has been %s.\n", (Var ? "enabled" : "disabled"));

		int LastDbg = zOptions.DebugLevel;
		zOptions.DebugLevel = 0;
		struct __zCamera TempCam = zCamera;

		if(zl_LoadScene(zOptions.SceneNo)) dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Fatal error!\n");

		zOptions.DebugLevel = LastDbg;
		zCamera = TempCam;
	}
}

void cn_Cmd_SetCombiner(unsigned char * Ptr)
{
	if(!zROM.IsROMLoaded) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No ROM loaded!\n");
		return;
	}

	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: No parameter specified!\n");
	} else {
		int Var = 0;
		sscanf((char*)Ptr+1, "%d", &Var);
		zOptions.EnableCombiner = Var;
		unsigned char Options = 0;
		if(zOptions.EnableTextures) Options |= BRDP_TEXTURES;
		if(zOptions.EnableCombiner) Options |= BRDP_COMBINER;
		RDP_SetRendererOptions(Options);
		dbgprintf(0, MSK_COLORTYPE_OKAY, "> Combiner emulation has been %s.\n", (Var ? "enabled" : "disabled"));

		int LastDbg = zOptions.DebugLevel;
		zOptions.DebugLevel = 0;
		struct __zCamera TempCam = zCamera;

		if(zl_LoadScene(zOptions.SceneNo)) dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Fatal error!\n");

		zOptions.DebugLevel = LastDbg;
		zCamera = TempCam;
	}
}

void cn_Cmd_SetActorRendering(unsigned char * Ptr)
{
	if(!zROM.IsROMLoaded) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No ROM loaded!\n");
		return;
	}

	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: No parameter specified!\n");
	} else {
		int Var = 0;
		sscanf((char*)Ptr+1, "%d", &Var);
		zOptions.EnableActors = Var;
		dbgprintf(0, MSK_COLORTYPE_OKAY, "> Actor rendering has been %s.\n", (Var ? "enabled" : "disabled"));

		int LastDbg = zOptions.DebugLevel;
		zOptions.DebugLevel = 0;
		struct __zCamera TempCam = zCamera;

		if(zl_LoadScene(zOptions.SceneNo)) dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Fatal error!\n");

		zOptions.DebugLevel = LastDbg;
		zCamera = TempCam;
	}
}

void cn_Cmd_ResetCam(unsigned char * Ptr)
{
	ca_Reset();
	dbgprintf(0, MSK_COLORTYPE_OKAY, "> Camera has been reset.\n");
}

void cn_Cmd_SetDebug(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: No parameter specified!\n");
	} else {
		int Var = 0;
		sscanf((char*)Ptr+1, "%d", &Var);
		zOptions.DebugLevel = Var;
		dbgprintf(0, MSK_COLORTYPE_OKAY, "> Debug level set to %i.\n", zOptions.DebugLevel);
	}
}

void cn_Cmd_Options(unsigned char * Ptr)
{
	if(!zROM.IsROMLoaded) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No ROM loaded!\n");
		return;
	}

	zProgram.HandleOptions = MSK_Dialog(&DlgOptions);
}

void cn_Cmd_About(unsigned char * Ptr)
{
	zProgram.HandleAbout = MSK_Dialog(&DlgAbout);
}

void cn_Cmd_ExtractFiles(unsigned char * Ptr)
{
	if(!zROM.IsROMLoaded) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No ROM loaded!\n");
		return;
	}

	dbgprintf(0, MSK_COLORTYPE_OKAY, "Extracting ROM data...\n");

	char Temp[256];

	sprintf(Temp, "%s//extr//%s", zProgram.AppPath, zGame.TitleText);
	oz_CreateFolder(Temp);

	DMA CurrentFile = {0, 0, 0, 0, 0, ""};
	int FileNo = 0;
	bool IsFileValid = true;

	CurrentFile = zl_DMAGetFile(FileNo);

	while((CurrentFile.VEnd != 0x00) || (CurrentFile.PStart != 0xFFFFFFFF)) {
		if(CurrentFile.VStart == CurrentFile.VEnd) break;

		CurrentFile = zl_DMAVirtualToPhysical(CurrentFile.VStart, CurrentFile.VEnd);

		if(zGame.HasFilenames) {
			zl_DMAGetFilename(CurrentFile.Filename, FileNo);
		} else {
			sprintf(CurrentFile.Filename, "%04i_%08X-%08X", FileNo, CurrentFile.PStart, CurrentFile.PEnd);
		}

		if((CurrentFile.PStart == 0xFFFFFFFF) || (CurrentFile.PEnd == 0xFFFFFFFF)) IsFileValid = false;

		dbgprintf(0, MSK_COLORTYPE_INFO, "File %i: %s, PStart 0x%08X, PEnd 0x%08X%s\n", FileNo, CurrentFile.Filename, CurrentFile.PStart, CurrentFile.PEnd, (IsFileValid ? "" : " XX"));
		FileNo++;

		sprintf(Temp, "%s//extr//%s//%s", zProgram.AppPath, zGame.TitleText, CurrentFile.Filename);

		if(IsFileValid) {
			FILE * File = fopen(Temp, "wb");
			if(File != NULL) {
				fwrite(&zROM.Data[CurrentFile.PStart], 1, (CurrentFile.PEnd - CurrentFile.PStart), File);
				fclose(File);
			} else {
				dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: Could not create file!\n");
				break;
			}

			unsigned int ID = Read32(zROM.Data, CurrentFile.PStart);
			if(ID == 0x59617A30) {
				strcat(Temp, ".dec");

				File = fopen(Temp, "wb");
				if(File != NULL) {
					unsigned int Size = Read32(zROM.Data, CurrentFile.PStart + 4);
					unsigned char * FileBuffer = malloc(sizeof(char) * Size);
					RDP_Yaz0Decode(&zROM.Data[CurrentFile.PStart], FileBuffer, Size);
					dbgprintf(0, MSK_COLORTYPE_WARNING, "%08X", Read32(FileBuffer, 0));
					fwrite(FileBuffer, 1, Size, File);
					fclose(File);
				} else {
					dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: Could not create file!\n");
					break;
				}
			}
		}

		CurrentFile = zl_DMAGetFile(FileNo);
		IsFileValid = true;
	}

	dbgprintf(0, MSK_COLORTYPE_OKAY, "Done!\n");
}

void cn_Cmd_ListFiles(unsigned char * Ptr)
{
	if(!zROM.IsROMLoaded) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No ROM loaded!\n");
		return;
	}

	if(!zGame.HasFilenames) dbgprintf(0, MSK_COLORTYPE_WARNING, "Warning: ROM does not contain filenames!\n");

	char Temp[256];

	DMA CurrentFile = {0, 0, 0, 0, 0, ""};
	int FileNo = 0;
	bool IsFileValid = true;

	CurrentFile = zl_DMAGetFile(FileNo);

	dbgprintf(0, MSK_COLORTYPE_INFO, "#    VStart   VEnd     PStart   PEnd     Filename\n");

	while((CurrentFile.VEnd != 0x00) || (CurrentFile.PStart != 0xFFFFFFFF)) {
		if(CurrentFile.VStart == CurrentFile.VEnd) break;

		CurrentFile = zl_DMAVirtualToPhysical(CurrentFile.VStart,CurrentFile.VEnd);

		if(zGame.HasFilenames) zl_DMAGetFilename(CurrentFile.Filename, FileNo);

		if((CurrentFile.PStart == 0xFFFFFFFF) || (CurrentFile.PEnd == 0xFFFFFFFF)) IsFileValid = false;

		sprintf(Temp, "%4i %08X %08X %08X %08X %s\n", FileNo, CurrentFile.VStart, CurrentFile.VEnd, CurrentFile.PStart, CurrentFile.PEnd, CurrentFile.Filename);
		dbgprintf(0, (IsFileValid ? MSK_COLORTYPE_INFO : MSK_COLORTYPE_WARNING), Temp);
		FileNo++;

		CurrentFile = zl_DMAGetFile(FileNo);
		IsFileValid = true;
	}

	dbgprintf(0, MSK_COLORTYPE_OKAY, "Done!\n");
}

void cn_Cmd_ShowROMInfo(unsigned char * Ptr)
{
	if(!zROM.IsROMLoaded) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No ROM loaded!\n");
		return;
	}

	zl_ShowROMInformation();
}

void cn_Cmd_ModifyActor(unsigned char * Ptr)
{
	if(!zROM.IsROMLoaded) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No ROM loaded!\n");
		return;
	}

	int ActorNo = 0;
	if(Ptr == NULL) {
		ActorNo = zOptions.SelectedActor;
	} else {
		sscanf((char *)Ptr + 1, "%d", &ActorNo);
	}

	if((ActorNo > zMHeader[0][zOptions.MapToRender].ActorCount) || (ActorNo < 0)) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Invalid Actor specified!\n");
		return;
	} else {
		zOptions.SelectedActor = ActorNo;
		zOptions.SelectedActorMap = zOptions.MapToRender;
	}

	char * DlgTitle = (char*)malloc(sizeof(char) * 256);
	sprintf(DlgTitle, "Map Actor #%i", zOptions.SelectedActor);

	__MSK_UI_Dialog DlgModifyActor =
	{
		DlgTitle, 18, 40,
		{
			{ MSK_UI_DLGOBJ_NUMBERSEL, 1,  1,  0,  "Number|65536|1|1|1", (short*)&zMapActor[zOptions.MapToRender][zOptions.SelectedActor].Number },
			{ MSK_UI_DLGOBJ_NUMBERSEL, 1,  21, 1,  "Var|65536|1|1|1",    (short*)&zMapActor[zOptions.MapToRender][zOptions.SelectedActor].Var },
			{ MSK_UI_DLGOBJ_LINE,      3,  1,  -1, "-1",                 NULL },
			{ MSK_UI_DLGOBJ_LABEL,     5,  10, -1, "Position:",          NULL },
			{ MSK_UI_DLGOBJ_LABEL,     5,  28, -1, "Rotation:",          NULL },
			{ MSK_UI_DLGOBJ_NUMBERSEL, 7,  1,  2,  "X     |65536|1|0|1", (short*)&zMapActor[zOptions.MapToRender][zOptions.SelectedActor].Pos.X },
			{ MSK_UI_DLGOBJ_NUMBERSEL, 9,  1,  3,  "Y     |65536|1|0|1", (short*)&zMapActor[zOptions.MapToRender][zOptions.SelectedActor].Pos.Y },
			{ MSK_UI_DLGOBJ_NUMBERSEL, 11, 1,  4,  "Z     |65536|1|0|1", (short*)&zMapActor[zOptions.MapToRender][zOptions.SelectedActor].Pos.Z },
			{ MSK_UI_DLGOBJ_NUMBERSEL, 7,  24, 5,  "|65536|1|0|1",       (short*)&zMapActor[zOptions.MapToRender][zOptions.SelectedActor].Rot.X },
			{ MSK_UI_DLGOBJ_NUMBERSEL, 9,  24, 6,  "|65536|1|0|1",       (short*)&zMapActor[zOptions.MapToRender][zOptions.SelectedActor].Rot.Y },
			{ MSK_UI_DLGOBJ_NUMBERSEL, 11, 24, 7,  "|65536|1|0|1",       (short*)&zMapActor[zOptions.MapToRender][zOptions.SelectedActor].Rot.Z },
			{ MSK_UI_DLGOBJ_LINE,      13, 1,  -1, "-1",                 NULL },
			{ MSK_UI_DLGOBJ_BUTTON,    -1, -1, 8,  "OK|1",               NULL }
		}
	};

	zProgram.HandleModifyActor = MSK_Dialog(&DlgModifyActor);
}

void cn_Cmd_SaveActors(unsigned char * Ptr)
{
	if(!zROM.IsROMLoaded) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No ROM loaded!\n");
		return;
	}

	zl_SaveMapActors(0, zOptions.MapToRender);
}

void cn_Cmd_SaveROM(unsigned char * Ptr)
{
	if(!zROM.IsROMLoaded) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: No ROM loaded!\n");
		return;
	}

	FILE * file;
	if((file = fopen(zROM.FilePath, "wb")) == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: File not found\n");
		return;
	}
	fwrite(zROM.Data, 1, zROM.Size, file);
	fclose(file);

	dbgprintf(0, MSK_COLORTYPE_OKAY, "- ROM has been saved!\n");
}
