#include "globals.h"

#include "zelda_ver.h"

bool zl_Init(char * Filename)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(\"%s\");\n", __FUNCTION__, Filename);

	dbgprintf(0, MSK_COLORTYPE_OKAY, "Loading ROM...\n");

	memset(&zROM, 0x00, sizeof(zROM));

	bool RetVal = TRUE;

	if(!zl_LoadROM(Filename)) {
		zOptions.EnableTextures = true;
		zOptions.EnableCombiner = true;

		GetFileName(Filename, zROM.Filename);

		zl_ShowROMInformation();

		if(zl_GetGameVersion()) return EXIT_FAILURE;
		if(zl_GetDMATable()) return EXIT_FAILURE;
		if(zl_GetFilenameTable()) zGame.HasFilenames = false;
		if(zl_GetSceneTable(zGame.CodeBuffer)) return EXIT_FAILURE;

		zl_SetMipsWatchers();

		if(zl_LoadScene(zOptions.SceneNo)) return EXIT_FAILURE;
	} else {
		return zROM.IsROMLoaded;
	}

	return RetVal;
}

int zl_LoadROM(char * Filename)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(\"%s\");\n", __FUNCTION__, Filename);

	FILE * file;
	if((file = fopen(Filename, "rb")) == NULL) {
		char Temp[MAX_PATH];
		GetFileName(Filename, Temp);
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: File '%s' not found\n", Temp);
		return EXIT_FAILURE;
	}

	strcpy(zROM.FilePath, Filename);

	fseek(file, 0, SEEK_END);
	zROM.Size = ftell(file);
	rewind(file);
	zROM.Data = (unsigned char*) malloc (sizeof(char) * zROM.Size);
	fread(zROM.Data, 1, zROM.Size, file);
	fclose(file);

	memcpy(zROM.Title, &zROM.Data[32], 20);
	memcpy(zROM.GameID, &zROM.Data[59], 4);
	zROM.Version = zROM.Data[63];

	zROM.CRC1 = Read32(zROM.Data, 16);
	zROM.CRC2 = Read32(zROM.Data, 20);

	return EXIT_SUCCESS;
}

void zl_ShowROMInformation()
{
	dbgprintf(0, MSK_COLORTYPE_INFO, " - Filename:      %s\n", zROM.Filename);
	dbgprintf(0, MSK_COLORTYPE_INFO, " - Size:          %iMB (%i Mbit)\n", (zROM.Size / 0x100000), (zROM.Size / 0x20000));
	dbgprintf(0, MSK_COLORTYPE_INFO, " - Title:         %s\n", zROM.Title);
	dbgprintf(0, MSK_COLORTYPE_INFO, " - Game ID:       %s\n", zROM.GameID);
	dbgprintf(0, MSK_COLORTYPE_INFO, " - Version:       1.%X\n", zROM.Version);
	dbgprintf(0, MSK_COLORTYPE_INFO, " - CRC1:          0x%08X\n", zROM.CRC1);
	dbgprintf(0, MSK_COLORTYPE_INFO, " - CRC2:          0x%08X\n", zROM.CRC2);
}

