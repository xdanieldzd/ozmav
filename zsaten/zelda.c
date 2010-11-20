#include "globals.h"

#include "zelda_ver.h"
#ifndef TRUE
 #define TRUE	1
#endif

bool zl_Init(char * Filename)
{
	dbgprintf(0, MSK_COLORTYPE_OKAY, "Loading ROM...\n");

	memset(&vGameROM, 0x00, sizeof(vGameROM));

	bool RetVal = TRUE;

	if(!zl_LoadROM(Filename)) {
		strcpy(vGameROM.fileName, getFilename(Filename));

		zl_ShowROMInformation();

		if(zl_GetGameVersion()) return EXIT_FAILURE;
		if(zl_GetDMATable()) return EXIT_FAILURE;
		if(zl_GetFilenameTable()) vZeldaInfo.hasFilenames = false;
		if(zl_GetCodeFile()) return EXIT_FAILURE;

		//zl_SetMipsWatchers();

		if(zl_ReadData()) return EXIT_FAILURE;
	} else {
		return vGameROM.isROMLoaded;
	}

	return RetVal;
}

int zl_LoadROM(char * Filename)
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s(\"%s\");\n", __FUNCTION__, Filename);

	FILE * file;
	if((file = fopen(Filename, "rb")) == NULL) {
		char Temp[MAX_PATH];
		strcpy(Temp, getFilename(Filename));
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: File '%s' not found\n", Filename);
		return EXIT_FAILURE;
	}

	strcpy(vGameROM.filePath, Filename);

	fseek(file, 0, SEEK_END);
	vGameROM.size = ftell(file);
	rewind(file);
	vGameROM.data = (unsigned char*) malloc (sizeof(char) * vGameROM.size);
	fread(vGameROM.data, 1, vGameROM.size, file);
	fclose(file);

	memcpy(vGameROM.title, &vGameROM.data[32], 20);
	memcpy(vGameROM.gameID, &vGameROM.data[59], 4);
	vGameROM.version = vGameROM.data[63];

	vGameROM.CRC1 = Read32(vGameROM.data, 16);
	vGameROM.CRC2 = Read32(vGameROM.data, 20);

	return EXIT_SUCCESS;
}

void zl_ShowROMInformation()
{
	dbgprintf(0, MSK_COLORTYPE_INFO, " - Filename:      %s\n", vGameROM.fileName);
	dbgprintf(0, MSK_COLORTYPE_INFO, " - Size:          %iMB (%i Mbit)\n", (vGameROM.size / 0x100000), (vGameROM.size / 0x20000));
	dbgprintf(0, MSK_COLORTYPE_INFO, " - Title:         %s\n", vGameROM.title);
	dbgprintf(0, MSK_COLORTYPE_INFO, " - Game ID:       %s\n", vGameROM.gameID);
	dbgprintf(0, MSK_COLORTYPE_INFO, " - Version:       1.%X\n", vGameROM.version);
	dbgprintf(0, MSK_COLORTYPE_INFO, " - CRC1:          0x%08X\n", vGameROM.CRC1);
	dbgprintf(0, MSK_COLORTYPE_INFO, " - CRC2:          0x%08X\n", vGameROM.CRC2);
}

