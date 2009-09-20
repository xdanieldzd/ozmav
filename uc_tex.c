/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	uc_tex.c - F3DEX2 texturing-related functions
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

int F3DEX2_Cmd_TEXTURE()
{
	F3DEX2_ResetTextureStruct();

	unsigned long W1 = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	Texture[0].S_Scale = (float) _FIXED2FLOAT(_SHIFTR(W1, 16, 16), 16);
	Texture[0].T_Scale = (float) _FIXED2FLOAT(_SHIFTR(W1, 0, 16), 16);

	if(Texture[0].S_Scale == 0.0f) Texture[0].S_Scale = 1.0f;
	if(Texture[0].T_Scale == 0.0f) Texture[0].T_Scale = 1.0f;

	Texture[1].S_Scale = Texture[0].S_Scale;
	Texture[1].T_Scale = Texture[0].T_Scale;

	//fprintf(FileSystemLog, "TEXTURE: unknown = %02X\n\n", Readout_CurrentByte4);

	return 0;
}

int F3DEX2_Cmd_SETTIMG()
{
	CurrentTextureID = 0;
	IsMultiTexture = false;

	if(Readout_NextGFXCommand1 == G_RDPTILESYNC) {
		Texture[0].PalDataSource = Readout_CurrentByte5;
		Texture[0].PalOffset = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

		return 0;
	}

	if(Readout_PrevGFXCommand1 == G_SETTILESIZE) {
		/* multitexture */
		CurrentTextureID = 1;
		IsMultiTexture = true;
	}

	Texture[CurrentTextureID].DataSource = Readout_CurrentByte5;
	Texture[CurrentTextureID].Offset = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	Texture[CurrentTextureID].Format_OGL = GL_RGBA;
	Texture[CurrentTextureID].Format_OGLPixel = GL_RGBA;

	return 0;
}

int F3DEX2_Cmd_SETTILE()
{
	unsigned long W0 = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned long W1 = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	/* FIX FOR CI TEXTURES - IF BYTES 5-8 ARE 07000000, LEAVE CURRENT PROPERTY SETTINGS ALONE */
	if(W1 == 0x07000000) return 0;

	Texture[CurrentTextureID].Format_N64 = Readout_CurrentByte2;

	Texture[CurrentTextureID].Y_Parameter = _SHIFTR( W1, 18, 2 );
	Texture[CurrentTextureID].X_Parameter = _SHIFTR( W1, 8, 2 );

	Texture[CurrentTextureID].Palette = _SHIFTR( W1, 20, 4 );
	Texture[CurrentTextureID].LineSize = _SHIFTR( W0, 9, 9 );

	Texture[CurrentTextureID].TempSShift = _SHIFTR( W1,  0, 4 );
	Texture[CurrentTextureID].TempTShift = _SHIFTR( W1, 10, 4 );

	Texture[CurrentTextureID].S_Mask = _SHIFTR( W1,  4, 4 );
	Texture[CurrentTextureID].T_Mask = _SHIFTR( W1, 14, 4 );
/*
	fprintf(FileSystemLog, "SETTILE %08X %08X: shifts = 0x%02X -> %4.2f, shiftt = 0x%02X -> %4.2f\n",
		W0, W1,
		TempSShift, Texture[CurrentTextureID].S_ShiftScale,
		TempTShift, Texture[CurrentTextureID].T_ShiftScale);*/
/*
	fprintf(FileSystemLog, "SETTILE: fmt %x siz %x line %x tmem %x tile %x palette %x cmt %x cms %x maskt %x masks %x shiftt %x shifts %x\n",
				(unsigned int)_SHIFTR( W0, 21, 3 ),	// fmt
	            (unsigned int)_SHIFTR( W0, 19, 2 ),	// siz
	            (unsigned int)_SHIFTR( W0,  9, 9 ),	// line
				(unsigned int)_SHIFTR( W0,  0, 9 ),	// tmem
				(unsigned int)_SHIFTR( W1, 24, 3 ),	// tile
				(unsigned int)_SHIFTR( W1, 20, 4 ),	// palette
				(unsigned int)_SHIFTR( W1, 18, 2 ),	// cmt
				(unsigned int)_SHIFTR( W1,  8, 2 ),	// cms
				(unsigned int)_SHIFTR( W1, 14, 4 ),	// maskt
				(unsigned int)_SHIFTR( W1,  4, 4 ),	// masks
				(unsigned int)_SHIFTR( W1, 10, 4 ),	// shiftt
				(unsigned int)_SHIFTR( W1,  0, 4 ) );	// shifts
*/
	return 0;
}