void zl_InitCombiner()
{
	RDP_CreateCombinerProgram(0x0011FFFF, 0xFFFFFC38);
	RDP_CreateCombinerProgram(0x00127E03, 0xFFFFFDF8);
	RDP_CreateCombinerProgram(0x00127E03, 0xFFFFF3F8);
	RDP_CreateCombinerProgram(0x00127E03, 0xFFFFF7F8);
	RDP_CreateCombinerProgram(0x00121603, 0xFF5BFFF8);
	RDP_CreateCombinerProgram(0x00267E04, 0x1F0CFDFF);
	RDP_CreateCombinerProgram(0x0041FFFF, 0xFFFFFC38);
	RDP_CreateCombinerProgram(0x00127E0C, 0xFFFFFDF8);
	RDP_CreateCombinerProgram(0x00267E04, 0x1FFCFDF8);
	RDP_CreateCombinerProgram(0x00262A04, 0x1F0C93FF);
	RDP_CreateCombinerProgram(0x00121803, 0xFF5BFFF8);
	RDP_CreateCombinerProgram(0x00121803, 0xFF0FFFFF);
	RDP_CreateCombinerProgram(0x0041FFFF, 0xFFFFF638);
	RDP_CreateCombinerProgram(0x0011FFFF, 0xFFFFF238);
	RDP_CreateCombinerProgram(0x0041C7FF, 0xFFFFFE38);
	RDP_CreateCombinerProgram(0x0041FFFF, 0xFFFFF838);

	RDP_CreateCombinerProgram(0x00127E60, 0xFFFFF3F8);
	RDP_CreateCombinerProgram(0x00272C04, 0x1F0C93FF);
	RDP_CreateCombinerProgram(0x0020AC04, 0xFF0F93FF);
	RDP_CreateCombinerProgram(0x0026A004, 0x1FFC93F8);
	RDP_CreateCombinerProgram(0x00277E04, 0x1F0CF7FF);
	RDP_CreateCombinerProgram(0x0020FE04, 0xFF0FF7FF);
	RDP_CreateCombinerProgram(0x00272E04, 0x1F0C93FF);
	RDP_CreateCombinerProgram(0x00272C04, 0x1F1093FF);
	RDP_CreateCombinerProgram(0x0020A203, 0xFF13FFFF);
	RDP_CreateCombinerProgram(0x0011FE04, 0xFFFFF7F8);
	RDP_CreateCombinerProgram(0x0020AC03, 0xFF0F93FF);
	RDP_CreateCombinerProgram(0x00272C03, 0x1F0C93FF);
	RDP_CreateCombinerProgram(0x0011FE04, 0xFF0FF3FF);
	RDP_CreateCombinerProgram(0x00119C04, 0xFFFFFFF8);
	RDP_CreateCombinerProgram(0x00271204, 0x1F0CFFFF);
	RDP_CreateCombinerProgram(0x0011FE04, 0xFFFFF3F8);
	RDP_CreateCombinerProgram(0x00272C80, 0x350CF37F);
}

