#include "globals.h"

int ExecuteCollisionScript(unsigned int ColScriptSeg, unsigned int ColScriptPos);

int InitLevelScriptInterpreter() /* From MariOZMAV */
{
	int i = 0;

	for(i = 0; i < SEG_COUNT; i++) {
		if(RAMSegment[i].Data != NULL) free(RAMSegment[i].Data);
		RAMSegment[i].IsSet = false;
	}

	/* ---- populating RAM segment 0x15 ---- */
	unsigned long TempSegLoad_Start = 0x2ABCA0;
	unsigned long TempSegLoad_End = 0x2AC6B0;
	unsigned long TempSegLoad_Length = TempSegLoad_End - TempSegLoad_Start;
	unsigned int TempSegLoad_Segment = 0x15;

	RAMSegment[TempSegLoad_Segment].Data = (unsigned char*) malloc (sizeof(char) * TempSegLoad_Length);
	RAMSegment[TempSegLoad_Segment].Length = TempSegLoad_Length;
	memcpy(RAMSegment[TempSegLoad_Segment].Data, &ROMBuffer[TempSegLoad_Start], TempSegLoad_Length);
	RAMSegment[TempSegLoad_Segment].IsSet = true;

	/* ---- loading actual level's layout script (hackish way) ---- */
	/* set base offset of pointer table*/
	LevelLayoutScript_Base = 0x2AC094;

	/* offset of level id's pointer data to "level layout script" inside table */
	TempOffset = LevelLayoutScript_Base + (LevelID * 0x14);

	/* get level layout script start & end offsets */
	LvlScript_Start = Read32(ROMBuffer, TempOffset + 4);
	LvlScript_End = Read32(ROMBuffer, TempOffset+8);

	dmsg("LvlScript_Start: %08X LvlScript_End: %08X\n", LvlScript_Start, LvlScript_End);

	/* get level layout script exec entry offset */
	LvlScript_Entry = Read24(ROMBuffer, TempOffset + 13);

	/* calculate length of layout script from start & end offsets */
	LvlScript_Length = LvlScript_End - LvlScript_Start;

	/* allocate buffer for layout script and copy it there */
	RAMSegment[0x0E].Data = (unsigned char*) malloc (sizeof(char) * LvlScript_Length);
	RAMSegment[0x0E].Length = LvlScript_Length;
	memcpy(RAMSegment[0x0E].Data, &ROMBuffer[LvlScript_Start], LvlScript_Length);
	RAMSegment[0x0E].IsSet = true;

	MacroPreset_Offset = 0x0EC7E0;
	MacroPreset_Length = 0x0B70;

	return 0;
}

