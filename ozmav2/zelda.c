#include "globals.h"

#include "zelda_ver.h"

int zl_Init(char * Filename)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(\"%s\");\n", __FUNCTION__, Filename);

	zOptions.EnableTextures = true;
	zOptions.EnableCombiner = true;

	memset(&zROM, 0x00, sizeof(zROM));

	strcpy(zROM.Filename, Filename);
	zl_LoadROM();
	if(zROM.Size == 0) return EXIT_FAILURE;

	int MagChar = '\\';
	#ifndef WIN32
	MagChar = '/';
	#endif
	char *Ptr;
	if(!(Ptr = strrchr(zROM.Filename, MagChar))) Ptr = zROM.Filename - 1;

	dbgprintf(0, MSK_COLORTYPE_INFO, "ROM information:\n");
	dbgprintf(0, MSK_COLORTYPE_INFO, "Filename:  %s\n", Ptr+1);
	dbgprintf(0, MSK_COLORTYPE_INFO, "Size:      %iMB (%i Mbit)\n", (zROM.Size / 0x100000), (zROM.Size / 0x20000));
	dbgprintf(0, MSK_COLORTYPE_INFO, "Title:     %s\n", zROM.Title);
	dbgprintf(0, MSK_COLORTYPE_INFO, "Game ID:   %s\n", zROM.GameID);
	dbgprintf(0, MSK_COLORTYPE_INFO, "Version:   1.%X\n", zROM.Version);
	dbgprintf(0, MSK_COLORTYPE_INFO, "CRC1:      0x%08X\n", zROM.CRC1);
	dbgprintf(0, MSK_COLORTYPE_INFO, "CRC2:      0x%08X\n\n", zROM.CRC2);

	if(zl_GetGameVersion()) return EXIT_FAILURE;
	if(zl_GetDMATable()) return EXIT_FAILURE;
	if(zl_GetFilenameTable()) zGame.HasFilenames = false;
	if(zl_GetSceneTable(zGame.STBuffer)) return EXIT_FAILURE;

	if(zl_LoadScene(zOptions.SceneNo)) return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int zl_LoadROM()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	FILE * file;
	if((file = fopen(zROM.Filename, "rb")) == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: File '%s' not found\n", zROM.Filename);
		return EXIT_FAILURE;
	}

	fseek(file, 0, SEEK_END);
	zROM.Size = ftell(file);
	rewind(file);
	zROM.Data = (unsigned char*) malloc (sizeof(char) * zROM.Size);
	size_t result = fread(zROM.Data, 1, zROM.Size, file);
	fclose(file);

	if(result != zROM.Size) return EXIT_FAILURE;

	memcpy(zROM.Title, &zROM.Data[32], 20);
	memcpy(zROM.GameID, &zROM.Data[59], 4);
	zROM.Version = zROM.Data[63];

	zROM.CRC1 = Read32(zROM.Data, 16);
	zROM.CRC2 = Read32(zROM.Data, 20);

	return EXIT_SUCCESS;
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
			zGame.SceneTableFileNo = GameVer[i].SceneTableFileNo;
			zGame.SceneTableOffset = GameVer[i].SceneTableOffset;
			zGame.SceneCount = GameVer[i].SceneCount;
			strcpy(zGame.TitleText, GameVer[i].TitleText);
			break;
		}
	}

	if(CheckOkay) {
		dbgprintf(0, MSK_COLORTYPE_OKAY, "ROM has been recognized as '%s'.\n\n", zGame.TitleText);
	} else {
		dbgprintf(0, MSK_COLORTYPE_ERROR,"- Error: ROM could not be recognized!\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int zl_LoadScene(int SceneNo)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%i);\n", __FUNCTION__, SceneNo);

	if((SceneNo < 0) || (SceneNo > zGame.SceneCount)) return EXIT_FAILURE;

	zl_ClearAllSegments();
	zl_ClearStructures(true);
	gl_ClearRenderer(true);
	dl_InitCombiner();

	md_InitModelDumping(SceneNo);

	unsigned int BaseOffset = zGame.SceneTableOffset + (SceneNo * (zGame.GameType ? 0x10 : 0x14));	// OoT = 0x14 bytes, MM = 0x10 bytes

	unsigned int SceneStart = Read32(zGame.STBuffer, BaseOffset);
	unsigned int SceneEnd = Read32(zGame.STBuffer, BaseOffset + 4);
	unsigned int SceneSize = SceneEnd - SceneStart;

	DMA Scene = zl_DMAVirtualToPhysical(SceneStart);
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Physical offsets: PStart 0x%08X, PEnd 0x%08X\n", Scene.PStart, Scene.PEnd);

	dbgprintf(1, MSK_COLORTYPE_INFO, "Loading Scene #%i:\n", SceneNo);
	if(zGame.HasFilenames) dbgprintf(1, MSK_COLORTYPE_INFO, " - Filename: %s\n", Scene.Filename);
	dbgprintf(1, MSK_COLORTYPE_INFO, " - Location in ROM: 0x%08X to 0x%08X (0x%04X bytes)\n\n", Scene.PStart, Scene.PEnd, SceneSize);

	zl_LoadToSegment(0x02, zROM.Data, Scene.PStart, SceneSize, zGame.IsCompressed);
	zl_ExecuteHeader(0x02, 0x00, 0, -1);

	unsigned char Segment = (zSHeader[0].MapOffset & 0xFF000000) >> 24;
	unsigned int Offset = (zSHeader[0].MapOffset & 0x00FFFFFF);

	int i = 0;
	for(i = 0; i < zSHeader[0].MapCount; i++) {
		zl_ClearSegment(0x03);
		zl_ClearStructures(false);
		gl_ClearRenderer(false);

		zGfx.DLCount[i] = 0;

		unsigned int MapStart = Read32(zRAM[Segment].Data, Offset + (i * 0x08));
		unsigned int MapEnd = Read32(zRAM[Segment].Data, Offset + 4 + (i * 0x08));
		unsigned int MapSize = MapEnd - MapStart;

		DMA Map = zl_DMAVirtualToPhysical(MapStart);
		dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Physical offsets: PStart 0x%08X, PEnd 0x%08X\n", Map.PStart, Map.PEnd);

		dbgprintf(1, MSK_COLORTYPE_INFO, "\nLoading Map #%i:\n", i);
		if(zGame.HasFilenames) dbgprintf(1, MSK_COLORTYPE_INFO, " - Filename: %s\n", Map.Filename);
		dbgprintf(1, MSK_COLORTYPE_INFO, " - Location in ROM: 0x%08X to 0x%08X (0x%04X bytes)\n\n", Map.PStart, Map.PEnd, MapSize);

		zl_LoadToSegment(0x03, zROM.Data, Map.PStart, MapSize, zGame.IsCompressed);
		zl_ExecuteHeader(0x03, 0x00, 0, i);

		zl_GetDisplayLists(i);
		zl_ExecuteDisplayLists(i);

		zGfx.DLCountTotal += zGfx.DLCount[i];
	}

	ca_Reset();

	md_StopModelDumping();

	dbgprintf(1, MSK_COLORTYPE_OKAY, "\nScene #%i has been loaded.\n\n", zOptions.SceneNo);

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

	char Check[5];

	DMA File = zl_DMAGetFile(zGame.SceneTableFileNo);
	unsigned int File_BufferSize = File.VEnd - File.VStart;
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Scene table in file #%i, File length is 0x%08X.\n", zGame.SceneTableFileNo, File_BufferSize);

	zGame.IsCompressed = true;
	if(File.PEnd == 0 || File.PEnd == 0xFFFFFFFF) zGame.IsCompressed = false;
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] File is %s.\n", zGame.IsCompressed ? "compressed, assuming whole ROM is compressed" : "not compressed");

	unsigned char * NewBuffer = malloc(sizeof(char) * File_BufferSize);
	if(NewBuffer != NULL) {
		memset(NewBuffer, 0x00, (sizeof(char) * File_BufferSize));
		zGame.STBuffer = NewBuffer;
	} else {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not allocate temporary buffer!\n");
		return EXIT_FAILURE;
	}

	if(zGame.IsCompressed) {
		memcpy(Check, &zROM.Data[File.PStart], 4);
		Check[4] = 0x00;
		unsigned int Yaz0Size = Read32(zROM.Data, File.PStart + 4);		// should be same as File.BufferSize

		if((!strcmp(Check, "Yaz0")) && (Yaz0Size == File_BufferSize)) {
			dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Decompressing data: Offset 0x%08X, Length 0x%08X...\n", File.PStart + 0x10, File_BufferSize);
			zl_Yaz0Decode(&zROM.Data[File.PStart + 0x10], zGame.STBuffer, File_BufferSize);
		} else {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not decompress data, data appears to be invalid!\n");
			return EXIT_FAILURE;
		}
	} else {
		dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Copying data: Offset 0x%08X, Length 0x%08X...\n", File.PStart, File_BufferSize);
		memcpy(zGame.STBuffer, &zROM.Data[File.PStart], File_BufferSize);
	}

	return EXIT_SUCCESS;
}

