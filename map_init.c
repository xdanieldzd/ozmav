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
int Viewer_GetMapHeaderList(int HeaderListPos, int CurrentMap)
{
	bool EndOfList = false;

	HeaderListPos = HeaderListPos / 4;

	MapHeader_List[0] = 0x08;
	MapHeader_TotalCount = 1;

	while (!EndOfList) {
		memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][HeaderListPos], 4);

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
int Viewer_GetMapHeader(int CurrentHeader, int CurrentMap)
{
	bool EndOfHeader = false;

	unsigned int InHeaderPos = MapHeader_List[CurrentHeader] / 4;

	sprintf(SystemLogMsg, "Map Header #%d (0x%08X):\n", CurrentHeader + 1, InHeaderPos * 4);
	HelperFunc_LogMessage(2, SystemLogMsg);

	while(!EndOfHeader) {
		memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][InHeaderPos], 4);
		memcpy(&Readout_Current2, &ZMapBuffer[CurrentMap][InHeaderPos + 1], 4);

		HelperFunc_SplitCurrentVals(true);

		switch(Readout_CurrentByte1) {
		case 0x01:
			MapHeader[CurrentMap][CurrentHeader].Actor_Count = Readout_CurrentByte2;
			MapHeader[CurrentMap][CurrentHeader].Actor_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tActors: %d, Actor data offset: 0x%06X\n",
				InHeaderPos * 4,
				MapHeader[CurrentMap][CurrentHeader].Actor_Count, (unsigned int)MapHeader[CurrentMap][CurrentHeader].Actor_DataOffset);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		case 0x0A:
			MapHeader[CurrentMap][CurrentHeader].MeshDataHeader = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tMesh data header: 0x%06X\n",
				InHeaderPos * 4,
				(unsigned int)MapHeader[CurrentMap][CurrentHeader].MeshDataHeader);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		case 0x0B:
			MapHeader[CurrentMap][CurrentHeader].Group_Count = Readout_CurrentByte2;
			MapHeader[CurrentMap][CurrentHeader].Group_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tGroups: %d, Group data offset: 0x%06X\n",
				InHeaderPos * 4,
				MapHeader[CurrentMap][CurrentHeader].Group_Count, (unsigned int)MapHeader[CurrentMap][CurrentHeader].Group_DataOffset);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		case 0x10:
			MapHeader[CurrentMap][CurrentHeader].MapTime = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			MapHeader[CurrentMap][CurrentHeader].TimeFlow = Readout_CurrentByte7;
			sprintf(SystemLogMsg, "  0x%08X:\tMap time: 0x%04X, Timeflow: 0x%02X\n",
				InHeaderPos * 4,
				(unsigned int)MapHeader[CurrentMap][CurrentHeader].MapTime, MapHeader[CurrentMap][CurrentHeader].TimeFlow);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		case 0x12:
			MapHeader[CurrentMap][CurrentHeader].Skybox = Readout_CurrentByte5;
			sprintf(SystemLogMsg, "  0x%08X:\tSkybox setting: 0x%02X\n",
				InHeaderPos * 4,
				MapHeader[CurrentMap][CurrentHeader].Skybox);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		case 0x14:
			EndOfHeader = true;
			sprintf(SystemLogMsg, "  0x%08X:\tEnd of header\n", InHeaderPos * 4);
			HelperFunc_LogMessage(2, SystemLogMsg);
			break;
		case 0x16:
			MapHeader[CurrentMap][CurrentHeader].EchoLevel = Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tEcho level: 0x%02X\n",
				InHeaderPos * 4,
				MapHeader[CurrentMap][CurrentHeader].EchoLevel);
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
		/* collision */
		case 0x03:
			SceneHeader[CurrentHeader].Col_DataSource = Readout_CurrentByte5;
			SceneHeader[CurrentHeader].Col_DataOffset = ((Readout_CurrentByte6 * 0x10000) + Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			sprintf(SystemLogMsg, "  0x%08X:\tCollision data offset: 0x%06X\n",
				InHeaderPos * 4,
				(unsigned int)SceneHeader[CurrentHeader].Col_DataOffset);
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
int Viewer_GetMapActors(int CurrentHeader, int CurrentMap)
{
	int InActorDataPos = MapHeader[CurrentMap][CurrentHeader].Actor_DataOffset / 4;

	ActorInfo_CurrentCount[CurrentMap] = 0;
	ActorInfo_Selected = 0;

	if(!(MapHeader[CurrentMap][CurrentHeader].Actor_Count) == 0) {
		while (!(ActorInfo_CurrentCount[CurrentMap] == MapHeader[CurrentMap][CurrentHeader].Actor_Count)) {
			memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][InActorDataPos], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[CurrentMap][InActorDataPos + 1], 4);

			HelperFunc_SplitCurrentVals(true);

			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].Number = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].X_Position = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].Y_Position = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].Z_Position = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][InActorDataPos + 2], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[CurrentMap][InActorDataPos + 3], 4);

			HelperFunc_SplitCurrentVals(true);

			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].X_Rotation = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].Y_Rotation = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].Z_Rotation = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			Actors[CurrentMap][ActorInfo_CurrentCount[CurrentMap]].Variable = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

			ActorInfo_CurrentCount[CurrentMap]++;
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