int ExecuteLevelScript()
{
	dmsg(" - Level layout script: 0x%06X to 0x%06X (0x%X bytes)\n - Entry point at 0x%06X\n\n", LvlScript_Start, LvlScript_End, LvlScript_Length, LvlScript_Entry);

	unsigned int ObjGeoOffset[256];
	memset(ObjGeoOffset, 0x00, sizeof(ObjGeoOffset));

	bool EndOfScript = false;
	ColVtxBuffer = (unsigned char *) malloc (sizeof(char) * 0x8000);
	ColTriBuffer = (unsigned char *) malloc (sizeof(char) * 0x10000);
	ZWaterBuffer = (unsigned char *) malloc (sizeof(char) * 0x400);	/* Room for 64 water boxes */
	memset(ColTriBuffer, 0x10000, 0x0);
	memset(ZWaterBuffer, 0x400, 0x0);
	ZWaterOffset = 0;
	#ifdef DEBUG
	int i;
	#endif

	while (!(EndOfScript) && (TempScriptPos < ROMFilesize)) {
		CurrentCmd = Read16(RAMSegment[TempScriptSegment].Data, TempScriptPos);
		CurrentCmdLength = RAMSegment[TempScriptSegment].Data[TempScriptPos + 1];
		JumpInScript = false;

		if(CurrentCmdLength == 0x00) EndOfScript = true;

		switch(CurrentCmd)
		{
			case 0x0204:
			case 0x1E04:
				// end
				EndOfScript = true;
				break;

			case 0x0608: {
				// jump
				unsigned int TargetSeg = RAMSegment[TempScriptSegment].Data[TempScriptPos + 4];

				if(RAMSegment[TargetSeg].IsSet) {
					TempScriptPos_Backup = TempScriptPos;
					TempScriptSegment_Backup = TempScriptSegment;
					TempScriptPos = Read24(RAMSegment[TempScriptSegment].Data, TempScriptPos + 5);
					TempScriptSegment = TargetSeg;
					JumpInScript = true;
				}
				break; }

			case 0x0704:
				// return
				TempScriptPos = (TempScriptPos_Backup + 4);
				TempScriptSegment = TempScriptSegment_Backup;
				break;

			case 0x170C: {
				// setup ram segment
				unsigned int TempSeg = RAMSegment[TempScriptSegment].Data[TempScriptPos + 3];
				dmsg("- Segment loader: 0x%02X\n", TempSeg);

				unsigned int ROMData_Start = Read32(RAMSegment[TempScriptSegment].Data, TempScriptPos + 4);
				unsigned int ROMData_End = Read32(RAMSegment[TempScriptSegment].Data, TempScriptPos + 8);
				unsigned int ROMData_Length = ROMData_End - ROMData_Start;

				RAMSegment[TempSeg].Data = (unsigned char *) malloc (sizeof(char) * ROMData_Length);
				memcpy(RAMSegment[TempSeg].Data, &ROMBuffer[ROMData_Start], ROMData_Length);
				RAMSegment[TempSeg].IsSet = true;
				RAMSegment[TempSeg].Length = ROMData_Length;

				dmsg("[ROM] RAM segment 0x%02X: 0x%08X to 0x%08X (0x%X bytes)\n", TempSeg, ROMData_Start, ROMData_End, ROMData_Length);
				break; }

			case 0x1A0C: {
				// textures
				unsigned int TempSeg = RAMSegment[TempScriptSegment].Data[TempScriptPos + 3];
				dmsg("- Texture loader: 0x%02X\n", TempSeg);
				if(TempSeg == 0x09) {
					unsigned int TexData_Start = Read32(RAMSegment[TempScriptSegment].Data, TempScriptPos + 4);
					unsigned int TexData_End = Read32(RAMSegment[TempScriptSegment].Data, TempScriptPos + 8);
					TexData_Start += Read32(ROMBuffer, TexData_Start + 8);
					TexData_Start += 2;
					unsigned int TexData_Length = TexData_End - TexData_Start;

					RAMSegment[TempSeg].Data = (unsigned char *) malloc (sizeof(char) * TexData_Length);
					memcpy(RAMSegment[TempSeg].Data, &ROMBuffer[TexData_Start], TexData_Length);
					RAMSegment[TempSeg].IsSet = true;
					RAMSegment[TempSeg].Length = TexData_Length;

					dmsg("[ROM] External texture data: 0x%08X to 0x%08X (0x%X bytes)\n", TexData_Start, TexData_End, TexData_Length);
				}
				break; }

			case 0x1D04: {
				// end of segment loading sequence
				// generate initial zmap data

				// only generate when segment 0x07 is already set, if it's not we're still executing segment 0x15
				if(RAMSegment[0x07].IsSet == false) break;

				msg(3, " - Generating initial Zelda map data...\n");

				ZMapFilesize = RAMSegment[0x07].Length + ZMAP_HEADERGAP;
				int PadSize = GetPaddingSize(ZMapFilesize, 0x08);
				TextureSize = RAMSegment[0x09].Length;

				dmsg("  - Size of main data:\t\t\t\t\t0x%06X\n", ZMapFilesize);
				dmsg("  - Size of padding:\t\t\t\t\t0x%04X\n", PadSize);
				dmsg("  - Size of external texture data:\t\t0x%06X\n", TextureSize);

				ZMapFilesize += PadSize;
				ZMapFilesize += TextureSize;

				dmsg("  - Total data size:\t\t\t\t\t0x%06X\n", ZMapFilesize);
				msg(3, " - Allocating and clearing map buffer...\n");
				ZMapBuffer = (unsigned char*) malloc (sizeof(char) * ZMapFilesize);
				memset(ZMapBuffer, 0x00, ZMapFilesize);

				msg(3, " - Copying main data and external texture data...\n");

				memcpy(&ZMapBuffer[ZMAP_HEADERGAP], RAMSegment[0x07].Data, RAMSegment[0x07].Length);
				memcpy(&ZMapBuffer[ZMapFilesize - TextureSize], RAMSegment[0x09].Data, TextureSize);

				msg(3, " - Done, resuming conversion.\n");
				break; }

			case 0x1F08: {
				// execute geometry script
				TempGeoScriptSegment = RAMSegment[TempScriptSegment].Data[TempScriptPos + 4];
				TempGeoScriptPos = Read24(RAMSegment[TempScriptSegment].Data, TempScriptPos + 5);
				TempGeoScriptSegment_Backup = 0x00, TempGeoScriptPos_Backup = 0x00;

				CurrentLevelArea = RAMSegment[TempScriptSegment].Data[TempScriptPos + 2];

				dmsg("\nCurrent level area set to 0x%02X (wanted 0x%02X)\n", CurrentLevelArea, LevelArea);

				if(LevelArea != CurrentLevelArea) break;

				ExecuteGeoScript(false);

				break; }

			case 0x2E08: // execute collision script
			{
				if(LevelArea != CurrentLevelArea) break;

				unsigned int ColScriptSeg = RAMSegment[TempScriptSegment].Data[TempScriptPos + 4];
				unsigned int ColScriptPos = Read24(RAMSegment[TempScriptSegment].Data, TempScriptPos + 5);

				if (RAMSegment[ColScriptSeg].IsSet == true)
					ExecuteCollisionScript(ColScriptSeg, ColScriptPos);

				break;
			}
			case 0x2208: {
				// load object w/ geo layout
				unsigned char ObjID = RAMSegment[TempScriptSegment].Data[TempScriptPos + 3];
				if(RAMSegment[TempScriptSegment].Data[TempScriptPos + 4] == 0x0E) {
					unsigned int GeoOffset = Read32(RAMSegment[TempScriptSegment].Data, TempScriptPos + 4);
					ObjGeoOffset[ObjID] = GeoOffset;
					InsertSpecialObjects(ObjID, GeoOffset);
				}
				break; }
			case 0x2418: {
				if(TempScriptSegment == 0x0E)
					SM64_Behavior_To_OoT(TempScriptSegment, TempScriptPos, 0);
				break; }
			case 0x3908: {
				dmsg("\nAdding macro objects for level area 0x%02X...\n", LevelArea);

				unsigned int MacroSeg = RAMSegment[TempScriptSegment].Data[TempScriptPos + 4];
				unsigned int MacroPos = Read24(RAMSegment[TempScriptSegment].Data, TempScriptPos + 5);

				bool EndOfObjects = false;
				while(!EndOfObjects) {
					unsigned int Preset = Read16(RAMSegment[MacroSeg].Data, MacroPos);
					Preset &= 0x01FF;
					unsigned int Behav = Read32(ROMBuffer, MacroPreset_Offset + ((Preset - 0x01F) << 3));

					if((Preset == 0x01E) || (Preset == 0x000)) {
						EndOfObjects = true;
					} else {
						SM64_Behavior_To_OoT(MacroSeg, MacroPos, Behav);
						MacroPos += 0x0A;
					}
				}

				break; }

			default:
				#ifdef DEBUG
				printf("Unknown: %04X:", CurrentCmd);
				for (i=TempScriptPos+2;i<TempScriptPos+CurrentCmdLength; i++)
					printf("%02X",RAMSegment[TempScriptSegment].Data[i]);
				printf("\n");
				#endif
				break;
		}

		if(!JumpInScript) TempScriptPos += CurrentCmdLength;
	}

	return 0;
}

