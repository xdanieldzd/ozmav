#include "globals.h"

#include "msk_data.h"

char * UCodeNames[] = { "Fast3D", "F3DEX", "F3DEX2" };

char WorkingDir[MAX_PATH];
bool DoScript = false;

char TempString[MAX_PATH];

void cn_InitCommands()
{
	int i;

	for(i = 0; i < (int)ArraySize(Cmds); i++) MSK_AddCommand(Cmds[i].Name, Cmds[i].Desc, Cmds[i].Func);

	memset(WorkingDir, 0, sizeof(WorkingDir));
}

void cn_Cmd_LoadScript(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: No parameter specified!");
	} else {
		char Filename[MAX_PATH];
		memset(Filename, 0, sizeof(Filename));
		sscanf((char*)Ptr+1, "%s", (char*)&Filename);

		if(Filename[0] == 0x00) {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Invalid parameters specified!");
			return;
		}

		FILE * FP;
		if((FP = fopen(Filename, "r")) == NULL) {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: File %s not found!\n", Filename);
			return;
		}

		GetFilePath(Filename, WorkingDir);
		DoScript = true;

		dbgprintf(0, MSK_COLORTYPE_OKAY, "Now running command script %s...", Filename);

		// clear all RAM
		int i;
		for(i = 0; i < MAX_SEGMENTS; i++) RDP_ClearSegment(i);
		RDP_ClearRDRAM();

		// clear DList addresses
		memset(zProgram.DListAddr, 0, ArraySize(zProgram.DListAddr));
		zProgram.DListCount = -1;
		zProgram.DListSel = -1;

		// clear libbadRDP data
		RDP_ClearStructures(true);
		RDP_ClearTextures();

		// analyze command script
		char Temp[MAX_PATH], TempCmd[MAX_PATH];
		while(fgets(Temp, MAX_PATH, FP) != NULL) {
			// get line and extract command
			sscanf((char*)Temp, "%s", (char*)&TempCmd);
			int i = 0;

			// if line is not a comment, go and check known commands
			if(TempCmd[0] != '#') {
				while(i < (int)ArraySize(Cmds)) {
					// if command exists, execute its function using the line's parameters
					if(!strcmp(Cmds[i].Name, TempCmd)) {
						Cmds[i].Func(Temp + strlen(TempCmd));
						break;
					}
					i++;
				}
			}
		}

		dbgprintf(0, MSK_COLORTYPE_OKAY, "Command script has been executed.");

		DoScript = false;

		fclose(FP);
	}
}

void cn_Cmd_SetUCode(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: No parameter specified!");
	} else {
		char UCodeName[MAX_PATH];
		memset(UCodeName, 0, sizeof(UCodeName));
		sscanf((char*)Ptr+1, "%s", (char*)&UCodeName);

		bool Error = false;
		if(UCodeName[0] == 0x00) {
			Error = true;
		} else {
			int i = 0;
			for(i = 0; i < (int)ArraySize(UCodeNames); i++) {
				if(!strcmp(UCodeName, UCodeNames[i])) {
					dl_ViewerInit(i);
					return;
				}
			}
			Error = true;
		}

		if(Error) {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Invalid parameter specified!");
			return;
		}
	}
}

void cn_Cmd_LoadFile(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: No parameter specified!");
	} else {
		int Segment = -1;
		char Filename[MAX_PATH];
		memset(Filename, 0, sizeof(Filename));
		sscanf((char*)Ptr+1, "0x%02X %s", &Segment, (char*)&Filename);

		if((Filename[0] == 0x00) || (Segment == -1)) {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Invalid parameters specified!");
			return;
		}

		if(DoScript) {
			strcpy(TempString, WorkingDir);
			strcat(TempString, Filename);
			dl_LoadFileToSegment(TempString, Segment);
		} else {
			dl_LoadFileToSegment(Filename, Segment);
		}
	}
}

void cn_Cmd_ClearSegment(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: No parameter specified!");
	} else {
		int Segment = -1;
		sscanf((char*)Ptr+1, "0x%02X", &Segment);

		if(!RDP_CheckAddressValidity((Segment << 24))) {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Invalid segment specified!");
			return;
		}

		dbgprintf(0, MSK_COLORTYPE_INFO, "Cleared segment 0x%02X.", Segment);
		RDP_ClearSegment(Segment);
	}
}

void cn_Cmd_LoadRAM(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: No parameter specified!");
	} else {
		char Filename[MAX_PATH];
		memset(Filename, 0, sizeof(Filename));
		sscanf((char*)Ptr+1, "%s", (char*)&Filename);

		if(Filename[0] == 0x00) {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Invalid parameters specified!");
			return;
		}

		zOptions.EnableGrid = false;

		if(DoScript) {
			strcpy(TempString, WorkingDir);
			strcat(TempString, Filename);
			dl_LoadRAMDump(TempString);
		} else {
			dl_LoadRAMDump(Filename);
		}
	}
}

void cn_Cmd_FindDLists(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: No parameter specified!");
	} else {
		int Segment = -1;
		sscanf((char*)Ptr+1, "0x%02X", &Segment);

		if(!RDP_CheckAddressValidity((Segment << 24))) {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Invalid segment specified!");
			return;
		}

		dl_FindDLists(Segment);
		gl_CreateSceneDLists();
	}
}

void cn_Cmd_AddDList(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: No parameter specified!");
	} else {
		int Address = -1;
		sscanf((char*)Ptr+1, "0x%08X", &Address);

		if(!RDP_CheckAddressValidity(Address)) {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Invalid address specified!");
			return;
		}

		dbgprintf(0, MSK_COLORTYPE_INFO, "Added Display List address 0x%08X.", Address);
		zProgram.DListAddr[++zProgram.DListCount] = Address;

		gl_CreateSceneDLists();
	}
}

void cn_Cmd_ClearDLists(unsigned char * Ptr)
{
	Ptr = NULL;

	memset(zProgram.DListAddr, 0, ArraySize(zProgram.DListAddr));
	zProgram.DListCount = -1;
	zProgram.DListSel = -1;

	dbgprintf(0, MSK_COLORTYPE_INFO, "All Display List addresses removed from list.");
}

void cn_Cmd_SetScale(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: No parameter specified!");
	} else {
		float Scale = 1.0f;
		sscanf((char*)Ptr+1, "%f", (float*)&Scale);

		if(Scale <= 0.0f) {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Invalid scale specified!");
			return;
		}

		dbgprintf(0, MSK_COLORTYPE_INFO, "Set scaling factor to %f.", Scale);
		zProgram.ScaleFactor = Scale;
	}
}

void cn_cmd_SegmentUsage(unsigned char * Ptr)
{
	Ptr = NULL;

	dbgprintf(0, MSK_COLORTYPE_OKAY, "Current RAM segments:");

	int i = 0;
	for(i = 0; i < MAX_SEGMENTS; i++) {
		if(RAM[i].IsSet) {
			dbgprintf(0, MSK_COLORTYPE_OKAY, "- Segment 0x%02X's base address is 0x%08X.", i, RAM[i].SourceOffset);
		} else {
			dbgprintf(0, MSK_COLORTYPE_WARNING, "- Segment 0x%02X is not set.", i);
		}
	}
}

void cn_Cmd_About(unsigned char * Ptr)
{
	Ptr = NULL;
	zProgram.HandleAbout = MSK_Dialog(&DlgAbout);
}