void zl_LoadToSegment(unsigned char Segment, unsigned char * Buffer, unsigned int Offset, unsigned int Size, bool IsCompressed)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(0x%02X, 0x%08X, 0x%08X, 0x%08X, %i);\n", __FUNCTION__,
		Segment, Buffer, Offset, Size, IsCompressed);

	zRAM[Segment].Data = (unsigned char*) malloc (sizeof(char) * Size);
	if(IsCompressed) {
		char Check[5];
		memcpy(Check, &zROM.Data[Offset], 4);
		Check[4] = 0x00;
		unsigned int Yaz0Size = Read32(zROM.Data, Offset + 4);

		if((!strcmp(Check, "Yaz0")) && (Yaz0Size == Size)) {
			dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Decompressing data: Offset 0x%08X, Length 0x%08X...\n", Offset + 0x10, Size);
			zl_Yaz0Decode(&zROM.Data[Offset + 0x10], zRAM[Segment].Data, Size);
		} else {
			dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Copying data: Offset 0x%08X, Length 0x%08X...\n", Offset, Size);
			memcpy(zRAM[Segment].Data, &Buffer[Offset], Size);
		}
	} else {
		dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Copying data: Offset 0x%08X, Length 0x%08X...\n", Offset, Size);
		memcpy(zRAM[Segment].Data, &Buffer[Offset], Size);
	}
	zRAM[Segment].IsSet = true;
	zRAM[Segment].Size = Size;
}