int ExecuteCollisionScript(unsigned int ColScriptSeg, unsigned int ColScriptPos)
{
	/* SM64 Collision format
	[command][...data...][command][...data...][command][...data...] etc

	COMMAND[16bits]	DATA				DESC
	0x0000-0x003F	xxxx[aaaabbbbcccc[vvvv*]]	Polygons, type is command, number of tris is x, each tri is 6 bytes, two for each tri
							 v is variable for certain types of polygons, making each poly 8 bytes. see list below
	0x0040		xxxx[aaaabbbbcccc]		Load x number of verts, each vertex being 6 bytes (3 signed 16 bit x/y/z)
	0x0041						NOP
	0x0042						End collision data
	0x0043		????				?
	0x0044		xxxx[aaaabbbbccccddddeeeeffff]	Water
	0x004D		aaaabbbbccccdddd		?
	0x0045-0x0064	?				?
	0x0065-0x00F8					See Polygons above.

	* these polygon types are 8 bytes each: 000E, 002C, 0024, 0025, 0027, 002D

	if you don't understand this just breakpoint the start of the collision data, the routines explain it all.

	*/
	dmsg("\nExecuting collision script for level area 0x%02X...\n", LevelArea);

	bool EndOfColScript = false;
	bool _0x40read = false;
	unsigned int CurrentColCmd = 0;
	unsigned int VtxCount;

	unsigned short WaterCount;
	signed short x1, x2, y, z1, z2;

	int i;

	while (!(EndOfColScript))
	{
		CurrentColCmd = Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos);
		switch(CurrentColCmd)
		{
			case 0x0041:	// "nop"
				ColScriptPos+=2;
				break;
			case 0x0042:	// End marker
				ColScriptEnd:
				EndOfColScript = true;
				break;
			case 0x0043:	// Special objects?
			{
				unsigned short CollisionSpecialCount = Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos + 2);
				dmsg("- %i Special collision objects [ignored]\n", CollisionSpecialCount);
				ColScriptPos += (CollisionSpecialCount << 3) + 2;
				// hackish code gtfo
				unsigned short CurrHalfWord = 0;
				while (1)
				{
					CurrHalfWord = Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos);
					if(CurrHalfWord == 0x0044)
						goto ColScriptWater;
					if(CurrHalfWord == 0x0042)
						goto ColScriptEnd;
					ColScriptPos+=2;
				}
				break;
			}
			case 0x0044:	// Water
			{
				ColScriptWater:

				WaterCount = Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos + 2);
				ColScriptPos+=4;

				ZWaterCount += WaterCount;

				dmsg("- %i Water boxes\n", WaterCount);
				for (i = 0; i < WaterCount; i++)
				{
					ColScriptPos+=2; // ID of mesh
					x1	= Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos); ColScriptPos+=2;
					z1	= Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos); ColScriptPos+=2;
					x2	= Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos); ColScriptPos+=2;
					z2	= Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos); ColScriptPos+=2;
					y	= Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos); ColScriptPos+=2;

					x1 /= SCALE;
					z1 /= SCALE;
					x2 /= SCALE;
					z2 /= SCALE;
					y /= SCALE;

					x2 -= x1;
					z2 -= z1;

					Write16(ZWaterBuffer, ZWaterOffset, x1);
					Write16(ZWaterBuffer, ZWaterOffset+2, y);
					Write16(ZWaterBuffer, ZWaterOffset+4, z1);
					Write16(ZWaterBuffer, ZWaterOffset+6, x2);
					Write16(ZWaterBuffer, ZWaterOffset+8, z2);
					ZWaterOffset += 0x10;

					dmsg(" - Water Box %d: X1 %5i, X2 %5i, Y %5i, Z1 %5i, Z2 %5i\n", i, x1, x2, y, z1, z2);
				}

				break;
			}
			case 0x004D:
				ColScriptPos+=0xA;
				break;
			case 0x0040:	// Verts
			{
				if (_0x40read) ColVtxCount += _ColVtxCount;
				_0x40read = true;
				VtxCount = Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos + 2);
				_ColVtxCount = VtxCount;

				unsigned int BufferSize = VtxCount * 6;
				memcpy(&ColVtxBuffer[ColVtxCount], &RAMSegment[ColScriptSeg].Data[ColScriptPos + 4], BufferSize);
				dmsg("- VtxCount %i ColVtxCount %i\n",VtxCount,ColVtxCount);
				// scale verts
				for (i=ColVtxCount; i<(ColVtxCount+BufferSize); i+=6)
				{
					/* Read */
					x1	= Read16(ColVtxBuffer, i);
					y	= Read16(ColVtxBuffer, i+2);
					z1	= Read16(ColVtxBuffer, i+4);
					/* Scale */
					x1	/= SCALE;
					y	/= SCALE;
					z1	/= SCALE;
					/* Write */
					Write16(ColVtxBuffer, i+0, x1);
					Write16(ColVtxBuffer, i+2, y);
					Write16(ColVtxBuffer, i+4, z1);
				}

				ColScriptPos += BufferSize + 4;
				break;
			}
			default:
			{
				if ((CurrentColCmd < 0x40)||(CurrentColCmd >= 0x65))
				{
					unsigned short ColType = CurrentColCmd & 0xFFFF;
					unsigned short TriCount = Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos + 2);

					ColScriptPos += 4;
					ColTriCount += TriCount;

					unsigned short p1, p2, p3;

					int _inc = ((ColType == 0x000E) ||
						(ColType == 0x002C) ||
						(ColType == 0x0024) ||
						(ColType == 0x0025) ||
						(ColType == 0x0027) ||
						(ColType == 0x002D)) ? 8 : 6;
					dmsg("- ColType: 0x%04X ZTriOffset 0x%04X TriCount %04i ColTriCount %04i\n",ColType,ZTriOffset,TriCount,ColTriCount);

					for(i = 0; i < TriCount; i++)
					{

						p1 = ColVtxCount + (Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos));
						p2 = ColVtxCount + (Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos + 2));
						p3 = ColVtxCount + (Read16(RAMSegment[ColScriptSeg].Data, ColScriptPos + 4));
						if((p1>ColVtxCount+_ColVtxCount)||(p2>ColVtxCount+_ColVtxCount)||(p3>ColVtxCount+_ColVtxCount))
						{
							msg(1, "WARNING: Collision vertex overflow!!!: %04i/%04i/%04i;max %04i", p1, p2, p3, ColVtxCount+_ColVtxCount);
							EndOfColScript = true;
							return -1; /* i hope this raises an error */
						}
						Write16(ColTriBuffer, ZTriOffset+2, p1);
						Write16(ColTriBuffer, ZTriOffset+4, p2);
						Write16(ColTriBuffer, ZTriOffset+6, p3);

						ZTriOffset += 16;
						ColScriptPos += _inc;
					}
				}
				else
				{
					dmsg("Unknown collision command: %04X\n", CurrentColCmd);
					EndOfColScript= true;
				}
				break;
			}
		}
	}
	ColVtxCount += _ColVtxCount;
	return 0;
}

