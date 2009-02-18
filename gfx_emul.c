/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	gfx_emul.c - Display List interpreter & main map renderer
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

/* VIEWER_RENDERMAP - MAIN FUNCTION FOR DISPLAY LIST HANDLING TO RENDER MAPS, FOR EITHER ALL OR A GIVEN DISPLAY LIST(S) */
int Viewer_RenderMap()
{
	Debug_MallocOperations = 0;
	Debug_FreeOperations = 0;

	/* OPEN GFX COMMAND LOG */
	if(!GFXLogOpened) FileGFXLog = fopen("gfxlog.txt", "w"); GFXLogOpened = true;

	if(!WavefrontObjOpened) FileWavefrontObj = fopen("map.obj", "w"); WavefrontObjOpened = true;
	if(!WavefrontMtlOpened) FileWavefrontMtl = fopen("map.mtl", "w"); WavefrontMtlOpened = true;

	WavefrontObjVertCount = 0;
	WavefrontObjVertCount_Previous = 0;

	/* IF GL DLISTS EXIST, DELETE THEM ALL */
	if(Renderer_GLDisplayList != 0) glDeleteLists(Renderer_GLDisplayList, 4096);
	/* REGENERATE GL DLISTS */
	Renderer_GLDisplayList = glGenLists(4096);

	/* SET GL DLIST BASE */
	glListBase(Renderer_GLDisplayList);
	Renderer_GLDisplayList_Current = Renderer_GLDisplayList;

	int i = 0;
	for(i = 0; i < SceneHeader[SceneHeader_Current].Map_Count; i++) {
		sprintf(ErrorMsg, "");

		ROM_CurrentMap_Temp = i;

		int j = 0;
		for(j = 0; j < DListInfo_CurrentCount[i]; j++) {
			if(Renderer_GLDisplayList_Current != 0) {
				glNewList(Renderer_GLDisplayList_Current + j, GL_COMPILE);
					DLTempPosition = DLists[i][j] / 4;

					SubDLCall = false;

					GetDLFromZMapScene = true;
					DLToRender = j + 1;
					DListParser_CurrentMap = i;
					Viewer_RenderMap_DListParser(false, DLTempPosition);

					DListHasEnded = false;
				glEndList();

				Renderer_GLDisplayList_Total++;

//				sprintf(ErrorMsg, "%s- DLists[%d][%d] = 0x%08X\n", ErrorMsg, ROM_CurrentMap, j, DLists[ROM_CurrentMap][j]);
			}
		}
//		sprintf(ErrorMsg, "%s\n- Filesize 0x%08X\n", ErrorMsg, ZMapFilesize[i]);
//		MessageBox(hwnd, ErrorMsg, "", 0);

		Renderer_GLDisplayList_Current += j;
	}

	TexCachePosition = 0;
	memset(CurrentTextures, 0x00, sizeof(CurrentTextures));

	Viewer_GetMapCollision(SceneHeader_Current);

/*	for(i = 0; i < SceneHeader[SceneHeader_Current].Map_Count; i++) {
		sprintf(ErrorMsg, "");

		int j = 0;
		if(!(MapHeader[i][MapHeader_Current].Actor_Count) == 0) {
			for(j = 0; j < MapHeader[i][MapHeader_Current].Actor_Count; j++) {
				unsigned int ID = Actors[i][j].Number;

				if(ActorTable[ID].Valid) {
					unsigned long ActorData_Start = ActorTable[ID].StartOffset;
					unsigned long ActorData_End = ActorTable[ID].EndOffset;
					unsigned long ActorData_Length = ActorData_End - ActorData_Start;

					unsigned long TempOffset;

					if(!(TempActorBuffer = (unsigned int*) malloc (sizeof(int) * ActorData_Length))) {
						MessageBox(hwnd, "Error while allocating actor scratch buffer!", "Error", MB_OK | MB_ICONSTOP);
						TempActorBuffer_Allocated = false;
						AreaLoaded = false;
						return 0;
					} else {
						TempActorBuffer_Allocated = true;
						Debug_MallocOperations++;
					}

					memcpy(TempActorBuffer, &ROMBuffer[ActorData_Start / 4], ActorData_Length);

					memcpy(&Readout_Current1, &TempActorBuffer[(ActorData_Length / 4) - 1], 4);
					HelperFunc_SplitCurrentVals(false);

					unsigned long TempDiff = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
					TempOffset = ActorData_Length - TempDiff;

					memcpy(&Readout_Current1, &TempActorBuffer[TempOffset / 4], 4);
					HelperFunc_SplitCurrentVals(false);

					TempOffset = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;

					memcpy(&Readout_Current1, &TempActorBuffer[(TempOffset / 4) + 2], 4);
					HelperFunc_SplitCurrentVals(false);

					ObjectID = (Readout_CurrentByte1 * 0x100) + Readout_CurrentByte2;

					bool TempValid = true;

					if((ObjectID < 0x03) || (ObjectID > 0x191)) {
						TempValid = false;
					} else {
						if(ObjectTable[ObjectID].Valid == false) {
							TempValid = false;
						}
					}

					if((TempValid) && (TempActorBuffer_Allocated)) {
						unsigned long CurrentObject_Start = ObjectTable[ObjectID].StartOffset;
						unsigned long CurrentObject_End = ObjectTable[ObjectID].EndOffset;
						CurrentObject_Length = CurrentObject_End - CurrentObject_Start;

						sprintf(ErrorMsg, "Object %04X: 0x%08X - 0x%08X (0x%06X bytes)\n", ObjectID, CurrentObject_Start, CurrentObject_End, CurrentObject_Length);
						HelperFunc_LogMessage(2, ErrorMsg);

						if(!(TempObjectBuffer = (unsigned int*) malloc (sizeof(int) * CurrentObject_Length))) {
							MessageBox(hwnd, "Error while allocating object scratch buffer!", "Error", MB_OK | MB_ICONSTOP);
							TempObjectBuffer_Allocated = false;
							AreaLoaded = false;
							return 0;
						} else {
							TempObjectBuffer_Allocated = true;
							Debug_MallocOperations++;
						}

						memcpy(TempObjectBuffer, &ROMBuffer[CurrentObject_Start / 4], CurrentObject_Length);

						unsigned int DListScanPosition = 0;
						bool DListFound = false;
						int DListCnt = 0;

						while ((DListScanPosition < CurrentObject_Length / 4) && (DListFound == false)) {
							memcpy(&Readout_Current1, &TempObjectBuffer[DListScanPosition], 4);
							memcpy(&Readout_Current2, &TempObjectBuffer[DListScanPosition + 1], 4);

							HelperFunc_SplitCurrentVals(true);

							if((Readout_CurrentByte1 == 0xE7) && (Readout_CurrentByte2 == 0x00)) {
								if((Readout_CurrentByte3 == 0x00) && (Readout_CurrentByte4 == 0x00)) {
									glNewList(Renderer_GLDisplayList + Renderer_GLDisplayList_Total + ID, GL_COMPILE);
										SubDLCall = false;
										GetDLFromZMapScene = false;
										if(DListCnt == 0) {
											Viewer_RenderMap_DListParser(false, DListScanPosition);
											DListFound = true;
										}

										PrimColor[0] = 0.0f;
										PrimColor[1] = 0.0f;
										PrimColor[2] = 0.0f;
										PrimColor[3] = 1.0f;

										Blender_Cycle1 = 0x00;
										Blender_Cycle2 = 0x00;

										DListHasEnded = false;

										DListCnt++;
									glEndList();
								}
							}
							DListScanPosition += 2;
						}

						if(TempObjectBuffer_Allocated) { free(TempObjectBuffer); TempObjectBuffer_Allocated = false; Debug_FreeOperations++; }
					} else {
						ActorTable[ID].Valid = false;
					}

					if(TempActorBuffer_Allocated) { free(TempActorBuffer); TempActorBuffer_Allocated = false; Debug_FreeOperations++; }
				}
			}
		}
	}
*/
	AreaLoaded = true;

	fclose(FileGFXLog); GFXLogOpened = false;

	fclose(FileWavefrontObj); WavefrontObjOpened = false;
	fclose(FileWavefrontMtl); WavefrontMtlOpened = false;

//	sprintf(ErrorMsg, "%d malloc operations, %d free operations while loading map", Debug_MallocOperations, Debug_FreeOperations);
//	MessageBox(hwnd, ErrorMsg, "Memory", MB_OK | MB_ICONINFORMATION);

	return 0;
}

/*	------------------------------------------------------------ */

