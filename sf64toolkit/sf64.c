#include "globals.h"

#include "sf64_ver.h"

unsigned char * MIO0Buffer;

bool sf_Init()
{
	// assume init goes right
	bool RetVal = TRUE;

	// try to load ROM
	if(!sf_DoLoadROM()) {
		// if ROM has been loaded, show infos
		MSK_ConsolePrint(MSK_COLORTYPE_INFO, " - Filename:      %s\n", ROM.Filename);
		MSK_ConsolePrint(MSK_COLORTYPE_INFO, " - Size:          %iMB (%i Mbit)\n", (ROM.Size / 0x100000), (ROM.Size / 0x20000));
		MSK_ConsolePrint(MSK_COLORTYPE_INFO, " - Title:         %s\n", ROM.Title);
		MSK_ConsolePrint(MSK_COLORTYPE_INFO, " - Game ID:       %s\n", ROM.GameID);
		MSK_ConsolePrint(MSK_COLORTYPE_INFO, " - Version:       1.%X\n", ROM.Version);
		MSK_ConsolePrint(MSK_COLORTYPE_INFO, " - CRC1:          0x%08X\n", ROM.CRC1);
		MSK_ConsolePrint(MSK_COLORTYPE_INFO, " - CRC2:          0x%08X\n\n", ROM.CRC2);

		// now, try to recognize the ROM
		bool CheckOkay = FALSE;
		int i = 0;
		for(i = 0; i < ArraySize(GameVer); i++) {
			if(!strcmp(ROM.GameID, GameVer[i].GameID) && ROM.Version == GameVer[i].Version && ROM.CRC1 == GameVer[i].CRC1 && ROM.CRC2 == GameVer[i].CRC2) {
				CheckOkay = TRUE;
				memcpy(&ThisGame, &GameVer[i], sizeof(__GameVer));
			}
		}

		if(!CheckOkay) {
			// if it couldn't be found, throw error
			MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: ROM could not be recognized!\n");
			RetVal = FALSE;
		} else {
			// if it was recognized, tell us and try to read the DMA table
			MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "- ROM has been recognized as '%s'.\n\n", ThisGame.TitleText);

			if(sf_ReadDMATable()) RetVal = FALSE;
		}
	} else {
		// if ROM couldn't even be loaded, just return the old status here, to (if true) preserve whatever ROM was loaded before
		return IsROMLoaded;
	}

	if(RetVal) {
		// if no error occured, tell us that the ROM has been loaded
		MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "- ROM has been loaded!\n");
	} else {
		// else, free the loaded ROM
		free(ROM.Data);
	}

	return RetVal;
}

void sf_LoadROM(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: No file specified!\n");
	} else {
		fn_GetFilename(Ptr+1);
		IsROMLoaded = sf_Init();
	}
}

int sf_DoLoadROM()
{
	FILE * File;
	if((File = fopen(ROM.Filepath, "rb")) == NULL) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: File '%s' not found!\n", ROM.Filename);
		return EXIT_FAILURE;
	}

	MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "- Loading ROM...\n");

	fseek(File, 0, SEEK_END);
	ROM.Size = ftell(File);
	rewind(File);
	ROM.Data = (unsigned char*) malloc (sizeof(char) * ROM.Size);
	fread(ROM.Data, 1, ROM.Size, File);
	fclose(File);

	memcpy(ROM.Title, &ROM.Data[32], 20);
	memcpy(ROM.GameID, &ROM.Data[59], 4);
	ROM.Version = ROM.Data[63];
	ROM.CRC1 = Read32(ROM.Data, 16);
	ROM.CRC2 = Read32(ROM.Data, 20);

	return EXIT_SUCCESS;
}

int sf_ReadDMATable()
{
	MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "- Reading DMA table...\n");

	int i = 0;
	DMAFileCount = 0;

	unsigned int CurrentPos = ThisGame.DMATableOffset;

	while(1) {	// keep going
		DMA[i].VStart = Read32(ROM.Data, CurrentPos);
		DMA[i].PStart = Read32(ROM.Data, CurrentPos + 4);
		DMA[i].PEnd = Read32(ROM.Data, CurrentPos + 8);
		DMA[i].CompFlag = Read32(ROM.Data, CurrentPos + 12);

		if((DMA[i].PStart == 0x00) && (DMA[i].PEnd == 0x00)) break;

		CurrentPos += 16;
		i++;
	}

	DMAFileCount = i;

	if(DMAFileCount == 0) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: No files found in DMA table!\n");
		return EXIT_FAILURE;
	} else {
		MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "- %i files found in DMA table.\n\n", DMAFileCount);
	}

	return EXIT_SUCCESS;
}

