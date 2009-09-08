/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	map_init.c - map/scene header, actor and DList loader
	------------------------------------------------------------ */

#include "globals.h"

typedef struct _SceneNameDataStruct {
	unsigned long Offset;
	char *Name;
} SceneNameDataStruct;

SceneNameDataStruct SceneNameData[256];

/*	------------------------------------------------------------ */

int Zelda_GetMapHeaderList(int HeaderListPos, int CurrentMap)
{
	bool EndOfList = false;

	HeaderListPos = HeaderListPos / 4;

	MapHeader_List[0] = 0x08;
	MapHeader_TotalCount = 1;

	while (!EndOfList) {
		memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][HeaderListPos], 4);

		Helper_SplitCurrentVals(false);

		if ((Readout_CurrentByte1 == 0x03)) {
			MapHeader_List[MapHeader_TotalCount] = (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			MapHeader_TotalCount++;
		} else if ((Readout_Current1 == 0x00000000)) {
			//ignore
		} else {
			EndOfList = true;
		}

		HeaderListPos++;
	}

	return 0;
}

int Zelda_GetMapHeader(int CurrentHeader, int CurrentMap)
{
	bool EndOfHeader = false;

	unsigned int InHeaderPos = MapHeader_List[CurrentHeader] / 4;

	memset(SystemLogMsg, 0x00, sizeof(SystemLogMsg));
	sprintf(SystemLogMsg, "Map Header #%d (0x%08X):\n", CurrentHeader + 1, InHeaderPos * 4);
	Helper_LogMessage(2, SystemLogMsg);

	while(!EndOfHeader) {
		memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][InHeaderPos], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[CurrentMap][InHeaderPos + 1], 4);

		Helper_SplitCurrentVals(true);

		switch(Readout_CurrentByte1) {
		/* actors */
		case 0x01:
			MapHeader[CurrentMap][CurrentHeader].Actor_Count = Readout_CurrentByte2;
			MapHeader[CurrentMap][CurrentHeader].Actor_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tActors: %d, Actor data offset: 0x%06X\n",
				InHeaderPos * 4,
				MapHeader[CurrentMap][CurrentHeader].Actor_Count, (unsigned int)MapHeader[CurrentMap][CurrentHeader].Actor_DataOffset);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* mesh data */
		case 0x0A:
			MapHeader[CurrentMap][CurrentHeader].MeshDataHeader = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tMesh data header: 0x%06X\n",
				InHeaderPos * 4,
				(unsigned int)MapHeader[CurrentMap][CurrentHeader].MeshDataHeader);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* actor groups */
		case 0x0B:
			MapHeader[CurrentMap][CurrentHeader].Group_Count = Readout_CurrentByte2;
			MapHeader[CurrentMap][CurrentHeader].Group_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tGroups: %d, Group data offset: 0x%06X\n",
				InHeaderPos * 4,
				MapHeader[CurrentMap][CurrentHeader].Group_Count, (unsigned int)MapHeader[CurrentMap][CurrentHeader].Group_DataOffset);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* time-related */
		case 0x10:
			MapHeader[CurrentMap][CurrentHeader].MapTime = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			MapHeader[CurrentMap][CurrentHeader].TimeFlow = Readout_CurrentByte7;
			sprintf(SystemLogMsg, "  0x%08X:\tMap time: 0x%04X, Timeflow: 0x%02X\n",
				InHeaderPos * 4,
				(unsigned int)MapHeader[CurrentMap][CurrentHeader].MapTime, MapHeader[CurrentMap][CurrentHeader].TimeFlow);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* skybox */
		case 0x12:
			MapHeader[CurrentMap][CurrentHeader].Skybox = Readout_CurrentByte5;
			sprintf(SystemLogMsg, "  0x%08X:\tSkybox setting: 0x%02X\n",
				InHeaderPos * 4,
				MapHeader[CurrentMap][CurrentHeader].Skybox);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* end of header */
		case 0x14:
			EndOfHeader = true;
			sprintf(SystemLogMsg, "  0x%08X:\tEnd of header\n", InHeaderPos * 4);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* echo level */
		case 0x16:
			MapHeader[CurrentMap][CurrentHeader].EchoLevel = Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tEcho level: 0x%02X\n",
				InHeaderPos * 4,
				MapHeader[CurrentMap][CurrentHeader].EchoLevel);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* unhandled option */
		default:
			sprintf(SystemLogMsg, "  0x%08X:\t<Unknown header option (%02X%02X%02X%02X %02X%02X%02X%02X)>\n",
				InHeaderPos * 4,
				Readout_CurrentByte1, Readout_CurrentByte2, Readout_CurrentByte3, Readout_CurrentByte4,
				Readout_CurrentByte5, Readout_CurrentByte6, Readout_CurrentByte7, Readout_CurrentByte8);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		}

		InHeaderPos += 2;
	}

	Readout_Current1 = 0x00;
	Readout_Current2 = 0x00;

	Helper_LogMessage(2, "\n");

	return 0;
}