/* VIEWER_RENDERMAP_DLISTPARSER - DLIST INTERPRETER MAIN LOOP, SCANS AND EXECUTES DLIST COMMANDS */
int Viewer_RenderMap_DListParser(bool CalledViaCmd, unsigned long Position)
{
	sprintf(WavefrontObjMsg, "mltlib map.mtl\n");
	fprintf(FileWavefrontObj, WavefrontObjMsg);

	if(CalledViaCmd) {
		sprintf(GFXLogMsg, "  [Calling DList at 0x%08X]\n", (unsigned int)Position * 4);
		HelperFunc_LogMessage(1, GFXLogMsg);
	} else {
		sprintf(GFXLogMsg, "Display List #%d (0x%08X):\n", DLToRender, (unsigned int)Position * 4);
		HelperFunc_LogMessage(1, GFXLogMsg);
	}

	while (!DListHasEnded) {
		if(GetDLFromZMapScene) {
			/* get data from map */
			memcpy(&Readout_Current1, &ZMapBuffer[DListParser_CurrentMap][Position], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[DListParser_CurrentMap][Position + 1], 4);

			memcpy(&Readout_NextGFXCommand1, &ZMapBuffer[DListParser_CurrentMap][Position + 2], 4);

			HelperFunc_SplitCurrentVals(true);
		} else {
			/* get data from temp object */
			memcpy(&Readout_Current1, &TempObjectBuffer[Position], 4);
			memcpy(&Readout_Current2, &TempObjectBuffer[Position + 1], 4);

			memcpy(&Readout_NextGFXCommand1, &TempObjectBuffer[Position + 2], 4);

			HelperFunc_SplitCurrentVals(true);
		}

		if(CalledViaCmd) HelperFunc_LogMessage(1, " ");

		switch(Readout_CurrentByte1) {
		case F3DEX2_VTX:
			sprintf(CurrentGFXCmd, "F3DEX2_VTX           ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDVertexList();
			break;
		case F3DEX2_TRI1:
			sprintf(CurrentGFXCmd, "F3DEX2_TRI1          ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDDrawTri1();
			break;
		case F3DEX2_TRI2:
			sprintf(CurrentGFXCmd, "F3DEX2_TRI2          ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDDrawTri2();
			break;
		case F3DEX2_TEXTURE:
			sprintf(CurrentGFXCmd, "F3DEX2_TEXTURE       ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDTexture();
			break;
		case G_SETTIMG:
			sprintf(CurrentGFXCmd, "G_SETTIMG            ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetTImage();
			break;
		case G_SETTILE:
			sprintf(CurrentGFXCmd, "G_SETTILE            ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetTile();
			break;
		case G_LOADBLOCK:
			sprintf(CurrentGFXCmd, "G_LOADBLOCK          ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_SETTILESIZE:
			sprintf(CurrentGFXCmd, "G_SETTILESIZE        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetTileSize();
			break;
		case G_RDPFULLSYNC:
			sprintf(CurrentGFXCmd, "G_RDPFULLSYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_RDPTILESYNC:
			sprintf(CurrentGFXCmd, "G_RDPTILESYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_RDPPIPESYNC:
			sprintf(CurrentGFXCmd, "G_RDPPIPESYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		case G_RDPLOADSYNC:
			sprintf(CurrentGFXCmd, "G_RDPLOADSYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			break;
		case F3DEX2_GEOMETRYMODE:
			sprintf(CurrentGFXCmd, "F3DEX2_GEOMETRYMODE  ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDGeometryMode();
			break;
		case F3DEX2_CULLDL:
			sprintf(CurrentGFXCmd, "F3DEX2_CULLDL        ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;
		case F3DEX2_SETOTHERMODE_H:
			sprintf(CurrentGFXCmd, "F3DEX2_SETOTHERMODE_H");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetOtherModeH();
			break;
		case F3DEX2_SETOTHERMODE_L:
			sprintf(CurrentGFXCmd, "F3DEX2_SETOTHERMODE_L");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetOtherModeL();
			break;
		case G_SETFOGCOLOR:
			sprintf(CurrentGFXCmd, "G_SETFOGCOLOR        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetFogColor();
			break;
		case G_SETPRIMCOLOR:
			sprintf(CurrentGFXCmd, "G_SETPRIMCOLOR       ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetPrimColor();
			break;
        case G_SETENVCOLOR:
            sprintf(CurrentGFXCmd, "G_SETENVCOLOR        ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetEnvColor();
		    break;
		case F3DEX2_RDPHALF_1:
			sprintf(CurrentGFXCmd, "F3DEX2_RDPHALF_1     ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDRDPHalf1();
			break;
		case G_LOADTLUT:
			sprintf(CurrentGFXCmd, "G_LOADTLUT           ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDLoadTLUT(Texture[0].PalDataSource, Texture[0].PalOffset);
			break;
		case G_SETCOMBINE:
			sprintf(CurrentGFXCmd, "G_SETCOMBINE         ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDSetCombine();
			break;
		case F3DEX2_MTX:
			sprintf(CurrentGFXCmd, "F3DEX2_MTX           ");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDMatrix();
			break;
		case F3DEX2_MODIFYVTX:
			sprintf(CurrentGFXCmd, "F3DEX2_MODIFYVTX     ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			break;

		case F3DEX2_DL:
			sprintf(CurrentGFXCmd, "F3DEX2_DL            ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDCallDList(GetDLFromZMapScene);
			break;
		case F3DEX2_BRANCH_Z:
			sprintf(CurrentGFXCmd, "F3DEX2_BRANCH_Z      ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDBranchZ();
			break;
		case F3DEX2_ENDDL:
			sprintf(CurrentGFXCmd, "F3DEX2_ENDDL         ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			DListHasEnded = true;
			break;
		default:
			sprintf(CurrentGFXCmd, "<unknown>            ");
			sprintf(CurrentGFXCmdNote, "-");
			HelperFunc_GFXLogCommand(Position);
			MessageBox(hwnd, "ERROR UNKNOWN GFX COMMAND!!", "ERROR", MB_OK | MB_ICONERROR);
			break;
		}

		if(DListHasEnded) {
			if(CalledViaCmd) {
				HelperFunc_LogMessage(1, "  [Return to original DList]\n");
			} else {
				HelperFunc_LogMessage(1, "\n");
			}
		}

		Position+=2;
	}

	if(CalledViaCmd) DListHasEnded = false;

	SubDLCall = false;

	return 0;
}

/* VIEWER_RENDERMAP_CMDVERTEXLIST - F3DEX2_VTX - GET OFFSET, AMOUNT & VERTEX BUFFER POSITION OF VERTICES TO USE */
int Viewer_RenderMap_CMDVertexList()
{
	unsigned int VertList_Temp = (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned int TempVertCount = ((VertList_Temp & 0x000FFF) / 2);
	unsigned int TempVertListStartEntry = TempVertCount - ((VertList_Temp & 0xFFF000) >> 12);

	unsigned int TempVertListBank = Readout_CurrentByte5;
	unsigned long TempVertListOffset = Readout_CurrentByte6 << 16;
	TempVertListOffset = TempVertListOffset + (Readout_CurrentByte7 << 8);
	TempVertListOffset = TempVertListOffset + Readout_CurrentByte8;

	Viewer_GetVertexList(TempVertListBank, TempVertListOffset, TempVertCount, TempVertListStartEntry);

	glEnable(GL_TEXTURE_2D);
	Renderer_GLTexture = Viewer_LoadTexture(0);
	glBindTexture(GL_TEXTURE_2D, Renderer_GLTexture);

	return 0;
}

/* VIEWER_GETVERTEXLIST - FOR F3DEX2_VTX - READ THE VERTEX LIST SPECIFIED BEFORE AND STORE THE DATA IN VERTEX STRUCT */
int Viewer_GetVertexList(unsigned int Bank, unsigned long Offset, unsigned int VertCount, unsigned int TempVertListStartEntry)
{
	unsigned long CurrentVert = TempVertListStartEntry;
	unsigned long VertListPosition = 0;

	unsigned char * VertListTempBuffer;
	VertListTempBuffer = (unsigned char *) malloc ((VertCount + 1) * 0x10);
	memset(VertListTempBuffer, 0x00, sizeof(VertListTempBuffer));

	Debug_MallocOperations++;

	if(Viewer_ZMemCopy(Bank, Offset, VertListTempBuffer, ((VertCount + 1) * 0x10)) == -1) {
		sprintf(ErrorMsg, "Invalid Vertex data source 0x%02X!", Bank);
//		MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	WavefrontObjVertCount_Previous = WavefrontObjVertCount;

	sprintf(WavefrontObjMsg, "usemtl material\n");
	fprintf(FileWavefrontObj, WavefrontObjMsg);

	while(CurrentVert < VertCount) {
		// X
		Vertex[CurrentVert].X = (VertListTempBuffer[VertListPosition] * 0x100) + VertListTempBuffer[VertListPosition + 1];
		VertListPosition+=2;
		// Y
		Vertex[CurrentVert].Y = (VertListTempBuffer[VertListPosition] * 0x100) + VertListTempBuffer[VertListPosition + 1];
		VertListPosition+=2;
		// Z
		Vertex[CurrentVert].Z = (VertListTempBuffer[VertListPosition] * 0x100) + VertListTempBuffer[VertListPosition + 1];
		VertListPosition+=4;

		// H
		Vertex[CurrentVert].H = (VertListTempBuffer[VertListPosition] * 0x100) + VertListTempBuffer[VertListPosition + 1];
		VertListPosition+=2;
		// V
		Vertex[CurrentVert].V = (VertListTempBuffer[VertListPosition] * 0x100) + VertListTempBuffer[VertListPosition + 1];
		VertListPosition+=2;

		// R, G, B, A
		Vertex[CurrentVert].R = VertListTempBuffer[VertListPosition];
		Vertex[CurrentVert].G = VertListTempBuffer[VertListPosition + 1];
		Vertex[CurrentVert].B = VertListTempBuffer[VertListPosition + 2];
		Vertex[CurrentVert].A = VertListTempBuffer[VertListPosition + 3];
		VertListPosition+=4;

		/* shitty obj extraction time! */

		sprintf(WavefrontObjMsg, "v %4.2f %4.2f %4.2f\n", (float)Vertex[CurrentVert].X / 32, (float)Vertex[CurrentVert].Y / 32, (float)Vertex[CurrentVert].Z / 32);
		fprintf(FileWavefrontObj, WavefrontObjMsg);
		sprintf(WavefrontObjMsg, "vt %4.2f %4.2f\n", (float)Vertex[CurrentVert].H, (float)Vertex[CurrentVert].V);
		fprintf(FileWavefrontObj, WavefrontObjMsg);
		sprintf(WavefrontObjMsg, "vn %4.2f %4.2f %4.2f\n", (float)Vertex[CurrentVert].R, (float)Vertex[CurrentVert].G, (float)Vertex[CurrentVert].B);
		fprintf(FileWavefrontObj, WavefrontObjMsg);

		sprintf(WavefrontMtlMsg, "newmtl material\n");
		fprintf(FileWavefrontMtl, WavefrontMtlMsg);
		sprintf(WavefrontMtlMsg, "Ka %4.2f %4.2f %4.2f\n", (float)Vertex[CurrentVert].R / 2, (float)Vertex[CurrentVert].G / 2, (float)Vertex[CurrentVert].B / 2);
		fprintf(FileWavefrontMtl, WavefrontMtlMsg);
		sprintf(WavefrontMtlMsg, "Ka %4.2f %4.2f %4.2f\n", (float)Vertex[CurrentVert].R, (float)Vertex[CurrentVert].G, (float)Vertex[CurrentVert].B);
		fprintf(FileWavefrontMtl, WavefrontMtlMsg);
		sprintf(WavefrontMtlMsg, "illum 1\n");
		fprintf(FileWavefrontMtl, WavefrontMtlMsg);

		WavefrontObjVertCount++;

/*		sprintf(ErrorMsg, "Vertex: %x, %x, %x, %x, %x, %x, %x, %x, %x",
			Vertex[CurrentVert].X, Vertex[CurrentVert].Y, Vertex[CurrentVert].Z,
			Vertex[CurrentVert].H, Vertex[CurrentVert].V,
			Vertex[CurrentVert].R, Vertex[CurrentVert].G, Vertex[CurrentVert].B, Vertex[CurrentVert].A);
		MessageBox(hwnd, ErrorMsg, "Vertex", MB_OK | MB_ICONINFORMATION);
*/
		CurrentVert++;
	}

	free(VertListTempBuffer);
	Debug_FreeOperations++;

	return 0;
}

/* VIEWER_RENDERMAP_CMDDRAWTRI1 - F3DEX2_TRI1 - GET VERTEX COORDINATES AND COLOR FROM VERTEX STRUCT & RENDER SINGLE TRIANGLE */
int Viewer_RenderMap_CMDDrawTri1()
{
	signed int CurrentX1_1 = Vertex[Readout_CurrentByte2 / 2].X;	// triangle 1, vertex 1, X
	signed int CurrentY1_1 = Vertex[Readout_CurrentByte2 / 2].Y;	// triangle 1, vertex 1, Y
	signed int CurrentZ1_1 = Vertex[Readout_CurrentByte2 / 2].Z;	// triangle 1, vertex 1, Z
	signed int CurrentX1_2 = Vertex[Readout_CurrentByte3 / 2].X;	// triangle 1, vertex 2, X
	signed int CurrentY1_2 = Vertex[Readout_CurrentByte3 / 2].Y;	// triangle 1, vertex 2, Y
	signed int CurrentZ1_2 = Vertex[Readout_CurrentByte3 / 2].Z;	// triangle 1, vertex 2, Z
	signed int CurrentX1_3 = Vertex[Readout_CurrentByte4 / 2].X;	// triangle 1, vertex 3, X
	signed int CurrentY1_3 = Vertex[Readout_CurrentByte4 / 2].Y;	// triangle 1, vertex 3, Y
	signed int CurrentZ1_3 = Vertex[Readout_CurrentByte4 / 2].Z;	// triangle 1, vertex 3, Z

	GLbyte CurrentR1_1 = Vertex[Readout_CurrentByte2 / 2].R;	// triangle 1, vertex 1, R
	GLbyte CurrentG1_1 = Vertex[Readout_CurrentByte2 / 2].G;	// triangle 1, vertex 1, G
	GLbyte CurrentB1_1 = Vertex[Readout_CurrentByte2 / 2].B;	// triangle 1, vertex 1, B
	GLbyte CurrentA1_1 = Vertex[Readout_CurrentByte2 / 2].A;	// triangle 1, vertex 1, A
	GLbyte CurrentR1_2 = Vertex[Readout_CurrentByte3 / 2].R;	// triangle 1, vertex 2, R
	GLbyte CurrentG1_2 = Vertex[Readout_CurrentByte3 / 2].G;	// triangle 1, vertex 2, G
	GLbyte CurrentB1_2 = Vertex[Readout_CurrentByte3 / 2].B;	// triangle 1, vertex 2, B
	GLbyte CurrentA1_2 = Vertex[Readout_CurrentByte3 / 2].A;	// triangle 1, vertex 2, A
	GLbyte CurrentR1_3 = Vertex[Readout_CurrentByte4 / 2].R;	// triangle 1, vertex 3, R
	GLbyte CurrentG1_3 = Vertex[Readout_CurrentByte4 / 2].G;	// triangle 1, vertex 3, G
	GLbyte CurrentB1_3 = Vertex[Readout_CurrentByte4 / 2].B;	// triangle 1, vertex 3, B
	GLbyte CurrentA1_3 = Vertex[Readout_CurrentByte4 / 2].A;	// triangle 1, vertex 3, A

	signed short CurrentH1_1 = Vertex[Readout_CurrentByte2 / 2].H;
	signed short CurrentH1_2 = Vertex[Readout_CurrentByte3 / 2].H;
	signed short CurrentH1_3 = Vertex[Readout_CurrentByte4 / 2].H;
	signed short CurrentV1_1 = Vertex[Readout_CurrentByte2 / 2].V;
	signed short CurrentV1_2 = Vertex[Readout_CurrentByte3 / 2].V;
	signed short CurrentV1_3 = Vertex[Readout_CurrentByte4 / 2].V;

	float TempU = 0, TempU2 = 0;
	float TempV = 0, TempV2 = 0;

	Viewer_RenderObject_HACKSelectClrAlpSource();
//	glColor4f(PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);

	glBegin(GL_TRIANGLES);
		TempU = (float) CurrentH1_1 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_1 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_IsRGBANormals) { glColor4ub (CurrentR1_1, CurrentG1_1, CurrentB1_1, CurrentA1_1); }
		glNormal3f (CurrentR1_1 / 255.0f, CurrentG1_1 / 255.0f, CurrentB1_1 / 255.0f);
		glVertex3d(CurrentX1_1, CurrentY1_1, CurrentZ1_1);

		TempU = (float) CurrentH1_2 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_2 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_IsRGBANormals) { glColor4ub (CurrentR1_2, CurrentG1_2, CurrentB1_2, CurrentA1_2); }
		glNormal3f (CurrentR1_2 / 255.0f, CurrentG1_2 / 255.0f, CurrentB1_2 / 255.0f);
		glVertex3d(CurrentX1_2, CurrentY1_2, CurrentZ1_2);

		TempU = (float) CurrentH1_3 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_3 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_IsRGBANormals) { glColor4ub (CurrentR1_3, CurrentG1_3, CurrentB1_3, CurrentA1_3); }
		glNormal3f (CurrentR1_3 / 255.0f, CurrentG1_3 / 255.0f, CurrentB1_3 / 255.0f);
		glVertex3d(CurrentX1_3, CurrentY1_3, CurrentZ1_3);
	glEnd();

	/* MORE shitty obj extraction time! */

	sprintf(WavefrontObjMsg, "f %d %d %d\n", (Readout_CurrentByte2 / 2) + 1 + WavefrontObjVertCount_Previous, (Readout_CurrentByte3 / 2) + 1 + WavefrontObjVertCount_Previous, (Readout_CurrentByte4 / 2) + 1 + WavefrontObjVertCount_Previous);
	fprintf(FileWavefrontObj, WavefrontObjMsg);

	return 0;
}

/* VIEWER_RENDERMAP_CMDDRAWTRI2 - F3DEX2_TRI2 - GET VERTEX COORDINATES AND COLOR FROM VERTEX STRUCT & RENDER TWO TRIANGLES */
int Viewer_RenderMap_CMDDrawTri2()
{
	signed int CurrentX1_1 = Vertex[Readout_CurrentByte2 / 2].X;	// triangle 1, vertex 1, X
	signed int CurrentY1_1 = Vertex[Readout_CurrentByte2 / 2].Y;	// triangle 1, vertex 1, Y
	signed int CurrentZ1_1 = Vertex[Readout_CurrentByte2 / 2].Z;	// triangle 1, vertex 1, Z
	signed int CurrentX1_2 = Vertex[Readout_CurrentByte3 / 2].X;	// triangle 1, vertex 2, X
	signed int CurrentY1_2 = Vertex[Readout_CurrentByte3 / 2].Y;	// triangle 1, vertex 2, Y
	signed int CurrentZ1_2 = Vertex[Readout_CurrentByte3 / 2].Z;	// triangle 1, vertex 2, Z
	signed int CurrentX1_3 = Vertex[Readout_CurrentByte4 / 2].X;	// triangle 1, vertex 3, X
	signed int CurrentY1_3 = Vertex[Readout_CurrentByte4 / 2].Y;	// triangle 1, vertex 3, Y
	signed int CurrentZ1_3 = Vertex[Readout_CurrentByte4 / 2].Z;	// triangle 1, vertex 3, Z

	GLbyte CurrentR1_1 = Vertex[Readout_CurrentByte2 / 2].R;	// triangle 1, vertex 1, R
	GLbyte CurrentG1_1 = Vertex[Readout_CurrentByte2 / 2].G;	// triangle 1, vertex 1, G
	GLbyte CurrentB1_1 = Vertex[Readout_CurrentByte2 / 2].B;	// triangle 1, vertex 1, B
	GLbyte CurrentA1_1 = Vertex[Readout_CurrentByte2 / 2].A;	// triangle 1, vertex 1, A
	GLbyte CurrentR1_2 = Vertex[Readout_CurrentByte3 / 2].R;	// triangle 1, vertex 2, R
	GLbyte CurrentG1_2 = Vertex[Readout_CurrentByte3 / 2].G;	// triangle 1, vertex 2, G
	GLbyte CurrentB1_2 = Vertex[Readout_CurrentByte3 / 2].B;	// triangle 1, vertex 2, B
	GLbyte CurrentA1_2 = Vertex[Readout_CurrentByte3 / 2].A;	// triangle 1, vertex 2, A
	GLbyte CurrentR1_3 = Vertex[Readout_CurrentByte4 / 2].R;	// triangle 1, vertex 3, R
	GLbyte CurrentG1_3 = Vertex[Readout_CurrentByte4 / 2].G;	// triangle 1, vertex 3, G
	GLbyte CurrentB1_3 = Vertex[Readout_CurrentByte4 / 2].B;	// triangle 1, vertex 3, B
	GLbyte CurrentA1_3 = Vertex[Readout_CurrentByte4 / 2].A;	// triangle 1, vertex 3, A

	signed short CurrentH1_1 = Vertex[Readout_CurrentByte2 / 2].H;
	signed short CurrentH1_2 = Vertex[Readout_CurrentByte3 / 2].H;
	signed short CurrentH1_3 = Vertex[Readout_CurrentByte4 / 2].H;
	signed short CurrentV1_1 = Vertex[Readout_CurrentByte2 / 2].V;
	signed short CurrentV1_2 = Vertex[Readout_CurrentByte3 / 2].V;
	signed short CurrentV1_3 = Vertex[Readout_CurrentByte4 / 2].V;

	signed int CurrentX2_1 = Vertex[Readout_CurrentByte6 / 2].X;	// triangle 2, vertex 1, X
	signed int CurrentY2_1 = Vertex[Readout_CurrentByte6 / 2].Y;	// triangle 2, vertex 1, Y
	signed int CurrentZ2_1 = Vertex[Readout_CurrentByte6 / 2].Z;	// triangle 2, vertex 1, Z
	signed int CurrentX2_2 = Vertex[Readout_CurrentByte7 / 2].X;	// triangle 2, vertex 2, X
	signed int CurrentY2_2 = Vertex[Readout_CurrentByte7 / 2].Y;	// triangle 2, vertex 2, Y
	signed int CurrentZ2_2 = Vertex[Readout_CurrentByte7 / 2].Z;	// triangle 2, vertex 2, Z
	signed int CurrentX2_3 = Vertex[Readout_CurrentByte8 / 2].X;	// triangle 2, vertex 3, X
	signed int CurrentY2_3 = Vertex[Readout_CurrentByte8 / 2].Y;	// triangle 2, vertex 3, Y
	signed int CurrentZ2_3 = Vertex[Readout_CurrentByte8 / 2].Z;	// triangle 2, vertex 3, Z

	GLbyte CurrentR2_1 = Vertex[Readout_CurrentByte6 / 2].R;	// triangle 2, vertex 1, R
	GLbyte CurrentG2_1 = Vertex[Readout_CurrentByte6 / 2].G;	// triangle 2, vertex 1, G
	GLbyte CurrentB2_1 = Vertex[Readout_CurrentByte6 / 2].B;	// triangle 2, vertex 1, B
	GLbyte CurrentA2_1 = Vertex[Readout_CurrentByte6 / 2].A;	// triangle 2, vertex 1, A
	GLbyte CurrentR2_2 = Vertex[Readout_CurrentByte7 / 2].R;	// triangle 2, vertex 2, R
	GLbyte CurrentG2_2 = Vertex[Readout_CurrentByte7 / 2].G;	// triangle 2, vertex 2, G
	GLbyte CurrentB2_2 = Vertex[Readout_CurrentByte7 / 2].B;	// triangle 2, vertex 2, B
	GLbyte CurrentA2_2 = Vertex[Readout_CurrentByte7 / 2].A;	// triangle 2, vertex 2, A
	GLbyte CurrentR2_3 = Vertex[Readout_CurrentByte8 / 2].R;	// triangle 2, vertex 3, R
	GLbyte CurrentG2_3 = Vertex[Readout_CurrentByte8 / 2].G;	// triangle 2, vertex 3, G
	GLbyte CurrentB2_3 = Vertex[Readout_CurrentByte8 / 2].B;	// triangle 2, vertex 3, B
	GLbyte CurrentA2_3 = Vertex[Readout_CurrentByte8 / 2].A;	// triangle 2, vertex 3, A

	signed short CurrentH2_1 = Vertex[Readout_CurrentByte6 / 2].H;
	signed short CurrentH2_2 = Vertex[Readout_CurrentByte7 / 2].H;
	signed short CurrentH2_3 = Vertex[Readout_CurrentByte8 / 2].H;
	signed short CurrentV2_1 = Vertex[Readout_CurrentByte6 / 2].V;
	signed short CurrentV2_2 = Vertex[Readout_CurrentByte7 / 2].V;
	signed short CurrentV2_3 = Vertex[Readout_CurrentByte8 / 2].V;

	float TempU = 0, TempU2 = 0;
	float TempV = 0, TempV2 = 0;

	Viewer_RenderObject_HACKSelectClrAlpSource();
//	glColor4f(PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);

	glBegin(GL_TRIANGLES);
		TempU = (float) CurrentH1_1 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_1 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_IsRGBANormals) { glColor4ub (CurrentR1_1, CurrentG1_1, CurrentB1_1, CurrentA1_1); }
		glNormal3f (CurrentR1_1 / 255.0f, CurrentG1_1 / 255.0f, CurrentB1_1 / 255.0f);
		glVertex3d(CurrentX1_1, CurrentY1_1, CurrentZ1_1);

		TempU = (float) CurrentH1_2 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_2 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_IsRGBANormals) { glColor4ub (CurrentR1_2, CurrentG1_2, CurrentB1_2, CurrentA1_2); }
		glNormal3f (CurrentR1_2 / 255.0f, CurrentG1_2 / 255.0f, CurrentB1_2 / 255.0f);
		glVertex3d(CurrentX1_2, CurrentY1_2, CurrentZ1_2);

		TempU = (float) CurrentH1_3 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_3 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_IsRGBANormals) { glColor4ub (CurrentR1_3, CurrentG1_3, CurrentB1_3, CurrentA1_3); }
		glNormal3f (CurrentR1_3 / 255.0f, CurrentG1_3 / 255.0f, CurrentB1_3 / 255.0f);
		glVertex3d(CurrentX1_3, CurrentY1_3, CurrentZ1_3);

		TempU = (float) CurrentH2_1 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV2_1 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_IsRGBANormals) { glColor4ub (CurrentR2_1, CurrentG2_1, CurrentB2_1, CurrentA2_1); }
		glNormal3f (CurrentR2_1 / 255.0f, CurrentG2_1 / 255.0f, CurrentB2_1 / 255.0f);
		glVertex3d(CurrentX2_1, CurrentY2_1, CurrentZ2_1);

		TempU = (float) CurrentH2_2 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV2_2 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_IsRGBANormals) { glColor4ub (CurrentR2_2, CurrentG2_2, CurrentB2_2, CurrentA2_2); }
		glNormal3f (CurrentR2_2 / 255.0f, CurrentG2_2 / 255.0f, CurrentB2_2 / 255.0f);
		glVertex3d(CurrentX2_2, CurrentY2_2, CurrentZ2_2);

		TempU = (float) CurrentH2_3 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV2_3 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_IsRGBANormals) { glColor4ub (CurrentR2_3, CurrentG2_3, CurrentB2_3, CurrentA2_3); }
		glNormal3f (CurrentR2_3 / 255.0f, CurrentG2_3 / 255.0f, CurrentB2_3 / 255.0f);
		glVertex3d(CurrentX2_3, CurrentY2_3, CurrentZ2_3);
	glEnd();

	/* MORE AND MORE shitty obj extraction time!! */

	sprintf(WavefrontObjMsg, "f %d %d %d\n", (Readout_CurrentByte2 / 2) + 1 + WavefrontObjVertCount_Previous, (Readout_CurrentByte3 / 2) + 1 + WavefrontObjVertCount_Previous, (Readout_CurrentByte4 / 2) + 1 + WavefrontObjVertCount_Previous);
	fprintf(FileWavefrontObj, WavefrontObjMsg);
	sprintf(WavefrontObjMsg, "f %d %d %d\n", (Readout_CurrentByte6 / 2) + 1 + WavefrontObjVertCount_Previous, (Readout_CurrentByte7 / 2) + 1 + WavefrontObjVertCount_Previous, (Readout_CurrentByte8 / 2) + 1 + WavefrontObjVertCount_Previous);
	fprintf(FileWavefrontObj, WavefrontObjMsg);

	return 0;
}

/* VIEWER_RENDERMAP_CMDTEXTURE - F3DEX2_TEXTURE */
int Viewer_RenderMap_CMDTexture()
{
	unsigned long W0 = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned long W1 = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	Texture[0].S_Scale = (float) _FIXED2FLOAT(_SHIFTR(W1, 16, 16), 16);
	Texture[0].T_Scale = (float) _FIXED2FLOAT(_SHIFTR(W1, 0, 16), 16);

	if(Texture[0].S_Scale == 0.0f) Texture[0].S_Scale = 1.0f;
	if(Texture[0].T_Scale == 0.0f) Texture[0].T_Scale = 1.0f;

	return 0;
}

/* VIEWER_RENDERMAP_CMDSETTIMAGE - G_SETTIMG - GET TEXTURE OFFSET AND STORE FOR FUTURE USE */
int Viewer_RenderMap_CMDSetTImage()
{
	switch(Readout_NextGFXCommand1) {
		case 0x000000E8:
			Texture[0].PalDataSource = Readout_CurrentByte5;

			Texture[0].PalOffset = Readout_CurrentByte6 << 16;
			Texture[0].PalOffset = Texture[0].PalOffset + (Readout_CurrentByte7 << 8);
			Texture[0].PalOffset = Texture[0].PalOffset + Readout_CurrentByte8;

			break;
		default:
			Texture[0].DataSource = Readout_CurrentByte5;

			Texture[0].Offset = Readout_CurrentByte6 << 16;
			Texture[0].Offset = Texture[0].Offset + (Readout_CurrentByte7 << 8);
			Texture[0].Offset = Texture[0].Offset + Readout_CurrentByte8;
			break;
	}

	Texture[0].Format_OGL = GL_RGBA;
	Texture[0].Format_OGLPixel = GL_RGBA;

	return 0;
}

/* VIEWER_RENDERMAP_CMDSETTILE - G_SETTILE - GET TEXTURE PROPERTIES AND STORE THEM FOR FUTURE USE (MIRROR, ETC.) */
int Viewer_RenderMap_CMDSetTile()
{
	unsigned long W0 = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned long W1 = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	/* FIX FOR CI TEXTURES - IF BYTES 5-8 ARE 07000000, LEAVE CURRENT PROPERTY SETTINGS ALONE */
	if(W1 == 0x07000000) return 0;

	unsigned char TempYParam = (Readout_CurrentByte6 << 4);
	TempYParam >>= 4;
	unsigned char TempXParam = (Readout_CurrentByte7 << 4);
	TempXParam >>= 4;

	switch(TempYParam) {
		case 0x01:
			Texture[0].Y_Parameter = 1;
			break;
		case 0x09:
			Texture[0].Y_Parameter = 2;
			break;
		case 0x05:
			Texture[0].Y_Parameter = 3;
			break;
		default:
			Texture[0].Y_Parameter = 1;
			break;
	}

	switch(TempXParam) {
		case 0x00:
			Texture[0].X_Parameter = 1;
			break;
		case 0x02:
			Texture[0].X_Parameter = 2;
			break;
		case 0x01:
			Texture[0].X_Parameter = 3;
			break;
		default:
			Texture[0].X_Parameter = 1;
			break;
	}

	Texture[0].Format_N64 = Readout_CurrentByte2;

	Texture[0].Palette = _SHIFTR( W1, 20, 4 );
	Texture[0].LineSize = _SHIFTR( W0, 9, 9 );

	return 0;
}

/* VIEWER_RENDERMAP_CMDSETTILESIZE - G_SETTILESIZE - GET TEXTURE SIZE/COORDINATES AND STORE THEM FOR FUTURE USE */
int Viewer_RenderMap_CMDSetTileSize()
{
	if(!(Readout_Current1 == 0x000000F2)) return 0;

	unsigned int TileSize_Temp1 = (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned int TileSize_Temp2 = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	unsigned int ULS = (TileSize_Temp1 & 0xFFF000) >> 14;
	unsigned int ULT = (TileSize_Temp1 & 0x000FFF) >> 2;
	unsigned int LRS = (TileSize_Temp2 & 0xFFF000) >> 14;
	unsigned int LRT = (TileSize_Temp2 & 0x000FFF) >> 2;

	Texture[0].Width  = ((LRS - ULS) + 1);
	Texture[0].Height = ((LRT - ULT) + 1);

	if(Texture[0].Width > 256) {
		Texture[0].Width = 256;
		Texture[0].WidthRender = Texture[0].Width;// - 64;
	} else {
		Texture[0].WidthRender = Texture[0].Width;
	}

	if(Texture[0].Height > 256) {
		Texture[0].Height = 256;
		Texture[0].HeightRender = Texture[0].Height;// - 64;
	} else {
		Texture[0].HeightRender = Texture[0].Height;
	}

	return 0;
}

/* VIEWER_RENDERMAP_CMDGEOMETRYMODE - F3DEX2_GEOMETRYMODE - GET GEOMETRY PARAMETERS AND EN-/DISABLE THEIR OGL EQUIVALENTS */
int Viewer_RenderMap_CMDGeometryMode()
{
	unsigned int Mode_Clear = (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned int Mode_Set = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	unsigned int ModeSwitch = 0;
	unsigned char ModeName[16];

	if(Mode_Clear == 0xFFFFFF) {
		ModeSwitch = Mode_Set;
		sprintf(ModeName, "SET");
	} else {
		ModeSwitch = Mode_Clear;
		sprintf(ModeName, "CLEAR");
	}

	G_TEXTURE_ENABLE		= (ModeSwitch & 0x000000) != 0;		// texturing
	G_ZBUFFER				= (ModeSwitch & 0x000001) != 0;		// z buffering
	G_SHADE					= (ModeSwitch & 0x000004) != 0;		// shading
	G_CULL_FRONT			= (ModeSwitch & 0x000200) != 0;		// front-face culling
	G_CULL_BACK				= (ModeSwitch & 0x000400) != 0;		// back-face culling
	G_CULL_BOTH				= (ModeSwitch & 0x000600) != 0;		// front-/back-face culling
	G_FOG					= (ModeSwitch & 0x010000) != 0;		// fog
	G_LIGHTING				= (ModeSwitch & 0x020000) != 0;		// lighting
	G_TEXTURE_GEN			= (ModeSwitch & 0x040000) != 0;		// texture coord generation (spherical)
	G_TEXTURE_GEN_LINEAR	= (ModeSwitch & 0x080000) != 0;		// texture coord generation (linear)
	G_LOD					= (ModeSwitch & 0x100000) != 0;		// level-of-detail
	G_SHADING_SMOOTH		= (ModeSwitch & 0x200000) != 0;		// smooth shading
	G_CLIPPING				= (ModeSwitch & 0x800000) != 0;		// clipping

	memset(ErrorMsg, 0x00, sizeof(ErrorMsg));
	sprintf(ErrorMsg, \
		"GEOMETRYMODE | %s (DList #%d): %s%s%s%s%s%s%s%s%s%s%s%s%s\n",
			ModeName,
			DLToRender,
			G_TEXTURE_ENABLE ? "G_TEXTURE_ENABLE | " : "",
			G_ZBUFFER ? "G_ZBUFFER | " : "",
			G_SHADE ? "G_SHADE | " : "",
			G_CULL_FRONT ? "G_CULL_FRONT | " : "",
			G_CULL_BACK ? "G_CULL_BACK | " : "",
			G_CULL_BOTH ? "G_CULL_BOTH | " : "",
			G_FOG ? "G_FOG | " : "",
			G_LIGHTING ? "G_LIGHTING | " : "",
			G_TEXTURE_GEN ? "G_TEXTURE_GEN | " : "",
			G_TEXTURE_GEN_LINEAR ? "G_TEXTURE_GEN_LINEAR | " : "",
			G_LOD ? "G_LOD | " : "",
			G_SHADING_SMOOTH ? "G_SHADING_SMOOTH | " : "",
			G_CLIPPING ? "G_CLIPPING" : "");
	HelperFunc_LogMessage(2, ErrorMsg);

	if(Mode_Clear == 0xFFFFFF) {
		Viewer_SetGeometryMode();
		return 0;
	} else {
		Viewer_ClearGeometryMode();
		return 0;
	}

	return 0;
}

int Viewer_ClearGeometryMode()
{
	if(!G_TEXTURE_ENABLE)		{ glDisable(GL_TEXTURE_2D); }
	if(!G_ZBUFFER)				{ glDisable(GL_DEPTH_TEST); }
	if(!G_SHADE)				{ /* ignore */ }
	if(!G_CULL_FRONT)			{ glDisable(GL_CULL_FACE); }
	if(!G_CULL_BACK)			{ glDisable(GL_CULL_FACE); }
	if(!G_CULL_BOTH)			{ /* ignore */ }
	if(!G_FOG)					{ /* ignore */ }
	if(!G_LIGHTING)				{ glDisable(GL_LIGHTING); glDisable(GL_NORMALIZE); Renderer_IsRGBANormals = false; }
	if(!G_TEXTURE_GEN)			{ /* ignore */ }
	if(!G_TEXTURE_GEN_LINEAR)	{ /* ignore */ }
	if(!G_LOD)					{ /* ignore */ }
	if(!G_SHADING_SMOOTH)		{ /* ignore */ }
	if(!G_CLIPPING)				{ /* ignore */ }

	return 0;
}

int Viewer_SetGeometryMode()
{
	if(G_TEXTURE_ENABLE)		{ glEnable(GL_TEXTURE_2D); }
	if(G_ZBUFFER)				{ glEnable(GL_DEPTH_TEST); }
	if(G_SHADE)					{ /* ignore */ }
	if(G_CULL_FRONT)			{ glEnable(GL_CULL_FACE); }
	if(G_CULL_BACK)				{ glEnable(GL_CULL_FACE); }
	if(G_CULL_BOTH)				{ /* ignore */ }
	if(G_FOG)					{ /* ignore */ }
	if(G_LIGHTING)				{ glEnable(GL_LIGHTING); glEnable(GL_NORMALIZE); Renderer_IsRGBANormals = true; }
	if(G_TEXTURE_GEN)			{ /* ignore */ }
	if(G_TEXTURE_GEN_LINEAR)	{ /* ignore */ }
	if(G_LOD)					{ /* ignore */ }
	if(G_SHADING_SMOOTH)		{ /* ignore */ }
	if(G_CLIPPING)				{ /* ignore */ }

	return 0;
}

/* VIEWER_RENDERMAP_CMDSETFOGCOLOR - G_SETFOGCOLOR - SET THE FOG COLOR */
int Viewer_RenderMap_CMDSetFogColor()
{
	FogColor[0] = (Readout_CurrentByte5 / 255.0f);
	FogColor[1] = (Readout_CurrentByte6 / 255.0f);
	FogColor[2] = (Readout_CurrentByte7 / 255.0f);
	FogColor[3] = (Readout_CurrentByte8 / 255.0f);

	glFogfv(GL_FOG_COLOR, FogColor);
	glClearColor(FogColor[0], FogColor[1], FogColor[2], FogColor[3]);

	return 0;
}

/* VIEWER_RENDERMAP_CMDSETPRIMCOLOR - G_SETPRIMCOLOR - SET THE PRIMARY COLOR */
int Viewer_RenderMap_CMDSetPrimColor()
{
	PrimColor[0] = (Readout_CurrentByte5 / 255.0f);
	PrimColor[1] = (Readout_CurrentByte6 / 255.0f);
	PrimColor[2] = (Readout_CurrentByte7 / 255.0f);
	PrimColor[3] = (Readout_CurrentByte8 / 255.0f);

	if(GLExtension_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);
	}

	return 0;
}

int Viewer_RenderMap_CMDSetEnvColor()
{
    EnvColor[0] = (Readout_CurrentByte5 / 255.0f);
	EnvColor[1] = (Readout_CurrentByte6 / 255.0f);
	EnvColor[2] = (Readout_CurrentByte7 / 255.0f);
	EnvColor[3] = (Readout_CurrentByte8 / 255.0f);

	if(GLExtension_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, EnvColor[0], EnvColor[1], EnvColor[2], EnvColor[3]);
	}

	return 0;
}

int Viewer_RenderMap_CMDRDPHalf1()
{
	Storage_RDPHalf1 = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	return 0;
}

int Viewer_RenderMap_CMDBranchZ()
{
	unsigned int TempRAMSeg = (Storage_RDPHalf1 & 0xFF000000) >> 24;
	unsigned long TempDLOffset = (Storage_RDPHalf1 & 0x00FFFFFF);

	if(TempRAMSeg == 0x03) {
		SubDLCall = true;
		Viewer_RenderMap_DListParser(true, TempDLOffset / 4);
	}

	return 0;
}

/* VIEWER_RENDERMAP_CMDLOADTLUT - G_LOADTLUT - GET PALETTE FOR CI TEXTURES FROM PREVIOUS TEXTURE OFFSET */
int Viewer_RenderMap_CMDLoadTLUT(unsigned int PaletteSrc, unsigned long PaletteOffset)
{
	/* clear the palette buffer */
	memset(PaletteData, 0x00, sizeof(PaletteData));

	/* calculate palette size, 16 or 256, from parameters */
	unsigned int TempPaletteSize = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
	unsigned int PaletteSize = ((TempPaletteSize & 0xFFF000) >> 14) + 1;

	/* copy raw palette into buffer */
	if(Viewer_ZMemCopy(PaletteSrc, PaletteOffset, PaletteData, PaletteSize * 2) == -1) {
		sprintf(ErrorMsg, "Invalid palette data source 0x%02X!", PaletteSrc);
		MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	/* initialize variables for palette conversion */
	unsigned int CurrentPaletteEntry = 0;
	unsigned int SourcePaletteData = 0;
	unsigned int InPalettePosition = 0;

	unsigned int PalLoop = 0;

	unsigned int RExtract = 0;
	unsigned int GExtract = 0;
	unsigned int BExtract = 0;
	unsigned int AExtract = 0;

	for(PalLoop = 0; PalLoop < PaletteSize; PalLoop++) {
		/* get raw rgba5551 data */
		SourcePaletteData = (PaletteData[InPalettePosition] * 0x100) + PaletteData[InPalettePosition + 1];

		/* extract r, g, b and a elements */
		RExtract = (SourcePaletteData & 0xF800);
		RExtract >>= 8;
		GExtract = (SourcePaletteData & 0x07C0);
		GExtract <<= 5;
		GExtract >>= 8;
		BExtract = (SourcePaletteData & 0x003E);
		BExtract <<= 18;
		BExtract >>= 16;

		if((SourcePaletteData & 0x0001)) { AExtract = 0xFF; } else { AExtract = 0x00; }

		/* set the current ci palette index to the rgba values from above */
		Palette[CurrentPaletteEntry].R = RExtract;
		Palette[CurrentPaletteEntry].G = GExtract;
		Palette[CurrentPaletteEntry].B = BExtract;
		Palette[CurrentPaletteEntry].A = AExtract;

		/* go on */
		CurrentPaletteEntry++;
		InPalettePosition += 2;
	}

	return 0;
}

/* VIEWER_RENDERMAP_CMDCALLDLIST - F3DEX2_DL - CALL AND RENDER ADDITIONAL DISPLAY LISTS FROM INSIDE OTHERS */
int Viewer_RenderMap_CMDCallDList(bool GetDLFromZMapScene)
{
	if(Readout_CurrentByte5 == 0x03) {
		unsigned long TempOffset;

		TempOffset = Readout_CurrentByte6 << 16;
		TempOffset = TempOffset + (Readout_CurrentByte7 << 8);
		TempOffset = TempOffset + Readout_CurrentByte8;

		SubDLCall = true;
		Viewer_RenderMap_DListParser(true, TempOffset / 4);
	}

	return 0;
}

//char ShaderArray[8192];
int buildFragmentShader()
{
	//char *ShaderString=&ShaderArray[0];
	char ShaderString[8192];
	sprintf(ShaderString,"!!ARBfp1.0\n");
	sprintf(ShaderString,"%sTEMP R0;\n",ShaderString);
    sprintf(ShaderString,"%sTEMP R1;\n",ShaderString);
    sprintf(ShaderString,"%sTEMP aR0;\n",ShaderString);
    sprintf(ShaderString,"%sTEMP aR1;\n",ShaderString);
    sprintf(ShaderString,"%sPARAM envcolor = program.env[0];\n",ShaderString);
    sprintf(ShaderString,"%sPARAM primcolor = program.env[1];\n",ShaderString);
    switch(cA0)
    {
        case 1: // cTEXEL0
            sprintf(ShaderString,"%sTEX R0, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // cTEXEL1
            sprintf(ShaderString,"%sTEX R0, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // cPRIM
            sprintf(ShaderString,"%sMOV R0, primcolor;\n",ShaderString);
/* !!! */	HACK_UseColorSource = 0;
            break;
        case 4: // cSHADE
            sprintf(ShaderString,"%sMOV R0, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // cENV
            sprintf(ShaderString,"%sMOV R0, envcolor;\n",ShaderString);
/* !!! */	HACK_UseColorSource = 1;
            break;
        case 6: // 1.0
            sprintf(ShaderString,"%sMOV R0, {1.0,1.0,1.0,1.0};\n",ShaderString);
/* !!! */	HACK_UseColorSource = 2;
            break;
        default:
            sprintf(ShaderString,"%sMOV R0, {0,0,0,0};\n",ShaderString);
            break;
    }
    switch(cB0)
    {
        case 1: // cTEXEL0
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // cTEXEL1
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // cPRIM
            sprintf(ShaderString,"%sMOV R1, primcolor;\n",ShaderString);
            break;
        case 4: // cSHADE
            sprintf(ShaderString,"%sMOV R1, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // cENV
            sprintf(ShaderString,"%sMOV R1, envcolor;\n",ShaderString);
            break;
        default:
            sprintf(ShaderString,"%sMOV R1, {0,0,0,0};\n",ShaderString);
            break;
    }
    sprintf(ShaderString,"%sSUB R0, R0, R1;\n",ShaderString);
    switch(cC0)
    {
        case 1: // cTEXEL0
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // cTEXEL1
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // cPRIM
            sprintf(ShaderString,"%sMOV R1, primcolor;\n",ShaderString);
            break;
        case 4: // cSHADE
            sprintf(ShaderString,"%sMOV R1, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // cENV
            sprintf(ShaderString,"%sMOV R1, envcolor;\n",ShaderString);
            break;
        case 8: // aTEXEL0
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            sprintf(ShaderString,"%sMOV R1, R1.aaaa;\n",ShaderString);
            break;
        case 9: // aTEXEL1
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            sprintf(ShaderString,"%sMOV R1, R1.aaaa;\n",ShaderString);
            break;
        case 10: // aPRIM
            sprintf(ShaderString,"%sMOV R1, primcolor;\n",ShaderString);
            sprintf(ShaderString,"%sMOV R1, R1.aaaa;\n",ShaderString);
            break;
        case 11: // aSHADE
            sprintf(ShaderString,"%sMOV R1, fragment.color.primary;\n",ShaderString);
            sprintf(ShaderString,"%sMOV R1, R1.aaaa;\n",ShaderString);
            break;
        case 12: // aENV
            sprintf(ShaderString,"%sMOV R1, envcolor;\n",ShaderString);
            sprintf(ShaderString,"%sMOV R1, R1.aaaa;\n",ShaderString);
            break;
        default:
            sprintf(ShaderString,"%sMOV R1, {0,0,0,0};\n",ShaderString);
            break;

    }
    sprintf(ShaderString,"%sMUL R0, R0, R1;\n",ShaderString);
    switch(cD0)
    {
        case 1: // cTEXEL0
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // cTEXEL1
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // cPRIM
            sprintf(ShaderString,"%sMOV R1, primcolor;\n",ShaderString);
            break;
        case 4: // cSHADE
            sprintf(ShaderString,"%sMOV R1, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // cENV
            sprintf(ShaderString,"%sMOV R1, envcolor;\n",ShaderString);
            break;
        case 6: // 1.0
            sprintf(ShaderString,"%sMOV R1, {1.0,1.0,1.0,1.0};\n",ShaderString);
            break;
        default:
            sprintf(ShaderString,"%sMOV R1, {0,0,0,0};\n",ShaderString);
            break;
    }
    sprintf(ShaderString,"%sADD R0, R0, R1;\n",ShaderString);

    switch(aA0)
    {
        case 1: // aTEXEL0
            sprintf(ShaderString,"%sTEX aR0, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // aTEXEL1
            sprintf(ShaderString,"%sTEX aR0, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // aPRIM
            sprintf(ShaderString,"%sMOV aR0, primcolor;\n",ShaderString);
/* !!! */	HACK_UseAlphaSource = 0;
            break;
        case 4: // aSHADE
            sprintf(ShaderString,"%sMOV aR0, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // aENV
            sprintf(ShaderString,"%sMOV aR0, envcolor;\n",ShaderString);
/* !!! */	HACK_UseAlphaSource = 1;
            break;
        case 6: // 1.0
            sprintf(ShaderString,"%sMOV aR0, {1.0,1.0,1.0,1.0};\n",ShaderString);
/* !!! */	HACK_UseAlphaSource = 2;
            break;
        default:
            sprintf(ShaderString,"%sMOV aR0, {0,0,0,0};\n",ShaderString);
            break;
    }

    switch(aB0)
    {
        case 1: // aTEXEL0
            sprintf(ShaderString,"%sTEX aR1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // aTEXEL1
            sprintf(ShaderString,"%sTEX aR1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // aPRIM
            sprintf(ShaderString,"%sMOV aR1, primcolor;\n",ShaderString);
            break;
        case 4: // aSHADE
            sprintf(ShaderString,"%sMOV aR1, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // aENV
            sprintf(ShaderString,"%sMOV aR1, envcolor;\n",ShaderString);
            break;
        case 6: // 1.0
            sprintf(ShaderString,"%sMOV aR1, {1.0,1.0,1.0,1.0};\n",ShaderString);
            break;
        default:
            sprintf(ShaderString,"%sMOV aR1, {0,0,0,0};\n",ShaderString);
            break;
    }
    sprintf(ShaderString,"%sSUB aR0, aR0, aR1;\n",ShaderString);
    switch(aC0)
    {
        case 1: // aTEXEL0
            sprintf(ShaderString,"%sTEX aR1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // aTEXEL1
            sprintf(ShaderString,"%sTEX aR1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // aPRIM
            sprintf(ShaderString,"%sMOV aR1, primcolor;\n",ShaderString);
            break;
        case 4: // aSHADE
            sprintf(ShaderString,"%sMOV aR1, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // aENV
            sprintf(ShaderString,"%sMOV aR1, envcolor;\n",ShaderString);
            break;
        default:
            sprintf(ShaderString,"%sMOV aR1, {0,0,0,0};\n",ShaderString);
            break;
    }
    sprintf(ShaderString,"%sMUL aR0, aR0, aR1;\n",ShaderString);
    switch(aD0)
    {
        case 1: // aTEXEL0
            sprintf(ShaderString,"%sTEX aR1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // aTEXEL1
            sprintf(ShaderString,"%sTEX aR1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // aPRIM
            sprintf(ShaderString,"%sMOV aR1, primcolor;\n",ShaderString);
            break;
        case 4: // aSHADE
            sprintf(ShaderString,"%sMOV aR1, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // aENV
            sprintf(ShaderString,"%sMOV aR1, envcolor;\n",ShaderString);
            break;
        case 6: // 1.0
            sprintf(ShaderString,"%sMOV aR1, {1.0,1.0,1.0,1.0};\n",ShaderString);
            break;
        default:
            sprintf(ShaderString,"%sMOV aR1, {0,0,0,0};\n",ShaderString);
            break;
    }
    sprintf(ShaderString,"%sADD aR0, aR0, aR1;\n",ShaderString);
    sprintf(ShaderString,"%sMOV R0.a, aR0.a;\n",ShaderString);
    sprintf(ShaderString,"%sMOV result.color, R0;\n",ShaderString);
    sprintf(ShaderString,"%sEND\n",ShaderString);

	/*sprintf(ShaderString, "!!ARBfp1.0\n" \
		"\tTEMP color;\n" \
		"\tMUL color, fragment.texcoord[0].y, 2.0;\n" \
		"\tADD color, 1.0, -color;\n" \
		"\tABS color, color;\n" \
		"\tADD result.color, 1.0, -color;\n" \
		"\tMOV result.color.a, 1.0;\n" \
		"END\n");
*/
	if(GLExtension_FragmentProgram) {
//		sprintf(ErrorMsg, "\nFRAGMENT SHADER!\n%s\n", ShaderString);
//		HelperFunc_LogMessage(2, ErrorMsg);

//		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		glGenProgramsARB(1, &fragProg);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fragProg);
		glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(ShaderString), ShaderString);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fragProg);
	}

    return 0;
}

int Viewer_RenderMap_CMDSetCombine()
{
    unsigned int COMBINE0 =	        (Readout_CurrentByte2 * 0x10000) +
									(Readout_CurrentByte3 * 0x100) +
									(Readout_CurrentByte4);

    unsigned int COMBINE1  =	    (Readout_CurrentByte5 * 0x1000000) +
									(Readout_CurrentByte6 * 0x10000) +
									(Readout_CurrentByte7 * 0x100) +
									 Readout_CurrentByte8;
    cA0=(COMBINE0>>20)&0xF;
    cB0=(COMBINE1>>28)&0xF;
    cC0=(COMBINE0>>15)&0x1F;
    cD0=(COMBINE1>>15)&0x7;

    aA0=(COMBINE0>>12)&0x7;
    aB0=(COMBINE1>>12)&0x7;
    aC0=(COMBINE0>>9)&0x7;
    aD0=(COMBINE1>>9)&0x7;

    cA1=(COMBINE0>>5)&0xF;
    cB1=(COMBINE1>>24)&0xF;
    cC1=(COMBINE0>>0)&0x1F;
    cD1=(COMBINE1>>6)&0x7;

    aA1=(COMBINE1>>21)&0x7;
    aB1=(COMBINE1>>3)&0x7;
    aC1=(COMBINE1>>18)&0x7;
    aD1=(COMBINE1>>0)&0x7;

    buildFragmentShader();

    return 0;

}

int Viewer_RenderMap_CMDSetOtherModeH()
{
	unsigned long TempExtract0 =	(Readout_CurrentByte1 * 0x1000000) +
									(Readout_CurrentByte2 * 0x10000) +
									(Readout_CurrentByte3 * 0x100) +
									Readout_CurrentByte4;

	unsigned long TempExtract1 =	(Readout_CurrentByte5 * 0x1000000) +
									(Readout_CurrentByte6 * 0x10000) +
									(Readout_CurrentByte7 * 0x100) +
									Readout_CurrentByte8;

	switch((TempExtract0 >> 8) & 0xFF) {
		case 0x14:
			RDPCycleMode = ((TempExtract1 >> 0x14) & 0x03);
			break;
		default:
			//unknown mode
			break;
	}

	return 0;
}

int Viewer_RenderMap_CMDSetOtherModeL()
{
    unsigned int LowBits =	        (Readout_CurrentByte2 * 0x10000) +
									(Readout_CurrentByte3 * 0x100) +
									(Readout_CurrentByte4);

    unsigned int HighBits  =	    (Readout_CurrentByte5 * 0x1000000) +
									(Readout_CurrentByte6 * 0x10000) +
									(Readout_CurrentByte7 * 0x100) +
									 Readout_CurrentByte8;

	byte MDSFT = 32 - ((LowBits >> 8) & 0xFF) - (LowBits & 0xFF) - 1;

	bool AA_EN			= (HighBits & 0x00000008) != 0;		/* anti-aliasing */
	bool Z_CMP			= (HighBits & 0x00000010) != 0;		/* zbuffer compare */
	bool Z_UPD			= (HighBits & 0x00000020) != 0;		/* zbuffer update */
	bool IM_RD			= (HighBits & 0x00000040) != 0;		/* reduced anti-aliasing ?? */
	bool CLR_ON_CVG		= (HighBits & 0x00000080) != 0;
	bool CVG_DST_WRAP	= (HighBits & 0x00000100) != 0;
	bool CVG_DST_FULL	= (HighBits & 0x00000200) != 0;
	bool CVG_DST_SAVE	= (HighBits & 0x00000300) != 0;
	bool ZMODE_INTER	= (HighBits & 0x00000400) != 0;
	bool ZMODE_XLU		= (HighBits & 0x00000800) != 0;
	bool ZMODE_DEC		= (HighBits & 0x00000C00) != 0;
	bool CVG_X_ALPHA	= (HighBits & 0x00001000) != 0;
	bool ALPHA_CVG_SEL	= (HighBits & 0x00002000) != 0;
	bool FORCE_BL		= (HighBits & 0x00004000) != 0;		/* force alpha blending */

	switch (MDSFT)
	{
		case 0: // alphacompare
			MessageBox(hwnd, "alphacompare", "", 0);
			break;
		case 2: // zsrcsel
			MessageBox(hwnd, "zsrcsel", "", 0);
			break;
		case 3: // rendermode
			if(Z_CMP)		{ glEnable(GL_DEPTH_TEST); } else { glDisable(GL_DEPTH_TEST); }
			if(Z_UPD)		{ glDepthMask(GL_TRUE); } else { glDepthMask(GL_FALSE); }
			if(ZMODE_DEC)	{ glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset(-1.0f, -1.0f); } else { glDisable(GL_POLYGON_OFFSET_FILL); }
			if(FORCE_BL)	{ glEnable(GL_BLEND); } else { /*glDisable(GL_BLEND);*/ }

/* ??? */	if(ZMODE_XLU)	{ glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glAlphaFunc(GL_GREATER, 0.0f); } else { glBlendFunc(GL_ONE, GL_ZERO); glAlphaFunc(GL_GEQUAL, 0.5f); }

			sprintf(ErrorMsg, \
				"\nSETOTHERMODE_L - RENDERMODE:\n" \
				" - Display List #%d\n" \
				" ------------------\n" \
				" - AA_EN %d\n" \
				" - Z_CMP %d, Z_UPD %d, IM_RD %d, CLR_ON_CVG %d\n" \
				" - CVG_DST_WRAP %d, CVG_DST_FULL %d, CVG_DST_SAVE %d, ZMODE_INTER %d\n" \
				" - ZMODE_XLU %d, ZMODE_DEC %d\n" \
				" - CVG_X_ALPHA %d, ALPHA_CVG_SEL %d, FORCE_BL %d\n\n",
					DLToRender,
					AA_EN,
					Z_CMP, Z_UPD, IM_RD, CLR_ON_CVG,
					CVG_DST_WRAP, CVG_DST_FULL, CVG_DST_SAVE, ZMODE_INTER,
					ZMODE_XLU, ZMODE_DEC,
					CVG_X_ALPHA, ALPHA_CVG_SEL, FORCE_BL);
			HelperFunc_LogMessage(2, ErrorMsg);

			break;
		case 16: // blender
			MessageBox(hwnd, "blender", "", 0);
			break;
	}

	//FOR CORRECTNESS WE WILL NEED TO EVENTUALLY MIGRATE ALL BELOW FUNCTIONS TO THE ABOVE SWITCH BLOCK!!!

/*	Blender_Cycle1 = HighBits >> 16;
	Blender_Cycle2 = (HighBits << 16) >> 16;

	GLenum Blender_SrcFactor =		GL_SRC_ALPHA;
	GLenum Blender_DstFactor =		GL_ONE_MINUS_SRC_ALPHA;
	GLenum Blender_AlphaFunc =		GL_GEQUAL;
	GLclampf Blender_AlphaRef =		0.5f;

	switch (Blender_Cycle1 + Blender_Cycle2) {
		case 0x0055 + 0x2048:
		case 0xC811 + 0x2078:								//no blending
		case 0xC811 + 0x3078:
		case 0x0C19 + 0x2078:
		case 0x0C19 + 0x3078:
			Blender_SrcFactor = GL_ONE;
			Blender_DstFactor = GL_ZERO;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.5f;
			break;

		case 0xC810 + 0x3478:								//syotes2 - everything
			Blender_SrcFactor = GL_ONE;
			Blender_DstFactor = GL_ZERO;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.5f;
			break;
		case 0xC810 + 0x49D8:								//spot00 + most other maps - water (does not always work)
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GREATER;
			Blender_AlphaRef = 0.0f;
			break;
		case 0x0C18 + 0x49D8:								//spot03 - water at waterfall
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_DST_COLOR;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.5f;
			break;
		case 0xC810 + 0x4A50:								//MAJORA north clocktown etc - misc things (nct: greenery on walls)
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GREATER;
			Blender_AlphaRef = 0.0f;
			break;
		case 0x0050 + 0x4B50:
		case 0x0C18 + 0x4B50:								//
		case 0xC810 + 0x4B50:								//spot00 - death mountain plane, spot04 - drawing at link's house
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.4f;
			break;
		case 0xC810 + 0x4DD8:								//spot00 - (used near path to gerudo valley?)
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GREATER;
			Blender_AlphaRef = 0.0f;
			break;
		case 0x0C18 + 0x4DD8:								//spot11 - around oasis
			Blender_SrcFactor = GL_ONE;
			Blender_DstFactor = GL_ZERO;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.5f;
			break;
		case 0x0C18 + 0x4F50:								//
		case 0xC810 + 0x4F50:								//spot00, spot02, spot04 - pathways
		case 0xC818 + 0x4F50:								//
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_NOTEQUAL;
			Blender_AlphaRef = 0.0f;
			break;
		case 0xC811 + 0x2D58:								//spot01 - doorways
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GREATER;
			Blender_AlphaRef = 0.0f;
			break;
		default:
			sprintf(ErrorMsg, "Unsupported mode 0x%04X 0x%04X.\nUsing default settings...", (unsigned int)Blender_Cycle1, (unsigned int)Blender_Cycle2);
			MessageBox(hwnd, ErrorMsg, "SetOtherMode_L Error", MB_OK | MB_ICONERROR);
			break;
	}

	glBlendFunc(Blender_SrcFactor, Blender_DstFactor);
	glAlphaFunc(Blender_AlphaFunc, Blender_AlphaRef);
*/
	return 0;
}

int Viewer_RenderMap_CMDMatrix()
{
	/* hacky implementation of F3DEX2_MTX - doesn't bloddy care about parameters or whatever */
	/* we just get a matrix, multiply it with the existing one and be done with it*/

	/* get shit ready */
	int i = 0, j = 0, RawMatrixCnt = 0;
	signed long TempMatrixData1 = 0, TempMatrixData2 = 0;

	/* get the matrix data offset */
	unsigned int MtxSegment = Readout_CurrentByte5;
	unsigned long MtxOffset = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	/* make some space for the raw data */
	unsigned char * RawMatrixData = (unsigned char *) malloc (sizeof(char) * 0x40);

	/* and get more shit ready */
	GLfloat Matrix[4][4];
	float fRecip = 1.0f / 65536.0f;

	/* no idea what segments 0x0C & 0x0D are, so let's just return when we get those */
	if((MtxSegment == 0x0C) || (MtxSegment == 0x0D)) {
		//????
		return 0;
	}

	/* also, if the dlist tries to read from ram, just pop the ogl matrix and return */
	/* (wonder if those 0x80 references are related to the wobblyness in jabu-jabu?) */
	if(!(MtxSegment == 0x80)) {
		/* load the raw data into the buffer */
		if(Viewer_ZMemCopy(MtxSegment, MtxOffset, RawMatrixData, 16 * 4) == -1) {
			sprintf(ErrorMsg, "Raw matrix data load FAILED! Offset 0x%02X|%06X", MtxSegment, MtxOffset);
			MessageBox(hwnd, ErrorMsg, "", MB_OK | MB_ICONERROR);
			return 0;
		}
	} else {
		/* here's the pop */
		glPopMatrix();
		return 0;
	}

	sprintf(ErrorMsg, "MATRIX: offset 0x%02X|%06X\n\n", MtxSegment, MtxOffset);

	/* getting ready to convert the raw data into a ogl compatible matrix */
	/* (the original daedalus graphics 0.08 source helped in understanding the raw data layout) */
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			TempMatrixData1 = ((RawMatrixData[RawMatrixCnt		] * 0x100) + RawMatrixData[RawMatrixCnt + 1		]);
			TempMatrixData2 = ((RawMatrixData[RawMatrixCnt + 32	] * 0x100) + RawMatrixData[RawMatrixCnt + 33	]);
			Matrix[i][j] = ((TempMatrixData1 << 16) | TempMatrixData2) * fRecip;

			sprintf(ErrorMsg, "%s[%12.5f]", ErrorMsg, Matrix[i][j]);

			RawMatrixCnt+=2;
		}
		sprintf(ErrorMsg, "%s\n", ErrorMsg);
	}

	HelperFunc_LogMessage(2, ErrorMsg);
	memset(ErrorMsg, 0x00, sizeof(ErrorMsg));

	/* now push the matrix, multiply the existing one with the one we just loaded */
	glPushMatrix();
	glMultMatrixf(*Matrix);

	return 0;
}

int Viewer_RenderObject_HACKSelectClrAlpSource()
{
	/* this allows selecting which color or alpha values to use without working combiner emulation.
	   it can however only select either prim, env or full 1.0f for either color or alpha, not multiply values or anything...
	   it MIGHT be possible to have it do color combining, but we should rather get the actual combiner emulation to work */

	float TempAlpha = 0.0f;
	switch(HACK_UseAlphaSource) {
		case 0: TempAlpha = PrimColor[3]; break;	/* prim alpha */
		case 1: TempAlpha = EnvColor[3]; break;		/* env alpha */
		case 2: TempAlpha = 1.0f; break;			/* full */

		default: TempAlpha = 1.0f; break;			/* default to full */
	}
	switch(HACK_UseColorSource) {
		case 0: glColor4f(PrimColor[0], PrimColor[1], PrimColor[2], TempAlpha); break;		/* prim color + previous alpha */
		case 1: glColor4f(EnvColor[0], EnvColor[1], EnvColor[2], TempAlpha); break;			/* env color + previous alpha */
		case 2: glColor4f(1.0f, 1.0f, 1.0f, TempAlpha); break;								/* full + previous alpha */

		default: glColor4f(1.0f, 1.0f, 0.0f, TempAlpha); break;								/* full + previous alpha */
	}
}

/*	------------------------------------------------------------ */

/* VIEWER_LOADTEXTURE - FETCH THE TEXTURE DATA AND CREATE AN OGL TEXTURE */
GLuint Viewer_LoadTexture(int TextureID)
{
	/* for now, don't do any texturing for objects - REALLY crashy-crashy atm */
	if(!GetDLFromZMapScene) return 0;

	if((Readout_NextGFXCommand1 == 0x00000003) || (Readout_NextGFXCommand1 == 0x000000E1)) return 0;

/*	sprintf(ErrorMsg, "TEXTURE %d:\n \
	Height %d, HeightRender %d, Width %d, WidthRender %d\n \
	DataSource %x, PalDataSource %x, Offset %x, PalOffset %x\n \
	Format_N64 %x, Format_OGL %x, Format_OGLPixel %x\n \
	Y_Parameter %d, X_Parameter %d, S_Scale %d, T_Scale %d\n \
	LineSize %d, Palette %d",
						TextureID,
						Texture[TextureID].Height, Texture[TextureID].HeightRender, Texture[TextureID].Width, Texture[TextureID].WidthRender,
						Texture[TextureID].DataSource, Texture[TextureID].PalDataSource, (unsigned int)Texture[TextureID].Offset, (unsigned int)Texture[TextureID].PalOffset,
						Texture[TextureID].Format_N64, Texture[TextureID].Format_OGL, Texture[TextureID].Format_OGLPixel,
						Texture[TextureID].Y_Parameter, Texture[TextureID].X_Parameter, Texture[TextureID].S_Scale, Texture[TextureID].T_Scale,
						Texture[TextureID].LineSize, Texture[TextureID].Palette);

	MessageBox(hwnd, ErrorMsg, "Texture", MB_OK | MB_ICONINFORMATION);
	HelperFunc_LogMessage(2, ErrorMsg);
*/
	int i = 0, j = 0;

	unsigned long TextureBufferSize = (Texture[TextureID].Height * Texture[TextureID].Width) * 0x08;

	bool UnhandledTextureSource = false;

	TextureData_OGL = (unsigned char *) malloc (TextureBufferSize);
	TextureData_N64 = (unsigned char *) malloc (TextureBufferSize);

	memset(TextureData_OGL, 0x00, TextureBufferSize);
	memset(TextureData_N64, 0x00, TextureBufferSize);

	Debug_MallocOperations++;
	Debug_MallocOperations++;

	/* create solid red texture for unsupported stuff, such as kakariko windows */
	unsigned char * EmptyTexture_Red;
	unsigned char * EmptyTexture_Green;

	EmptyTexture_Red = (unsigned char *) malloc (sizeof(char) * TextureBufferSize);
	EmptyTexture_Green = (unsigned char *) malloc (sizeof(char) * TextureBufferSize);

	Debug_MallocOperations++;
	Debug_MallocOperations++;

	for(i = 0; i < TextureBufferSize; i+=4) {
		EmptyTexture_Red[i]			= 0xFF;
		EmptyTexture_Red[i + 1]		= 0x00;
		EmptyTexture_Red[i + 2]		= 0x00;
		EmptyTexture_Red[i + 3]		= 0xFF;

		EmptyTexture_Green[i]		= 0x00;
		EmptyTexture_Green[i + 1]	= 0xFF;
		EmptyTexture_Green[i + 2]	= 0x00;
		EmptyTexture_Green[i + 3]	= 0xFF;
	}

	if(Viewer_ZMemCopy(Texture[TextureID].DataSource, Texture[TextureID].Offset, TextureData_N64, TextureBufferSize / 4) == -1) {
		UnhandledTextureSource = true;
//		sprintf(ErrorMsg, "Unhandled texture source 0x%02X|%06X!", Texture[TextureID].DataSource, Texture[TextureID].Offset);
//		MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
		Texture[TextureID].Format_OGL = GL_RGBA;
		Texture[TextureID].Format_OGLPixel = GL_RGBA;
		memcpy(TextureData_OGL, EmptyTexture_Red, TextureBufferSize);
	}

	if(!UnhandledTextureSource) {
		switch(Texture[TextureID].Format_N64) {
		/* RGBA FORMAT */
		case 0x00:
		case 0x08:
		case 0x10:
			{
			unsigned int LoadRGBA_RGBA5551 = 0;

			unsigned int LoadRGBA_RExtract = 0;
			unsigned int LoadRGBA_GExtract = 0;
			unsigned int LoadRGBA_BExtract = 0;
			unsigned int LoadRGBA_AExtract = 0;

			unsigned int LoadRGBA_InTexturePosition_N64 = 0;
			unsigned int LoadRGBA_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width; i++) {
					LoadRGBA_RGBA5551 = (TextureData_N64[LoadRGBA_InTexturePosition_N64] * 0x100) + TextureData_N64[LoadRGBA_InTexturePosition_N64 + 1];

					LoadRGBA_RExtract = (LoadRGBA_RGBA5551 & 0xF800);
					LoadRGBA_RExtract >>= 8;
					LoadRGBA_GExtract = (LoadRGBA_RGBA5551 & 0x07C0);
					LoadRGBA_GExtract <<= 5;
					LoadRGBA_GExtract >>= 8;
					LoadRGBA_BExtract = (LoadRGBA_RGBA5551 & 0x003E);
					LoadRGBA_BExtract <<= 18;
					LoadRGBA_BExtract >>= 16;

					if((LoadRGBA_RGBA5551 & 0x0001)) {
						LoadRGBA_AExtract = 0xFF;
					} else {
						LoadRGBA_AExtract = 0x00;
					}

					TextureData_OGL[LoadRGBA_InTexturePosition_OGL]     = LoadRGBA_RExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 1] = LoadRGBA_GExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 2] = LoadRGBA_BExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 3] = LoadRGBA_AExtract;

					LoadRGBA_InTexturePosition_N64 += 2;
					LoadRGBA_InTexturePosition_OGL += 4;
				}
				LoadRGBA_InTexturePosition_N64 += Texture[TextureID].LineSize * 4 - Texture[TextureID].Width;
			}

			break;
			}
		case 0x18:
			{
			/* 32-bit RGBA - not used in levels */
			unsigned int LoadRGBA_RGBA5551 = 0;

			unsigned int LoadRGBA_RExtract = 0;
			unsigned int LoadRGBA_GExtract = 0;
			unsigned int LoadRGBA_BExtract = 0;
			unsigned int LoadRGBA_AExtract = 0;

			unsigned int LoadRGBA_InTexturePosition_N64 = 0;
			unsigned int LoadRGBA_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width; i++) {
					LoadRGBA_RGBA5551 = (TextureData_N64[LoadRGBA_InTexturePosition_N64] * 0x100) + TextureData_N64[LoadRGBA_InTexturePosition_N64 + 1];

					LoadRGBA_RExtract = (LoadRGBA_RGBA5551 & 0xF800);
					LoadRGBA_RExtract >>= 8;
					LoadRGBA_GExtract = (LoadRGBA_RGBA5551 & 0x07C0);
					LoadRGBA_GExtract <<= 5;
					LoadRGBA_GExtract >>= 8;
					LoadRGBA_BExtract = (LoadRGBA_RGBA5551 & 0x003E);
					LoadRGBA_BExtract <<= 18;
					LoadRGBA_BExtract >>= 16;

					if((LoadRGBA_RGBA5551 & 0x0001)) {
						LoadRGBA_AExtract = 0xFF;
					} else {
						LoadRGBA_AExtract = 0x00;
					}

					TextureData_OGL[LoadRGBA_InTexturePosition_OGL]     = LoadRGBA_RExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 1] = LoadRGBA_GExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 2] = LoadRGBA_BExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 3] = LoadRGBA_AExtract;

					LoadRGBA_InTexturePosition_N64 += 2;

					LoadRGBA_RGBA5551 = (TextureData_N64[LoadRGBA_InTexturePosition_N64] * 0x100) + TextureData_N64[LoadRGBA_InTexturePosition_N64 + 1];

					LoadRGBA_RExtract = (LoadRGBA_RGBA5551 & 0xF800);
					LoadRGBA_RExtract >>= 8;
					LoadRGBA_GExtract = (LoadRGBA_RGBA5551 & 0x07C0);
					LoadRGBA_GExtract <<= 5;
					LoadRGBA_GExtract >>= 8;
					LoadRGBA_BExtract = (LoadRGBA_RGBA5551 & 0x003E);
					LoadRGBA_BExtract <<= 18;
					LoadRGBA_BExtract >>= 16;

					if((LoadRGBA_RGBA5551 & 0x0001)) {
						LoadRGBA_AExtract = 0xFF;
					} else {
						LoadRGBA_AExtract = 0x00;
					}

					TextureData_OGL[LoadRGBA_InTexturePosition_OGL]     = LoadRGBA_RExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 1] = LoadRGBA_GExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 2] = LoadRGBA_BExtract;
					TextureData_OGL[LoadRGBA_InTexturePosition_OGL + 3] = LoadRGBA_AExtract;

					LoadRGBA_InTexturePosition_N64 += 2;
					LoadRGBA_InTexturePosition_OGL += 4;
				}
				LoadRGBA_InTexturePosition_N64 += Texture[TextureID].LineSize * 2 - (Texture[TextureID].Width / 2);
			}

			break;
			}
		/* CI FORMAT */
		case 0x40:
		case 0x50:
			{
			unsigned int LoadCI_CIData1 = 0;
			unsigned int LoadCI_CIData2 = 0;

			unsigned int LoadCI_InTexturePosition_N64 = 0;
			unsigned int LoadCI_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width / 2; i++) {
					LoadCI_CIData1 = (TextureData_N64[LoadCI_InTexturePosition_N64] & 0xF0) >> 4;
					LoadCI_CIData2 = TextureData_N64[LoadCI_InTexturePosition_N64] & 0x0F;

					TextureData_OGL[LoadCI_InTexturePosition_OGL]     = Palette[LoadCI_CIData1].R;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 1] = Palette[LoadCI_CIData1].G;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 2] = Palette[LoadCI_CIData1].B;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 3] = Palette[LoadCI_CIData1].A;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 4] = Palette[LoadCI_CIData2].R;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 5] = Palette[LoadCI_CIData2].G;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 6] = Palette[LoadCI_CIData2].B;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 7] = Palette[LoadCI_CIData2].A;

					LoadCI_InTexturePosition_N64 += 1;
					LoadCI_InTexturePosition_OGL += 8;
				}
				LoadCI_InTexturePosition_N64 += Texture[TextureID].LineSize * 8 - (Texture[TextureID].Width / 2);
			}

			break;
			}
		case 0x48:
			{
			unsigned int LoadCI_CIData = 0;

			unsigned int LoadCI_InTexturePosition_N64 = 0;
			unsigned int LoadCI_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width; i++) {
					LoadCI_CIData = TextureData_N64[LoadCI_InTexturePosition_N64];

					TextureData_OGL[LoadCI_InTexturePosition_OGL]     = Palette[LoadCI_CIData].R;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 1] = Palette[LoadCI_CIData].G;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 2] = Palette[LoadCI_CIData].B;
					TextureData_OGL[LoadCI_InTexturePosition_OGL + 3] = Palette[LoadCI_CIData].A;

					LoadCI_InTexturePosition_N64 += 1;
					LoadCI_InTexturePosition_OGL += 4;
				}
				LoadCI_InTexturePosition_N64 += Texture[TextureID].LineSize * 8 - Texture[TextureID].Width;
			}

			break;
			}
		/* IA FORMAT */
		case 0x60:
			{
			unsigned int LoadIA_IAData = 0;

			unsigned int LoadIA_IExtract1 = 0;
			unsigned int LoadIA_AExtract1 = 0;
			unsigned int LoadIA_IExtract2 = 0;
			unsigned int LoadIA_AExtract2 = 0;

			unsigned int LoadIA_InTexturePosition_N64 = 0;
			unsigned int LoadIA_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width / 2; i++) {
					LoadIA_IAData = (TextureData_N64[LoadIA_InTexturePosition_N64] & 0xF0) >> 4;
					LoadIA_IExtract1 = (LoadIA_IAData & 0x0E) << 4;
					if((LoadIA_IAData & 0x01)) { LoadIA_AExtract1 = 0xFF; } else { LoadIA_AExtract1 = 0x00; }

					LoadIA_IAData = (TextureData_N64[LoadIA_InTexturePosition_N64] & 0x0F);
					LoadIA_IExtract2 = (LoadIA_IAData & 0x0E) << 4;
					if((LoadIA_IAData & 0x01)) { LoadIA_AExtract2 = 0xFF; } else { LoadIA_AExtract2 = 0x00; }

					TextureData_OGL[LoadIA_InTexturePosition_OGL]     = LoadIA_IExtract1;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 1] = LoadIA_IExtract1;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 2] = LoadIA_IExtract1;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 3] = LoadIA_AExtract1;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 4] = LoadIA_IExtract2;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 5] = LoadIA_IExtract2;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 6] = LoadIA_IExtract2;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 7] = LoadIA_AExtract2;

					LoadIA_InTexturePosition_N64 += 1;
					LoadIA_InTexturePosition_OGL += 8;
				}
				LoadIA_InTexturePosition_N64 += Texture[TextureID].LineSize * 8 - (Texture[TextureID].Width / 2);
			}

			break;
			}
		case 0x68:
			{
			unsigned int LoadIA_IAData = 0;

			unsigned int LoadIA_IExtract = 0;
			unsigned int LoadIA_AExtract = 0;

			unsigned int LoadIA_InTexturePosition_N64 = 0;
			unsigned int LoadIA_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width; i++) {
					LoadIA_IAData = TextureData_N64[LoadIA_InTexturePosition_N64];
					LoadIA_IExtract = (LoadIA_IAData & 0xFE);
					if((LoadIA_IAData & 0x01)) { LoadIA_AExtract = 0xFF; } else { LoadIA_AExtract = 0x00; }

					TextureData_OGL[LoadIA_InTexturePosition_OGL]     = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 1] = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 2] = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 3] = LoadIA_AExtract;

					LoadIA_InTexturePosition_N64 += 1;
					LoadIA_InTexturePosition_OGL += 4;
				}
				LoadIA_InTexturePosition_N64 += Texture[TextureID].LineSize * 8 - Texture[TextureID].Width;
			}

			break;
			}
		case 0x70:
			{
			unsigned int LoadIA_IAData = 0;

			unsigned int LoadIA_IExtract = 0;
			unsigned int LoadIA_AExtract = 0;

			unsigned int LoadIA_InTexturePosition_N64 = 0;
			unsigned int LoadIA_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width; i++) {
					LoadIA_IAData = TextureData_N64[LoadIA_InTexturePosition_N64];
					LoadIA_IExtract = LoadIA_IAData;

					LoadIA_IAData = TextureData_N64[LoadIA_InTexturePosition_N64 + 1];
					LoadIA_AExtract = LoadIA_IAData;

					TextureData_OGL[LoadIA_InTexturePosition_OGL]     = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 1] = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 2] = LoadIA_IExtract;
					TextureData_OGL[LoadIA_InTexturePosition_OGL + 3] = LoadIA_AExtract;

					LoadIA_InTexturePosition_N64 += 2;
					LoadIA_InTexturePosition_OGL += 4;
				}
				LoadIA_InTexturePosition_N64 += Texture[TextureID].LineSize * 4 - Texture[TextureID].Width;
			}

			break;
			}
		/* I FORMAT */
		case 0x80:
		case 0x90:
			{
			unsigned int LoadI_IData = 0;

			unsigned int LoadI_IExtract1 = 0;
			unsigned int LoadI_IExtract2 = 0;

			unsigned int LoadI_InTexturePosition_N64 = 0;
			unsigned int LoadI_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width / 2; i++) {
					LoadI_IData = (TextureData_N64[LoadI_InTexturePosition_N64] & 0xF0) >> 4;
					LoadI_IExtract1 = (LoadI_IData & 0x0F) << 4;

					LoadI_IData = (TextureData_N64[LoadI_InTexturePosition_N64] & 0x0F);
					LoadI_IExtract2 = (LoadI_IData & 0x0F) << 4;

					TextureData_OGL[LoadI_InTexturePosition_OGL]     = LoadI_IExtract1;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 1] = LoadI_IExtract1;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 2] = LoadI_IExtract1;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 3] = 0xFF;//(PrimColor[3] * 255);

					TextureData_OGL[LoadI_InTexturePosition_OGL + 4] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 5] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 6] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 7] = 0xFF;//(PrimColor[3] * 255);

					LoadI_InTexturePosition_N64 += 1;
					LoadI_InTexturePosition_OGL += 8;
				}
				LoadI_InTexturePosition_N64 += Texture[TextureID].LineSize * 8 - (Texture[TextureID].Width / 2);
			}

			break;
			}
		case 0x88:
			{
			unsigned int LoadI_IData = 0;

			unsigned int LoadI_InTexturePosition_N64 = 0;
			unsigned int LoadI_InTexturePosition_OGL = 0;

			for(j = 0; j < Texture[TextureID].Height; j++) {
				for(i = 0; i < Texture[TextureID].Width; i++) {
					LoadI_IData = TextureData_N64[LoadI_InTexturePosition_N64];

					TextureData_OGL[LoadI_InTexturePosition_OGL]     = LoadI_IData;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 1] = LoadI_IData;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 2] = LoadI_IData;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 3] = 0xFF;//(PrimColor[3] * 255);

					LoadI_InTexturePosition_N64 += 1;
					LoadI_InTexturePosition_OGL += 4;
				}
				LoadI_InTexturePosition_N64 += Texture[TextureID].LineSize * 8 - Texture[TextureID].Width;
			}

			break;
			}
		/* FALLBACK - gives us an empty texture */
		default:
			{
			sprintf(ErrorMsg, "Unhandled Texture Type 0x%02X!", Texture[TextureID].Format_N64);
			MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
			Texture[TextureID].Format_OGL = GL_RGBA;
			Texture[TextureID].Format_OGLPixel = GL_RGBA;
			memcpy(TextureData_OGL, EmptyTexture_Green, TextureBufferSize);
			break;
			}
		}
	}

	bool SearchingCache = true;
	bool NewTexture = false;
	unsigned int OtherCacheCriteria = 0;

	i = 0;

	if(GetDLFromZMapScene) {
		OtherCacheCriteria = ROM_CurrentMap;
	} else {
		OtherCacheCriteria = ObjectID;
//		i=1024;					/* CACHING BROKEN FOR ACTORS - SKIPPING CACHE VIA I=1024 */
	}

	while(SearchingCache) {
		if((CurrentTextures[i].Offset == Texture[TextureID].Offset) &&
			(CurrentTextures[i].DataSource == Texture[TextureID].DataSource) &&
			(CurrentTextures[i].OtherCriteria == OtherCacheCriteria)) {
				SearchingCache = false;
				TempGLTexture = CurrentTextures[i].GLTextureID;
		} else {
			if(i != 1024) {
				i++;
			} else {
				SearchingCache = false;

				glGenTextures(1, &TempGLTexture);

				CurrentTextures[TexCachePosition].GLTextureID = TempGLTexture;
				CurrentTextures[TexCachePosition].Offset = Texture[TextureID].Offset;
				CurrentTextures[TexCachePosition].DataSource = Texture[TextureID].DataSource;
				CurrentTextures[TexCachePosition].OtherCriteria = OtherCacheCriteria;
				TexCachePosition++;

				NewTexture = true;
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, TempGLTexture);

	if(NewTexture) {
		switch(Texture[TextureID].Y_Parameter) {
			case 1:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); break;
			case 2:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); break;
			case 3:  if(GLExtension_TextureMirror) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB); break;
			default: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); break;
		}

		switch(Texture[TextureID].X_Parameter) {
			case 1:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); break;
			case 2:  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); break;
			case 3:  if(GLExtension_TextureMirror) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB); break;
			default: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); break;
		}

		gluBuild2DMipmaps(GL_TEXTURE_2D, Texture[TextureID].Format_OGL, Texture[TextureID].WidthRender, Texture[TextureID].HeightRender, Texture[TextureID].Format_OGLPixel, GL_UNSIGNED_BYTE, TextureData_OGL);
	} else {
		//
	}

	if(GLExtension_AnisoFilter) {
		float AnisoMax;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &AnisoMax);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, AnisoMax);
	} else {
		//
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Renderer_FilteringMode_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Renderer_FilteringMode_Mag);

	free(TextureData_N64);
	free(TextureData_OGL);

	Debug_FreeOperations++;
	Debug_FreeOperations++;

	free(EmptyTexture_Red);
	free(EmptyTexture_Green);

	Debug_FreeOperations++;
	Debug_FreeOperations++;

	return TempGLTexture;
}

/*	------------------------------------------------------------ */