int zl_GetGameVersion()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	bool CheckOkay = false;

	int i = 0;
	for(i = 0; i < ArraySize(GameVer); i++) {
		if(!strcmp(zROM.Title, GameVer[i].Title) && !strcmp(zROM.GameID, GameVer[i].GameID) && zROM.Version == GameVer[i].Version) {
			CheckOkay = true;
			zGame.GameType = GameVer[i].GameType;
			zGame.CodeFileNo = GameVer[i].CodeFileNo;
			zGame.CodeRAMOffset = GameVer[i].CodeRAMOffset;
			zGame.SceneTableOffset = GameVer[i].SceneTableOffset;
			zGame.ObjectTableOffset = GameVer[i].ObjectTableOffset;
			zGame.ActorTableOffset = GameVer[i].ActorTableOffset;
			zGame.SceneCount = GameVer[i].SceneCount;
			zGame.ObjectCount = GameVer[i].ObjectCount;
			strcpy(zGame.TitleText, GameVer[i].TitleText);
			break;
		}
	}

	if(CheckOkay) {
		dbgprintf(0, MSK_COLORTYPE_OKAY, "\nROM has been recognized as '%s'.\n\n", zGame.TitleText);
	} else {
		dbgprintf(0, MSK_COLORTYPE_ERROR,"\n- Error: ROM could not be recognized!\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int zl_LoadScene(int SceneNo)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%i);\n", __FUNCTION__, SceneNo);

	if((SceneNo < 0) || (SceneNo > zGame.SceneCount)) return EXIT_FAILURE;

	RDP_ClearStructures(true);
	zl_ClearAllSegments();
	zl_ClearViewerStructures();
	gl_ClearDisplayLists();

	RDP_ClearTextures();

	RDP_InitParser(F3DEX2);
	zl_InitCombiner();

	// default to rendering the first map of the scene
	zOptions.MapToRender = 0;

	unsigned int BaseOffset = zGame.SceneTableOffset + (SceneNo * (zGame.GameType ? 0x10 : 0x14));	// OoT = 0x14 bytes, MM = 0x10 bytes

	unsigned int SceneStart = Read32(zGame.CodeBuffer, BaseOffset);
	unsigned int SceneEnd = Read32(zGame.CodeBuffer, BaseOffset + 4);
	unsigned int SceneSize = SceneEnd - SceneStart;

	DMA Scene = zl_DMAVirtualToPhysical(SceneStart);
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Physical offsets: PStart 0x%08X, PEnd 0x%08X\n", Scene.PStart, Scene.PEnd);

	if((Scene.PStart != 0x00) && (Scene.PEnd != 0x00)) {
		dbgprintf(1, MSK_COLORTYPE_INFO, "Loading Scene #%i:\n", SceneNo);
		if(zGame.HasFilenames) dbgprintf(1, MSK_COLORTYPE_INFO, " - Filename: %s\n", Scene.Filename);
		dbgprintf(1, MSK_COLORTYPE_INFO, " - Location in ROM: 0x%08X to 0x%08X (0x%04X bytes)\n\n", Scene.PStart, Scene.PEnd, SceneSize);

		RDP_LoadToSegment(0x02, zROM.Data, Scene.PStart, SceneSize);
		if(zl_ExecuteHeader(0x02, 0x00, 0, -1)) return EXIT_FAILURE;

		unsigned char Segment = zSHeader[0].MapOffset >> 24;
		unsigned int Offset = (zSHeader[0].MapOffset & 0x00FFFFFF);

		zl_GetMapObjects(0, -1);
		zl_GetMapActors(0, -1);

		int i = 0;
		for(i = 0; i < zSHeader[0].MapCount; i++) {
			RDP_ClearSegment(0x03);
			RDP_ClearStructures(false);

			zGfx.DLCount[i] = 0;

			unsigned int MapStart = Read32(RAM[Segment].Data, Offset + (i * 0x08));
			unsigned int MapEnd = Read32(RAM[Segment].Data, Offset + 4 + (i * 0x08));
			unsigned int MapSize = MapEnd - MapStart;

			DMA Map = zl_DMAVirtualToPhysical(MapStart);
			dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Physical offsets: PStart 0x%08X, PEnd 0x%08X\n", Map.PStart, Map.PEnd);

			dbgprintf(1, MSK_COLORTYPE_INFO, "\nLoading Map #%i:\n", i);
			if(zGame.HasFilenames) dbgprintf(1, MSK_COLORTYPE_INFO, " - Filename: %s\n", Map.Filename);
			dbgprintf(1, MSK_COLORTYPE_INFO, " - Location in ROM: 0x%08X to 0x%08X (0x%04X bytes)\n\n", Map.PStart, Map.PEnd, MapSize);

			RDP_LoadToSegment(0x03, zROM.Data, Map.PStart, MapSize);
			if(zl_ExecuteHeader(0x03, 0x00, 0, i)) break;

			zl_GetMapObjects(0, i);
			zl_GetMapActors(0, i);

			zl_GetDisplayLists(i);
			zl_ExecuteDisplayLists(i);
		}

		ca_Reset();

		dbgprintf(1, MSK_COLORTYPE_OKAY, "\nScene #%i has been loaded.\n", zOptions.SceneNo);
	} else {
		dbgprintf(1, MSK_COLORTYPE_ERROR, "- Error: Could not load Scene #%i.\n", zOptions.SceneNo);
	}
	return EXIT_SUCCESS;
}