int Zelda_GetSceneHeaderList(int HeaderListPos)
{
	bool EndOfList = false;

	HeaderListPos = HeaderListPos / 4;

	SceneHeader_List[0] = 0x08;
	SceneHeader_TotalCount = 1;

	while (!EndOfList) {
		memcpy(&Readout_Current1, &ZSceneBuffer[HeaderListPos], 4);

		Helper_SplitCurrentVals(false);

		if ((Readout_CurrentByte1 == 0x02)) {
			SceneHeader_List[SceneHeader_TotalCount] = (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			SceneHeader_TotalCount++;
		} else if ((Readout_Current1 == 0x00000000)) {
			//ignore
		} else {
			EndOfList = true;
		}

		HeaderListPos++;
	}

	return 0;
}

int Zelda_GetSceneHeader(int CurrentHeader)
{
	bool EndOfHeader = false;

	int InHeaderPos = SceneHeader_List[CurrentHeader] / 4;

	memset(SystemLogMsg, 0x00, sizeof(SystemLogMsg));
	sprintf(SystemLogMsg, "Scene Header #%d (0x%08X):\n", CurrentHeader + 1, InHeaderPos * 4);
	Helper_LogMessage(2, SystemLogMsg);

	while(!EndOfHeader) {
		memcpy(&Readout_Current1, &ZSceneBuffer[InHeaderPos], 4);
		memcpy(&Readout_Current2, &ZSceneBuffer[InHeaderPos + 1], 4);

		Helper_SplitCurrentVals(true);

		switch(Readout_CurrentByte1) {
		/* scene actors */
		case 0x00:
			SceneHeader[CurrentHeader].ScActor_Count = Readout_CurrentByte2;
			SceneHeader[CurrentHeader].ScActor_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tActors: %d, Actor data offset: 0x%06X\n",
				InHeaderPos * 4,
				SceneHeader[CurrentHeader].ScActor_Count, (unsigned int)SceneHeader[CurrentHeader].ScActor_DataOffset);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* collision */
		case 0x03:
			SceneHeader[CurrentHeader].Col_DataSource = Readout_CurrentByte5;
			SceneHeader[CurrentHeader].Col_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tCollision data offset: 0x%06X\n",
				InHeaderPos * 4,
				(unsigned int)SceneHeader[CurrentHeader].Col_DataOffset);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* maps */
		case 0x04:
			SceneHeader[CurrentHeader].Map_Count = Readout_CurrentByte2;
			SceneHeader[CurrentHeader].Map_ListOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tMaps: %d, Map list offset: 0x%06X\n",
				InHeaderPos * 4,
				SceneHeader[CurrentHeader].Map_Count, (unsigned int)SceneHeader[CurrentHeader].Map_ListOffset);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* doors */
		case 0x0E:
			SceneHeader[CurrentHeader].Door_Count = Readout_CurrentByte2;
			SceneHeader[CurrentHeader].Door_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tDoors: %d, Door data offset: 0x%06X\n",
				InHeaderPos * 4,
				SceneHeader[CurrentHeader].Door_Count, (unsigned int)SceneHeader[CurrentHeader].Door_DataOffset);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* environment */
		case 0x0F:
			SceneHeader[CurrentHeader].EnvSetting_Count = Readout_CurrentByte2;
			SceneHeader[CurrentHeader].EnvSetting_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tEnvironments: %d, environment data offset: 0x%06X\n",
				InHeaderPos * 4,
				SceneHeader[CurrentHeader].EnvSetting_Count, (unsigned int)SceneHeader[CurrentHeader].EnvSetting_DataOffset);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* end of header */
		case 0x14:
			EndOfHeader = true;
			sprintf(SystemLogMsg, "  0x%08X:\tEnd of header\n", InHeaderPos * 4);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		/* unhandled option */
		default:
			sprintf(SystemLogMsg, "  0x%08X:\t<Unknown header option (%02X%02X%02X%02X %02X%02X%02X%02X)>\n",
				InHeaderPos * 4,
				Readout_CurrentByte1, Readout_CurrentByte2, Readout_CurrentByte3, Readout_CurrentByte4,
				Readout_CurrentByte5, Readout_CurrentByte6, Readout_CurrentByte7, Readout_CurrentByte8);
			Helper_LogMessage(2, SystemLogMsg);
			break;
		}

		InHeaderPos += 2;
	}

	Readout_Current1 = 0x00;
	Readout_Current2 = 0x00;

	Helper_LogMessage(2, "\n");

	return 0;
}

int Zelda_GetMapActors(int CurrentHeader, int CurrentMap)
{
	if(!(MapHeader[CurrentMap][CurrentHeader].Actor_Count) == 0) {
		unsigned long DataOffset = (Map_Start[CurrentMap] + MapHeader[CurrentMap][CurrentHeader].Actor_DataOffset);
		int DataLength = MapHeader[CurrentMap][CurrentHeader].Actor_Count * 0x10;

		memcpy(&Actors[CurrentMap], &ROMBuffer[DataOffset / 4], DataLength);
		swab((const void*)Actors[CurrentMap], (void*)Actors[CurrentMap], sizeof(Actors[CurrentMap]));
	}
/*
	int InActorDataPos = MapHeader[CurrentMap][CurrentHeader].Actor_DataOffset / 4;

	ActorInfo_CurrentCount[CurrentMap] = 0;
	ActorInfo_Selected = 0;

	if(!(MapHeader[CurrentMap][CurrentHeader].Actor_Count) == 0) {
		while (!(ActorInfo_CurrentCount[CurrentMap] == MapHeader[CurrentMap][CurrentHeader].Actor_Count)) {
			memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][InActorDataPos], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[CurrentMap][InActorDataPos + 1], 4);

			Helper_SplitCurrentVals(true);

			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].Number = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].X_Position = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].Y_Position = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].Z_Position = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][InActorDataPos + 2], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[CurrentMap][InActorDataPos + 3], 4);

			Helper_SplitCurrentVals(true);

			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].X_Rotation = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].Y_Rotation = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].Z_Rotation = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].Variable = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			ActorInfo_CurrentCount[CurrentMap]++;
			InActorDataPos += 4;
		}
	}
*/
	return 0;
}