void sf_ListDMATable(unsigned char * Ptr)
{
	if(!IsROMLoaded) {
		MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "- No ROM loaded, cannot list files!\n");
		return;
	}

	MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "- Listing files in DMA table...\n");

	int i = 0;
	while(!(i == DMAFileCount)) {
		MSK_ConsolePrint(MSK_COLORTYPE_INFO, " - File %i: VStart 0x%06X, PStart 0x%06X, PEnd 0x%06X%s\n", i + 1, DMA[i].VStart, DMA[i].PStart, DMA[i].PEnd, (DMA[i].CompFlag ? " (MIO0)" : ""));
		i++;
	}

	MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "- %i files in DMA table.\n", i);
}

void sf_ExtractFiles(unsigned char * Ptr)
{
	if(!IsROMLoaded) {
		MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "- No ROM loaded, cannot extract files!\n");
		return;
	}

	MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "- Extracting all files in DMA table...\n");

	char Filepath[MAX_PATH];
	sprintf(Filepath, "%s%s%c", AppPath, ROM.GameID, Sep);

	char Cmd[MAX_PATH];
	#ifdef WIN32
	sprintf(Cmd, "mkdir \"%s\" 2> NUL", Filepath);	//redirect eventual errors to null
	#else
	sprintf(Cmd, "mkdir -p %s", Filepath);
	#endif
	system(Cmd);

	int i = 0;
	FILE * File, * LayoutFile;
	char Filename[MAX_PATH];
	char LayoutFilename[MAX_PATH];

	strcpy(LayoutFilename, Filepath);
	sprintf(LayoutFilename, "%slayout.txt", LayoutFilename);

	if((LayoutFile = fopen(LayoutFilename, "w")) == NULL) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Could not create layout file!\n");
		ReturnVal.s8 = EXIT_FAILURE;
		return;
	}

	// write dummy filesize to layout file
	int RebuildFilesize = 0;
	fprintf(LayoutFile, "0x00000000 rebuilt.z64\n");

	while(!(i == DMAFileCount)) {
		strcpy(Filename, Filepath);
		sprintf(Filename, "%s%02i_%08X-%08X_vs%08X.%s", Filename, i, DMA[i].PStart, DMA[i].PEnd, DMA[i].VStart, (DMA[i].CompFlag ? "mio" : "bin"));

		fprintf(LayoutFile, "%02i_%08X-%08X_vs%08X.bin\n", i, DMA[i].PStart, DMA[i].PEnd, DMA[i].VStart);

		if((File = fopen(Filename, "wb")) == NULL) {
			MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Could not create output file!\n");
			ReturnVal.s8 = EXIT_FAILURE;
			break;
		}

		int Size = DMA[i].PEnd - DMA[i].PStart;
		fwrite(&ROM.Data[DMA[i].PStart], 1, Size, File);
		fclose(File);

		if(DMA[i].CompFlag) {
			// create additional decompressed file
			strcpy(Filename, Filepath);
			sprintf(Filename, "%s%02i_%08X-%08X_vs%08X.bin", Filename, i, DMA[i].PStart, DMA[i].PEnd, DMA[i].VStart);

			int MIO0Size = sf_DecompressMIO0(DMA[i].PStart);
			if(MIO0Size == EXIT_FAILURE) {
				MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "\n- Error: Failed to decompress MIO0 data!\n");
				ReturnVal.s8 = EXIT_FAILURE;
				break;
			}

			if((File = fopen(Filename, "wb")) == NULL) {
				MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Could not create output file!\n");
				ReturnVal.s8 = EXIT_FAILURE;
				break;
			}
			fwrite(MIO0Buffer, 1, MIO0Size, File);
			fclose(File);

			free(MIO0Buffer);

			// add decompressed size to filesize
			RebuildFilesize += MIO0Size;
		} else {
			// add size to filesize
			RebuildFilesize += Size;
		}

		i++;
	}

	// pad filesize to multiple of 0x400000
	RebuildFilesize += GetPaddingSize(RebuildFilesize, 0x400000);

	// rewind and write filesize to layout file
	rewind(LayoutFile);
	fprintf(LayoutFile, "0x%08X rebuilt.z64\n", RebuildFilesize);

	fclose(LayoutFile);

	MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "- %i files extracted.\n", i);
}