int zl_GetGameVersion()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	bool CheckOkay = false;

	int i = 0;
	for(i = 0; i < arraySize(GameVer); i++) {
		if(!strcmp(vGameROM.title, GameVer[i].title) && !strcmp(vGameROM.gameID, GameVer[i].gameID) && vGameROM.version == GameVer[i].version) {
			CheckOkay = true;
			vZeldaInfo.gameType = GameVer[i].gameType;
			vZeldaInfo.codeFileNo = GameVer[i].codeFileNo;
			vZeldaInfo.codeRAMOffset = GameVer[i].codeRAMOffset;
			vZeldaInfo.objectTableOffset = GameVer[i].objectTableOffset;
			vZeldaInfo.actorTableOffset = GameVer[i].actorTableOffset;
			vZeldaInfo.objectCount = GameVer[i].objectCount;
			vZeldaInfo.actorCount = GameVer[i].actorCount;
			strcpy(vZeldaInfo.titleText, GameVer[i].titleText);
			break;
		}
	}

	if(CheckOkay) {
		dbgprintf(0, MSK_COLORTYPE_OKAY, "\nROM has been recognized as '%s'.\n\n", vZeldaInfo.titleText);
	} else {
		dbgprintf(0, MSK_COLORTYPE_ERROR,"\n- Error: ROM could not be recognized!\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int zl_ReadData()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	dbgprintf(0, MSK_COLORTYPE_OKAY, "Reading actor table...\n");

	unsigned int BaseOffset = vZeldaInfo.actorTableOffset;
	int ActorNumber = 0;
	int ValidActorCount = 0;
	DMA Actor;

	while(ActorNumber < vZeldaInfo.actorCount) {
		vActors[ActorNumber].isValid = false;

		vActors[ActorNumber].PStart =			Read32(vZeldaInfo.codeBuffer, BaseOffset);
		vActors[ActorNumber].PEnd =				Read32(vZeldaInfo.codeBuffer, BaseOffset + 4);
		vActors[ActorNumber].VStart =			Read32(vZeldaInfo.codeBuffer, BaseOffset + 8);
		vActors[ActorNumber].VEnd =				Read32(vZeldaInfo.codeBuffer, BaseOffset + 12);
		vActors[ActorNumber].ProfileVStart =	Read32(vZeldaInfo.codeBuffer, BaseOffset + 20);
		vActors[ActorNumber].NameRStart =		Read32(vZeldaInfo.codeBuffer, BaseOffset + 24);

		// if game is not compressed...
		if((!vZeldaInfo.isCompressed) &&/* (vActors[ActorNumber].ProfileVStart != 0) &&*/ (vActors[ActorNumber].NameRStart != 0)) {
			// calculate where the actor name starts inside the code file
			vActors[ActorNumber].NameCStart = (vActors[ActorNumber].NameRStart - vZeldaInfo.codeRAMOffset);

			// and read the name out
			unsigned char * Current = &vZeldaInfo.codeBuffer[vActors[ActorNumber].NameCStart];
			Current += sprintf(vActors[ActorNumber].ActorName, "%s", Current);
			while(!*Current) Current++;
		}
//00C010B0  80832210     00026890  bin src  ovl_player_actor
		Actor = zl_DMAVirtualToPhysical(vActors[ActorNumber].PStart, vActors[ActorNumber].PEnd);
		if((Actor.PStart != 0) && (Actor.PEnd != 0)) {
			vActors[ActorNumber].ActorData = zl_DMAToBuffer(Actor);
			vActors[ActorNumber].ActorSize = Actor.VEnd - Actor.VStart;
			vActors[ActorNumber].ObjectNumber = Read16(vActors[ActorNumber].ActorData, (vActors[ActorNumber].ProfileVStart - vActors[ActorNumber].VStart) + 8);
			vActors[ActorNumber].AltObjectNumber = 0;

			dbgprintf(2, MSK_COLORTYPE_INFO, "\n- Actor 0x%04X is called '%s', %i bytes\n", ActorNumber, vActors[ActorNumber].ActorName, vActors[ActorNumber].ActorSize);

			dbgprintf(2, MSK_COLORTYPE_INFO, "- Actor uses object 0x%04X\n", vActors[ActorNumber].ObjectNumber);

			vActors[ActorNumber].isValid = true;

			ValidActorCount++;
		} else if( ActorNumber == 0 ) {
			Actor = zl_DMAGetFileByFilename("ovl_player_actor");
			if(Actor.ID == -1)
				goto end;
			vActors[ActorNumber].ActorData	= zl_DMAToBuffer(Actor);
			vActors[ActorNumber].ActorSize	= Actor.VEnd - Actor.VStart;
			vActors[ActorNumber].ObjectNumber = 0x15;
			vActors[ActorNumber].AltObjectNumber = 0x14;
			vActors[ActorNumber].PStart	= Actor.VStart;
			vActors[ActorNumber].PEnd	= Actor.VEnd;
			vActors[ActorNumber].VStart	= 0x80832210; // WARNING HACK FIXME
			vActors[ActorNumber].VEnd	= vActors[ActorNumber].VStart + vActors[ActorNumber].ActorSize;
			vActors[ActorNumber].isValid = true;

			ValidActorCount++;
		}

		end:
		BaseOffset += 0x20;
		ActorNumber++;
	}

	dbgprintf(0, MSK_COLORTYPE_OKAY, "Found %i valid actor(s).\n", ValidActorCount);

	dbgprintf(0, MSK_COLORTYPE_OKAY, "Reading object table...\n");

	BaseOffset = vZeldaInfo.objectTableOffset;
	int ObjectNumber = 0;
	int ValidObjectCount = 0;

	while(ObjectNumber < vZeldaInfo.objectCount) {
		vObjects[ObjectNumber].isValid = false;

		vObjects[ObjectNumber].VStart =	Read32(vZeldaInfo.codeBuffer, BaseOffset);
		vObjects[ObjectNumber].VEnd =	Read32(vZeldaInfo.codeBuffer, BaseOffset + 4);

		DMA Object = zl_DMAVirtualToPhysical(vObjects[ObjectNumber].VStart, vObjects[ObjectNumber].VEnd);
		vObjects[ObjectNumber].PStart =	Object.PStart;
		vObjects[ObjectNumber].PEnd =	Object.PEnd;

		if((vObjects[ObjectNumber].PStart != 0) && (vObjects[ObjectNumber].PEnd != 0)) {
			vObjects[ObjectNumber].ObjectData = zl_DMAToBuffer(Object);
			vObjects[ObjectNumber].ObjectSize = Object.PEnd - Object.PStart;

			vObjects[ObjectNumber].ObjectSegment = 0x06;
			if(ObjectNumber == 0x0001) {
				vObjects[ObjectNumber].ObjectSegment = 0x04;
			} else if(ObjectNumber == 0x0002 || ObjectNumber == 0x0003) {
				vObjects[ObjectNumber].ObjectSegment = 0x05;
			}

			if(Object.Filename != NULL) {
				strcpy(vObjects[ObjectNumber].ObjectName, Object.Filename);
				dbgprintf(2, MSK_COLORTYPE_INFO, "- Object 0x%04X is called '%s', %i bytes\n", ObjectNumber, vObjects[ObjectNumber].ObjectName, vObjects[ObjectNumber].ObjectSize);
			}

			vObjects[ObjectNumber].isValid = true;
			ValidObjectCount++;
		}

		BaseOffset += 0x08;
		ObjectNumber++;
	}

	dbgprintf(0, MSK_COLORTYPE_OKAY, "Found %i valid object(s).", ValidObjectCount);

	return EXIT_SUCCESS;
}

DMA zl_DMAGetFile(int DMAFileNo)
{
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%i);\n", __FUNCTION__, DMAFileNo);

	DMA File = {0, 0, 0, 0, 0, ""};

	File.ID = DMAFileNo;

	unsigned int Offset = vZeldaInfo.DMATableOffset + (DMAFileNo * 0x10);
	File.VStart = Read32(vGameROM.data, Offset);
	File.VEnd = Read32(vGameROM.data, Offset + 4);
	File.PStart = Read32(vGameROM.data, Offset + 8);
	File.PEnd = Read32(vGameROM.data, Offset + 12);

	dbgprintf(2, MSK_COLORTYPE_OKAY, "| VStart 0x%08X, VEnd 0x%08X, PStart 0x%08X, PEnd 0x%08X\n", File.VStart, File.VEnd, File.PStart, File.PEnd);

	zl_DMAGetFilename(File.Filename, DMAFileNo);

	return File;
}