int Zelda_GetSceneActors(int CurrentHeader)
{
	if(!(SceneHeader[CurrentHeader].ScActor_Count) == 0) {
		unsigned long DataOffset = (Scene_Start + SceneHeader[CurrentHeader].ScActor_DataOffset);
		int DataLength = SceneHeader[CurrentHeader].ScActor_Count * 0x10;

		memcpy(&ScActors, &ROMBuffer[DataOffset / 4], DataLength);
		swab((const void*)ScActors, (void*)ScActors, sizeof(ScActors));
	}

/*	int InScActorDataPos = SceneHeader[CurrentHeader].ScActor_DataOffset / 4;

	ScActorInfo_CurrentCount = 0;
	ScActorInfo_Selected = 0;

	if(!(SceneHeader[CurrentHeader].ScActor_Count) == 0) {
		while (!(ScActorInfo_CurrentCount == SceneHeader[CurrentHeader].ScActor_Count)) {
			memcpy(&Readout_Current1, &ZSceneBuffer[InScActorDataPos], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[InScActorDataPos + 1], 4);

			Helper_SplitCurrentVals(true);

			ScActors[ScActorInfo_CurrentCount].Number = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			ScActors[ScActorInfo_CurrentCount].X_Position = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			ScActors[ScActorInfo_CurrentCount].Y_Position = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			ScActors[ScActorInfo_CurrentCount].Z_Position = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			memcpy(&Readout_Current1, &ZSceneBuffer[InScActorDataPos + 2], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[InScActorDataPos + 3], 4);

			Helper_SplitCurrentVals(true);

			ScActors[ScActorInfo_CurrentCount].X_Rotation = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			ScActors[ScActorInfo_CurrentCount].Y_Rotation = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			ScActors[ScActorInfo_CurrentCount].Z_Rotation = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			ScActors[ScActorInfo_CurrentCount].Variable = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			ScActorInfo_CurrentCount++;
			InScActorDataPos += 4;
		}
	}
*/
	return 0;
}

int Zelda_GetDoorData(int CurrentHeader)
{
	if(!(SceneHeader[CurrentHeader].Door_Count) == 0) {
		unsigned long DataOffset = (Scene_Start + SceneHeader[CurrentHeader].Door_DataOffset);
		int DataLength = SceneHeader[CurrentHeader].Door_Count * 0x10;

		memcpy(&Doors, &ROMBuffer[DataOffset / 4], DataLength);
		swab((const void*)Doors, (void*)Doors, sizeof(Doors));
	}

/*	char temp[256];
	sprintf(temp, "offset %08x, len %02x\n\nactor %04x",
		DataOffset, DataLength,
		Doors[0].Number);
	MessageBox(hwnd, temp, "", 0);
*/
/*	int InDoorDataPos = SceneHeader[CurrentHeader].Door_DataOffset / 4;

	DoorInfo_CurrentCount = 0;
	DoorInfo_Selected = 0;

	if(!(SceneHeader[CurrentHeader].Door_Count) == 0) {
		while (!(DoorInfo_CurrentCount == SceneHeader[CurrentHeader].Door_Count)) {
			memcpy(&Readout_Current1, &ZSceneBuffer[InDoorDataPos], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[InDoorDataPos + 1], 4);

			Helper_SplitCurrentVals(true);

			Doors[DoorInfo_CurrentCount].Unknown1 = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			Doors[DoorInfo_CurrentCount].Unknown2 = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Doors[DoorInfo_CurrentCount].Number = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			Doors[DoorInfo_CurrentCount].X_Position = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			memcpy(&Readout_Current1, &ZSceneBuffer[InDoorDataPos + 2], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[InDoorDataPos + 3], 4);

			Helper_SplitCurrentVals(true);

			Doors[DoorInfo_CurrentCount].Y_Position = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			Doors[DoorInfo_CurrentCount].Z_Position = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Doors[DoorInfo_CurrentCount].Y_Rotation = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			Doors[DoorInfo_CurrentCount].Variable = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
*//*
			sprintf(SystemLogMsg, "unk1 %d, unk2 %d, number %d, xpos %d, ypos %d, zpos %d, yrot %d, var %d\n",
				Doors[DoorInfo_CurrentCount].Unknown1, Doors[DoorInfo_CurrentCount].Unknown2,
				Doors[DoorInfo_CurrentCount].Number, Doors[DoorInfo_CurrentCount].X_Position,
				Doors[DoorInfo_CurrentCount].Y_Position, Doors[DoorInfo_CurrentCount].Z_Position,
				Doors[DoorInfo_CurrentCount].Y_Rotation, Doors[DoorInfo_CurrentCount].Variable);
			Helper_LogMessage(2, SystemLogMsg);
*/
/*			DoorInfo_CurrentCount++;
			InDoorDataPos += 4;
		}
	}
*/
	return 0;
}

