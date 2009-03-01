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

	MVMatrixCount = 0;

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
									SubDLCall = false;
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

	fclose(FileGFXLog); GFXLogOpened = false;

	fclose(FileWavefrontObj); WavefrontObjOpened = false;
	fclose(FileWavefrontMtl); WavefrontMtlOpened = false;

//	sprintf(ErrorMsg, "%d malloc operations, %d free operations while loading map", Debug_MallocOperations, Debug_FreeOperations);
//	MessageBox(hwnd, ErrorMsg, "Memory", MB_OK | MB_ICONINFORMATION);

	return 0;
}

/*	------------------------------------------------------------ */

int Viewer_RenderMap_DListParser(bool CalledViaCmd, unsigned long Position)
{
	sprintf(WavefrontObjMsg, "mltlib map.mtl\n");
	fprintf(FileWavefrontObj, WavefrontObjMsg);

	if(CalledViaCmd) {
		sprintf(GFXLogMsg, "  [Calling DList at 0x%08X]\n", (unsigned int)Position * 4);
		Helper_LogMessage(1, GFXLogMsg);
	} else {
		sprintf(GFXLogMsg, "Display List #%d (0x%08X):\n", DLToRender, (unsigned int)Position * 4);
		Helper_LogMessage(1, GFXLogMsg);
	}

	while (!DListHasEnded) {
		if(GetDLFromZMapScene) {
			/* get data from map */
			memcpy(&Readout_Current1, &ZMapBuffer[DListParser_CurrentMap][Position], 4);
			memcpy(&Readout_Current2, &ZMapBuffer[DListParser_CurrentMap][Position + 1], 4);

			memcpy(&Readout_NextGFXCommand1, &ZMapBuffer[DListParser_CurrentMap][Position + 2], 4);

			Helper_SplitCurrentVals(true);
		} else {
			/* get data from temp object */
			memcpy(&Readout_Current1, &TempObjectBuffer[Position], 4);
			memcpy(&Readout_Current2, &TempObjectBuffer[Position + 1], 4);

			memcpy(&Readout_NextGFXCommand1, &TempObjectBuffer[Position + 2], 4);

			Helper_SplitCurrentVals(true);
		}

		if(CalledViaCmd) Helper_LogMessage(1, " ");

		DLTempPosition = Position;

		switch(Readout_CurrentByte1) {
		case F3DEX2_VTX:
			sprintf(CurrentGFXCmd, "F3DEX2_VTX           ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_VTX();
			break;
		case F3DEX2_TRI1:
			sprintf(CurrentGFXCmd, "F3DEX2_TRI1          ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_TRI1();
			break;
		case F3DEX2_TRI2:
			sprintf(CurrentGFXCmd, "F3DEX2_TRI2          ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_TRI2();
			break;
		case F3DEX2_TEXTURE:
			sprintf(CurrentGFXCmd, "F3DEX2_TEXTURE       ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_TEXTURE();
			break;
		case G_SETTIMG:
			sprintf(CurrentGFXCmd, "G_SETTIMG            ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_SETTIMG();
			break;
		case G_SETTILE:
			sprintf(CurrentGFXCmd, "G_SETTILE            ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_SETTILE();
			break;
		case G_LOADBLOCK:
			sprintf(CurrentGFXCmd, "G_LOADBLOCK          ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;
		case G_SETTILESIZE:
			sprintf(CurrentGFXCmd, "G_SETTILESIZE        ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_SETTILESIZE();
			break;
		case G_RDPFULLSYNC:
			sprintf(CurrentGFXCmd, "G_RDPFULLSYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			break;
		case G_RDPTILESYNC:
			sprintf(CurrentGFXCmd, "G_RDPTILESYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			break;
		case G_RDPPIPESYNC:
			sprintf(CurrentGFXCmd, "G_RDPPIPESYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			break;
		case G_RDPLOADSYNC:
			sprintf(CurrentGFXCmd, "G_RDPLOADSYNC        ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			break;
		case F3DEX2_GEOMETRYMODE:
			sprintf(CurrentGFXCmd, "F3DEX2_GEOMETRYMODE  ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_GEOMETRYMODE();
			break;
		case F3DEX2_CULLDL:
			sprintf(CurrentGFXCmd, "F3DEX2_CULLDL        ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;
		case F3DEX2_SETOTHERMODE_H:
			sprintf(CurrentGFXCmd, "F3DEX2_SETOTHERMODE_H");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_SETOTHERMODE_H();
			break;
		case F3DEX2_SETOTHERMODE_L:
			sprintf(CurrentGFXCmd, "F3DEX2_SETOTHERMODE_L");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_SETOTHERMODE_L();
			break;
		case G_SETFOGCOLOR:
			sprintf(CurrentGFXCmd, "G_SETFOGCOLOR        ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_SETFOGCOLOR();
			break;
		case G_SETPRIMCOLOR:
			sprintf(CurrentGFXCmd, "G_SETPRIMCOLOR       ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_SETPRIMCOLOR();
			break;
        case G_SETENVCOLOR:
            sprintf(CurrentGFXCmd, "G_SETENVCOLOR        ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_SETENVCOLOR();
		    break;
		case F3DEX2_RDPHALF_1:
			sprintf(CurrentGFXCmd, "F3DEX2_RDPHALF_1     ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_RDPHALF_1();
			break;
		case G_LOADTLUT:
			sprintf(CurrentGFXCmd, "G_LOADTLUT           ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_LOADTLUT(Texture[0].PalDataSource, Texture[0].PalOffset);
			break;
		case G_SETCOMBINE:
			sprintf(CurrentGFXCmd, "G_SETCOMBINE         ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_SETCOMBINE();
			break;
		case F3DEX2_MTX:
			sprintf(CurrentGFXCmd, "F3DEX2_MTX           ");
			sprintf(CurrentGFXCmdNote, "<partially handled>");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_MTX();
			break;
		case F3DEX2_MODIFYVTX:
			sprintf(CurrentGFXCmd, "F3DEX2_MODIFYVTX     ");
			sprintf(CurrentGFXCmdNote, "<unemulated>");
			Helper_GFXLogCommand(Position);
			break;

		case F3DEX2_DL:
			sprintf(CurrentGFXCmd, "F3DEX2_DL            ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_DL(GetDLFromZMapScene);
			break;
		case F3DEX2_BRANCH_Z:
			sprintf(CurrentGFXCmd, "F3DEX2_BRANCH_Z      ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			F3DEX2_Cmd_BRANCH_Z();
			break;
		case F3DEX2_ENDDL:
			sprintf(CurrentGFXCmd, "F3DEX2_ENDDL         ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			DListHasEnded = true;
			break;
		default:
			sprintf(CurrentGFXCmd, "<unknown>            ");
			sprintf(CurrentGFXCmdNote, "-");
			Helper_GFXLogCommand(Position);
			MessageBox(hwnd, "ERROR UNKNOWN GFX COMMAND!!", "ERROR", MB_OK | MB_ICONERROR);
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

	SubDLCall = false;

	return 0;
}

/*	------------------------------------------------------------ */
