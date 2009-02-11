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

		ROM_CurrentMap = i;

		int j = 0;
		for(j = 0; j < DListInfo_CurrentCount[ROM_CurrentMap]; j++) {
			if(Renderer_GLDisplayList_Current != 0) {
				glNewList(Renderer_GLDisplayList_Current + j, GL_COMPILE);
					DLTempPosition = DLists[ROM_CurrentMap][j] / 4;

					SubDLCall = false;

					/* reset stuff set by geometrymode */
					glDisable(GL_FOG);
					glDisable(GL_LIGHTING); glDisable(GL_NORMALIZE); Renderer_EnableLighting = false;
					glDisable(GL_CULL_FACE);

					GetDLFromZMapScene = true;
					Viewer_RenderMap_DListParser(false, j, DLTempPosition);

					PrimColor[0] = 0.0f;
					PrimColor[1] = 0.0f;
					PrimColor[2] = 0.0f;
					PrimColor[3] = 1.0f;

                    EnvColor[0] = 0.0f;
					EnvColor[1] = 0.0f;
					EnvColor[2] = 0.0f;
					EnvColor[3] = 1.0f;

					Blender_Cycle1 = 0x00;
					Blender_Cycle2 = 0x00;

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

		ROM_CurrentMap = i;

		int j = 0;
		if(!(MapHeader[ROM_CurrentMap][MapHeader_Current].Actor_Count) == 0) {
			for(j = 0; j < MapHeader[ROM_CurrentMap][MapHeader_Current].Actor_Count; j++) {
				unsigned int ID = Actors[ROM_CurrentMap][j].Number;

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
											Viewer_RenderMap_DListParser(false, j, DListScanPosition);
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
						glNewList(Renderer_GLDisplayList + Renderer_GLDisplayList_Total + ID, GL_COMPILE);
							glBegin(GL_QUADS);
								glColor3f(0.0f, 1.0f, 0.0f);

								glVertex3s( 12, 12, 12);   //V2
								glVertex3s( 12,-12, 12);   //V1
								glVertex3s( 12,-12,-12);   //V3
								glVertex3s( 12, 12,-12);   //V4

								glVertex3s( 12, 12,-12);   //V4
								glVertex3s( 12,-12,-12);   //V3
								glVertex3s(-12,-12,-12);   //V5
								glVertex3s(-12, 12,-12);   //V6

								glVertex3s(-12, 12,-12);   //V6
								glVertex3s(-12,-12,-12);   //V5
								glVertex3s(-12,-12, 12);   //V7
								glVertex3s(-12, 12, 12);   //V8

								glVertex3s(-12, 12,-12);   //V6
								glVertex3s(-12, 12, 12);   //V8
								glVertex3s( 12, 12, 12);   //V2
								glVertex3s( 12, 12,-12);   //V4

								glVertex3s(-12,-12, 12);   //V7
								glVertex3s(-12,-12,-12);   //V5
								glVertex3s( 12,-12,-12);   //V3
								glVertex3s( 12,-12, 12);   //V1

								//front
								glColor3f(1.0f, 1.0f, 1.0f);

								glVertex3s(-12, 12, 12);   //V8
								glVertex3s(-12,-12, 12);   //V7
								glVertex3s( 12,-12, 12);   //V1
								glVertex3s( 12, 12, 12);   //V2
							glEnd();
						glEndList();
					}

					if(TempActorBuffer_Allocated) { free(TempActorBuffer); TempActorBuffer_Allocated = false; Debug_FreeOperations++; }
				}
			}
		}
	}
*/
	AreaLoaded = true;

	fclose(FileGFXLog); GFXLogOpened = false;

//	sprintf(ErrorMsg, "%d malloc operations, %d free operations while loading map", Debug_MallocOperations, Debug_FreeOperations);
//	MessageBox(hwnd, ErrorMsg, "Memory", MB_OK | MB_ICONINFORMATION);

	return 0;
}

/*	------------------------------------------------------------ */