int Zelda_GetMapDisplayLists(unsigned long Fsize, int CurrentMap)
{
	DListInfo_CurrentCount[CurrentMap] = 0;
	unsigned long TempOffset = 0;

	/* EXPERIMENTAL dlist detection by mesh header data - watch out for missing geometry! (there shouldn't be any tho) */

	/* MESH HEADER LAYOUT:
		[aa bb 0000 cccccccc dddddddd]
		aa = map type
		bb = number of dlists
		cccccccc = pointer to offset w/ dlist pointers
		dddddddd = [unknown pointer]

		LAYOUT DIFFERS WITH TYPE 01 MAPS (prerendered jpeg background stuff)
	*/
	TempOffset = (MapHeader[CurrentMap][MapHeader_Current].MeshDataHeader / 4);
	memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][TempOffset], 4);
	Helper_SplitCurrentVals(false);

	unsigned int MeshHeaderSetup = Readout_CurrentByte1;
	unsigned int MeshAmount = Readout_CurrentByte2;
	unsigned int TotalMeshCount = 0;

	switch(MeshHeaderSetup) {
	case 0x00: {
		/* simple maps, ex. besitu ??? */

		/* LAYOUT:
			[aaaaaaaa] [aaaaaaaa] [aaaaaaaa] ...
			aaaaaaaa = pointer to dlist
		*/
		TempOffset = (MapHeader[CurrentMap][MapHeader_Current].MeshDataHeader / 4) + 3;
		unsigned long MeshScanPosition = TempOffset;

		while(TotalMeshCount < MeshAmount + 1) {
			memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][MeshScanPosition], 4);
			Helper_SplitCurrentVals(false);

			if(Readout_CurrentByte1 == 0x03) {
				DLists[CurrentMap][DListInfo_CurrentCount[CurrentMap]] = (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
				DListInfo_CurrentCount[CurrentMap]++;
				DListInfo_TotalCount++;
			}

			MeshScanPosition++;

			TotalMeshCount++;
		}
		break;
		}
	case 0x01: {
		/* static prerendered maps, ex. market_alley ??? */

		/* LAYOUT:
			[aaaaaaaa] [aaaaaaaa] [aaaaaaaa] ...
			aaaaaaaa = pointer to dlist
		*/
		TempOffset = (MapHeader[CurrentMap][MapHeader_Current].MeshDataHeader / 4) + 1;
		memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][TempOffset], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[CurrentMap][TempOffset + 1], 4);
		Helper_SplitCurrentVals(true);

		if((Readout_CurrentByte1 == 0x03)) {
			TempOffset = ((Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4) / 4;

			memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][TempOffset], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[CurrentMap][TempOffset + 1], 4);
			Helper_SplitCurrentVals(true);

			if((Readout_CurrentByte1 == 0x03)) {
				TempOffset = Readout_CurrentByte2 << 16;
				TempOffset = TempOffset + (Readout_CurrentByte3 << 8);
				TempOffset = TempOffset + Readout_CurrentByte4;

				DLists[CurrentMap][DListInfo_CurrentCount[CurrentMap]] = TempOffset;

				DListInfo_CurrentCount[CurrentMap]++;
				DListInfo_TotalCount++;
			}
		}
		break;
		}
	case 0x02: {
		/* complicated maps, ex. spot00 ??? */

		/* LAYOUT:
			[aaaaaaaa bbbbbbbb cccccccc dddddddd] [aaaaaaaa bbbbbbbb cccccccc dddddddd] ...
			aaaaaaaa = unknown data
			bbbbbbbb = unknown data
			cccccccc = pointer to "primary" dlist (all 0 if not used)
			dddddddd = pointer to "secondary" dlist (all 0 if not used)
		*/
		TempOffset = (MapHeader[CurrentMap][MapHeader_Current].MeshDataHeader / 4) + 3;
		unsigned long MeshScanPosition = TempOffset;

		unsigned long Unknown1 = 0;
		unsigned long Unknown2 = 0;
		unsigned long DListStart1 = 0;
		unsigned long DListStart2 = 0;

		unsigned long DLSecondary[1024];
		unsigned int DListInfo_SecondaryCount = 0;

		while(TotalMeshCount < MeshAmount) {
			memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][MeshScanPosition], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[CurrentMap][MeshScanPosition + 1], 4);
			Helper_SplitCurrentVals(true);

			Unknown1 = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Unknown2 = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			MeshScanPosition += 2;

			memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][MeshScanPosition], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[CurrentMap][MeshScanPosition + 1], 4);
			Helper_SplitCurrentVals(true);

			if(Readout_CurrentByte1 == 0x03) {
				/* primary dlists - terrain geometry, etc */
				DListStart1 = (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
				DLists[CurrentMap][DListInfo_CurrentCount[CurrentMap]] = DListStart1;
				DListInfo_CurrentCount[CurrentMap]++;
				DListInfo_TotalCount++;
			}

			if(Readout_CurrentByte5 == 0x03) {
				/* secondary dlists - water, pathways, etc */
				DListStart2 = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
				DLSecondary[DListInfo_SecondaryCount] = DListStart2;
				DListInfo_SecondaryCount++;
				DListInfo_TotalCount++;
			}

			MeshScanPosition += 2;

			TotalMeshCount++;
		}

		int i = 0;
		for(i = 0; i < DListInfo_SecondaryCount; i++) {
			/* add secondary dlists to main dlist array */
			DLists[CurrentMap][DListInfo_CurrentCount[CurrentMap]] = DLSecondary[i];
			DListInfo_CurrentCount[CurrentMap]++;
		}

		break;
		}
	default: {
		MessageBox(hwnd, "Unknown Mesh Header setup!", "Error", MB_OK | MB_ICONEXCLAMATION);
		break;
		}
	}

	/* crap, epic fail! no dlists found!
	   (that shouldn't happen anymore tho) */
	if(DListInfo_CurrentCount[CurrentMap] == 0) {
		MessageBox(hwnd, "No Display Lists found!", "Error", MB_OK | MB_ICONEXCLAMATION);
	}

	return 0;
}