void sf_CreateExpandedROM(unsigned char * Ptr)
{
	if(!IsROMLoaded) {
		MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "- No ROM loaded, cannot create expanded ROM!\n");
		return;
	}

	if(Ptr == NULL) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: No output file specified!\n");
	} else {
		char Filename[256];
		strcpy(Filename, AppPath);
		strcat(Filename, Ptr+1);

		MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "- Creating ROM...\n");

		unsigned char * DecROMBuffer = (unsigned char*) malloc (sizeof(char) * (ROM.Size + 0x400000));

		int i = 0;
		while(!(i == DMAFileCount)) {
			if(DMA[i].CompFlag == 0x00) {
				//virtual start = virtual start
				//physical start = virtual start
				//physical end = virtual start + (physical end - physical start)
				//compression flag = 0

				memcpy(&DecROMBuffer[DMA[i].VStart], &ROM.Data[DMA[i].PStart], (DMA[i].PEnd - DMA[i].PStart));

				Write32(DecROMBuffer, (ThisGame.DMATableOffset + (16 * i) + 4), DMA[i].VStart);
				Write32(DecROMBuffer, (ThisGame.DMATableOffset + (16 * i) + 8), (DMA[i].VStart + (DMA[i].PEnd - DMA[i].PStart)));

				MSK_ConsolePrint(MSK_COLORTYPE_INFO, " - File %i: Raw data at 0x%06X copied to 0x%06X.\n", i + 1, DMA[i].PStart, DMA[i].VStart);

			} else {
				//virtual start = virtual start
				//physical start = virtual start
				//physical end = virtual start + mio0 filesize
				//compression flag = 0

				int MIO0Size = sf_DecompressMIO0(DMA[i].PStart);
				if(MIO0Size == EXIT_FAILURE) {
					MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "\n- Error: Failed to decompress MIO0 data!\n");
					free(DecROMBuffer);
					ReturnVal.s8 = EXIT_FAILURE;
					break;
				}

				memcpy(&DecROMBuffer[DMA[i].VStart], MIO0Buffer, MIO0Size);

				Write32(DecROMBuffer, (ThisGame.DMATableOffset + (16 * i) + 4), DMA[i].VStart);
				Write32(DecROMBuffer, (ThisGame.DMATableOffset + (16 * i) + 8), (DMA[i].VStart + MIO0Size));
				Write32(DecROMBuffer, (ThisGame.DMATableOffset + (16 * i) + 12), 0x00);

				MSK_ConsolePrint(MSK_COLORTYPE_INFO, " - File %i: MIO0 data at 0x%06X decompressed to 0x%06X.\n", i + 1, DMA[i].PStart, DMA[i].VStart);

				free(MIO0Buffer);
			}

			i++;
		}

		if(i == DMAFileCount) {
			MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "\n- Okay, now checking and fixing CRCs...\n");

			// now fixing checksum
			// using modified n64sums.c by Parasyte, see that file for more
			if(FixChecksum(DecROMBuffer)) {
				MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Could not fix CRCs!\n");
				ReturnVal.s8 = EXIT_FAILURE;
			}
		} else {
			ReturnVal.s8 = EXIT_FAILURE;
		}

		if(ReturnVal.s8 != EXIT_FAILURE) {
			FILE * File;
			if((File = fopen(Filename, "wb")) == NULL) {
				MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Could not create output file!\n");
			} else {
				fwrite(DecROMBuffer, 1, (ROM.Size + 0x400000), File);
				fclose(File);

				MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "\n- Expanded ROM has been created.\n");
			}
		} else {
			MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "\n- Error: Could not create expanded ROM! Please check log for errors!\n");
		}

		free(DecROMBuffer);
		free(ROM.Data);

		ReturnVal.s8 = EXIT_SUCCESS;
	}
}

void sf_CreateFreshROM(unsigned char * Ptr)
{
	if(Ptr == NULL) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: No layout file specified!\n");
	} else {
		MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "- Creating ROM from layout file...\n");

		// get layout file name
		char Temp[256];
		strcpy(Temp, AppPath);
		strcat(Temp, Ptr+1);
