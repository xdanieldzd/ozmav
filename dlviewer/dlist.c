#include "globals.h"

unsigned char Cmd_DL = 0;
unsigned char Cmd_ENDDL = 0;
unsigned char Cmd_TEXTURE = 0;

void dl_ViewerInit(int UCode)
{
	RDP_ClearStructures(true);
	RDP_ClearTextures();

	if(zProgram.UCode != UCode) {
		zProgram.UCode = UCode;
		RDP_InitParser(zProgram.UCode);

		switch(zProgram.UCode) {
			case F3D:
			case F3DEX:
				Cmd_DL = 0x06;
				Cmd_ENDDL = 0xB8;
				Cmd_TEXTURE = 0xBB;
				break;
			case F3DEX2:
				Cmd_DL = 0xDE;
				Cmd_ENDDL = 0xDF;
				Cmd_TEXTURE = 0xD7;
				break;
		}
		dbgprintf(0, MSK_COLORTYPE_OKAY, "Viewer initialized, using %s microcode.\n", UCodeNames[UCode]);
	}
}

void dl_LoadFileToSegment(char * Filename, unsigned char Segment)
{
	FILE * file;
	if((file = fopen(Filename, "rb")) == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: File %s not found!\n", Filename);
		return;
	}

	fseek(file, 0, SEEK_END);
	int Size = ftell(file);
	rewind(file);
	unsigned char * Data = (unsigned char*) malloc (sizeof(char) * Size);
	fread(Data, 1, Size, file);
	fclose(file);

	dl_ViewerInit(zProgram.UCode);

	if(RDP_CheckAddressValidity(Segment << 24)) {
		RDP_ClearSegment(Segment);
	}

	RDP_LoadToSegment(Segment, Data, 0, Size);

	free(Data);

	dbgprintf(0, MSK_COLORTYPE_INFO, "Loaded %s to segment 0x%02X.", Filename, Segment);
}

void dl_LoadRAMDump(char * Filename)
{
	FILE * file;
	if((file = fopen(Filename, "rb")) == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: File %s not found!\n", Filename);
		return;
	}

	fseek(file, 0, SEEK_END);
	int Size = ftell(file);
	rewind(file);
	unsigned char * TempRDRAM = (unsigned char*) malloc(sizeof(char) * Size);
	fread(TempRDRAM, 1, Size, file);
	fclose(file);

	RDP_LoadToRDRAM(TempRDRAM, Size);

	free(TempRDRAM);

	dbgprintf(0, MSK_COLORTYPE_INFO, "Loaded %s to RDRAM.", Filename);

/*	zProgram.DListSel = 0;
	//zProgram.DListAddr[0] = 0x802E1AD0; //sin
	//zProgram.DListAddr[0] = 0x8016E5A0; //eva
	zProgram.DListAddr[0] = 0x8016A648; //oot
	zProgram.DListCount = 0;*/
}

bool dl_IsDLEndInBetween(unsigned char Segment, unsigned int From, unsigned int To)
{
	int i = From;

	while(i < To) {
		unsigned int W0 = Read32(RAM[Segment].Data, i);
		unsigned int W1 = Read32(RAM[Segment].Data, i + 4);

		if((W0 == (Cmd_ENDDL << 24)) && (W1 == 0)) return true;

		i++;
	}

	return false;
}

void dl_FindDLists(unsigned char Segment)
{
	zProgram.DListCount = -1;
	zProgram.DListSel = -1;

	int i = 0;
	unsigned int FullAddr = 0;

	while(1) {
		FullAddr = ((Segment << 24) | i);
		if(!RDP_CheckAddressValidity(FullAddr + 8)) break;

		unsigned int W0 = Read32(RAM[Segment].Data, i);
		unsigned int W1 = Read32(RAM[Segment].Data, i + 4);

		if((W0 == (Cmd_DL << 24)) && (W1 != 0) && (RDP_CheckAddressValidity(W1))) {
			zProgram.DListAddr[++zProgram.DListCount] = W1;
			dbgprintf(0, MSK_COLORTYPE_INFO, "Found Display List at address 0x%08X.", zProgram.DListAddr[zProgram.DListCount]);
		}

		i += 8;
	}

	i = 0;

	if(zProgram.DListCount == -1) {
		while(1) {
			FullAddr = ((Segment << 24) | i);
			if(!RDP_CheckAddressValidity(FullAddr + 8)) break;

			unsigned int W0 = Read32(RAM[Segment].Data, i);
			unsigned int W1 = Read32(RAM[Segment].Data, i + 4);

			if(	((W0 == 0xE7000000) && (W1 == 0)) ||
				((W0 == 0xE8000000) && (W1 == 0)) ||
				(W0 >> 8 == 0xFA0000) ||
				(W0 == 0xFB000000) ||
				(W0 >> 8 == (Cmd_TEXTURE << 16)) ||
				((W0 >> 24 == 0xFD) && ((W0 & 0x0000FFFF) == 0) && (RDP_CheckAddressValidity(W1)))) {
					if(zProgram.DListCount == -1) {
						zProgram.DListAddr[++zProgram.DListCount] = FullAddr;
						dbgprintf(0, MSK_COLORTYPE_INFO, "Found initial Display List at address 0x%08X.", zProgram.DListAddr[zProgram.DListCount]);
					} else {
//						dbgprintf(0,0,"%08X -> %08X",i,RAM[Segment].Size);
						if(	dl_IsDLEndInBetween(Segment, (zProgram.DListAddr[zProgram.DListCount] & 0x00FFFFFF), i) &&
							dl_IsDLEndInBetween(Segment, i, RAM[Segment].Size)) {
								zProgram.DListAddr[++zProgram.DListCount] = FullAddr;
								dbgprintf(0, MSK_COLORTYPE_INFO, "Assuming next Display List at address 0x%08X.", zProgram.DListAddr[zProgram.DListCount]);
							}
					}
			}

			i += 8;
		}
	}

	if(zProgram.DListCount == 0) zProgram.DListSel = 0;

	dbgprintf(0, MSK_COLORTYPE_INFO, "Found %i Display Lists.", zProgram.DListCount + 1);
}
