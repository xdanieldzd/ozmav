/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	uc_misc.c - F3DEX2 misc. functions (DLists, RDPHALF, etc)
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

int F3DEX2_Cmd_RDPHALF_1()
{
	Storage_RDPHalf1 = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	return 0;
}

int F3DEX2_Cmd_RDPHALF_2()
{
	Storage_RDPHalf2 = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	return 0;
}

int F3DEX2_Cmd_BRANCH_Z()
{
	unsigned int TempRAMSeg = (Storage_RDPHalf1 & 0xFF000000) >> 24;
	unsigned long TempDLOffset = (Storage_RDPHalf1 & 0x00FFFFFF);

	if(TempRAMSeg == 0x03) {
		Viewer_RenderMap_DListParser(true, TempDLOffset / 4);
	}

	return 0;
}

int F3DEX2_Cmd_DL(bool GetDLFromZMapScene)
{
	if(Readout_CurrentByte5 == 0x03) {
		unsigned long TempOffset;

		TempOffset = Readout_CurrentByte6 << 16;
		TempOffset = TempOffset + (Readout_CurrentByte7 << 8);
		TempOffset = TempOffset + Readout_CurrentByte8;

		Viewer_RenderMap_DListParser(true, TempOffset / 4);
	} else {
		//fprintf(FileSystemLog, "- Cannot execute Display List in RAM segment 0x%02X!\n", Readout_CurrentByte5);
	}

	return 0;
}