//		strcat(Temp, "sf64-rebuild\\layout.txt");

		// figure out path (needed as working dir)
		char FreshPath[MAX_PATH];
		strcpy(FreshPath, Temp);
		fn_GetPath(Temp, FreshPath);

		FILE * Fp = NULL;
		unsigned int ROMFilesize = 0;
		int FileCount = 0; char DMAFilename[256][256];

		// check for layout file
		if((Fp = fopen(Temp, "rb")) == NULL) {
			MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: File '%s' not found!\n", Temp);
		} else {
			// get target rom filesize & name from layout file
			fscanf(Fp, "0x%x %s", &ROMFilesize, Temp);
			char TargetFilename[256];
			strcpy(TargetFilename, FreshPath);
			strcat(TargetFilename, Temp);
//			MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "!!! target filesize is 0x%x\n", ROMFilesize);
//			MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "!!! target file is (path+)%s\n", Temp);

			// while not at end of layout file, get file name
			while(fscanf(Fp, "%s", (char*)&DMAFilename[FileCount++]) != EOF);
			fclose(Fp);

			FileCount--;

			// create new rom buffer & clear it to 0xff
			unsigned char * NewROMBuffer = (unsigned char*)malloc(sizeof(char) * ROMFilesize);
			memset(NewROMBuffer, 0xFF, ROMFilesize);

			// create struct for new DMA table
			struct __DMA NewDMA[256];

			// loop through all files and insert them into new buffer
			int i = 0;
			unsigned int ROMOffset = 0;
			while(i < FileCount) {
//				MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "!!! file %i is %s\n", i, DMAFilename[i]);
				strcpy(Temp, FreshPath);
				strcat(Temp, DMAFilename[i]);
				if((Fp = fopen(Temp, "rb")) == NULL) {
					MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: File '%s' not found!\n", Temp);
					break;
				}

				// read file into buffer at current offset
				fseek(Fp, 0, SEEK_END);
				int Size = ftell(Fp);
				rewind(Fp);
				fread(&NewROMBuffer[ROMOffset], 1, Size, Fp);

				// take note of offsets etc. for new DMA table
				NewDMA[i].VStart = ROMOffset;
				NewDMA[i].PStart = ROMOffset;
				NewDMA[i].PEnd = ROMOffset + Size;
				NewDMA[i].CompFlag = 0;
//				if(i == 1) MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "!!! code found, now at 0x%06x\n", NewDMA[i].PStart);
//				if(i == 2) MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "!!! dmatable found, now at 0x%06x\n", NewDMA[i].PStart);

				ROMOffset += Size;
				fclose(Fp);

				i++;
			}

			// go through all files again for offset fixing
			if(i == FileCount) {
				int ReplacementCount = 0;
				i = 0;
				while(i < FileCount) {
					// fix file references in boot file
					// only check for file virtual starts if file comes after DMA table
					if(i >= 2) {
						unsigned int OldVStart = Read32(NewROMBuffer, (NewDMA[2].VStart + (i * 0x10)));

						// start/end offsets of boot file
						unsigned int CheckOffset = NewDMA[1].VStart;
						unsigned int MaxOffset = NewDMA[1].PEnd;
//						unsigned int CheckOffset = 0;
//						unsigned int MaxOffset = ROMFilesize;

//						MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "!!! file %i, old VSTART 0x%06x, new VSTART 0x%06x\n", i, OldVStart, NewDMA[i].VStart);

						// go through it
						while(CheckOffset < MaxOffset) {
							// get 32-bit value to check
							unsigned int Check = Read32(NewROMBuffer, CheckOffset);

							// if the value equals the current file's old virtual start AND the old start is different from the new one
							if((Check == OldVStart) && (OldVStart != NewDMA[i].VStart)) {
								// ...write the new virtual start to this offset
								Write32(NewROMBuffer, CheckOffset, NewDMA[i].VStart);
								MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "!!! %i, 0x%06x: VSTART of %i, replaced 0x%06x with 0x%06x\n", ReplacementCount + 1, CheckOffset, i, Check, NewDMA[i].VStart);
								ReplacementCount++;
							}
							CheckOffset+=4;
						}
					}

					// fix file references in DMA table
					Write32(NewROMBuffer, (NewDMA[2].VStart + (i * 0x10)), NewDMA[i].VStart);
					Write32(NewROMBuffer, (NewDMA[2].VStart + (i * 0x10) + 4), NewDMA[i].PStart);
					Write32(NewROMBuffer, (NewDMA[2].VStart + (i * 0x10) + 8), NewDMA[i].PEnd);
					Write32(NewROMBuffer, (NewDMA[2].VStart + (i * 0x10) + 12), NewDMA[i].CompFlag);

					i++;
				}
//				MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "!!! %i replacements\n", ReplacementCount);
			} else {
				ReturnVal.s8 = EXIT_FAILURE;
			}

			// if all went well, check and fix the CRCs
			if(i == FileCount) {
				MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "\n- Okay, now checking and fixing CRCs...\n");

				if(FixChecksum(NewROMBuffer)) {
					MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Could not fix CRCs!\n");
					ReturnVal.s8 = EXIT_FAILURE;
				}
			} else {
				ReturnVal.s8 = EXIT_FAILURE;
			}

			// if the CRC stuff went well, create our new ROM
			if(ReturnVal.s8 != EXIT_FAILURE) {
				FILE * File;
				if((File = fopen(TargetFilename, "wb")) == NULL) {
					MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Could not create output file!\n");
				} else {
					fwrite(NewROMBuffer, 1, ROMFilesize, File);
					fclose(File);

					MSK_ConsolePrint(MSK_COLORTYPE_OKAY, "\n- ROM has been created.\n");
				}
			} else {
				MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "\n- Error: Could not create ROM! Please check log for errors!\n");
			}

			// free the new ROM's buffer
			free(NewROMBuffer);

			// signal success to the program
			ReturnVal.s8 = EXIT_SUCCESS;
		}
	}
}