void zl_DMAGetFilename(char * Name, int DMAFileNo)
{
	if(vZeldaInfo.hasFilenames) {
		int FilenameNo = 0;
		unsigned char * Current;

		for(Current = &vGameROM.data[vZeldaInfo.filenameTableOffset], FilenameNo = 0; FilenameNo < (DMAFileNo + 1); FilenameNo++) {
			Current += sprintf(Name, "%s", Current);
			while(!*Current) Current++;
		}
	}
}

DMA zl_DMAGetFileByFilename(char * Name)
{
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%s);\n", __FUNCTION__, Name);

	DMA File = {0, 0, 0, 0, 0, ""};
	char Filename[MAX_PATH];

	//int i;

	// turn entered filename into lower case
	//for(i = 0; Name[i]; i++) Name[i] = tolower(Name[i]); bad idea; also corrupts memory

	// does ROM contain filenames?
	if(vZeldaInfo.hasFilenames) {
		int FileNo = 0;

		File = zl_DMAGetFile(FileNo);

		// while we're not at the end of the DMA table...
		while((File.VEnd != 0x00) || (File.PStart != 0xFFFFFFFF)) {
			if(File.VStart == File.VEnd) break;
			File = zl_DMAVirtualToPhysical(File.VStart, File.VEnd);

			// read the filename out and turn it into lower case
			zl_DMAGetFilename(Filename, FileNo);
			//for(i = 0; Filename[i]; i++) Filename[i] = tolower(Filename[i]);

			// compare found filename with entered one...
			if(!strcmp(Name, Filename)) {
				// filename found, return file information
				return File;
			}

			FileNo++;
			File = zl_DMAGetFile(FileNo);
		}
	}

	DMA Empty = {-1, 0, 0, 0, 0, ""};
	return Empty;
}