/* VIEWER_RENDERMAP_DLISTPARSER - DLIST INTERPRETER MAIN LOOP, SCANS AND EXECUTES DLIST COMMANDS */
int Viewer_RenderMap_DListParser(bool CalledFromRDPHalf, unsigned int DLToRender, unsigned long Position)
{
	if(CalledFromRDPHalf) {
		sprintf(GFXLogMsg, "  [DList called via RDPHALF_1 (0x%08X)]\n", (unsigned int)Position * 4);
		HelperFunc_LogMessage(1, GFXLogMsg);
	} else {
		sprintf(GFXLogMsg, "Display List #%d (0x%08X):\n", DLToRender + 1, (unsigned int)Position * 4);
		HelperFunc_LogMessage(1, GFXLogMsg);
	}

	while (!DListHasEnded) {
		if(GetDLFromZMapScene) {
			/* get data from map */
			memcpy(&Readout_Current1, &ZMapBuffer[ROM_CurrentMap][Position], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[ROM_CurrentMap][Position + 1], 4);

			memcpy(&Readout_NextGFXCommand1, &ZMapBuffer[ROM_CurrentMap][Position + 2], 4);

			HelperFunc_SplitCurrentVals(true);
		} else {
			/* get data from temp object */
			memcpy(&Readout_Current1, &TempObjectBuffer[Position], 4);
			memcpy(&Readout_Current2, &TempObjectBuffer[Position + 1], 4);

			memcpy(&Readout_NextGFXCommand1, &TempObjectBuffer[Position + 2], 4);

			HelperFunc_SplitCurrentVals(true);
		}

		if(CalledFromRDPHalf) HelperFunc_LogMessage(1, " ");

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
			sprintf(CurrentGFXCmdNote, "<partially handled>");
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
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDRDPHalf1_CMDDListStart_CMDDListStart(GetDLFromZMapScene);
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
		case F3DEX2_DL:
			sprintf(CurrentGFXCmd, "F3DEX2_DL            ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
			Viewer_RenderMap_CMDRDPHalf1_CMDDListStart_CMDDListStart(GetDLFromZMapScene);
			break;
		case F3DEX2_BRANCH_Z:
			sprintf(CurrentGFXCmd, "F3DEX2_BRANCH_Z      ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			HelperFunc_GFXLogCommand(Position);
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
			break;
		}

		if(DListHasEnded) {
			if(CalledFromRDPHalf) {
				HelperFunc_LogMessage(1, "  [Return to original DList]\n");
			} else {
				HelperFunc_LogMessage(1, "\n");
			}
		}

		Position+=2;
	}

	if(CalledFromRDPHalf) DListHasEnded = false;

	SubDLCall = false;

	return 0;
}

/* VIEWER_RENDERMAP_CMDVERTEXLIST - F3DEX2_VTX - GET OFFSET, AMOUNT & VERTEX BUFFER POSITION OF VERTICES TO USE */
int Viewer_RenderMap_CMDVertexList()
{
	glEnable(GL_TEXTURE_2D);
	Renderer_GLTexture = Viewer_LoadTexture(0);
	glBindTexture(GL_TEXTURE_2D, Renderer_GLTexture);

	unsigned int VertList_Temp = (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned int TempVertCount = ((VertList_Temp & 0x000FFF) / 2);
	unsigned int TempVertListStartEntry = TempVertCount - ((VertList_Temp & 0xFFF000) >> 12);

	unsigned int TempVertListBank = Readout_CurrentByte5;
	unsigned long TempVertListOffset = Readout_CurrentByte6 << 16;
	TempVertListOffset = TempVertListOffset + (Readout_CurrentByte7 << 8);
	TempVertListOffset = TempVertListOffset + Readout_CurrentByte8;

	Viewer_GetVertexList(TempVertListBank, TempVertListOffset, TempVertCount, TempVertListStartEntry);

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

	while(CurrentVert < VertCount + 1) {
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

	glColor4f(PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);

	glBegin(GL_TRIANGLES);
		TempU = (float) CurrentH1_1 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_1 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_1, CurrentG1_1, CurrentB1_1, CurrentA1_1); }
		glNormal3f (CurrentR1_1 / 255.0f, CurrentG1_1 / 255.0f, CurrentB1_1 / 255.0f);
		glVertex3d(CurrentX1_1, CurrentY1_1, CurrentZ1_1);

		TempU = (float) CurrentH1_2 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_2 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_2, CurrentG1_2, CurrentB1_2, CurrentA1_2); }
		glNormal3f (CurrentR1_2 / 255.0f, CurrentG1_2 / 255.0f, CurrentB1_2 / 255.0f);
		glVertex3d(CurrentX1_2, CurrentY1_2, CurrentZ1_2);

		TempU = (float) CurrentH1_3 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_3 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_3, CurrentG1_3, CurrentB1_3, CurrentA1_3); }
		glNormal3f (CurrentR1_3 / 255.0f, CurrentG1_3 / 255.0f, CurrentB1_3 / 255.0f);
		glVertex3d(CurrentX1_3, CurrentY1_3, CurrentZ1_3);
	glEnd();

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

	glColor4f(PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);

	glBegin(GL_TRIANGLES);
		TempU = (float) CurrentH1_1 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_1 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_1, CurrentG1_1, CurrentB1_1, CurrentA1_1); }
		glNormal3f (CurrentR1_1 / 255.0f, CurrentG1_1 / 255.0f, CurrentB1_1 / 255.0f);
		glVertex3d(CurrentX1_1, CurrentY1_1, CurrentZ1_1);

		TempU = (float) CurrentH1_2 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_2 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_2, CurrentG1_2, CurrentB1_2, CurrentA1_2); }
		glNormal3f (CurrentR1_2 / 255.0f, CurrentG1_2 / 255.0f, CurrentB1_2 / 255.0f);
		glVertex3d(CurrentX1_2, CurrentY1_2, CurrentZ1_2);

		TempU = (float) CurrentH1_3 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV1_3 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR1_3, CurrentG1_3, CurrentB1_3, CurrentA1_3); }
		glNormal3f (CurrentR1_3 / 255.0f, CurrentG1_3 / 255.0f, CurrentB1_3 / 255.0f);
		glVertex3d(CurrentX1_3, CurrentY1_3, CurrentZ1_3);

		TempU = (float) CurrentH2_1 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV2_1 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR2_1, CurrentG2_1, CurrentB2_1, CurrentA2_1); }
		glNormal3f (CurrentR2_1 / 255.0f, CurrentG2_1 / 255.0f, CurrentB2_1 / 255.0f);
		glVertex3d(CurrentX2_1, CurrentY2_1, CurrentZ2_1);

		TempU = (float) CurrentH2_2 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV2_2 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR2_2, CurrentG2_2, CurrentB2_2, CurrentA2_2); }
		glNormal3f (CurrentR2_2 / 255.0f, CurrentG2_2 / 255.0f, CurrentB2_2 / 255.0f);
		glVertex3d(CurrentX2_2, CurrentY2_2, CurrentZ2_2);

		TempU = (float) CurrentH2_3 * Texture[0].S_Scale / 32 / Texture[0].WidthRender;
		TempV = (float) CurrentV2_3 * Texture[0].T_Scale / 32 / Texture[0].HeightRender;
		glTexCoord2f(TempU, TempV);
		if(!Renderer_EnableLighting) { glColor4ub (CurrentR2_3, CurrentG2_3, CurrentB2_3, CurrentA2_3); }
		glNormal3f (CurrentR2_3 / 255.0f, CurrentG2_3 / 255.0f, CurrentB2_3 / 255.0f);
		glVertex3d(CurrentX2_3, CurrentY2_3, CurrentZ2_3);
	glEnd();

	return 0;
}