int ExecuteGeoScript(bool IsObject)
{
	#ifdef DEBUG
	if(IsObject) {
		printf("\nExecuting object geometry script for level area 0x%02X...\n\n", LevelArea);
	} else {
		printf("\nExecuting level geometry script for level area 0x%02X...\n\n", LevelArea);
	}
	#endif

	bool EndOfGeoScript = false;
	JumpInGeoScript = false;
	unsigned int CurrentGeoCmd = 0, CurrentGeoCmdLength = 0;

	//emulate a stack for jumps
	unsigned int TempGeoScriptSegment_Backup[4] = {0, 0, 0, 0};
	unsigned int TempGeoScriptPos_Backup[4] = {0, 0, 0, 0};
	int TargetSeg=0, TargetPos=0;
	int StackCount = 0;
	while (!(EndOfGeoScript) && ((TempGeoScriptPos + LvlScript_Start) < ROMFilesize)) {
		CurrentGeoCmd = Read16(RAMSegment[TempGeoScriptSegment].Data, TempGeoScriptPos);
		CurrentGeoCmdLength = 0;
		JumpInGeoScript = false;

		switch(CurrentGeoCmd) {
			case 0x0100:
				// end
				EndOfGeoScript = true;
				break;

			case 0x0201:
				// jump
				TargetSeg = RAMSegment[TempGeoScriptSegment].Data[TempGeoScriptPos + 4];
				TargetPos = Read24(RAMSegment[TempGeoScriptSegment].Data, TempGeoScriptPos + 5);

				if(!RAMSegment[TargetSeg].IsSet)
					break;

				//Backup old seg/pos
				TempGeoScriptPos_Backup[StackCount] = TempGeoScriptPos + 8;
				TempGeoScriptSegment_Backup[StackCount] = TempGeoScriptSegment;

				//Set new seg/pos
				TempGeoScriptPos = TargetPos;
				TempGeoScriptSegment = TargetSeg;

				JumpInGeoScript = true;
				StackCount++;
				break;

			case 0x0300:
				// return
				StackCount--;
				TempGeoScriptPos = TempGeoScriptPos_Backup[StackCount];
				TempGeoScriptSegment = TempGeoScriptSegment_Backup[StackCount];
				break;

			case 0x0400:
			case 0x0500:
			case 0x0900:
			case 0x0B00:
			case 0x0C00:
			case 0x0C01:
				CurrentGeoCmdLength = 0x04;
				break;
			case 0x0E00:
			case 0x1600:
			case 0x1800:
			case 0x1900:
			case 0x1D00:
				CurrentGeoCmdLength = 0x08;
				break;
			case 0x0800:
			case 0x0A01:
			case 0x1700:
			case 0x1770:
				CurrentGeoCmdLength = 0x0C;
				break;
			case 0x0F00:
				CurrentGeoCmdLength = 0x14;
				break;

			case 0x1501:
			case 0x1502:
			case 0x1503:
			case 0x1504:
			case 0x1505:
			case 0x1506:
				// Display Lists
				CurrentGeoCmdLength = 0x08;
				unsigned int TempDLSegment = RAMSegment[TempGeoScriptSegment].Data[TempGeoScriptPos + 4];
				unsigned int TempDLOffset = Read24(RAMSegment[TempGeoScriptSegment].Data, TempGeoScriptPos + 5);
				DListOffsets[DLCount] = TempDLOffset + ZMAP_HEADERGAP;
				DLCount++;

				ConvertDList(TempDLSegment, TempDLOffset, false);
				DListHasEnded = false;
				break;
			default:
				CurrentGeoCmdLength = 0x04;
				break;
		}

		if(!JumpInGeoScript) TempGeoScriptPos += CurrentGeoCmdLength;
	}

	dmsg("\n");

	return 0;
}

int InsertSpecialObjects(unsigned int ObjID, unsigned int GeoOffset)
{
	// ugly hack to put "special objects" into levels, so far only the castle tower

	bool ObjFound = false;

	// IF castle grounds AND castle tower
	if((LevelID == 0x0C) && (ObjID == 0x03)) {
		Xrepos = 0; Yrepos = 2867; Zrepos = -3924;
		ObjFound = true;
	}

	// IF level segment AND object found above
	if((GeoOffset & 0x0E000000) && (ObjFound == true)) {
		TempGeoScriptSegment = (GeoOffset & 0xFF000000) >> 24;
		TempGeoScriptPos = (GeoOffset & 0x00FFFFFF);
		TempGeoScriptSegment_Backup = 0x00, TempGeoScriptPos_Backup = 0x00;

		ExecuteGeoScript(true);
	}

	// reset position reposition
	Xrepos = 0; Yrepos = 0; Zrepos = 0;

	return 0;
}