DMA zl_DMAGetFile(int DMAFileNo)
{
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%i);\n", __FUNCTION__, DMAFileNo);

	DMA File = {0, 0, 0, 0, 0, ""};

	File.ID = DMAFileNo;

	unsigned int Offset = zGame.DMATableOffset + (DMAFileNo * 0x10);
	File.VStart = Read32(zROM.Data, Offset);
	File.VEnd = Read32(zROM.Data, Offset + 4);
	File.PStart = Read32(zROM.Data, Offset + 8);
	File.PEnd = Read32(zROM.Data, Offset + 12);

	dbgprintf(2, MSK_COLORTYPE_OKAY, "| VStart 0x%08X, VEnd 0x%08X, PStart 0x%08X, PEnd 0x%08X\n", File.VStart, File.VEnd, File.PStart, File.PEnd);

	zl_DMAGetFilename(File.Filename, DMAFileNo);

	return File;
}

void zl_DMAGetFilename(char * Name, int DMAFileNo)
{
	if(zGame.HasFilenames) {
		int FilenameNo = 0;
		unsigned char * Current;

		for(Current = &zROM.Data[zGame.FilenameTableOffset], FilenameNo = 0; FilenameNo < (DMAFileNo + 1); FilenameNo++) {
			Current += sprintf(Name, "%s", Current);
			while(!*Current) Current++;
		}
	}
}

DMA zl_DMAVirtualToPhysical(unsigned int VStart)
{
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] %s(0x%08X);\n", __FUNCTION__, VStart);

	DMA File = {0, 0, 0, 0, 0, ""};

	unsigned int Offset = zGame.DMATableOffset;
	while(Offset < zGame.DMATableOffset + (zGame.DMATable.VEnd - zGame.DMATable.VStart)) {
		unsigned int Temp = Read32(zROM.Data, Offset);
		if(Temp == VStart) {
			File.VStart = Temp;
			File.VEnd = Read32(zROM.Data, Offset + 4);
			File.PStart = Read32(zROM.Data, Offset + 8);
			File.PEnd = Read32(zROM.Data, Offset + 12);

			if(File.VStart == File.PStart) {		// not compressed
				File.PEnd = File.VEnd;
			}

			if(File.PEnd == 0x00) {
				File.PEnd = File.PStart + (File.VEnd - File.VStart);
			}

			break;
		}
		Offset+=16;
		File.ID++;
	}

	dbgprintf(2, MSK_COLORTYPE_OKAY, "| VStart 0x%08X -> PStart 0x%08X, PEnd 0x%08X\n", VStart, File.PStart, File.PEnd);

	zl_DMAGetFilename(File.Filename, File.ID);

	return File;
}

int zl_GetFilenameTable()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	zGame.Bootfile = zl_DMAGetFile(1);

	char Check[8];
	int Pos = zGame.Bootfile.PStart, MaxPos = zGame.Bootfile.PEnd;
	if(zGame.Bootfile.PEnd == 0 || zGame.Bootfile.PEnd == 0xFFFFFFFF) MaxPos = zGame.Bootfile.VEnd;

	while(Pos < MaxPos) {
		memcpy(Check, &zROM.Data[Pos], 7);
		Check[7] = 0x00;
		if(!strcmp(Check, "makerom")) {
			zGame.FilenameTableOffset = Pos;
			dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Filename table found at 0x%08X.\n", zGame.FilenameTableOffset);
			zGame.HasFilenames = true;
			break;
		}
		Pos += 8;
	}

	if(Pos >= MaxPos) {
		dbgprintf(0, MSK_COLORTYPE_WARNING, "- Warning: Could not find filename table!\n\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int zl_GetDMATable()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	char Check[10], Creator[16], Build[20];

	int Pos = 0, MaxPos = 0x100000;
	while(Pos < MaxPos) {
		memcpy(Check, &zROM.Data[Pos], 9);
		Check[9] = 0x00;
		if(!strcmp(Check, "zelda@srd")) {
			memcpy(Creator, &zROM.Data[Pos], 16);
			if(Creator[15] != 0x00) {
				memcpy(Build, &zROM.Data[Pos + 12], 20);
			} else {
				memcpy(Build, &zROM.Data[Pos + 16], 20);
			}
			dbgprintf(1, MSK_COLORTYPE_INFO, "Creator:   %s\n", Creator);
			dbgprintf(1, MSK_COLORTYPE_INFO, "Build:     %s\n\n", Build);
			break;
		}
		Pos += 8;
	}

	if(Pos >= MaxPos) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not find DMA table!\n");
		return EXIT_FAILURE;
	}

	zGame.DMATableOffset = (Pos + 0x30);
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] DMA table found at 0x%08X.\n", zGame.DMATableOffset);

	zGame.DMATable = zl_DMAGetFile(2);

	return EXIT_SUCCESS;
}