/* VIEWER_RENDERMAP_CMDTEXTURE - F3DEX2_TEXTURE */
int Viewer_RenderMap_CMDTexture()
{
	Texture[0].S_Scale = ((Readout_CurrentByte5 * 0x100) + Readout_CurrentByte6) + 2;
	Texture[0].T_Scale = ((Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8) + 2;

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

	return 0;
}

/* VIEWER_RENDERMAP_CMDSETTILE - G_SETTILE - GET TEXTURE PROPERTIES AND STORE THEM FOR FUTURE USE (MIRROR, ETC.) */
int Viewer_RenderMap_CMDSetTile()
{
	/* FIX FOR CI TEXTURES - IF BYTES 5-8 ARE 07000000, LEAVE CURRENT PROPERTY SETTINGS ALONE */
	if(Readout_Current2 == 0x00000007) return 0;

	unsigned char TempXParameter = Readout_CurrentByte7 * 0x10;

	switch(Readout_CurrentByte6) {
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

	switch(TempXParameter) {
		case 0x00:
			Texture[0].X_Parameter = 1;
			break;
		case 0x20:
			Texture[0].X_Parameter = 2;
			break;
		case 0x10:
			Texture[0].X_Parameter = 3;
			break;
		default:
			Texture[0].X_Parameter = 1;
			break;
	}

	Texture[0].LineSize = Readout_CurrentByte3 / 2;
	Texture[0].Format_N64 = Readout_CurrentByte2;
	Texture[0].Palette = (Readout_Current1 >> 20) & 0x0F;
	Texture[0].Format_OGL = GL_RGBA;
	Texture[0].Format_OGLPixel = GL_RGBA;

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
	int Convert;
	int Counter, Counter2;
	bool Binary[4];

	/* ---- FOG, LIGHTING, AUTOMATIC TEXTURE MAPPING (SPHERICAL + LINEAR) ---- */
	Convert = Readout_CurrentByte2;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(!Binary[0]) {
		/*disable fog*/ }
		glDisable(GL_FOG);
	if(!Binary[1]) {
		/*disable lighting*/
		glDisable(GL_LIGHTING); glDisable(GL_NORMALIZE); Renderer_EnableLighting = false; }
	if(!Binary[2]) {
		/*disable tex-mapping spherical*/ }
	if(!Binary[3]) {
		/*disable tex-mapping linear*/ }

	Convert = Readout_CurrentByte6;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(Binary[0]) {
		/*enable fog*/ }
//		glEnable(GL_FOG);
	if(Binary[1]) {
		/*enable lighting*/
		glEnable(GL_LIGHTING); glEnable(GL_NORMALIZE); Renderer_EnableLighting = true; }
	if(Binary[2]) {
		/*enable tex-mapping spherical*/ }
	if(Binary[3]) {
		/*disable tex-mapping linear*/ }

	/* ---- FACE CULLING ---- */
	Convert = Readout_CurrentByte3;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(!Binary[0]) {
		/*UNUSED?*/ }
	if(!Binary[1]) {
		/*disable front-face culling*/
		glDisable(GL_CULL_FACE); }
	if(!Binary[2]) {
		/*disable back-face culling*/
		glDisable(GL_CULL_FACE); }
	if(!Binary[3]) {
		/*UNUSED?*/ }

	Convert = Readout_CurrentByte7;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(Binary[0]) {
		/*UNUSED?*/ }
	if(Binary[1]) {
		/*enable front-face culling*/
		glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); }
	if(Binary[2]) {
		/*enable back-face culling*/
		glEnable(GL_CULL_FACE); glCullFace(GL_BACK); }
	if(Binary[3]) {
		/*UNUSED?*/ }

	/* ---- VERTEX COLOR SHADING, Z-BUFFERING ---- */
	Convert = Readout_CurrentByte4;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(!Binary[0]) {
		/*UNUSED?*/ }
	if(!Binary[1]) {
		/*disable vertex color shading*/ }
	if(!Binary[2]) {
		/*disable z-buffering*/
		glDisable(GL_DEPTH_TEST); }
	if(!Binary[3]) {
		/*UNUSED?*/ }

	Convert = Readout_CurrentByte8;
	Counter = 0, Counter2 = 4;
	for(Counter = 0; Counter < 4; Counter++) {
		Binary[Counter] = Convert % 2;
		Convert = Convert / 2;
	}
	if(Binary[0]) {
		/*UNUSED?*/ }
	if(Binary[1]) {
		/*enable vertex color shading*/ }
	if(Binary[2]) {
		/*enable z-buffering*/
		glEnable(GL_DEPTH_TEST); }
	if(Binary[3]) {
		/*UNUSED?*/ }

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

	return 0;
}

/* VIEWER_RENDERMAP_CMDSETPRIMCOLOR - G_SETPRIMCOLOR - SET THE PRIMARY COLOR */
int Viewer_RenderMap_CMDSetPrimColor()
{
	PrimColor[0] = (Readout_CurrentByte5 / 255.0f);
	PrimColor[1] = (Readout_CurrentByte6 / 255.0f);
	PrimColor[2] = (Readout_CurrentByte7 / 255.0f);
	PrimColor[3] = (Readout_CurrentByte8 / 255.0f);
	glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);
	return 0;
}