int F3DEX2_Cmd_SETTILESIZE()
{
	unsigned long W0 = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned long W1 = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	F3DEX2_ChangeTileSize(	_SHIFTR( W1, 24,  3 ),		// tile
							_SHIFTR( W0, 12, 12 ),		// uls
							_SHIFTR( W0,  0, 12 ),		// ult
							_SHIFTR( W1, 12, 12 ),		// lrs
							_SHIFTR( W1,  0, 12 ) );	// dxt

	return 0;
}

int F3DEX2_ChangeTileSize(unsigned int Tile, unsigned int ULS, unsigned int ULT, unsigned int LRS, unsigned int LRT)
{
	fprintf(FileGFXLog, "Tile:%d, ULS:%d, ULT:%d, LRS:%d, LRT:%d\n", Tile, ULS, ULT, LRS, LRT);

/*	if((ULS != 0) || (ULT != 0)) return 0;

	Texture[CurrentTextureID].Width  = ((_SHIFTR( LRS, 2, 10 ) - _SHIFTR( ULS, 2, 10 )) + 1);
	Texture[CurrentTextureID].Height = ((_SHIFTR( LRT, 2, 10 ) - _SHIFTR( ULT, 2, 10 )) + 1);

	Texture[CurrentTextureID].Width >>= Texture[CurrentTextureID].S_Mask;
	Texture[CurrentTextureID].Width <<= Texture[CurrentTextureID].S_Mask;
	Texture[CurrentTextureID].Height >>= Texture[CurrentTextureID].T_Mask;
	Texture[CurrentTextureID].Height <<= Texture[CurrentTextureID].T_Mask;
*/
	Texture[CurrentTextureID].Tile = Tile;
	Texture[CurrentTextureID].ULS = ULS;
	Texture[CurrentTextureID].ULT = ULT;
	Texture[CurrentTextureID].LRS = LRS;
	Texture[CurrentTextureID].LRT = LRT;

	return 0;
}