int zl_GetSceneTable()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	char Check[4];

	DMA File = zl_DMAGetFile(zGame.CodeFileNo);
	unsigned int File_BufferSize = File.VEnd - File.VStart;
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Scene table in file #%i, File length is 0x%08X.\n", zGame.CodeFileNo, File_BufferSize);

	zGame.IsCompressed = true;
	if(File.PEnd == 0 || File.PEnd == 0xFFFFFFFF) zGame.IsCompressed = false;
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] File is %s.\n", zGame.IsCompressed ? "compressed, assuming whole ROM is compressed" : "not compressed");

	unsigned char * NewBuffer = malloc(sizeof(char) * File_BufferSize);
	if(NewBuffer != NULL) {
		memset(NewBuffer, 0x00, (sizeof(char) * File_BufferSize));
		zGame.CodeBuffer = NewBuffer;
	} else {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not allocate temporary buffer!\n");
		return EXIT_FAILURE;
	}

	if(zGame.IsCompressed && File.PEnd) {
		memcpy(Check, &zROM.Data[File.PStart], 4);
		unsigned int Yaz0Size = Read32(zROM.Data, File.PStart + 4);		// should be same as File.BufferSize

		if((!strncmp(Check, "Yaz0", 4)) && (Yaz0Size == File_BufferSize)) {
			dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Decompressing data: Offset 0x%08X, Length 0x%08X...\n", File.PStart, File_BufferSize);
			RDP_Yaz0Decode(&zROM.Data[File.PStart], zGame.CodeBuffer, File_BufferSize);
		} else {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not decompress data, data appears to be invalid!\n");
			return EXIT_FAILURE;
		}
	} else {
		dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Copying data: Offset 0x%08X, Length 0x%08X...\n", File.PStart, File_BufferSize);
		memcpy(zGame.CodeBuffer, &zROM.Data[File.PStart], File_BufferSize);
	}

	return EXIT_SUCCESS;
}

unsigned char * zl_DMAToBuffer(DMA File)
{
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%08X);\n", __FUNCTION__, File);

	unsigned int File_BufferSize = File.VEnd - File.VStart;
	unsigned char * buffer = (unsigned char *)malloc(File_BufferSize);
	char Check[4];
	if(zGame.IsCompressed && File.PEnd) {
		memcpy(Check, &zROM.Data[File.PStart], 4);
		unsigned int Yaz0Size = Read32(zROM.Data, File.PStart + 4);		// should be same as File.BufferSize

		if((!strncmp(Check, "Yaz0", 4)) && (Yaz0Size == File_BufferSize)) {
			dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Decompressing data: Offset 0x%08X, Length 0x%08X...\n", File.PStart, File_BufferSize);
			RDP_Yaz0Decode(&zROM.Data[File.PStart], buffer, File_BufferSize);
		} else {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not decompress data, data appears to be invalid!\n");
			return NULL;
		}
	} else {
		dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Copying data: Offset 0x%08X, Length 0x%08X...\n", File.PStart, File_BufferSize);
		memcpy(buffer, &zROM.Data[File.PStart], File_BufferSize);
	}
	return buffer;
}

