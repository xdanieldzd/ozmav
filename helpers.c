/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	helpers.c - misc. helper functions (log file handling, etc.)
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

int Zelda_MemCopy(unsigned int SourceBank, unsigned long SourceOffset, unsigned char * Target, unsigned long Size)
{
	switch(SourceBank) {
	case 0x00:
		/* invalid source, ignore */
		break;
	case 0x02:
		/* data comes from scene file */
		if(((SourceOffset + Size) / 4) < ZSceneFilesize) {
			memcpy(Target, &ZSceneBuffer[SourceOffset / 4], Size);
		} else {
			return -1;
		}
		break;
	case 0x03:
		/* data comes from map file */
		if(((SourceOffset + Size) / 4) < ZMapFilesize[ROM_CurrentMap_Temp]) {
			memcpy(Target, &ZMapBuffer[ROM_CurrentMap_Temp][SourceOffset / 4], Size);
		} else {
			return -1;
		}
		break;

	/* MM TEST */
/*	case 0x06:
		if((SourceOffset + Size / 4) < ROMFilesize) {
			memcpy(Target, &ROMBuffer[(0x650ef0 * 7 / 4) + (SourceOffset / 4)], Size);		// MM JPN 1.0 - mountain village (snow) ????
		} else {
			return -1;
		}
		break;
*/
//	case 0x04:
//		/* data comes from gameplay_keep */
//		if((SourceOffset + Size / 4) < GameplayKeepFilesize) {
//			memcpy(Target, &GameplayKeepBuffer[SourceOffset / 4], Size);
//		} else {
//			return -1;
//		}
//		break;
//	case 0x05:
//		/* data comes from gameplay_dangeon_keep */
//		if((SourceOffset + Size / 4) < GameplayFDKeepFilesize) {
//			memcpy(Target, &GameplayFDKeepBuffer[SourceOffset / 4], Size);
//		} else {
//			return -1;
//		}
//		break;
//	case 0x06:
		/* data comes from object file */
/*		if(((SourceOffset + Size / 4) < CurrentObject_Length) && (TempObjectBuffer_Allocated)) {
			memcpy(Target, &TempObjectBuffer[SourceOffset / 4], Size);
		} else {
			return -1;
		}
		break;
*/	default:
		/* fallback if source is not handled / no valid source was found */
		memset(Target, 0xFF, Size);
		return -1;
		break;
	}

//	memset(SystemLogMsg, 0x00, sizeof(SystemLogMsg));
//	sprintf(SystemLogMsg, "ZMemCopy: Copying 0x%04X bytes from 0x%02X|%06X\n", Size, SourceBank, SourceOffset);
//	Helper_LogMessage(2, SystemLogMsg);

	return 0;
}

/*	------------------------------------------------------------ */

void Helper_SplitCurrentVals(bool SplitDual)
{
	Readout_CurrentByte1 = Readout_Current1 << 24;
	Readout_CurrentByte1 = Readout_CurrentByte1 >> 24;
	Readout_CurrentByte2 = Readout_Current1 << 16;
	Readout_CurrentByte2 = Readout_CurrentByte2 >> 24;
	Readout_CurrentByte3 = Readout_Current1 << 8;
	Readout_CurrentByte3 = Readout_CurrentByte3 >> 24;
	Readout_CurrentByte4 = Readout_Current1;
	Readout_CurrentByte4 = Readout_CurrentByte4 >> 24;

	if(SplitDual) {
		Readout_CurrentByte5 = Readout_Current2 << 24;
		Readout_CurrentByte5 = Readout_CurrentByte5 >> 24;
		Readout_CurrentByte6 = Readout_Current2 << 16;
		Readout_CurrentByte6 = Readout_CurrentByte6 >> 24;
		Readout_CurrentByte7 = Readout_Current2 << 8;
		Readout_CurrentByte7 = Readout_CurrentByte7 >> 24;
		Readout_CurrentByte8 = Readout_Current2;
		Readout_CurrentByte8 = Readout_CurrentByte8 >> 24;
	}
}

int Helper_LogMessage(int LogType, char Message[])
{
	switch(LogType) {
	case 1:
		fprintf(FileGFXLog, Message);
		break;
	case 2:
		fprintf(FileSystemLog, Message);
		break;
	}
	return 0;
}

int Helper_GFXLogCommand(unsigned int Position)
{
	sprintf(GFXLogMsg, "  0x%08X:\t%s\t\t[%02X%02X%02X%02X %02X%02X%02X%02X] %s\n",
		Position * 4, CurrentGFXCmd,
		Readout_CurrentByte1, Readout_CurrentByte2, Readout_CurrentByte3, Readout_CurrentByte4,
		Readout_CurrentByte5, Readout_CurrentByte6, Readout_CurrentByte7, Readout_CurrentByte8,
		CurrentGFXCmdNote);
	Helper_LogMessage(1, GFXLogMsg);

	return 0;
}

int Helper_CalculateFPS()
{
	if(GetTickCount() - Renderer_LastFPS >= 1000)
	{
		Renderer_LastFPS = GetTickCount();
		Renderer_FPS = Renderer_FrameNo;
		Renderer_FrameNo = 0;
	}
	Renderer_FrameNo++;

	sprintf(Renderer_FPSMessage, "%d FPS", Renderer_FPS);
	SendMessage(hstatus, SB_SETTEXT, 0, (LPARAM)Renderer_FPSMessage);

	return 0;
}

/*	------------------------------------------------------------ */

int Helper_FileReadLine(FILE * Source, int End, unsigned char * Target)
{
	if(ftell(Source) >= End) return -1;

	memset(Target, 0x00, sizeof(Target));
	char CurChar = 0; int Length = 0;

	while((CurChar != 0x0D) && (CurChar != 0x0A)) {
		fread(&CurChar, 1, 1, Source);
		if((CurChar != 0x0D) && (CurChar != 0x0A)) { sprintf(Target, "%s%c", Target, CurChar); Length++; }
	}

	fseek(Source, 1, SEEK_CUR);

	return 0;
}

/*	------------------------------------------------------------ */