void zl_ClearSegment(unsigned char Segment)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(0x%02X);\n", __FUNCTION__, Segment);

	if(zRAM[Segment].IsSet == true) {
		free(zRAM[Segment].Data);
		zRAM[Segment].IsSet = false;
		zRAM[Segment].Size = 0;
	}
}

void zl_ClearAllSegments()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	int i = 0;
	for(i = 0; i < 64; i++) {
		zl_ClearSegment(i);
	}
}

// Yaz0 decompression code from yaz0dec by thakis - http://www.amnoid.de/gc/
void zl_Yaz0Decode(unsigned char * Input, unsigned char * Output, int DecSize)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(0x%08X, 0x%08X, 0x%08X);\n", __FUNCTION__, Input, Output, DecSize);

	int SrcPlace = 0, DstPlace = 0;	//current read/write positions

	unsigned int ValidBitCount = 0;	//number of valid bits left in "code" byte
	unsigned char CodeByte = 0;
	while(DstPlace < DecSize) {
		//read new "code" byte if the current one is used up
		if(ValidBitCount == 0) {
			CodeByte = Input[SrcPlace];
			++SrcPlace;
			ValidBitCount = 8;
		}

		if((CodeByte & 0x80) != 0) {
			//straight copy
			Output[DstPlace] = Input[SrcPlace];
			DstPlace++;
			SrcPlace++;
		} else {
			//RLE part
			unsigned char Byte1 = Input[SrcPlace];
			unsigned char Byte2 = Input[SrcPlace + 1];
			SrcPlace += 2;

			unsigned int Dist = ((Byte1 & 0xF) << 8) | Byte2;
			unsigned int CopySource = DstPlace - (Dist + 1);
			unsigned int NumBytes = Byte1 >> 4;
			if(NumBytes == 0) {
				NumBytes = Input[SrcPlace] + 0x12;
				SrcPlace++;
			} else
				NumBytes += 2;

			//copy run
			int i;
			for(i = 0; i < NumBytes; ++i) {
				Output[DstPlace] = Output[CopySource];
				CopySource++;
				DstPlace++;
			}
		}

		//use next bit from "code" byte
		CodeByte <<= 1;
		ValidBitCount-=1;
	}
}

bool zl_CheckAddressValidity(unsigned int Address)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(0x%08X);\n", __FUNCTION__, Address);

	unsigned char Segment = (Address & 0xFF000000) >> 24;
	unsigned int Offset = (Address & 0x00FFFFFF);

	if(zRAM[Segment].IsSet == false) {
		dbgprintf(1, MSK_COLORTYPE_WARNING, "- Warning: Segment 0x%02X was not initialized, cannot access offset 0x%06X!\n", Segment, Offset);
		return false;
	} else if(zRAM[Segment].Size < Offset) {
		dbgprintf(1, MSK_COLORTYPE_WARNING, "- Warning: Offset 0x%06X is out of bounds for segment 0x%02X!\n", Offset, Segment);
		dbgprintf(1, MSK_COLORTYPE_WARNING, " (Segment size: 0x%06X)\n", zRAM[Segment].Size);
		return false;
	}

	return true;
}

