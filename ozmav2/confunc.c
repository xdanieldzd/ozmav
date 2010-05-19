#include "globals.h"
#include "dialog.h"

void cn_Cmd_LoadScene(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		char * SceneSelParam = (char*)malloc(sizeof(char) * 256);
		sprintf(SceneSelParam, "Scene ID|%i|1", zGame.SceneCount + 1);

		__MSK_UI_Dialog DlgLoadScene =
		{
			"Load Scene", 10, 40,
			{
				{ MSK_UI_DLGOBJ_LABEL,		1,	1,	-1,	"Select which Scene to load:",		NULL },
				{ MSK_UI_DLGOBJ_NUMBERSEL,	3,	1,	0,	(char*)SceneSelParam,				(int*)&zOptions.SceneNo },
				{ MSK_UI_DLGOBJ_LINE,		5,	1,	-1,	"-1",								NULL },
				{ MSK_UI_DLGOBJ_BUTTON,		-1,	-1,	1,	"OK|1",								NULL }
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
	int LastDbg = zOptions.DebugLevel;
	bool LastDump = zOptions.DumpModel;

	zOptions.DebugLevel = 0;
	struct __zCamera TempCam = zCamera;
	zOptions.DumpModel = true;

	if(zl_LoadScene(zOptions.SceneNo)) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: Fatal error!\n");
	} else {
		dbgprintf(0, MSK_COLORTYPE_OKAY, "> Model has been dumped.\n");
	}

	zOptions.DumpModel = LastDump;
	zOptions.DebugLevel = LastDbg;
	zCamera = TempCam;
}

void cn_Cmd_SetTexture(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: No parameter specified!\n");
	} else {
		int Var = 0;
		sscanf((char*)Ptr+1, "%d", &Var);
		zOptions.EnableTextures = Var;
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
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "> Error: No parameter specified!\n");
	} else {
		int Var = 0;
		sscanf((char*)Ptr+1, "%d", &Var);
		zOptions.EnableCombiner = Var;
		dbgprintf(0, MSK_COLORTYPE_OKAY, "> Combiner emulation has been %s.\n", (Var ? "enabled" : "disabled"));

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
	zProgram.HandleOptions = MSK_Dialog(&DlgOptions);
}

void cn_Cmd_About(unsigned char * Ptr)
{
	zProgram.HandleAbout = MSK_Dialog(&DlgAbout);
}

void cn_Cmd_ExtractFiles(unsigned char * Ptr)
{
	dbgprintf(0, MSK_COLORTYPE_OKAY, "Extracting ROM data...\n");

	char Temp[256];
	sprintf(Temp, ".//extr");
	oz_CreateFolder(Temp);
	sprintf(Temp, ".//extr//%s", zGame.TitleText);
	oz_CreateFolder(Temp);

	DMA CurrentFile = {0, 0, 0, 0, 0, ""};
	int FileNo = 0;
	bool IsFileValid = true;

	CurrentFile = zl_DMAGetFile(FileNo);

	while((CurrentFile.VEnd != 0x00) || (CurrentFile.PStart != 0xFFFFFFFF)) {
		if(CurrentFile.VStart == CurrentFile.VEnd) break;

		CurrentFile = zl_DMAVirtualToPhysical(CurrentFile.VStart);

		if(zGame.HasFilenames) {
			zl_DMAGetFilename(CurrentFile.Filename, FileNo);
		} else {
			sprintf(CurrentFile.Filename, "%04i_%08X-%08X", FileNo, CurrentFile.PStart, CurrentFile.PEnd);
		}

		if((CurrentFile.PStart == 0xFFFFFFFF) || (CurrentFile.PEnd == 0xFFFFFFFF)) IsFileValid = false;

		dbgprintf(0, MSK_COLORTYPE_INFO, "File %i: %s, PStart 0x%08X, PEnd 0x%08X%s\n", FileNo, CurrentFile.Filename, CurrentFile.PStart, CurrentFile.PEnd, (IsFileValid ? "" : " XX"));
		FileNo++;

		sprintf(Temp, ".//extr//%s//%s", zGame.TitleText, CurrentFile.Filename);
//		dbgprintf(0, MSK_COLORTYPE_INFO, Temp);

		if(IsFileValid) {
			FILE * File = fopen(Temp, "wb");
			if(File != NULL) {
				fwrite(&zROM.Data[CurrentFile.PStart], 1, (CurrentFile.PEnd - CurrentFile.PStart), File);
				fclose(File);
			} else {
				dbgprintf(0, MSK_COLORTYPE_ERROR, "Error: Could not create file!\n");
				break;
			}
		}

		CurrentFile = zl_DMAGetFile(FileNo);
		IsFileValid = true;
	}

	dbgprintf(0, MSK_COLORTYPE_OKAY, "Done!\n");
}

void cn_Cmd_ListFiles(unsigned char * Ptr)
{
	if(!zGame.HasFilenames) dbgprintf(0, MSK_COLORTYPE_WARNING, "Error: ROM does not contain filenames!\n");

	char Temp[256];

	DMA CurrentFile = {0, 0, 0, 0, 0, ""};
	int FileNo = 0;
	bool IsFileValid = true;

	CurrentFile = zl_DMAGetFile(FileNo);

	dbgprintf(0, MSK_COLORTYPE_INFO, "#    VStart   VEnd     PStart   PEnd     Filename\n");

	while((CurrentFile.VEnd != 0x00) || (CurrentFile.PStart != 0xFFFFFFFF)) {
		if(CurrentFile.VStart == CurrentFile.VEnd) break;

		CurrentFile = zl_DMAVirtualToPhysical(CurrentFile.VStart);

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
