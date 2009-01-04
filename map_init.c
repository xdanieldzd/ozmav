/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	map_init.c - map/scene header, actor and DList loader
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

/* VIEWER_GETMAPHEADERLIST - IF THE LOADED MAP FILE CONTAINS MULTIPLE MAP HEADERS, STORE THE HEADER LIST FOR LATER USE */
int Viewer_GetMapHeaderList(int HeaderListPos)
{
	bool EndOfList = false;

	HeaderListPos = HeaderListPos / 4;

	MapHeader_List[0] = 0x08;
	MapHeader_TotalCount = 1;

	while (!EndOfList) {
		memcpy(&Readout_Current1, &ZMapBuffer[ROM_CurrentMap][HeaderListPos], 4);

		HelperFunc_SplitCurrentVals(false);

		if ((Readout_CurrentByte1 == 0x03)) {
			MapHeader_List[MapHeader_TotalCount] = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
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

/* VIEWER_GETMAPHEADER - READ THE CURRENTLY SELECTED MAP HEADER AND STORE ITS INFORMATION IN THE MAPHEADER STRUCT */
int Viewer_GetMapHeader(int CurrentHeader)
{
	bool EndOfHeader = false;

	unsigned int InHeaderPos = MapHeader_List[CurrentHeader] / 4;

	sprintf(SystemLogMsg, "Map Header #%d (0x%08X):\n", CurrentHeader + 1, InHeaderPos * 4);
	HelperFunc_LogMessage(2, SystemLogMsg);

	while(!EndOfHeader) {
		memcpy(&Readout_Current1, &ZMapBuffer[ROM_CurrentMap][InHeaderPos], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[ROM_CurrentMap][InHeaderPos + 1], 4);

		HelperFunc_SplitCurrentVals(true);

		switch(Readout_CurrentByte1) {
		case 0x01:
			MapHeader[ROM_CurrentMap][CurrentHeader].Actor_Count = Readout_CurrentByte2;
			MapHeader[ROM_CurrentMap][CurrentHeader].Actor_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tActors: %d, Actor data offset: 0x%06X\n",
				InHeaderPos * 4,
				MapHeader[ROM_CurrentMap][CurrentHeader].Actor_Count, (unsigned int)MapHeader[ROM_CurrentMap][CurrentHeader].Actor_DataOffset);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		case 0x0A:
			MapHeader[ROM_CurrentMap][CurrentHeader].MeshDataHeader = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tMesh data header: 0x%06X\n",
				InHeaderPos * 4,
				(unsigned int)MapHeader[ROM_CurrentMap][CurrentHeader].MeshDataHeader);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		case 0x0B:
			MapHeader[ROM_CurrentMap][CurrentHeader].Group_Count = Readout_CurrentByte2;
			MapHeader[ROM_CurrentMap][CurrentHeader].Group_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tGroups: %d, Group data offset: 0x%06X\n",
				InHeaderPos * 4,
				MapHeader[ROM_CurrentMap][CurrentHeader].Group_Count, (unsigned int)MapHeader[ROM_CurrentMap][CurrentHeader].Group_DataOffset);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		case 0x10:
			MapHeader[ROM_CurrentMap][CurrentHeader].MapTime = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			MapHeader[ROM_CurrentMap][CurrentHeader].TimeFlow = Readout_CurrentByte7;
			sprintf(SystemLogMsg, "  0x%08X:\tMap time: 0x%04X, Timeflow: 0x%02X\n",
				InHeaderPos * 4,
				(unsigned int)MapHeader[ROM_CurrentMap][CurrentHeader].MapTime, MapHeader[ROM_CurrentMap][CurrentHeader].TimeFlow);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		case 0x12:
			MapHeader[ROM_CurrentMap][CurrentHeader].Skybox = Readout_CurrentByte5;
			sprintf(SystemLogMsg, "  0x%08X:\tSkybox setting: 0x%02X\n",
				InHeaderPos * 4,
				MapHeader[ROM_CurrentMap][CurrentHeader].Skybox);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		case 0x14:
			EndOfHeader = true;
			sprintf(SystemLogMsg, "  0x%08X:\tEnd of header\n", InHeaderPos * 4);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		case 0x16:
			MapHeader[ROM_CurrentMap][CurrentHeader].EchoLevel = Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tEcho level: 0x%02X\n",
				InHeaderPos * 4,
				MapHeader[ROM_CurrentMap][CurrentHeader].EchoLevel);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		default:
			sprintf(SystemLogMsg, "  0x%08X:\t<Unknown header option (%02X%02X%02X%02X %02X%02X%02X%02X)>\n",
				InHeaderPos * 4,
				Readout_CurrentByte1, Readout_CurrentByte2, Readout_CurrentByte3, Readout_CurrentByte4,
				Readout_CurrentByte5, Readout_CurrentByte6, Readout_CurrentByte7, Readout_CurrentByte8);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		}

		InHeaderPos += 2;
	}

	Readout_Current1 = 0x00;
	Readout_Current2 = 0x00;

	HelperFunc_LogMessage(2, "\n");

	return 0;
}