void zl_ClearStructures(bool Full)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	int i = 0, j = 0;

	static const struct __zVertex zVertex_Empty;
	for(i = 0; i < 32; i++) zVertex[i] = zVertex_Empty;

	static const struct __zTexture zTexture_Empty;
	zTexture[0] = zTexture_Empty;
	zTexture[1] = zTexture_Empty;

	static const struct __zTextureCache zTextureCache_Empty;
	for(i = 0; i < 512; i++) zTextureCache[i] = zTextureCache_Empty;
	zProgram.TextureCachePosition = 0;

	static const struct __zRGBA zRGBA_Empty;
	zGfx.BlendColor = zRGBA_Empty;
	zGfx.EnvColor = zRGBA_Empty;
	zGfx.FogColor = zRGBA_Empty;
	static const struct __zFillColor zFillColor_Empty;
	zGfx.FillColor = zFillColor_Empty;
	static const struct __zPrimColor zPrimColor_Empty;
	zGfx.PrimColor = zPrimColor_Empty;

	zGfx.DLStackPos = 0;

	zGfx.ChangedModes = 0;
	zGfx.GeometryMode = 0;
	zGfx.OtherModeL = 0;
	zGfx.OtherModeH = 0;
	zGfx.Store_RDPHalf1 = 0; zGfx.Store_RDPHalf2 = 0;
	zGfx.Combiner0 = 0; zGfx.Combiner1 = 0;

	if(Full) {
		i = 0; j = 0;

		static const struct __zHeader zHeader_Empty;
		for(i = 0; i < 256; i++) zSHeader[i] = zHeader_Empty;
//		for(i = 0; i < 256; i++) for(j = 0; j < 256; j++) zMHeader[i][j] = zHeader_Empty;		// SEGFAULT

		static const struct __zFragmentCache zFragmentCache_Empty;
		for(i = 0; i < 256; i++) zFragmentCache[i] = zFragmentCache_Empty;
		zProgram.FragCachePosition = 0;

		static const struct __zCamera zCamera_Empty;
		zCamera = zCamera_Empty;
	}
}

void zl_ExecuteHeader(unsigned char Segment, unsigned int Offset, int SHeaderNumber, int MHeaderNumber)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(0x%02X, 0x%08X, %i, %i);\n", __FUNCTION__, Segment, Offset, SHeaderNumber, MHeaderNumber);

	if(!zl_CheckAddressValidity((Segment << 24) | Offset)) return;

	bool EndOfHeader = false;

	while(!EndOfHeader) {
		unsigned int w0 = Read32(zRAM[Segment].Data, Offset);
		unsigned int w1 = Read32(zRAM[Segment].Data, Offset + 4);

		switch(w0 >> 24) {
			case 0x00:
				zSHeader[SHeaderNumber].SActorCount = (w0 & 0x00FF0000) >> 16;
				zSHeader[SHeaderNumber].SActorOffset = w1;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - %i Scene actor(s) at 0x%08X.\n", zSHeader[SHeaderNumber].SActorCount, zSHeader[SHeaderNumber].SActorOffset);
				break;
			case 0x01:
				zMHeader[SHeaderNumber][MHeaderNumber].MActorCount = (w0 & 0x00FF0000) >> 16;
				zMHeader[SHeaderNumber][MHeaderNumber].MActorOffset = w1;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - %i Map actor(s) at 0x%08X.\n", zMHeader[SHeaderNumber][MHeaderNumber].MActorCount, zMHeader[SHeaderNumber][MHeaderNumber].MActorOffset);
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
				zMHeader[SHeaderNumber][MHeaderNumber].MapTime = (w0 & 0xFFFF0000) >> 16;
				zMHeader[SHeaderNumber][MHeaderNumber].TimeFlow = (w1 & 0x0000FF00) >> 8;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - 'Map time' is %i, 'time flow setting' is %i.\n", zMHeader[SHeaderNumber][MHeaderNumber].MapTime, zMHeader[SHeaderNumber][MHeaderNumber].TimeFlow);
				break;
			case 0x12:
				zMHeader[SHeaderNumber][MHeaderNumber].Skybox = (w0 & 0xFF000000) >> 24;
				dbgprintf(1, MSK_COLORTYPE_INFO, " - 'Skybox setting' is %i.\n", zMHeader[SHeaderNumber][MHeaderNumber].Skybox);
				break;
			case 0x14:
				EndOfHeader = true;
				break;
			case 0x16:
				zMHeader[SHeaderNumber][MHeaderNumber].EchoLevel = (w0 & 0x000000FF);
				dbgprintf(1, MSK_COLORTYPE_INFO, " - 'Echo level setting' is %i.\n", zMHeader[SHeaderNumber][MHeaderNumber].EchoLevel);
				break;
			default:
				break;
		}

		Offset += 8;
	}

	dbgprintf(1, MSK_COLORTYPE_INFO, "\n");
}