int Zelda_GetMapCollision(int CurrentHeader)
{
	unsigned int TotalColPoly = 0;
	unsigned int TotalColVert = 0;

	unsigned long TempOffset = (SceneHeader[CurrentHeader].Col_DataOffset / 4) + 3;
	memcpy(&Readout_Current1, &ZSceneBuffer[TempOffset], 4);
	Helper_SplitCurrentVals(false);

	unsigned long ColVertAmount = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;

	TempOffset = (SceneHeader[CurrentHeader].Col_DataOffset / 4) + 4;
	memcpy(&Readout_Current1, &ZSceneBuffer[TempOffset], 4);
	Helper_SplitCurrentVals(false);

	if((Readout_CurrentByte1 == 0x02)) {
		TempOffset = Readout_CurrentByte2 << 16;
		TempOffset = TempOffset + (Readout_CurrentByte3 << 8);
		TempOffset = TempOffset + Readout_CurrentByte4;

		unsigned long ColVertScanPosition = TempOffset / 4;

		while (TotalColVert < ColVertAmount) {
			/* vertex 1 - xyz */
			/* vertex 2 - x.. */
			memcpy(&Readout_Current1, &ZSceneBuffer[ColVertScanPosition], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[ColVertScanPosition + 1], 4);
			Helper_SplitCurrentVals(true);

			CollisionVertex[TotalColVert].X = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			CollisionVertex[TotalColVert].Y = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			CollisionVertex[TotalColVert].Z = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			TotalColVert++;
			CollisionVertex[TotalColVert].X = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			ColVertScanPosition += 2;

			/* vertex 2 - .yz */
			/* vertex 3 - xy. */
			memcpy(&Readout_Current1, &ZSceneBuffer[ColVertScanPosition], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[ColVertScanPosition + 1], 4);
			Helper_SplitCurrentVals(true);

			CollisionVertex[TotalColVert].Y = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			CollisionVertex[TotalColVert].Z = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			TotalColVert++;
			CollisionVertex[TotalColVert].X = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			CollisionVertex[TotalColVert].Y = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			ColVertScanPosition += 2;

			/* vertex 3 - ..z */
			/* vertex 4 - xyz */
			memcpy(&Readout_Current1, &ZSceneBuffer[ColVertScanPosition], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[ColVertScanPosition + 1], 4);
			Helper_SplitCurrentVals(true);

			CollisionVertex[TotalColVert].Z = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			TotalColVert++;
			CollisionVertex[TotalColVert].X = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			CollisionVertex[TotalColVert].Y = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			CollisionVertex[TotalColVert].Z = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			TotalColVert++;

			ColVertScanPosition += 2;

			if(Renderer_EnableWavefrontDump) {
				sprintf(WavefrontObjColMsg, "v %4.2f %4.2f %4.2f\n",
					(float)CollisionVertex[TotalColVert - 4].X / 32,
					(float)CollisionVertex[TotalColVert - 4].Y / 32,
					(float)CollisionVertex[TotalColVert - 4].Z / 32);
				fprintf(FileWavefrontObjCol, WavefrontObjColMsg);
				WavefrontObjVertCount++;
				sprintf(WavefrontObjColMsg, "v %4.2f %4.2f %4.2f\n",
					(float)CollisionVertex[TotalColVert - 3].X / 32,
					(float)CollisionVertex[TotalColVert - 3].Y / 32,
					(float)CollisionVertex[TotalColVert - 3].Z / 32);
				fprintf(FileWavefrontObjCol, WavefrontObjColMsg);
				WavefrontObjVertCount++;
				sprintf(WavefrontObjColMsg, "v %4.2f %4.2f %4.2f\n",
					(float)CollisionVertex[TotalColVert - 2].X / 32,
					(float)CollisionVertex[TotalColVert - 2].Y / 32,
					(float)CollisionVertex[TotalColVert - 2].Z / 32);
				fprintf(FileWavefrontObjCol, WavefrontObjColMsg);
				WavefrontObjVertCount++;
				sprintf(WavefrontObjColMsg, "v %4.2f %4.2f %4.2f\n",
					(float)CollisionVertex[TotalColVert - 1].X / 32,
					(float)CollisionVertex[TotalColVert - 1].Y / 32,
					(float)CollisionVertex[TotalColVert - 1].Z / 32);
				fprintf(FileWavefrontObjCol, WavefrontObjColMsg);
				WavefrontObjVertCount++;
			}
		}
	}

/*	Helper_LogMessage(2, "COLLISION RENDERING:\n");
	int i = 0;
	for(i = 0; i < TotalColVert; i++) {
		sprintf(SystemLogMsg, " - Vertex #%5d: X %5d, Y %5d, Z %5d\n", i, CollisionVertex[i].X, CollisionVertex[i].Y, CollisionVertex[i].Z);
		Helper_LogMessage(2, SystemLogMsg);
	}
*/
	TempOffset = (SceneHeader[CurrentHeader].Col_DataOffset / 4) + 5;
	memcpy(&Readout_Current1, &ZSceneBuffer[TempOffset], 4);
	Helper_SplitCurrentVals(false);

	unsigned long ColPolyAmount = ((Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2);

	TempOffset = (SceneHeader[CurrentHeader].Col_DataOffset / 4) + 6;
	memcpy(&Readout_Current1, &ZSceneBuffer[TempOffset], 4);
	Helper_SplitCurrentVals(false);

//	Helper_LogMessage(2, "\n");

	if((Readout_CurrentByte1 == 0x02)) {
		TempOffset = Readout_CurrentByte2 << 16;
		TempOffset = TempOffset + (Readout_CurrentByte3 << 8);
		TempOffset = TempOffset + Readout_CurrentByte4;

		unsigned long ColPolyScanPosition = TempOffset / 4;

		if(Renderer_GLDisplayList_Current != 0) {
			glNewList(Renderer_GLDisplayList_Current, GL_COMPILE);
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(-2.0f, -2.0f);

				glEnable(GL_CULL_FACE);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_LIGHTING);

				while (TotalColPoly < ColPolyAmount) {
					memcpy(&Readout_Current1, &ZSceneBuffer[ColPolyScanPosition], 4);
					memcpy(&Readout_Current2, &ZSceneBuffer[ColPolyScanPosition + 1], 4);
					Helper_SplitCurrentVals(true);

					unsigned int ColType = ((Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2);

					unsigned int ColVertex1 = ((Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4);
					ColVertex1 = (ColVertex1 & 0x0FFF);

					unsigned int ColVertex2 = ((Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6);
					ColVertex2 = (ColVertex2 & 0x0FFF);

					unsigned int ColVertex3 = ((Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8);
					ColVertex3 = (ColVertex3 & 0x0FFF);

					if((ColVertex1 >= TotalColVert) || (ColVertex2 >= TotalColVert) || (ColVertex2 >= TotalColVert)) {
						sprintf(SystemLogMsg, " - WARNING! Requested vertex (%d, %d, %d) > vertex count %d!\n\n", ColVertex1, ColVertex2, ColVertex3, TotalColVert);
						Helper_LogMessage(2, SystemLogMsg);
					} else {
						switch(ColType) {
							case 0: glColor4f(0.0f, 1.0f, 0.0f, Renderer_CollisionAlpha); break;
							case 1: glColor4f(1.0f, 0.0f, 0.0f, Renderer_CollisionAlpha); break;
							case 2: glColor4f(0.0f, 0.0f, 1.0f, Renderer_CollisionAlpha); break;
							case 3: glColor4f(1.0f, 1.0f, 0.0f, Renderer_CollisionAlpha); break;
							case 4: glColor4f(0.0f, 1.0f, 1.0f, Renderer_CollisionAlpha); break;
							case 5: glColor4f(1.0f, 0.0f, 1.0f, Renderer_CollisionAlpha); break;
							case 6: glColor4f(1.0f, 1.0f, 1.0f, Renderer_CollisionAlpha); break;
							default: glColor4f(0.0f, 1.0f, 0.0f, Renderer_CollisionAlpha); break;
						}

						glBegin(GL_TRIANGLES);
							glVertex3d(CollisionVertex[ColVertex1].X, CollisionVertex[ColVertex1].Y, CollisionVertex[ColVertex1].Z);
							glVertex3d(CollisionVertex[ColVertex2].X, CollisionVertex[ColVertex2].Y, CollisionVertex[ColVertex2].Z);
							glVertex3d(CollisionVertex[ColVertex3].X, CollisionVertex[ColVertex3].Y, CollisionVertex[ColVertex3].Z);
						glEnd();

						if(Renderer_EnableWavefrontDump) {
							unsigned int Vert1 = (ColVertex1) + 1 + WavefrontObjColVertCount_Previous;
							unsigned int Vert2 = (ColVertex2) + 1 + WavefrontObjColVertCount_Previous;
							unsigned int Vert3 = (ColVertex3) + 1 + WavefrontObjColVertCount_Previous;
							sprintf(WavefrontObjColMsg, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
								Vert1, Vert1, Vert1,
								Vert2, Vert2, Vert2,
								Vert3, Vert3, Vert3);
							fprintf(FileWavefrontObjCol, WavefrontObjColMsg);
						}

/*						sprintf(SystemLogMsg, " - Polygon %5d:\n  - Collision type: 0x%04X\n  - Vertex 1 (#%5d), X %5d, Y %5d, Z %5d\n  - Vertex 2 (#%5d), X %5d, Y %5d, Z %5d\n  - Vertex 3 (#%5d), X %5d, Y %5d, Z %5d\n\n",
							TotalColPoly,
							ColType,
							ColVertex1, CollisionVertex[ColVertex1].X, CollisionVertex[ColVertex1].Y, CollisionVertex[ColVertex1].Z,
							ColVertex2, CollisionVertex[ColVertex2].X, CollisionVertex[ColVertex2].Y, CollisionVertex[ColVertex2].Z,
							ColVertex3, CollisionVertex[ColVertex3].X, CollisionVertex[ColVertex3].Y, CollisionVertex[ColVertex3].Z);
						Helper_LogMessage(2, SystemLogMsg);
	*/				}

					TotalColPoly++;
					ColPolyScanPosition += 4;

					Renderer_GLDisplayList_Total++;
				}

				glEnable(GL_LIGHTING);
				glEnable(GL_TEXTURE_2D);
				glDisable(GL_CULL_FACE);

				glDisable(GL_POLYGON_OFFSET_FILL);
			glEndList();
		}
	}

	return 0;
}

int Zelda_GetEnvironmentSettings(int CurrentHeader)
{
	/* PROBABLY NOT POSSIBLE SINCE SWAB FUNCTION SWAPS BYTES REGARDLESS OF TYPE (CHAR, SHORT, LONG)! */
/*	unsigned long DataOffset = (Scene_Start + SceneHeader[CurrentHeader].EnvSetting_DataOffset);
	int DataLength = SceneHeader[CurrentHeader].EnvSetting_Count * 0x16;

	memcpy(&EnvSetting, &ROMBuffer[DataOffset / 4], DataLength);
	swab((const void*)EnvSetting, (void*)EnvSetting, sizeof(EnvSetting));

//	EnvSetting[0].FogDistance = EndianSwap(EnvSetting[0].FogDistance, sizeof(EnvSetting[0].FogDistance));
*/
	int InEnvDataPos = SceneHeader[CurrentHeader].EnvSetting_DataOffset / 4;

	CurrentEnvSetting = 0;
	int EnvSetting_CurrentCount = 0;

	if(SceneHeader[CurrentHeader].EnvSetting_Count > 1) CurrentEnvSetting = 1;		/* set to daylight env (outside, dunno inside) */

	if(!(SceneHeader[CurrentHeader].EnvSetting_Count) == 0) {
		while (EnvSetting_CurrentCount <= SceneHeader[CurrentHeader].EnvSetting_Count) {
			memcpy(&Readout_Current1, &ZSceneBuffer[InEnvDataPos], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[InEnvDataPos + 1], 4);
			Helper_SplitCurrentVals(true);
			InEnvDataPos += 2;

			EnvSetting[EnvSetting_CurrentCount].Color1.R = Readout_CurrentByte1;
			EnvSetting[EnvSetting_CurrentCount].Color1.G = Readout_CurrentByte2;
			EnvSetting[EnvSetting_CurrentCount].Color1.B = Readout_CurrentByte3;
			EnvSetting[EnvSetting_CurrentCount].Color2.R = Readout_CurrentByte4;
			EnvSetting[EnvSetting_CurrentCount].Color2.G = Readout_CurrentByte5;
			EnvSetting[EnvSetting_CurrentCount].Color2.B = Readout_CurrentByte6;
			EnvSetting[EnvSetting_CurrentCount].Color3.R = Readout_CurrentByte7;
			EnvSetting[EnvSetting_CurrentCount].Color3.G = Readout_CurrentByte8;

			memcpy(&Readout_Current1, &ZSceneBuffer[InEnvDataPos], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[InEnvDataPos + 1], 4);
			Helper_SplitCurrentVals(true);
			InEnvDataPos += 2;

			EnvSetting[EnvSetting_CurrentCount].Color3.B = Readout_CurrentByte1;
			EnvSetting[EnvSetting_CurrentCount].Color4.R = Readout_CurrentByte2;
			EnvSetting[EnvSetting_CurrentCount].Color4.G = Readout_CurrentByte3;
			EnvSetting[EnvSetting_CurrentCount].Color4.B = Readout_CurrentByte4;
			EnvSetting[EnvSetting_CurrentCount].Color5.R = Readout_CurrentByte5;
			EnvSetting[EnvSetting_CurrentCount].Color5.G = Readout_CurrentByte6;
			EnvSetting[EnvSetting_CurrentCount].Color5.B = Readout_CurrentByte7;
			EnvSetting[EnvSetting_CurrentCount].FogColor.R = Readout_CurrentByte8;

			memcpy(&Readout_Current1, &ZSceneBuffer[InEnvDataPos], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[InEnvDataPos + 1], 4);
			Helper_SplitCurrentVals(true);
			InEnvDataPos += 2;

			EnvSetting[EnvSetting_CurrentCount].FogColor.G = Readout_CurrentByte1;
			EnvSetting[EnvSetting_CurrentCount].FogColor.B = Readout_CurrentByte2;
			EnvSetting[EnvSetting_CurrentCount].FogDistance = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			EnvSetting[EnvSetting_CurrentCount].DrawDistance = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			EnvSetting_CurrentCount++;
			EnvSetting[EnvSetting_CurrentCount].Color1.R = Readout_CurrentByte7;
			EnvSetting[EnvSetting_CurrentCount].Color1.G = Readout_CurrentByte8;

			memcpy(&Readout_Current1, &ZSceneBuffer[InEnvDataPos], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[InEnvDataPos + 1], 4);
			Helper_SplitCurrentVals(true);
			InEnvDataPos += 2;

			EnvSetting[EnvSetting_CurrentCount].Color1.B = Readout_CurrentByte1;
			EnvSetting[EnvSetting_CurrentCount].Color2.R = Readout_CurrentByte2;
			EnvSetting[EnvSetting_CurrentCount].Color2.G = Readout_CurrentByte3;
			EnvSetting[EnvSetting_CurrentCount].Color2.B = Readout_CurrentByte4;
			EnvSetting[EnvSetting_CurrentCount].Color3.R = Readout_CurrentByte5;
			EnvSetting[EnvSetting_CurrentCount].Color3.G = Readout_CurrentByte6;
			EnvSetting[EnvSetting_CurrentCount].Color3.B = Readout_CurrentByte7;
			EnvSetting[EnvSetting_CurrentCount].Color4.R = Readout_CurrentByte8;

			memcpy(&Readout_Current1, &ZSceneBuffer[InEnvDataPos], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[InEnvDataPos + 1], 4);
			Helper_SplitCurrentVals(true);
			InEnvDataPos += 2;

			EnvSetting[EnvSetting_CurrentCount].Color4.G = Readout_CurrentByte1;
			EnvSetting[EnvSetting_CurrentCount].Color4.B = Readout_CurrentByte2;
			EnvSetting[EnvSetting_CurrentCount].Color5.R = Readout_CurrentByte3;
			EnvSetting[EnvSetting_CurrentCount].Color5.G = Readout_CurrentByte4;
			EnvSetting[EnvSetting_CurrentCount].Color5.B = Readout_CurrentByte5;
			EnvSetting[EnvSetting_CurrentCount].FogColor.R = Readout_CurrentByte6;
			EnvSetting[EnvSetting_CurrentCount].FogColor.G = Readout_CurrentByte7;
			EnvSetting[EnvSetting_CurrentCount].FogColor.B = Readout_CurrentByte8;

			memcpy(&Readout_Current1, &ZSceneBuffer[InEnvDataPos], 4);
			Helper_SplitCurrentVals(false);
			InEnvDataPos++;

			EnvSetting[EnvSetting_CurrentCount].FogDistance = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			EnvSetting[EnvSetting_CurrentCount].DrawDistance = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			EnvSetting_CurrentCount++;
		}

		Zelda_SelectEnvSettings();
/*
		char temp[256];
//		sprintf(temp, "offset %08x, len %02x\n\ncolor1 %02x %02x %02x\ncolor2 %02x %02x %02x\n\nfogdist (raw) %04x\nfogdist (conv) %f",
		sprintf(temp, "color1 %02x %02x %02x\ncolor2 %02x %02x %02x\n\nfogdist (raw) %04x\nfogdist (conv) %f",
//			DataOffset, DataLength,
			EnvSetting[0].Color1.R, EnvSetting[0].Color1.G, EnvSetting[0].Color1.B,
			EnvSetting[0].Color2.R, EnvSetting[0].Color2.G, EnvSetting[0].Color2.B,
			EnvSetting[0].FogDistance,
			FogDistance);
		MessageBox(hwnd, temp, "env", 0);
	*/
	}

	return 0;
}

int Zelda_SelectEnvSettings()
{
	FogColor[0] = (EnvSetting[CurrentEnvSetting].FogColor.R / 255.0f);
	FogColor[1] = (EnvSetting[CurrentEnvSetting].FogColor.G / 255.0f);
	FogColor[2] = (EnvSetting[CurrentEnvSetting].FogColor.B / 255.0f);

	float FogDistance = ((EnvSetting[CurrentEnvSetting].FogDistance & 0x00FF) / 6.0f);	//utterly wrong, but looks okay
	glFogf(GL_FOG_END, FogDistance);

	glFogfv(GL_FOG_COLOR, FogColor);
	glClearColor(FogColor[0], FogColor[1], FogColor[2], FogColor[3]);

	if(GLExtension_VertFragProgram) {
		LightAmbient[0] = (EnvSetting[CurrentEnvSetting].Color1.R / 255.0f);
		LightAmbient[1] = (EnvSetting[CurrentEnvSetting].Color1.G / 255.0f);
		LightAmbient[2] = (EnvSetting[CurrentEnvSetting].Color1.B / 255.0f);

		LightDiffuse[0] = (EnvSetting[CurrentEnvSetting].Color2.R / 255.0f);
		LightDiffuse[1] = (EnvSetting[CurrentEnvSetting].Color2.G / 255.0f);
		LightDiffuse[2] = (EnvSetting[CurrentEnvSetting].Color2.B / 255.0f);

		LightSpecular[0] = (EnvSetting[CurrentEnvSetting].Color3.R / 255.0f);
		LightSpecular[1] = (EnvSetting[CurrentEnvSetting].Color3.G / 255.0f);
		LightSpecular[2] = (EnvSetting[CurrentEnvSetting].Color3.B / 255.0f);

		LightPosition[0] = (EnvSetting[CurrentEnvSetting].Color4.R / 255.0f);
		LightPosition[1] = (EnvSetting[CurrentEnvSetting].Color4.G / 255.0f);
		LightPosition[2] = (EnvSetting[CurrentEnvSetting].Color4.B / 255.0f);

		glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0, LightAmbient[0], LightAmbient[1], LightAmbient[2], LightAmbient[3]);
		glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, 1, LightDiffuse[0], LightDiffuse[1], LightDiffuse[2], LightDiffuse[3]);
		glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, 1, LightSpecular[0], LightSpecular[1], LightSpecular[2], LightSpecular[3]);
		glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, 3, LightPosition[0], LightPosition[1], LightPosition[2], LightPosition[3]);
	}

	return 0;
}