void zl_ClearAllSegments()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	int i = 0;
	for(i = 0; i < 64; i++) {
		RDP_ClearSegment(i);
	}
}

void zl_ClearViewerStructures()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	int i = 0, j = 0;

	static const struct __zHeader zHeader_Empty;
	for(i = 0; i < ArraySize(zSHeader); i++) zSHeader[i] = zHeader_Empty;
	for(i = 0; i < 256; i++) for(j = 0; j < 256; j++) zMHeader[i][j] = zHeader_Empty;

	static const struct __zCamera zCamera_Empty;
	zCamera = zCamera_Empty;

	for(i = 0; i < ArraySize(zObject); i++) {
		zObject[i].IsSet = false;
		zObject[i].StartOffset = -1;
		zObject[i].EndOffset = -1;
	}

	static const struct __zActor zActor_Empty = {false,0,0,0,0, 0,0,0,"",0,0,NULL,0.01f,0,0,0};
	for(i = 0; i < ArraySize(zActor); i++) {
		zActor[i] = zActor_Empty;
	}
}

int zl_ExecuteHeader(unsigned char Segment, unsigned int Offset, int SHeaderNumber, int MHeaderNumber)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(0x%02X, 0x%08X, %i, %i);\n", __FUNCTION__, Segment, Offset, SHeaderNumber, MHeaderNumber);

	if(!RDP_CheckAddressValidity((Segment << 24) | Offset)) return EXIT_FAILURE;

	// if we're executing a Map header...
	if(MHeaderNumber != -1) {
		// check if the first command is either 0x08, 0x16 or 0x18...
		if((RAM[Segment].Data[0] != 0x08) && (RAM[Segment].Data[0] != 0x16) && (RAM[Segment].Data[0] != 0x18)) {
			// ...and if it's not die, as we can assume that the header is invalid (see unfixed syotes, Scene 104)
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Invalid or non-standard Map header!");
			return EXIT_FAILURE;
		}
	}

	bool EndOfHeader = false;

	while(!EndOfHeader) {
		unsigned int w0 = Read32(RAM[Segment].Data, Offset);
		unsigned int w1 = Read32(RAM[Segment].Data, Offset + 4);

		switch(w0 >> 24) {
			case 0x00:
				zSHeader[SHeaderNumber].LinkCount = (w0 & 0x00FF0000) >> 16;
				zSHeader[SHeaderNumber].LinkOffset = w1;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - %i Link(s) at 0x%08X.\n", zSHeader[SHeaderNumber].LinkCount, zSHeader[SHeaderNumber].LinkOffset);
				break;
			case 0x01:
				zMHeader[SHeaderNumber][MHeaderNumber].ActorCount = (w0 & 0x00FF0000) >> 16;
				zMHeader[SHeaderNumber][MHeaderNumber].ActorOffset = w1;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - %i Actor(s) at 0x%08X.\n", zMHeader[SHeaderNumber][MHeaderNumber].ActorCount, zMHeader[SHeaderNumber][MHeaderNumber].ActorOffset);
				break;
			case 0x03:
				zSHeader[SHeaderNumber].ColDataOffset = w1;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - Collision data at 0x%08X.\n", zSHeader[SHeaderNumber].ColDataOffset);
				break;
			case 0x04:
				zSHeader[SHeaderNumber].MapCount = (w0 & 0x00FF0000) >> 16;
				zSHeader[SHeaderNumber].MapOffset = w1;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - %i map definition(s) at 0x%08X.\n", zSHeader[SHeaderNumber].MapCount, zSHeader[SHeaderNumber].MapOffset);
				break;
			case 0x07:
				zSHeader[SHeaderNumber].GameplayObj = (w1 & 0x000000FF);
				dbgprintf(1, MSK_COLORTYPE_INFO, " - Additional Gameplay objects from object 0x%04X.\n", zSHeader[SHeaderNumber].GameplayObj);
				break;
			case 0x0A:
				zMHeader[SHeaderNumber][MHeaderNumber].MeshHeaderOffset = w1;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - Mesh header at 0x%08X.\n", zMHeader[SHeaderNumber][MHeaderNumber].MeshHeaderOffset);
				break;
			case 0x0B:
				zMHeader[SHeaderNumber][MHeaderNumber].ObjCount = (w0 & 0x00FF0000) >> 16;
				zMHeader[SHeaderNumber][MHeaderNumber].ObjOffset = w1;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - %i object(s) at 0x%08X.\n", zMHeader[SHeaderNumber][MHeaderNumber].ObjCount, zMHeader[SHeaderNumber][MHeaderNumber].ObjOffset);
				break;
			case 0x0E:
				zSHeader[SHeaderNumber].DoorCount = (w0 & 0x00FF0000) >> 16;
				zSHeader[SHeaderNumber].DoorOffset = w1;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - %i door(s) at 0x%08X.\n", zSHeader[SHeaderNumber].DoorCount, zSHeader[SHeaderNumber].DoorOffset);
				break;
			case 0x0F:
				zSHeader[SHeaderNumber].EnvSetCount = (w0 & 0x00FF0000) >> 16;
				zSHeader[SHeaderNumber].EnvSetOffset = w1;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - %i environment(s) at 0x%08X.\n", zSHeader[SHeaderNumber].EnvSetCount, zSHeader[SHeaderNumber].EnvSetOffset);
				break;
			case 0x10:
				zMHeader[SHeaderNumber][MHeaderNumber].MapTime = w0 >> 16;
				zMHeader[SHeaderNumber][MHeaderNumber].TimeFlow = (w1 & 0x0000FF00) >> 8;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - 'Map time' is %i, 'time flow setting' is %i.\n", zMHeader[SHeaderNumber][MHeaderNumber].MapTime, zMHeader[SHeaderNumber][MHeaderNumber].TimeFlow);
				break;
			case 0x11:
				zMHeader[SHeaderNumber][MHeaderNumber].Skybox = w1 >> 24;
				zMHeader[SHeaderNumber][MHeaderNumber].TimeOfDay = (w1 & 0x00000F00) >> 8;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - Skybox background is %i, 'time of day' is %i.\n", zMHeader[SHeaderNumber][MHeaderNumber].Skybox, zMHeader[SHeaderNumber][MHeaderNumber].TimeOfDay);
				break;
			case 0x14:
				EndOfHeader = true;
				break;
			case 0x16:
				zMHeader[SHeaderNumber][MHeaderNumber].EchoLevel = (w0 & 0x000000FF);
				dbgprintf(1, MSK_COLORTYPE_INFO, " - 'Echo level setting' is %i.\n", zMHeader[SHeaderNumber][MHeaderNumber].EchoLevel);
				break;
			default:
				dbgprintf(1, MSK_COLORTYPE_WARNING, " - Unknown header command 0x%08X 0x%08X.\n", w0, w1);
				break;
		}

		Offset += 8;
	}

	dbgprintf(1, MSK_COLORTYPE_INFO, "\n");

	return EXIT_SUCCESS;
}