void zl_GetDisplayLists(int MapNumber)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%i);\n", __FUNCTION__, MapNumber);

	unsigned char Segment = (zMHeader[0][MapNumber].MeshHeaderOffset & 0xFF000000) >> 24;
	unsigned int Offset = (zMHeader[0][MapNumber].MeshHeaderOffset & 0x00FFFFFF);
	unsigned int MeshHeader = Read32(zRAM[Segment].Data, Offset);

	unsigned char MeshType = (MeshHeader & 0xFF000000) >> 24;
	unsigned char MeshTotal = (MeshHeader & 0x00FF0000) >> 16;

	dbgprintf(1, MSK_COLORTYPE_INFO, " - Mesh setup type %i, %i meshes.\n", MeshType, MeshTotal);

	int MeshCount = 0;

	switch(MeshType) {
		case 0x00: {
			unsigned int DListStart1, DListStart2;
			Offset += 12;

			while(MeshCount < MeshTotal) {
				DListStart1 = Read32(zRAM[Segment].Data, Offset);
				DListStart2 = Read32(zRAM[Segment].Data, Offset + 4);

				if(DListStart1 != 0) zGfx.DLOffset[MapNumber][zGfx.DLCount[MapNumber]++] = DListStart1;
				if(DListStart2 != 0) zGfx.DLOffset[MapNumber][zGfx.DLCount[MapNumber]++] = DListStart2;

				Offset += 8;

				MeshCount++;
			}
			break; }

		case 0x01: {
			unsigned int DListStart;
			Offset += 4;

			DListStart = Read32(zRAM[Segment].Data, Offset);
			DListStart = Read32(zRAM[Segment].Data, (DListStart & 0x00FFFFFF));

			if(DListStart != 0) zGfx.DLOffset[MapNumber][zGfx.DLCount[MapNumber]++] = DListStart;

			break; }

		case 0x02: {
			short ClipMaxX, ClipMaxZ, ClipMinX, ClipMinZ;
			unsigned int DListStart1, DListStart2;
			Offset += 12;

			while(MeshCount < MeshTotal) {
				ClipMaxX = Read16(zRAM[Segment].Data, Offset);
				ClipMaxZ = Read16(zRAM[Segment].Data, Offset + 2);
				ClipMinX = Read16(zRAM[Segment].Data, Offset + 4);
				ClipMinZ = Read16(zRAM[Segment].Data, Offset + 6);
				DListStart1 = Read32(zRAM[Segment].Data, Offset + 8);
				DListStart2 = Read32(zRAM[Segment].Data, Offset + 12);

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
		dbgprintf(1, MSK_COLORTYPE_INFO, " - Found %i Display Lists.\n", zGfx.DLCount[MapNumber]);
	}
}

void zl_ExecuteDisplayLists(int MapNumber)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	zGfx.DLStackPos = 0;

	int DL = 0;

	zGfx.GLListCount[MapNumber] = glGenLists(zGfx.DLCount[MapNumber]);
	glListBase(zGfx.GLListCount[MapNumber]);

	while(DL < zGfx.DLCount[MapNumber]) {
		if(zl_CheckAddressValidity(zGfx.DLOffset[MapNumber][DL])) {
			glNewList(zGfx.GLListCount[MapNumber] + DL, GL_COMPILE);
				zGfx.DLStackPos = 0;
				dl_ParseDisplayList(zGfx.DLOffset[MapNumber][DL]);
			glEndList();
		}

		DL++;
	}
}

void zl_DeInit()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	if(zROM.Data != NULL) free(zROM.Data);
	if(zGame.STBuffer != NULL) free(zGame.STBuffer);
}

void zl_HexPrint(unsigned char * Buffer, int Offset, int Length)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(0x%08X, 0x%08X, 0x%08X);\n", __FUNCTION__, Buffer, Offset, Length);

	int i = 0, j = 0;
	for(i = Offset; i < Offset + Length; i+=16) {
		dbgprintf(0, MSK_COLORTYPE_INFO, "%08X | ", i);
		for(j = 0; j < 16; j+=4) {
			int k = Read32(Buffer, (i + j));
			dbgprintf(0, MSK_COLORTYPE_INFO, "%08X ", k);
		}
		dbgprintf(0, MSK_COLORTYPE_INFO, "\n");
	}
}