/* VIEWER_GETMAPDISPLAYLISTS - SCAN THE CURRENT MAP FOR F3DZEX DISPLAY LISTS AND STORE THEIR OFFSETS IN THE DLISTS STRUCT */
int Viewer_GetMapDisplayLists(unsigned long Fsize, int CurrentMap)
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
	HelperFunc_SplitCurrentVals(false);

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
			HelperFunc_SplitCurrentVals(false);

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
		HelperFunc_SplitCurrentVals(true);

		if((Readout_CurrentByte1 == 0x03)) {
			TempOffset = ((Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4) / 4;

			memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][TempOffset], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[CurrentMap][TempOffset + 1], 4);
			HelperFunc_SplitCurrentVals(true);

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
			HelperFunc_SplitCurrentVals(true);

			Unknown1 = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			Unknown2 = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			MeshScanPosition += 2;

			memcpy(&Readout_Current1, &ZMapBuffer[CurrentMap][MeshScanPosition], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[CurrentMap][MeshScanPosition + 1], 4);
			HelperFunc_SplitCurrentVals(true);

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

int Viewer_GetMapCollision(int CurrentHeader)
{
	unsigned int TotalColPoly = 0;
	unsigned int TotalColVert = 0;

	unsigned long TempOffset = (SceneHeader[CurrentHeader].Col_DataOffset / 4) + 3;
	memcpy(&Readout_Current1, &ZSceneBuffer[TempOffset], 4);
	HelperFunc_SplitCurrentVals(false);

	unsigned long ColVertAmount = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;

	TempOffset = (SceneHeader[CurrentHeader].Col_DataOffset / 4) + 4;
	memcpy(&Readout_Current1, &ZSceneBuffer[TempOffset], 4);
	HelperFunc_SplitCurrentVals(false);

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
			HelperFunc_SplitCurrentVals(true);

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
			HelperFunc_SplitCurrentVals(true);

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
			HelperFunc_SplitCurrentVals(true);

			CollisionVertex[TotalColVert].Z = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;
			TotalColVert++;
			CollisionVertex[TotalColVert].X = (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
			CollisionVertex[TotalColVert].Y = (Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6;
			CollisionVertex[TotalColVert].Z = (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
			TotalColVert++;

			ColVertScanPosition += 2;
		}
	}

	HelperFunc_LogMessage(2, "COLLISION RENDERING:\n");
	int i = 0;
	for(i = 0; i < TotalColVert; i++) {
		sprintf(SystemLogMsg, " - Vertex #%5d: X %5d, Y %5d, Z %5d\n", i, CollisionVertex[i].X, CollisionVertex[i].Y, CollisionVertex[i].Z);
		HelperFunc_LogMessage(2, SystemLogMsg);
	}

	TempOffset = (SceneHeader[CurrentHeader].Col_DataOffset / 4) + 5;
	memcpy(&Readout_Current1, &ZSceneBuffer[TempOffset], 4);
	HelperFunc_SplitCurrentVals(false);

	unsigned long ColPolyAmount = ((Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2);

	TempOffset = (SceneHeader[CurrentHeader].Col_DataOffset / 4) + 6;
	memcpy(&Readout_Current1, &ZSceneBuffer[TempOffset], 4);
	HelperFunc_SplitCurrentVals(false);

	HelperFunc_LogMessage(2, "\n");

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
					HelperFunc_SplitCurrentVals(true);

					unsigned int ColType = ((Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2);

					unsigned int ColVertex1 = ((Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4);
					ColVertex1 = (ColVertex1 & 0x0FFF);

					unsigned int ColVertex2 = ((Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6);
					ColVertex2 = (ColVertex2 & 0x0FFF);

					unsigned int ColVertex3 = ((Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8);
					ColVertex3 = (ColVertex3 & 0x0FFF);

					if((ColVertex1 >= TotalColVert) || (ColVertex2 >= TotalColVert) || (ColVertex2 >= TotalColVert)) {
						sprintf(SystemLogMsg, " - WARNING! Requested vertex (%d, %d, %d) > vertex count %d!\n\n", ColVertex1, ColVertex2, ColVertex3, TotalColVert);
						HelperFunc_LogMessage(2, SystemLogMsg);
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

						sprintf(SystemLogMsg, " - Polygon %5d:\n  - Collision type: 0x%04X\n  - Vertex 1 (#%5d), X %5d, Y %5d, Z %5d\n  - Vertex 2 (#%5d), X %5d, Y %5d, Z %5d\n  - Vertex 3 (#%5d), X %5d, Y %5d, Z %5d\n\n",
							TotalColPoly,
							ColType,
							ColVertex1, CollisionVertex[ColVertex1].X, CollisionVertex[ColVertex1].Y, CollisionVertex[ColVertex1].Z,
							ColVertex2, CollisionVertex[ColVertex2].X, CollisionVertex[ColVertex2].Y, CollisionVertex[ColVertex2].Z,
							ColVertex3, CollisionVertex[ColVertex3].X, CollisionVertex[ColVertex3].Y, CollisionVertex[ColVertex3].Z);
						HelperFunc_LogMessage(2, SystemLogMsg);
					}

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

/*	------------------------------------------------------------ */