DMA zl_DMAVirtualToPhysical(unsigned int VStart, unsigned int VEnd)
{
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] %s(0x%08X);\n", __FUNCTION__, VStart);

	DMA File = {0, 0, 0, 0, 0, ""};

	unsigned int Offset = vZeldaInfo.DMATableOffset;
	while(Offset < vZeldaInfo.DMATableOffset + (vZeldaInfo.DMATable.VEnd - vZeldaInfo.DMATable.VStart)) {
		unsigned int Temp = Read32(vGameROM.data, Offset);
		if(Temp == VStart) {
			File.VStart = Temp;
			File.VEnd = Read32(vGameROM.data, Offset + 4);
			File.PStart = Read32(vGameROM.data, Offset + 8);
			File.PEnd = Read32(vGameROM.data, Offset + 12);

			if(File.VStart == File.PStart) {		// not compressed
				File.PEnd = File.VEnd;
			}

			if(File.PEnd == 0x00) {
				File.PEnd = File.PStart + (File.VEnd - File.VStart);
			}

			goto found;
		}
		Offset+=16;
		File.ID++;
	}

found:
	dbgprintf(2, MSK_COLORTYPE_OKAY, "| VStart 0x%08X -> PStart 0x%08X, PEnd 0x%08X\n", VStart, File.PStart, File.PEnd);

	zl_DMAGetFilename(File.Filename, File.ID);

	return File;
}