int Zelda_GetSceneName()
{
	memset(Scene_Name, 0x00, sizeof(Scene_Name));

	unsigned int Offset = 0; char Name[256];

	FILE * SceneNames;
	char Temp[256]; int NameCount = 0;
	sprintf(Temp, "%s\\scenes.txt", AppPath);
	SceneNames = fopen(Temp, "rb");

	if(SceneNames) {
		fseek(SceneNames, 0, SEEK_END);
		int End = ftell(SceneNames);
		rewind(SceneNames);

		while(NameCount < ROM_MaxSceneCount) {
			/* get offset */
			if(Helper_FileReadLine(SceneNames, End, Temp) == -1) break;
			sscanf(Temp, "%x", &Offset);
			/* get name */
			if(Helper_FileReadLine(SceneNames, End, Name) == -1) break;

			/* if offset matches currently loaded scene, set scene name and break out */
			if(Offset == Scene_Start) { strcpy(Scene_Name, Name); break; }

			NameCount++;
		}
	}

	/* if scene name isn't equal last name read out (= not found) OR scenes.txt wasn't found, set scene name to display offset */
	if(strcmp(Scene_Name, Name)) sprintf(Scene_Name, "0x%08X", Scene_Start);

	return 0;
}

/*	------------------------------------------------------------ */