// MIO0 decompression code by HyperHacker
int sf_DecompressMIO0(unsigned int SrcOffset)
{
	MIO0Header Header;
	unsigned char MapByte = 0x80, CurMapByte, Length;
	unsigned short SData, Dist;
	unsigned int NumBytesOutput = 0;
	unsigned int MapLoc = 0;	// current compression map position
	unsigned int CompLoc = 0;	// current compressed data position
	unsigned int RawLoc = 0;	// current raw data position
	unsigned int OutLoc = 0;	// current output position

	int i;

	Header.ID = Read32(ROM.Data, SrcOffset);
	Header.OutputSize = Read32(ROM.Data, SrcOffset + 4);
	Header.CompLoc = Read32(ROM.Data, SrcOffset + 8);
	Header.RawLoc = Read32(ROM.Data, SrcOffset + 12);

	// "MIO0"
	if(Header.ID != 0x4D494F30) {
		unsigned char *b = (unsigned char*)&Header.ID;
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Can't decompress, data is not MIO0 format! (Sig is '%c%c%c%c')\n", b[0], b[1], b[2], b[3]);
		return EXIT_FAILURE;
	}

	MIO0Buffer = (unsigned char*)malloc(Header.OutputSize);

	if(!MIO0Buffer) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Could not allocate buffer for MIO0 data!\n");
		return EXIT_FAILURE;
	}

	MapLoc = SrcOffset + 0x10;
	CompLoc = SrcOffset + Header.CompLoc;
	RawLoc = SrcOffset + Header.RawLoc;

	CurMapByte = ROM.Data[MapLoc];

	while(NumBytesOutput < Header.OutputSize) {
		// raw
		if(CurMapByte & MapByte) {
			MIO0Buffer[OutLoc] = ROM.Data[RawLoc]; // copy a byte to output.
			OutLoc++;
			RawLoc++;
			NumBytesOutput++;
		}

		// compressed
		else {
			SData = Read16(ROM.Data, CompLoc); // get compressed data
			Length = (SData >> 12) + 3;
			Dist = (SData & 0xFFF) + 1;

			// sanity check: can't copy from before first byte
			if(((int)OutLoc - Dist) < 0) {
				MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Invalid distance (%06X - %d = %d; Length %d) at 0x%06X!\n", OutLoc, Dist, OutLoc - Dist, Length, CompLoc);
				free(MIO0Buffer);
				return EXIT_FAILURE;
			}

			// copy from output
			for(i = 0; i < Length; i++) {
				MIO0Buffer[OutLoc] = MIO0Buffer[OutLoc - Dist];
				OutLoc++;
				NumBytesOutput++;
				if(NumBytesOutput >= Header.OutputSize) break;
			}
			CompLoc += 2;
		}

		MapByte >>= 1; // next map bit

		// if we did them all, get the next map byte
		if(!MapByte) {
			MapByte = 0x80;
			MapLoc++;
			CurMapByte = ROM.Data[MapLoc];

			// sanity check: map pointer should never reach this
			int Check = CompLoc;
			if(RawLoc < CompLoc) Check = RawLoc;

			if(MapLoc > Check) {
				MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Compression map overflows into %s!", ((CompLoc < RawLoc) ? "raw data" : "compressed data"));
				MSK_ConsolePrint(MSK_COLORTYPE_ERROR, " - OutLoc=%06X, MapLoc=%06X, RawLoc=%06X, CompLoc=%06X\n", OutLoc, MapLoc, RawLoc, CompLoc);
				free(MIO0Buffer);
				return EXIT_FAILURE;
			}
		}
	}

	return NumBytesOutput;
}