int Viewer_RenderMap_CMDSetEnvColor()
{
    EnvColor[0] = (Readout_CurrentByte5 / 255.0f);
	EnvColor[1] = (Readout_CurrentByte6 / 255.0f);
	EnvColor[2] = (Readout_CurrentByte7 / 255.0f);
	EnvColor[3] = (Readout_CurrentByte8 / 255.0f);
	glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, EnvColor[0], EnvColor[1], EnvColor[2], PrimColor[3]);
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

/* Viewer_RenderMap_CMDRDPHalf1_CMDDListStart_CMDDListStart - F3DEX2_RDPHALF_1 & F3DEX2_DL - CALL AND RENDER ADDITIONAL DISPLAY LISTS FROM INSIDE OTHERS */
int Viewer_RenderMap_CMDRDPHalf1_CMDDListStart_CMDDListStart(bool GetDLFromZMapScene)
{
	if(Readout_CurrentByte5 == 0x03) {
		unsigned long TempOffset;

		TempOffset = Readout_CurrentByte6 << 16;
		TempOffset = TempOffset + (Readout_CurrentByte7 << 8);
		TempOffset = TempOffset + Readout_CurrentByte8;

		SubDLCall = true;
		Viewer_RenderMap_DListParser(true, 0, TempOffset / 4);
	}

	return 0;
}