int zl_GetFilenameTable()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	vZeldaInfo.bootfile = zl_DMAGetFile(1);

	char Check[8];
	int Pos = vZeldaInfo.bootfile.PStart, MaxPos = vZeldaInfo.bootfile.PEnd;
	if(vZeldaInfo.bootfile.PEnd == 0 || vZeldaInfo.bootfile.PEnd == 0xFFFFFFFF) MaxPos = vZeldaInfo.bootfile.VEnd;

	while(Pos < MaxPos) {
		memcpy(Check, &vGameROM.data[Pos], 7);
		Check[7] = 0x00;
		if(!strcmp(Check, "makerom")) {
			vZeldaInfo.filenameTableOffset = Pos;
			dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Filename table found at 0x%08X.\n", vZeldaInfo.filenameTableOffset);
			vZeldaInfo.hasFilenames = true;
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
		memcpy(Check, &vGameROM.data[Pos], 9);
		Check[9] = 0x00;
		if(!strcmp(Check, "zelda@srd")) {
			memcpy(Creator, &vGameROM.data[Pos], 16);
			if(Creator[15] != 0x00) {
				memcpy(Build, &vGameROM.data[Pos + 12], 20);
			} else {
				memcpy(Build, &vGameROM.data[Pos + 16], 20);
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

	vZeldaInfo.DMATableOffset = (Pos + 0x30);
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] DMA table found at 0x%08X.\n", vZeldaInfo.DMATableOffset);

	vZeldaInfo.DMATable = zl_DMAGetFile(2);

	return EXIT_SUCCESS;
}

int zl_GetCodeFile()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	char Check[4];

	DMA File = zl_DMAGetFile(vZeldaInfo.codeFileNo);
	unsigned int File_BufferSize = File.VEnd - File.VStart;
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] 'Code' is file #%i, File length is 0x%08X.\n", vZeldaInfo.codeFileNo, File_BufferSize);

	vZeldaInfo.isCompressed = true;
	if(File.PEnd == 0 || File.PEnd == 0xFFFFFFFF) vZeldaInfo.isCompressed = false;
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] File is %s.\n", vZeldaInfo.isCompressed ? "compressed, assuming whole ROM is compressed" : "not compressed");

	unsigned char * NewBuffer = malloc(sizeof(char) * File_BufferSize);
	if(NewBuffer != NULL) {
		memset(NewBuffer, 0x00, (sizeof(char) * File_BufferSize));
		vZeldaInfo.codeBuffer = NewBuffer;
	} else {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not allocate temporary buffer!\n");
		return EXIT_FAILURE;
	}

	if(vZeldaInfo.isCompressed && File.PEnd) {
		memcpy(Check, &vGameROM.data[File.PStart], 4);
		unsigned int Yaz0Size = Read32(vGameROM.data, File.PStart + 4);		// should be same as File.BufferSize

		if((!strncmp(Check, "Yaz0", 4)) && (Yaz0Size == File_BufferSize)) {
			dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Decompressing data: Offset 0x%08X, Length 0x%08X...\n", File.PStart, File_BufferSize);
			RDP_Yaz0Decode(&vGameROM.data[File.PStart], vZeldaInfo.codeBuffer, File_BufferSize);
		} else {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not decompress data, data appears to be invalid!\n");
			return EXIT_FAILURE;
		}
	} else {
		dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Copying data: Offset 0x%08X, Length 0x%08X...\n", File.PStart, File_BufferSize);
		memcpy(vZeldaInfo.codeBuffer, &vGameROM.data[File.PStart], File_BufferSize);
	}

	return EXIT_SUCCESS;
}

unsigned char * zl_DMAToBuffer(DMA File)
{
	dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] %s(%08X);\n", __FUNCTION__, File);

	unsigned int File_BufferSize = File.VEnd - File.VStart;
	unsigned char * buffer = (unsigned char *)malloc(File_BufferSize);
	char Check[4];
	if(vZeldaInfo.isCompressed && File.PEnd) {
		memcpy(Check, &vGameROM.data[File.PStart], 4);
		unsigned int Yaz0Size = Read32(vGameROM.data, File.PStart + 4);		// should be same as File.BufferSize

		if((!strncmp(Check, "Yaz0", 4)) && (Yaz0Size == File_BufferSize)) {
			dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Decompressing data: Offset 0x%08X, Length 0x%08X...\n", File.PStart, File_BufferSize);
			RDP_Yaz0Decode(&vGameROM.data[File.PStart], buffer, File_BufferSize);
		} else {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not decompress data, data appears to be invalid!\n");
			return NULL;
		}
	} else {
		dbgprintf(2, MSK_COLORTYPE_OKAY, "[DEBUG] Copying data: Offset 0x%08X, Length 0x%08X...\n", File.PStart, File_BufferSize);
		memcpy(buffer, &vGameROM.data[File.PStart], File_BufferSize);
	}
	return buffer;
}

void zl_ClearAllSegments()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	int i = 0;
	for(i = 0; i < MAX_SEGMENTS; i++) {
		RDP_ClearSegment(i);
	}
}

void zl_DeInit()
{
	dbgprintf(3, MSK_COLORTYPE_OKAY, "[DEBUG] %s();\n", __FUNCTION__);

	if(vGameROM.data != NULL) free(vGameROM.data);
	if(vZeldaInfo.codeBuffer != NULL) free(vZeldaInfo.codeBuffer);

	int i = 0;
	for(i = 0; i < vZeldaInfo.actorCount; i++) {
		if(vActors[i].ActorData != NULL) free(vActors[i].ActorData);
	}
	for(i = 0; i < vZeldaInfo.objectCount; i++) {
		if(vObjects[i].ObjectData != NULL) free(vObjects[i].ObjectData);
	}
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