void zl_GetDisplayLists(int MapNumber)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%i);\n", __FUNCTION__, MapNumber);

	unsigned char Segment = zMHeader[0][MapNumber].MeshHeaderOffset >> 24;
	unsigned int Offset = (zMHeader[0][MapNumber].MeshHeaderOffset & 0x00FFFFFF);
	unsigned int MeshHeader = Read32(RAM[Segment].Data, Offset);

	unsigned char MeshType = MeshHeader >> 24;
	unsigned char MeshTotal = (MeshHeader & 0x00FF0000) >> 16;

	dbgprintf(1, MSK_COLORTYPE_INFO, "- Mesh setup type %i, %i meshes.\n", MeshType, MeshTotal);

	int MeshCount = 0;

	switch(MeshType) {
		case 0x00: {
			unsigned int DListStart1, DListStart2;
			Offset += 12;

			while(MeshCount < MeshTotal) {
				DListStart1 = Read32(RAM[Segment].Data, Offset);
				DListStart2 = Read32(RAM[Segment].Data, Offset + 4);

				if(DListStart1 != 0) zGfx.DLOffset[MapNumber][zGfx.DLCount[MapNumber]++] = DListStart1;
				if(DListStart2 != 0) zGfx.DLOffset[MapNumber][zGfx.DLCount[MapNumber]++] = DListStart2;

				Offset += 8;

				MeshCount++;
			}
			break; }

		case 0x01: {
			unsigned int DListStart;
			Offset += 4;

			DListStart = Read32(RAM[Segment].Data, Offset);
			DListStart = Read32(RAM[Segment].Data, (DListStart & 0x00FFFFFF));

			if(DListStart != 0) zGfx.DLOffset[MapNumber][zGfx.DLCount[MapNumber]++] = DListStart;

			break; }

		case 0x02: {
			short ClipMaxX, ClipMaxZ, ClipMinX, ClipMinZ;
			unsigned int DListStart1, DListStart2;
			Offset += 12;

			while(MeshCount < MeshTotal) {
				ClipMaxX = Read16(RAM[Segment].Data, Offset);
				ClipMaxZ = Read16(RAM[Segment].Data, Offset + 2);
				ClipMinX = Read16(RAM[Segment].Data, Offset + 4);
				ClipMinZ = Read16(RAM[Segment].Data, Offset + 6);
				DListStart1 = Read32(RAM[Segment].Data, Offset + 8);
				DListStart2 = Read32(RAM[Segment].Data, Offset + 12);

				if(DListStart1 != 0) zGfx.DLOffset[MapNumber][zGfx.DLCount[MapNumber]++] = DListStart1;
				if(DListStart2 != 0) zGfx.DLOffset[MapNumber][zGfx.DLCount[MapNumber]++] = DListStart2;

				Offset += 16;

				MeshCount++;
			}
			break; }

		default: {
			dbgprintf(1, MSK_COLORTYPE_WARNING, "- Warning: Unsupported mesh setup type %i!\n", MeshType);
			break; }
	}

	if(zGfx.DLCount > 0) {
		dbgprintf(1, MSK_COLORTYPE_INFO, "- Found %i Display Lists.\n", zGfx.DLCount[MapNumber]);
	}
}