char ShaderArray[8192];
int buildFragmentShader()
{
	char *ShaderString=&ShaderArray[0];
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
            break;
        case 4: // cSHADE
            sprintf(ShaderString,"%sMOV R0, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // cENV
            sprintf(ShaderString,"%sMOV R0, envcolor;\n",ShaderString);
            break;
        case 6: // 1.0
            sprintf(ShaderString,"%sMOV R0, {1.0,1.0,1.0,1.0};\n",ShaderString);
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
            break;
        case 4: // aSHADE
            sprintf(ShaderString,"%sMOV aR0, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // aENV
            sprintf(ShaderString,"%sMOV aR0, envcolor;\n",ShaderString);
            break;
        case 6: // 1.0
            sprintf(ShaderString,"%sMOV aR0, {1.0,1.0,1.0,1.0};\n",ShaderString);
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

	if(GLExtension_FragmentProgram) {
//		MessageBox(hwnd,ShaderArray,"FRAGMENT SHADER!", MB_OK | MB_ICONINFORMATION);
//		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		glGenProgramsARB(1, &fragProg);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fragProg);
		glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(ShaderArray), ShaderArray);
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
    byte zmode = 0;
    byte forceblend = 0;

    switch(MDSFT)
    {
        case 0: // alphacompare
            break;
        case 2: //zsrcsel
            break;
        case 3: // rendermode
            zmode = (HighBits >> 10) &3;
            forceblend = (HighBits >> 14) &1;
            if(zmode==3){glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset(-1.0,-1.0);} else {glDisable(GL_POLYGON_OFFSET_FILL);}
            if(forceblend==1){glEnable(GL_BLEND);}else{glDisable(GL_BLEND);}
            break;
        case 16: // blender
            break;
    }

    //FOR CORRECTNESS WE WILL NEED TO EVENTUALLY MIGRATE ALL BELOW FUNCTIONS TO THE ABOVE SWITCH BLOCK!!!

	Blender_Cycle1 = HighBits >> 16;
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

	return 0;
}

/*	------------------------------------------------------------ */

/* VIEWER_LOADTEXTURE - FETCH THE TEXTURE DATA AND CREATE AN OGL TEXTURE */
GLuint Viewer_LoadTexture(int TextureID)
{
	/* for now, don't do any texturing for objects - REALLY crashy-crashy atm */
//	if(!GetDLFromZMapScene) return 0;

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
	int i = 0;
	int j = 0;

//	unsigned long TextureBufferSize = 0x10000;
	unsigned long TextureBufferSize = (Texture[TextureID].Height * Texture[TextureID].Width) * 8;

/*	if(TextureBufferSize > 0x10000) {
		sprintf(ErrorMsg, "warning: texture buffer > 0x10000!!\n%d * %d * 4 = 0x%08x", Texture[TextureID].Height, Texture[TextureID].Width, TextureBufferSize);
		MessageBox(hwnd, ErrorMsg, "", 0);
	}
*/
	bool UnhandledTextureSource = false;

	TextureData_OGL = (unsigned char *) malloc (sizeof(char) * TextureBufferSize);
	TextureData_N64 = (unsigned char *) malloc (sizeof(char) * TextureBufferSize);

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
					TextureData_OGL[LoadI_InTexturePosition_OGL + 3] = (PrimColor[3] * 255);

					TextureData_OGL[LoadI_InTexturePosition_OGL + 4] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 5] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 6] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 7] = (PrimColor[3] * 255);

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
					TextureData_OGL[LoadI_InTexturePosition_OGL + 3] = (PrimColor[3] * 255);

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