int F3DEX2_Cmd_LOADTLUT(unsigned int PaletteSrc, unsigned long PaletteOffset)
{
	/* clear the palette buffer */
	memset(PaletteData, 0x00, sizeof(PaletteData));

	/* calculate palette size, 16 or 256, from parameters */
	unsigned int TempPaletteSize = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;
	unsigned int PaletteSize = ((TempPaletteSize & 0xFFF000) >> 14) + 1;

	/* copy raw palette into buffer */
	if(Zelda_MemCopy(PaletteSrc, PaletteOffset, PaletteData, PaletteSize * 2) == -1) {
		sprintf(ErrorMsg, "Invalid palette data source 0x%02X!", PaletteSrc);
//		MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
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

int F3DEX2_Cmd_LOADBLOCK()
{
	unsigned long W0 = (Readout_CurrentByte1 * 0x1000000) + (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned long W1 = (Readout_CurrentByte5 * 0x1000000) + (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	Texture[CurrentTextureID].AnimDXT = _SHIFTR( W1, 0, 12 );

	F3DEX2_ChangeTileSize(	_SHIFTR( W1, 24,  3 ),		// tile
							_SHIFTR( W0, 12, 12 ),		// uls
							_SHIFTR( W0,  0, 12 ),		// ult
							_SHIFTR( W1, 12, 12 ),		// lrs
							_SHIFTR( W1,  0, 12 ) );	// dxt

	/* STILL don't know nearly enough about this to emulate it in any way */

	return 0;
}

/*	------------------------------------------------------------ */

inline unsigned long Pow2(unsigned long dim) {
	unsigned long i = 1;

	while (i < dim) i <<= 1;

	return i;
}

inline unsigned long PowOf(unsigned long dim) {
	unsigned long num = 1;
	unsigned long i = 0;

	while (num < dim) {
		num <<= 1;
		i++;
	}

	return i;
}

int F3DEX2_CalcFinalTextureSize(int TextureID)
{
	/* TO BE DEBUGGED!! */

	unsigned int MaxTexel = 0;
	unsigned int Line_Shift = 0;

	switch(Texture[TextureID].Format_N64) {
		/* 4-bit */
		case 0x00: { MaxTexel = 4096; Line_Shift = 4; break; }	// RGBA
		case 0x40: { MaxTexel = 4096; Line_Shift = 4; break; }	// CI
		case 0x60: { MaxTexel = 8192; Line_Shift = 4; break; }	// IA
		case 0x80: { MaxTexel = 8192; Line_Shift = 4; break; }	// I

		/* 8-bit */
		case 0x08: { MaxTexel = 2048; Line_Shift = 3; break; }	// RGBA
		case 0x48: { MaxTexel = 2048; Line_Shift = 3; break; }	// CI
		case 0x68: { MaxTexel = 4096; Line_Shift = 3; break; }	// IA
		case 0x88: { MaxTexel = 4096; Line_Shift = 3; break; }	// I

		/* 16-bit */
		case 0x10: { MaxTexel = 2048; Line_Shift = 2; break; }	// RGBA
		case 0x50: { MaxTexel = 2048; Line_Shift = 0; break; }	// CI
		case 0x70: { MaxTexel = 2048; Line_Shift = 2; break; }	// IA
		case 0x90: { MaxTexel = 2048; Line_Shift = 0; break; }	// I

		/* 32-bit */
		case 0x18: { MaxTexel = 1024; Line_Shift = 2; break; }	// RGBA
	}

	unsigned int Line_Width = Texture[TextureID].LineSize << Line_Shift;

	unsigned int Tile_Width = Texture[TextureID].LRS - Texture[TextureID].ULS + 1;
	unsigned int Tile_Height = Texture[TextureID].LRT - Texture[TextureID].ULT + 1;

	unsigned int Mask_Width = 1 << Texture[TextureID].S_Mask;
	unsigned int Mask_Height = 1 << Texture[TextureID].T_Mask;

	unsigned int Line_Height = 0;
	if(Line_Width) Line_Height = min(MaxTexel / Line_Width, Tile_Height);

	if(Texture[TextureID].S_Mask && ((Mask_Width * Mask_Height) <= MaxTexel)) {
		Texture[TextureID].Width = Mask_Width;
	} else if((Tile_Width * Tile_Height) <= MaxTexel) {
		Texture[TextureID].Width = Tile_Width;
	} else {
		Texture[TextureID].Width = Line_Width;
	}

	if(Texture[TextureID].T_Mask && ((Mask_Width * Mask_Height) <= MaxTexel)) {
		Texture[TextureID].Height = Mask_Height;
	} else if((Tile_Width * Tile_Height) <= MaxTexel) {
		Texture[TextureID].Height = Tile_Height;
	} else {
		Texture[TextureID].Height = Line_Height;
	}

	unsigned int Clamp_Width = (Texture[TextureID].X_Parameter & G_TX_CLAMP) ? Tile_Width : Texture[TextureID].Width;
	unsigned int Clamp_Height = (Texture[TextureID].Y_Parameter & G_TX_CLAMP) ? Tile_Height : Texture[TextureID].Height;

	if(Clamp_Width > 256) Texture[TextureID].X_Parameter &= G_TX_CLAMP;
	if(Clamp_Height > 256) Texture[TextureID].Y_Parameter &= G_TX_CLAMP;

	if(Mask_Width > Texture[TextureID].Width) {
		Texture[TextureID].S_Mask = PowOf(Texture[TextureID].Width);
		Mask_Width = 1 << Texture[TextureID].S_Mask;
	}
	if(Mask_Height > Texture[TextureID].Height) {
		Texture[TextureID].T_Mask = PowOf(Texture[TextureID].Height);
		Mask_Height = 1 << Texture[TextureID].T_Mask;
	}

	if(Texture[TextureID].X_Parameter & G_TX_CLAMP) {
		Texture[TextureID].RealWidth = Pow2(Clamp_Width);
	} else if(Texture[TextureID].X_Parameter & G_TX_MIRROR) {
		Texture[TextureID].RealWidth = Mask_Width << 1;
	} else {
		Texture[TextureID].RealWidth = Pow2(Texture[TextureID].Width);
	}

	if(Texture[TextureID].Y_Parameter & G_TX_CLAMP) {
		Texture[TextureID].RealHeight = Pow2(Clamp_Height);
	} else if(Texture[TextureID].Y_Parameter & G_TX_MIRROR) {
		Texture[TextureID].RealHeight = Mask_Height << 1;
	} else {
		Texture[TextureID].RealHeight = Pow2(Texture[TextureID].Height);
	}

	Texture[TextureID].S_ShiftScale = 1.0f;
	Texture[TextureID].T_ShiftScale = 1.0f;

	if(Texture[TextureID].TempSShift > 10) {
		Texture[TextureID].S_ShiftScale = (float)(1 << (16 - Texture[TextureID].TempSShift));
	} else if(Texture[TextureID].TempSShift > 0) {
		Texture[TextureID].S_ShiftScale /= (float)(1 << Texture[TextureID].TempSShift);
	}

	if(Texture[TextureID].TempTShift > 10) {
		Texture[TextureID].T_ShiftScale = (float)(1 << (16 - Texture[TextureID].TempTShift));
	} else if(Texture[TextureID].TempTShift > 0) {
		Texture[TextureID].T_ShiftScale /= (float)(1 << Texture[TextureID].TempTShift);
	}

	if(GLExtension_VertFragProgram) {
		glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, TextureID, Texture[TextureID].S_ShiftScale, Texture[TextureID].T_ShiftScale, 1.0f, 1.0f);
	}

	/* NULLIFYING SOME STUFF ABOVE SINCE IT'S STILL BROKEN */
	/* --------------------------------------------------- */
	Texture[TextureID].RealWidth = Pow2(Texture[TextureID].Width);
	Texture[TextureID].RealHeight = Pow2(Texture[TextureID].Height);
	/* --------------------------------------------------- */

	return 0;
}

GLuint F3DEX2_LoadTexture(int TextureID)
{
	/* for now, don't do any texturing for objects - REALLY crashy-crashy atm */
//	if(!GetDLFromZMapScene) return 0;

	if((Readout_NextGFXCommand1 == 0x00000003) || (Readout_NextGFXCommand1 == 0x000000E1)) return 0;

	if(!(Texture[TextureID].Format_OGL == GL_RGBA)) return 0;

	int i = 0, j = 0;

	F3DEX2_CalcFinalTextureSize(TextureID);

	unsigned int BytesPerPixel = 0x08;
	switch(Texture[TextureID].Format_N64) {
		/* 4bit, 8bit */
		case 0x00: case 0x40: case 0x60: case 0x80:
		case 0x08: case 0x48: case 0x68: case 0x88:
			BytesPerPixel = 0x04;
			break;
		/* 16bit */
		case 0x10: case 0x50: case 0x70: case 0x90:
			BytesPerPixel = 0x08;
			break;
		/* 32bit */
		case 0x18:
			BytesPerPixel = 0x10;
			break;
	}

	unsigned long TextureBufferSize = (Texture[TextureID].RealHeight * Texture[TextureID].RealWidth) * BytesPerPixel;

	bool UnhandledTextureSource = false;

	TextureData_OGL = (unsigned char *) malloc (TextureBufferSize);
	TextureData_N64 = (unsigned char *) malloc (TextureBufferSize);

	memset(TextureData_OGL, 0x00, TextureBufferSize);
	memset(TextureData_N64, 0x00, TextureBufferSize);

	Debug_MallocOperations++;
	Debug_MallocOperations++;

	/* create solid red texture for unsupported stuff, such as kakariko windows */
	unsigned char * EmptyTexture;
	unsigned char * EmptyTexture_Green;

	EmptyTexture = (unsigned char *) malloc (sizeof(char) * TextureBufferSize);
	EmptyTexture_Green = (unsigned char *) malloc (sizeof(char) * TextureBufferSize);

	Debug_MallocOperations++;
	Debug_MallocOperations++;

	for(i = 0; i < TextureBufferSize; i+=4) {
		EmptyTexture[i]			= 0xFF;
		EmptyTexture[i + 1]		= 0x00;
		EmptyTexture[i + 2]		= 0x00;
		EmptyTexture[i + 3]		= 0xFF;

		EmptyTexture_Green[i]		= 0x00;
		EmptyTexture_Green[i + 1]	= 0xFF;
		EmptyTexture_Green[i + 2]	= 0x00;
		EmptyTexture_Green[i + 3]	= 0xFF;
	}
/*
	if(Renderer_UsePlaceholderTexture) {
		FILE * PlaceholderTexture;
		char Temp[1024];
		sprintf(Temp, "%s\\texture.raw", AppPath);
		PlaceholderTexture = fopen(Temp, "rb");
		fread(EmptyTexture, TextureBufferSize, 1, PlaceholderTexture);
		fclose(PlaceholderTexture);
	}
*/
	if(Zelda_MemCopy(Texture[TextureID].DataSource, Texture[TextureID].Offset, TextureData_N64, TextureBufferSize / 4) == -1) {
		UnhandledTextureSource = true;
		if(Texture[TextureID].DataSource <= 0x03) {
			sprintf(ErrorMsg, "Unhandled texture source 0x%02X|%06X!\n(Format 0x%02X)", Texture[TextureID].DataSource, (unsigned int)Texture[TextureID].Offset, Texture[TextureID].Format_N64);
//			MessageBox(hwnd, ErrorMsg, "Error", MB_OK | MB_ICONERROR);
		}
		Texture[TextureID].Format_OGL = GL_RGBA;
		Texture[TextureID].Format_OGLPixel = GL_RGBA;
		memcpy(TextureData_OGL, EmptyTexture, TextureBufferSize);
	}

	/* generate simple checksum for cache structure */
	Texture[TextureID].Checksum = 0;
	for(i = 0; i < TextureBufferSize; i++) {
		Texture[TextureID].Checksum += TextureData_N64[i];
	}

	/* texture cache management */
	bool SearchingCache = true, NewTexture = false;
	unsigned int OtherCacheCriteria = 0;

	i = 0;

	if(GetDLFromZMapScene) {
		OtherCacheCriteria = ROM_CurrentMap;
	} else {
		OtherCacheCriteria = ObjectID;
		i=1024;					/* CACHING BROKEN FOR ACTORS - SKIPPING CACHE VIA I=1024 */
	}

	while(SearchingCache) {
		if((CurrentTextures[i].Offset == Texture[TextureID].Offset) &&
			(CurrentTextures[i].DataSource == Texture[TextureID].DataSource) &&
			(CurrentTextures[i].OtherCriteria == OtherCacheCriteria) &&
			(CurrentTextures[i].Checksum == Texture[TextureID].Checksum)) {
				SearchingCache = false;
				TempGLTexture = CurrentTextures[i].GLTextureID;
				//fprintf(FileSystemLog, "- texture in cache %d: %02X%06X, %04X\n", i, CurrentTextures[i].DataSource, CurrentTextures[i].Offset, CurrentTextures[i].Checksum);
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
				CurrentTextures[TexCachePosition].Checksum = Texture[TextureID].Checksum;
				TexCachePosition++;

				NewTexture = true;

				//fprintf(FileSystemLog, "- new texture %d: %02X%06X, %04X\n", TexCachePosition-1, CurrentTextures[TexCachePosition-1].DataSource, CurrentTextures[TexCachePosition-1].Offset, CurrentTextures[TexCachePosition-1].Checksum);
			}
		}
	}

	if((!UnhandledTextureSource) && (NewTexture)) {
		/* texture debug information */
/*		char Log_PalOffset[256];
		sprintf(Log_PalOffset, "- CI palette offset: 0x%02X%06X\n", Texture[TextureID].PalDataSource, (unsigned int)Texture[TextureID].PalOffset);
		char Log_PalNo[256];
		sprintf(Log_PalNo, "- Palette #%d\n", Texture[0].Palette);
		char Log_YParam[256];
		sprintf(Log_YParam, "- Y parameters: %s%s%s\n",
			((Texture[TextureID].Y_Parameter & G_TX_WRAP) ? "G_TX_WRAP | " : ""),
			((Texture[TextureID].Y_Parameter & G_TX_MIRROR) ? "G_TX_MIRROR | " : ""),
			((Texture[TextureID].Y_Parameter & G_TX_CLAMP) ? "G_TX_CLAMP | " : ""));
		char Log_XParam[256];
		sprintf(Log_XParam, "- X parameters: %s%s%s\n",
			((Texture[TextureID].X_Parameter & G_TX_WRAP) ? "G_TX_WRAP" : ""),
			((Texture[TextureID].X_Parameter & G_TX_MIRROR) ? "G_TX_MIRROR" : ""),
			((Texture[TextureID].X_Parameter & G_TX_CLAMP) ? "G_TX_CLAMP" : ""));

		bool IsCITex = false;
		if(Texture[TextureID].PalDataSource != 0) IsCITex = true;

		fprintf(FileSystemLog,
			"Texture unit %d:\n"
			"---------------\n"
			"- Offset: 0x%02X%06X\n"
			"%s"
			"- Size: %d * %d\n"
			"- Format: %s (0x%02X)\n"
			"%s"
			"%s"
			"- S scale %4.2f, T scale %4.2f\n"
			"- S shift scale %4.2f, T shift scale %4.2f\n"
			"- S mask %d, T mask %d\n"
			"- 'LineSize' %d\n"
			"%s"
			"- 'AnimDXT' %d\n"
			"- Checksum: 0x%04X\n\n",
				TextureID,
				Texture[TextureID].DataSource, (unsigned int)Texture[TextureID].Offset,
				(IsCITex ? Log_PalOffset : ""),
				Texture[TextureID].Width, Texture[TextureID].Height,
				"", Texture[TextureID].Format_N64,
				Log_YParam,
				Log_XParam,
				Texture[TextureID].S_Scale, Texture[TextureID].T_Scale,
				Texture[TextureID].S_ShiftScale, Texture[TextureID].T_ShiftScale,
				Texture[TextureID].S_Mask, Texture[TextureID].T_Mask,
				Texture[TextureID].LineSize,
				(IsCITex ? Log_PalNo : ""),
				Texture[TextureID].AnimDXT,
				Texture[TextureID].Checksum
		);
*/
		/* convert texture */
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
			/* 32-bit RGBA */
			memcpy(TextureData_OGL, TextureData_N64, (Texture[TextureID].Height * Texture[TextureID].Width * 4));
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
					/* okay-looking guesstimate */
					LoadIA_IExtract = (LoadIA_IAData & 0xF0) + 0x0F;
					LoadIA_AExtract = (LoadIA_IAData & 0x0F) << 4;

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
					TextureData_OGL[LoadI_InTexturePosition_OGL + 3] = 0xFF;//(LoadI_IExtract1 < 0x80 ? 0x00 : 0xFF);

					TextureData_OGL[LoadI_InTexturePosition_OGL + 4] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 5] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 6] = LoadI_IExtract2;
					TextureData_OGL[LoadI_InTexturePosition_OGL + 7] = 0xFF;//(LoadI_IExtract2 < 0x80 ? 0x00 : 0xFF);

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
					TextureData_OGL[LoadI_InTexturePosition_OGL + 3] = 0xFF;

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

	glBindTexture(GL_TEXTURE_2D, TempGLTexture);

	if(NewTexture) {
		if(Texture[TextureID].Y_Parameter & G_TX_CLAMP) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); }
		if(Texture[TextureID].Y_Parameter & G_TX_MIRROR) { if(GLExtension_TextureMirror) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB); }
		if(Texture[TextureID].Y_Parameter & G_TX_WRAP) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); }

		if(Texture[TextureID].X_Parameter & G_TX_CLAMP) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); }
		if(Texture[TextureID].X_Parameter & G_TX_MIRROR) { if(GLExtension_TextureMirror) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB); }
		if(Texture[TextureID].X_Parameter & G_TX_WRAP) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); }

		gluBuild2DMipmaps(GL_TEXTURE_2D, Texture[TextureID].Format_OGL, Texture[TextureID].RealWidth, Texture[TextureID].RealHeight, Texture[TextureID].Format_OGLPixel, GL_UNSIGNED_BYTE, TextureData_OGL);
	} else {
		//
	}

	if(Renderer_EnableWavefrontDump) {
		/* texture dumping */
		static char TextureFilename[256] = "";
		static char TextureFullPath[1024] = "";
		sprintf(TextureFilename, "0x%02X_0x%02X_0x%02X%06X.bmp", (unsigned int)ROM_SceneToLoad, Texture[TextureID].Format_N64, Texture[TextureID].DataSource, (unsigned int)Texture[TextureID].Offset);
		sprintf(TextureFullPath, "%s\\dump\\%s", AppPath, TextureFilename);
		long BMPSize = 0;
		unsigned char * BMPBuf = ConvertToBMP(TextureData_OGL, Texture[TextureID].RealWidth, Texture[TextureID].RealHeight, &BMPSize);
		SaveBMP(BMPBuf, Texture[TextureID].RealWidth, Texture[TextureID].RealHeight, BMPSize, TextureFullPath);

		sprintf(WavefrontMtlMsg, "newmtl material%d\n", WavefrontObjMaterialCnt);
		fprintf(FileWavefrontMtl, WavefrontMtlMsg);
		sprintf(WavefrontMtlMsg, "Kd 0.0000 0.0000 0.0000\n");			// diffuse color
		fprintf(FileWavefrontMtl, WavefrontMtlMsg);
		sprintf(WavefrontMtlMsg, "illum 1\n");
		fprintf(FileWavefrontMtl, WavefrontMtlMsg);
		sprintf(WavefrontMtlMsg, "map_Kd %s\n", TextureFilename);
		fprintf(FileWavefrontMtl, WavefrontMtlMsg);

		WavefrontObjMaterialCnt++;
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

	free(EmptyTexture);
	free(EmptyTexture_Green);

	Debug_FreeOperations++;
	Debug_FreeOperations++;

	return TempGLTexture;
}

int F3DEX2_ResetTextureStruct()
{
	/* reset texture struct to prevent texture loader from going out of whack when there aren't any on a map (ex. sasatest) */
/*	int i = 0;
	for(i = 0; i < 2; i++) {
		Texture[i].Height = 0x00, Texture[i].Width = 0x00;
		Texture[i].DataSource = 0x00, Texture[i].PalDataSource = 0x00, Texture[i].Offset = 0x00, Texture[i].PalOffset = 0x00;
		Texture[i].Format_N64 = 0x00, Texture[i].Format_OGL = 0x00, Texture[i].Format_OGLPixel = 0x00;
		Texture[i].Y_Parameter = 0x00, Texture[i].X_Parameter = 0x00, Texture[i].S_Scale = 0x00, Texture[i].T_Scale = 0x00;
		Texture[i].LineSize = 0x00, Texture[i].Palette = 0x00, Texture[i].AnimDXT = 0x00;
		Texture[i].S_ShiftScale = 0x00; Texture[i].T_ShiftScale = 0x00;
	}
*/
	memset(Texture, 0x00, sizeof(Texture));

	return 0;
}