/* VIEWER_GETSCENEHEADERLIST - IF THE LOADED SCENE FILE CONTAINS MULTIPLE SCENE HEADERS, STORE THE HEADER LIST FOR LATER USE */
int Viewer_GetSceneHeaderList(int HeaderListPos)
{
	bool EndOfList = false;

	HeaderListPos = HeaderListPos / 4;

	SceneHeader_List[0] = 0x08;
	SceneHeader_TotalCount = 1;

	while (!EndOfList) {
		memcpy(&Readout_Current1, &ZSceneBuffer[HeaderListPos], 4);

		HelperFunc_SplitCurrentVals(false);

		if ((Readout_CurrentByte1 == 0x02)) {
			SceneHeader_List[SceneHeader_TotalCount] = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
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

/* VIEWER_GETSCENEHEADER - READ THE CURRENT SCENE HEADER AND STORE ITS INFORMATION IN THE SCENEHEADER STRUCT */
int Viewer_GetSceneHeader(int CurrentHeader)
{
	bool EndOfHeader = false;

	int InHeaderPos = SceneHeader_List[CurrentHeader] / 4;

	sprintf(SystemLogMsg, "Scene Header #%d (0x%08X):\n", CurrentHeader + 1, InHeaderPos * 4);
	HelperFunc_LogMessage(2, SystemLogMsg);

	while(!EndOfHeader) {
		memcpy(&Readout_Current1, &ZSceneBuffer[InHeaderPos], 4);
		memcpy(&Readout_Current2, &ZSceneBuffer[InHeaderPos + 1], 4);

		HelperFunc_SplitCurrentVals(true);

		switch(Readout_CurrentByte1) {
		/* scene actors */
		case 0x00:
			SceneHeader[CurrentHeader].ScActor_Count = Readout_CurrentByte2;
			SceneHeader[CurrentHeader].ScActor_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tActors: %d, Actor data offset: 0x%06X\n",
				InHeaderPos * 4,
				SceneHeader[CurrentHeader].ScActor_Count, (unsigned int)SceneHeader[CurrentHeader].ScActor_DataOffset);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		/* maps */
		case 0x04:
			SceneHeader[CurrentHeader].Map_Count = Readout_CurrentByte2;
			SceneHeader[CurrentHeader].Map_ListOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tMaps: %d, Map list offset: 0x%06X\n",
				InHeaderPos * 4,
				SceneHeader[CurrentHeader].Map_Count, (unsigned int)SceneHeader[CurrentHeader].Map_ListOffset);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		/* end of header */
		case 0x14:
			EndOfHeader = true;
			sprintf(SystemLogMsg, "  0x%08X:\tEnd of header\n", InHeaderPos * 4);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		/* unhandled option */
		default:
			sprintf(SystemLogMsg, "  0x%08X:\t<Unknown header option (%02X%02X%02X%02X %02X%02X%02X%02X)>\n",
				InHeaderPos * 4,
				Readout_CurrentByte1, Readout_CurrentByte2, Readout_CurrentByte3, Readout_CurrentByte4,
				Readout_CurrentByte5, Readout_CurrentByte6, Readout_CurrentByte7, Readout_CurrentByte8);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		}

		InHeaderPos += 2;
	}

	Readout_Current1 = 0x00;
	Readout_Current2 = 0x00;

	HelperFunc_LogMessage(2, "\n");

	return 0;
}

/* VIEWER_GETMAPACTORS - READ THE MAP ACTOR DATA FROM THE OFFSET SPECIFIED INSIDE THE CURRENT MAP HEADER */
int Viewer_GetMapActors(int CurrentHeader)
{
	int InActorDataPos = MapHeader[ROM_CurrentMap][CurrentHeader].Actor_DataOffset / 4;

	ActorInfo_CurrentCount[ROM_CurrentMap] = 0;
	ActorInfo_Selected = 0;

	if(!(MapHeader[ROM_CurrentMap][CurrentHeader].Actor_Count) == 0) {
		while (!(ActorInfo_CurrentCount[ROM_CurrentMap] == MapHeader[ROM_CurrentMap][CurrentHeader].Actor_Count)) {
			memcpy(&Readout_Current1, &ZMapBuffer[ROM_CurrentMap][InActorDataPos], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[ROM_CurrentMap][InActorDataPos + 1], 4);

			HelperFunc_SplitCurrentVals(true);

			Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Number = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].X_Position = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Y_Position = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Z_Position = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			memcpy(&Readout_Current1, &ZMapBuffer[ROM_CurrentMap][InActorDataPos + 2], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[ROM_CurrentMap][InActorDataPos + 3], 4);

			HelperFunc_SplitCurrentVals(true);

			Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].X_Rotation = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Y_Rotation = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Z_Rotation = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			Actors[ROM_CurrentMap][ActorInfo_CurrentCount[ROM_CurrentMap]].Variable = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			ActorInfo_CurrentCount[ROM_CurrentMap]++;
			InActorDataPos += 4;
		}
	}

	return 0;
}