void zl_ExecuteDisplayLists(int MapNumber)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	int DL = 0;

	zGfx.GLListCount[MapNumber] = glGenLists(zGfx.DLCount[MapNumber]);
	glListBase(zGfx.GLListCount[MapNumber]);

	while(DL < zGfx.DLCount[MapNumber]) {
		if(RDP_CheckAddressValidity(zGfx.DLOffset[MapNumber][DL])) {
			glNewList(zGfx.GLListCount[MapNumber] + DL, GL_COMPILE);
				RDP_ParseDisplayList(zGfx.DLOffset[MapNumber][DL], true);
			glEndList();
		}

		DL++;
	}
}

void zl_DeInit()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	if(zROM.Data != NULL) free(zROM.Data);
	if(zGame.CodeBuffer != NULL) free(zGame.CodeBuffer);
}

void zl_HexPrint(unsigned char * Buffer, int Offset, int Length)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(0x%08X, 0x%08X, 0x%08X);\n", __FUNCTION__, Buffer, Offset, Length);

	int i = 0;
	for(i = Offset; i < Offset + Length; i += 16) {
		dbgprintf(0, MSK_COLORTYPE_INFO, "%08X | %08X %08X %08X %08X",
			i,
			Read32(Buffer, i), Read32(Buffer, i + 4), Read32(Buffer, i + 8), Read32(Buffer, i + 12));
	}
}
