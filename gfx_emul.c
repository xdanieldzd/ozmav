/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	gfx_emul.c - Display List interpreter framework
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

int Viewer_RenderMap()
{
	Debug_MallocOperations = 0;
	Debug_FreeOperations = 0;

	/* OPEN GFX COMMAND LOG */
	char Temp[1024];
	sprintf(Temp, "%s\\gfxlog.txt", AppPath);
	if(!GFXLogOpened) FileGFXLog = fopen(Temp, "w"); GFXLogOpened = true;

	/* OPEN COMBINER LOG */
	sprintf(Temp, "%s\\comb.txt", AppPath);
	FileCombinerLog = fopen(Temp, "w");

	int i = 0;
	for(i = 0; i < 257; i++) {
		glDeleteProgramsARB(1, &FPCache[i].FragProg);
		FPCache[i].Combine0 = 0;
		FPCache[i].Combine1 = 0;
		FPCache[i].FragProg = 0;
	}
	FPCachePosition = 0;

	if(Renderer_EnableWavefrontDump) {
		static char WavefrontFilename[] = "";
		sprintf(WavefrontFilename, "%s\\dump\\0x%02X.obj", AppPath, (unsigned int)ROM_SceneToLoad);
		if(!WavefrontObjOpened) FileWavefrontObj = fopen(WavefrontFilename, "w"); WavefrontObjOpened = true;
		sprintf(WavefrontFilename, "%s\\dump\\0x%02X.mtl", AppPath, (unsigned int)ROM_SceneToLoad);
		if(!WavefrontMtlOpened) FileWavefrontMtl = fopen(WavefrontFilename, "w"); WavefrontMtlOpened = true;

		sprintf(WavefrontFilename, "%s\\dump\\0x%02X_col.obj", AppPath, (unsigned int)ROM_SceneToLoad);
		if(!WavefrontObjColOpened) FileWavefrontObjCol = fopen(WavefrontFilename, "w"); WavefrontObjColOpened = true;

		WavefrontObjVertCount = 0;
		WavefrontObjVertCount_Previous = 0;

		WavefrontObjMaterialCnt = 0;

		WavefrontObjColVertCount = 0;
		WavefrontObjColVertCount_Previous = 0;
	}

	/* IF GL DLISTS EXIST, DELETE THEM ALL */
	if(Renderer_GLDisplayList != 0) glDeleteLists(Renderer_GLDisplayList, 4096);
	/* REGENERATE GL DLISTS */
	Renderer_GLDisplayList = glGenLists(4096);

	/* SET GL DLIST BASE */
	glListBase(Renderer_GLDisplayList);
	Renderer_GLDisplayList_Current = Renderer_GLDisplayList;

	MVMatrixCount = 0;

	for(i = 0; i < SceneHeader[SceneHeader_Current].Map_Count; i++) {
		ROM_CurrentMap_Temp = i;

		int j = 0;
		for(j = 0; j < DListInfo_CurrentCount[i]; j++) {
			if(Renderer_GLDisplayList_Current != 0) {
				glNewList(Renderer_GLDisplayList_Current + j, GL_COMPILE_AND_EXECUTE);
					DLTempPosition = DLists[i][j] / 4;

					GetDLFromZMapScene = true;
					DLToRender = j + 1;
					DListParser_CurrentMap = i;
					Viewer_RenderMap_DListParser(false, DLTempPosition);

					DListHasEnded = false;
				glEndList();

				Renderer_GLDisplayList_Total++;
			}
		}

		Renderer_GLDisplayList_Current += j;
	}

	TexCachePosition = 0;
	memset(CurrentTextures, 0x00, sizeof(CurrentTextures));

	Zelda_GetMapCollision(SceneHeader_Current);

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

					sprintf(ErrorMsg, "Object %04X: Actor data @ 0x%08X - 0x%08X (0x%06X bytes)\n", ObjectID, ActorData_Start, ActorData_End, ActorData_Length);
					Helper_LogMessage(2, ErrorMsg);

					memcpy(TempActorBuffer, &ROMBuffer[ActorData_Start / 4], ActorData_Length);

					memcpy(&Readout_Current1, &TempActorBuffer[(ActorData_Length / 4) - 1], 4);
					Helper_SplitCurrentVals(false);

					unsigned long TempDiff = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
					TempOffset = ActorData_Length - TempDiff;

					memcpy(&Readout_Current1, &TempActorBuffer[TempOffset / 4], 4);
					Helper_SplitCurrentVals(false);

					TempOffset = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;

					memcpy(&Readout_Current1, &TempActorBuffer[(TempOffset / 4) + 2], 4);
					Helper_SplitCurrentVals(false);

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

						sprintf(ErrorMsg, "Object %04X: Graphics data @ 0x%08X - 0x%08X (0x%06X bytes)\n", ObjectID, CurrentObject_Start, CurrentObject_End, CurrentObject_Length);
						Helper_LogMessage(2, ErrorMsg);

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

						while ((DListScanPosition <= CurrentObject_Length / 4) && (DListFound == false)) {
							memcpy(&Readout_Current1, &TempObjectBuffer[DListScanPosition], 4);
							memcpy(&Readout_Current2, &TempObjectBuffer[DListScanPosition + 1], 4);

							Helper_SplitCurrentVals(true);

							if(Readout_Current1 == G_RDPPIPESYNC) {
								glNewList(Renderer_GLDisplayList + Renderer_GLDisplayList_Total + ID, GL_COMPILE);
									GetDLFromZMapScene = false;
									if(DListCnt == 0) {
										sprintf(ErrorMsg, "- fake DList entry @ 0x%08X (ROM), 0x06%06X (OBJ)\n", CurrentObject_Start + DListScanPosition * 4, DListScanPosition * 4);
										Helper_LogMessage(2, ErrorMsg);

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

	fclose(FileCombinerLog);
	fclose(FileGFXLog); GFXLogOpened = false;

	if(Renderer_EnableWavefrontDump) {
		fclose(FileWavefrontObj); WavefrontObjOpened = false;
		fclose(FileWavefrontMtl); WavefrontMtlOpened = false;
		fclose(FileWavefrontObjCol); WavefrontObjColOpened = false;
	}

//	sprintf(ErrorMsg, "%d malloc operations, %d free operations while loading map", Debug_MallocOperations, Debug_FreeOperations);
//	MessageBox(hwnd, ErrorMsg, "Memory", MB_OK | MB_ICONINFORMATION);

	return 0;
}

/*	------------------------------------------------------------ */

int Viewer_RenderMap_DListParser(bool CalledViaCmd, unsigned long Position)
{
	if(Renderer_EnableWavefrontDump) {
		sprintf(WavefrontObjMsg, "mltlib 0x%02X.mtl\n", (unsigned int)ROM_SceneToLoad);
		fprintf(FileWavefrontObj, WavefrontObjMsg);
	}

	if(CalledViaCmd) {
		sprintf(GFXLogMsg, "  [Calling DList at 0x%08X]\n", (unsigned int)Position * 4);
		Helper_LogMessage(1, GFXLogMsg);
	} else {
		sprintf(GFXLogMsg, "Display List #%d (0x%08X):\n", DLToRender, (unsigned int)Position * 4);
		Helper_LogMessage(1, GFXLogMsg);
//		Helper_LogMessage(2, GFXLogMsg);
	}

	while (!DListHasEnded) {
		if(GetDLFromZMapScene) {
			/* get data from map */
			memcpy(&Readout_PrevGFXCommand1, &ZMapBuffer[DListParser_CurrentMap][Position - 2], 4);

			memcpy(&Readout_Current1, &ZMapBuffer[DListParser_CurrentMap][Position], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[DListParser_CurrentMap][Position + 1], 4);

			memcpy(&Readout_NextGFXCommand1, &ZMapBuffer[DListParser_CurrentMap][Position + 2], 4);

			Helper_SplitCurrentVals(true);
		} else {
			/* get data from temp object */
			memcpy(&Readout_PrevGFXCommand1, &TempObjectBuffer[Position - 1], 4);

			memcpy(&Readout_Current1, &TempObjectBuffer[Position], 4);
			memcpy(&Readout_Current2, &TempObjectBuffer[Position + 1], 4);

			memcpy(&Readout_NextGFXCommand1, &TempObjectBuffer[Position + 2], 4);

			Helper_SplitCurrentVals(true);
		}

		if(CalledViaCmd) Helper_LogMessage(1, " ");

		DLTempPosition = Position;

		switch(Readout_CurrentByte1) {
		/* 0x01 */
		case F3DEX2_VTX:
			sprintf(CurrentGFXCmd, "F3DEX2_VTX           ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_VTX();
			break;

		/* 0x02 */
		case F3DEX2_MODIFYVTX:
			sprintf(CurrentGFXCmd, "F3DEX2_MODIFYVTX     ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0x03 */
		case F3DEX2_CULLDL:
			sprintf(CurrentGFXCmd, "F3DEX2_CULLDL        ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0x04 */
		case F3DEX2_BRANCH_Z:
			sprintf(CurrentGFXCmd, "F3DEX2_BRANCH_Z      ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_BRANCH_Z();
			break;

		/* 0x05 */
		case F3DEX2_TRI1:
			sprintf(CurrentGFXCmd, "F3DEX2_TRI1          ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_TRI1();
			break;

		/* 0x06 */
		case F3DEX2_TRI2:
			sprintf(CurrentGFXCmd, "F3DEX2_TRI2          ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_TRI2();
			break;

		/* 0x07 */
		case F3DEX2_QUAD:
			sprintf(CurrentGFXCmd, "F3DEX2_QUAD          ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_QUAD();
			break;

		/* -------------------------------- */

		/* 0xD3 */
		case F3DEX2_SPECIAL_3:
			sprintf(CurrentGFXCmd, "F3DEX2_SPECIAL_3     ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xD4 */
		case F3DEX2_SPECIAL_2:
			sprintf(CurrentGFXCmd, "F3DEX2_SPECIAL_2     ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xD5 */
		case F3DEX2_SPECIAL_1:
			sprintf(CurrentGFXCmd, "F3DEX2_SPECIAL_1     ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xD6 */
		case F3DEX2_DMA_IO:
			sprintf(CurrentGFXCmd, "F3DEX2_DMA_IO        ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xD7 */
		case F3DEX2_TEXTURE:
			sprintf(CurrentGFXCmd, "F3DEX2_TEXTURE       ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_TEXTURE();
			break;

		/* 0xD8 */
		case F3DEX2_POPMTX:
			sprintf(CurrentGFXCmd, "F3DEX2_POPMTX        ");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_POPMTX();
			break;

		/* 0xD9 */
		case F3DEX2_GEOMETRYMODE:
			sprintf(CurrentGFXCmd, "F3DEX2_GEOMETRYMODE  ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_GEOMETRYMODE();
			break;

		/* 0xDA */
		case F3DEX2_MTX:
			sprintf(CurrentGFXCmd, "F3DEX2_MTX           ");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_MTX();
			break;

		/* 0xDB */
		case F3DEX2_MOVEWORD:
			sprintf(CurrentGFXCmd, "F3DEX2_MOVEWORD      ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xDC */
		case F3DEX2_MOVEMEM:
			sprintf(CurrentGFXCmd, "F3DEX2_MOVEMEM       ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xDD */
		case F3DEX2_LOAD_UCODE:
			sprintf(CurrentGFXCmd, "F3DEX2_LOAD_UCODE    ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xDE */
		case F3DEX2_DL:
			sprintf(CurrentGFXCmd, "F3DEX2_DL            ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_DL(GetDLFromZMapScene);
			break;

		/* 0xDF */
		case F3DEX2_ENDDL:
			sprintf(CurrentGFXCmd, "F3DEX2_ENDDL         ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			DListHasEnded = true;
			break;

		/* 0xE0 */
		case F3DEX2_SPNOOP:
			sprintf(CurrentGFXCmd, "F3DEX2_SPNOOP        ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xE1 */
		case F3DEX2_RDPHALF_1:
			sprintf(CurrentGFXCmd, "F3DEX2_RDPHALF_1     ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_RDPHALF_1();
			break;

		/* 0xE2 */
		case F3DEX2_SETOTHERMODE_L:
			sprintf(CurrentGFXCmd, "F3DEX2_SETOTHERMODE_L");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_SETOTHERMODE_L();
			break;

		/* 0xE3 */
		case F3DEX2_SETOTHERMODE_H:
			sprintf(CurrentGFXCmd, "F3DEX2_SETOTHERMODE_H");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_SETOTHERMODE_H();
			break;

		/* 0xE4 */
		case G_TEXRECT:
			sprintf(CurrentGFXCmd, "G_TEXRECT            ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xE5 */
		case G_TEXRECTFLIP:
			sprintf(CurrentGFXCmd, "G_TEXRECTFLIP        ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xE6 */
		case G_RDPLOADSYNC:
			sprintf(CurrentGFXCmd, "G_RDPLOADSYNC        ");
			sprintf(CurrentGFXCmdNote, "<ignored>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xE7 */
		case G_RDPPIPESYNC:
			sprintf(CurrentGFXCmd, "G_RDPPIPESYNC        ");
			sprintf(CurrentGFXCmdNote, "<ignored>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xE8 */
		case G_RDPTILESYNC:
			sprintf(CurrentGFXCmd, "G_RDPTILESYNC        ");
			sprintf(CurrentGFXCmdNote, "<ignored>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xE9 */
		case G_RDPFULLSYNC:
			sprintf(CurrentGFXCmd, "G_RDPFULLSYNC        ");
			sprintf(CurrentGFXCmdNote, "<ignored>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xEA */
		case G_SETKEYGB:
			sprintf(CurrentGFXCmd, "G_SETKEYGB           ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xEB */
		case G_SETKEYR:
			sprintf(CurrentGFXCmd, "G_SETKEYR            ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xEC */
		case G_SETCONVERT:
			sprintf(CurrentGFXCmd, "G_SETCONVERT         ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xED */
		case G_SETSCISSOR:
			sprintf(CurrentGFXCmd, "G_SETSCISSOR         ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xEE */
		case G_SETPRIMDEPTH:
			sprintf(CurrentGFXCmd, "G_SETPRIMDEPTH       ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xEF */
		case G_RDPSETOTHERMODE:
			sprintf(CurrentGFXCmd, "G_RDPSETOTHERMODE    ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xF0 */
		case G_LOADTLUT:
			sprintf(CurrentGFXCmd, "G_LOADTLUT           ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_LOADTLUT(Texture[CurrentTextureID].PalDataSource, Texture[CurrentTextureID].PalOffset);
			break;

		/* 0xF1 */
		case F3DEX2_RDPHALF_2:
			sprintf(CurrentGFXCmd, "F3DEX2_RDPHALF_2     ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_RDPHALF_2();
			break;

		/* 0xF2 */
		case G_SETTILESIZE:
			sprintf(CurrentGFXCmd, "G_SETTILESIZE        ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_SETTILESIZE();
			break;

		/* 0xF3 */
		case G_LOADBLOCK:
			sprintf(CurrentGFXCmd, "G_LOADBLOCK          ");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_LOADBLOCK();
			break;

		/* 0xF4 */
		case G_LOADTILE:
			sprintf(CurrentGFXCmd, "G_LOADTILE           ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xF5 */
		case G_SETTILE:
			sprintf(CurrentGFXCmd, "G_SETTILE            ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_SETTILE();
			break;

		/* 0xF6 */
		case G_FILLRECT:
			sprintf(CurrentGFXCmd, "G_FILLRECT           ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xF7 */
		case G_SETFILLCOLOR:
			sprintf(CurrentGFXCmd, "G_SETFILLCOLOR       ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xF8 */
		case G_SETFOGCOLOR:
			sprintf(CurrentGFXCmd, "G_SETFOGCOLOR        ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_SETFOGCOLOR();
			break;

		/* 0xF9 */
		case G_SETBLENDCOLOR:
			sprintf(CurrentGFXCmd, "G_SETBLENDCOLOR      ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_SETBLENDCOLOR();
			break;

		/* 0xFA */
		case G_SETPRIMCOLOR:
			sprintf(CurrentGFXCmd, "G_SETPRIMCOLOR       ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_SETPRIMCOLOR();
			break;

		/* 0xFB */
		case G_SETENVCOLOR:
			sprintf(CurrentGFXCmd, "G_SETENVCOLOR        ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_SETENVCOLOR();
		    break;

		/* 0xFC */
		case G_SETCOMBINE:
			sprintf(CurrentGFXCmd, "G_SETCOMBINE         ");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_SETCOMBINE();
			break;

		/* 0xFD */
		case G_SETTIMG:
			sprintf(CurrentGFXCmd, "G_SETTIMG            ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);

			F3DEX2_Cmd_SETTIMG();
			break;

		/* 0xFE */
		case G_SETZIMG:
			sprintf(CurrentGFXCmd, "G_SETZIMG            ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* 0xFF */
		case G_SETCIMG:
			sprintf(CurrentGFXCmd, "G_SETCIMG            ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		/* -------------------------------- */

		/* UNKNOWN */
		default:
			sprintf(CurrentGFXCmd, "<unknown - 0x%02X>     ", Readout_CurrentByte1);
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			MessageBox(hwnd, "Unknown command in Display List!\n\nStopping list execution...", "Interpreter Error", MB_OK | MB_ICONERROR);
			DListHasEnded = true;

			break;
		}

		if(DListHasEnded) {
			if(CalledViaCmd) {
				Helper_LogMessage(1, "  [Return to original DList]\n");
			} else {
				Helper_LogMessage(1, "\n");
			}
		}

		Position+=2;
	}

	if(CalledViaCmd) DListHasEnded = false;

	return 0;
}

/*	------------------------------------------------------------ */