/* VIEWER_GETSCENEACTORS - READ THE SCENE ACTOR DATA FROM THE OFFSET SPECIFIED INSIDE THE CURRENT SCENE HEADER */
int Viewer_GetSceneActors(int CurrentHeader)
{
	int InScActorDataPos = SceneHeader[CurrentHeader].ScActor_DataOffset / 4;

	ScActorInfo_CurrentCount = 0;
	ScActorInfo_Selected = 0;

	if(!(SceneHeader[CurrentHeader].ScActor_Count) == 0) {
		while (!(ScActorInfo_CurrentCount == SceneHeader[CurrentHeader].ScActor_Count)) {
			memcpy(&Readout_Current1, &ZSceneBuffer[InScActorDataPos], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[InScActorDataPos + 1], 4);

			HelperFunc_SplitCurrentVals(true);

			ScActors[ScActorInfo_CurrentCount].Number = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			ScActors[ScActorInfo_CurrentCount].X_Position = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			ScActors[ScActorInfo_CurrentCount].Y_Position = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			ScActors[ScActorInfo_CurrentCount].Z_Position = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			memcpy(&Readout_Current1, &ZSceneBuffer[InScActorDataPos + 2], 4);
			memcpy(&Readout_Current2, &ZSceneBuffer[InScActorDataPos + 3], 4);

			HelperFunc_SplitCurrentVals(true);

			ScActors[ScActorInfo_CurrentCount].X_Rotation = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			ScActors[ScActorInfo_CurrentCount].Y_Rotation = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			ScActors[ScActorInfo_CurrentCount].Z_Rotation = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			ScActors[ScActorInfo_CurrentCount].Variable = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			ScActorInfo_CurrentCount++;
			InScActorDataPos += 4;
		}
	}

	return 0;
}

/* Viewer_GetMapDisplayLists - SCAN THE CURRENT MAP FOR F3DZEX DISPLAY LISTS AND STORE THEIR OFFSETS IN THE DLISTS STRUCT */
int Viewer_GetMapDisplayLists(unsigned long Fsize)
{
	unsigned int DListScanPosition = 0;
	DListInfo_CurrentCount[ROM_CurrentMap] = 0;
	unsigned long TempOffset = 0;

	/* the old method: scan the map for 0xDE commands and take their parameters as dlist offsets */
	while (DListScanPosition < Fsize / 4) {
		memcpy(&Readout_Current1, &ZMapBuffer[ROM_CurrentMap][DListScanPosition], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[ROM_CurrentMap][DListScanPosition + 1], 4);

		HelperFunc_SplitCurrentVals(true);

		if ((Readout_CurrentByte1 == F3DEX2_DL) && (Readout_CurrentByte2 == 0x00)) {
			if((Readout_CurrentByte3 == 0x00) && (Readout_CurrentByte4 == 0x00)) {
				if((Readout_CurrentByte5 == 0x03)) {
					TempOffset = Readout_CurrentByte6 << 16;
					TempOffset = TempOffset + (Readout_CurrentByte7 << 8);
					TempOffset = TempOffset + Readout_CurrentByte8;

					//sprintf(ErrorMsg, "MAP %d: dlist %d @ %08X", ROM_CurrentMap, DListInfo_CurrentCount[ROM_CurrentMap] + 1, TempOffset);
					//MessageBox(hwnd, ErrorMsg, "", 0);

					DLists[ROM_CurrentMap][DListInfo_CurrentCount[ROM_CurrentMap]] = TempOffset;

					DListInfo_CurrentCount[ROM_CurrentMap]++;
					DListInfo_TotalCount++;
				}
			}
		}
		DListScanPosition += 2;
	}

	/* if that fails: EXPERIMENTAL dlist detection by mesh header data when no dlists have been found (sutaru, testroom) */
	if(DListInfo_CurrentCount[ROM_CurrentMap] == 0) {
		TempOffset = (MapHeader[ROM_CurrentMap][MapHeader_Current].MeshDataHeader / 4) + 3;
		memcpy(&Readout_Current1, &ZMapBuffer[ROM_CurrentMap][TempOffset], 4);
		HelperFunc_SplitCurrentVals(false);

		if((Readout_CurrentByte1 == 0x03)) {
			TempOffset = Readout_CurrentByte2 << 16;
			TempOffset = TempOffset + (Readout_CurrentByte3 << 8);
			TempOffset = TempOffset + Readout_CurrentByte4;

			DLists[ROM_CurrentMap][DListInfo_CurrentCount[ROM_CurrentMap]] = TempOffset;

			DListInfo_CurrentCount[ROM_CurrentMap]++;
			DListInfo_TotalCount++;
		}
	}

	/* crap, epic fail! no dlists found! */
	if(DListInfo_CurrentCount[ROM_CurrentMap] == 0) {
		MessageBox(hwnd, "No Display Lists found!", "Error", MB_OK | MB_ICONEXCLAMATION);
	}

	return 0;
}

/*	------------------------------------------------------------ */