#include "globals.h"

// ----------------------------------------

RDPInstruction RDP_UcodeCmd[256];

unsigned int DListAddress;
unsigned char Segment; unsigned int Offset;

unsigned int wp0, wp1;
unsigned int w0, w1;
unsigned int wn0, wn1;

unsigned int G_TEXTURE_ENABLE;
unsigned int G_SHADING_SMOOTH;
unsigned int G_CULL_FRONT;
unsigned int G_CULL_BACK;
unsigned int G_CULL_BOTH;
unsigned int G_CLIPPING;

unsigned int G_MTX_STACKSIZE;
unsigned int G_MTX_MODELVIEW;
unsigned int G_MTX_PROJECTION;
unsigned int G_MTX_MUL;
unsigned int G_MTX_LOAD;
unsigned int G_MTX_NOPUSH;
unsigned int G_MTX_PUSH;

#ifdef WIN32
PFNGLMULTITEXCOORD1FARBPROC			glMultiTexCoord1fARB = NULL;
PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB = NULL;
PFNGLMULTITEXCOORD3FARBPROC			glMultiTexCoord3fARB = NULL;
PFNGLMULTITEXCOORD4FARBPROC			glMultiTexCoord4fARB = NULL;
PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC		glClientActiveTextureARB = NULL;
#endif

PFNGLGENPROGRAMSARBPROC				glGenProgramsARB = NULL;
PFNGLBINDPROGRAMARBPROC				glBindProgramARB = NULL;
PFNGLDELETEPROGRAMSARBPROC			glDeleteProgramsARB = NULL;
PFNGLPROGRAMSTRINGARBPROC			glProgramStringARB = NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC	glProgramEnvParameter4fARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC	glProgramLocalParameter4fARB = NULL;

__System System;
__Matrix Matrix;
__Gfx Gfx;
__Palette Palette[256];
__Vertex Vertex[32];
__Texture Texture[2];
__FragmentCache FragmentCache[CACHE_FRAGMENT];
__TextureCache TextureCache[CACHE_TEXTURES];
__OpenGL OpenGL;

// ----------------------------------------

void RDP_InitParser(int UcodeID)
{
	int i = 0;
	for(i = 0; i < 256; i++) {
		RDP_UcodeCmd[i] = RDP_UnemulatedCmd;
	}

	RDP_UcodeCmd[G_SETCIMG]         = RDP_G_SETCIMG;
	RDP_UcodeCmd[G_SETZIMG]         = RDP_G_SETZIMG;
	RDP_UcodeCmd[G_SETTIMG]         = RDP_G_SETTIMG;
	RDP_UcodeCmd[G_SETCOMBINE]      = RDP_G_SETCOMBINE;
	RDP_UcodeCmd[G_SETENVCOLOR]     = RDP_G_SETENVCOLOR;
	RDP_UcodeCmd[G_SETPRIMCOLOR]    = RDP_G_SETPRIMCOLOR;
	RDP_UcodeCmd[G_SETBLENDCOLOR]   = RDP_G_SETBLENDCOLOR;
	RDP_UcodeCmd[G_SETFOGCOLOR]     = RDP_G_SETFOGCOLOR;
	RDP_UcodeCmd[G_SETFILLCOLOR]    = RDP_G_SETFILLCOLOR;
	RDP_UcodeCmd[G_FILLRECT]        = RDP_G_FILLRECT;
	RDP_UcodeCmd[G_SETTILE]         = RDP_G_SETTILE;
	RDP_UcodeCmd[G_LOADTILE]        = RDP_G_LOADTILE;
	RDP_UcodeCmd[G_LOADBLOCK]       = RDP_G_LOADBLOCK;
	RDP_UcodeCmd[G_SETTILESIZE]     = RDP_G_SETTILESIZE;
	RDP_UcodeCmd[G_LOADTLUT]        = RDP_G_LOADTLUT;
	RDP_UcodeCmd[G_RDPSETOTHERMODE] = RDP_G_RDPSETOTHERMODE;
	RDP_UcodeCmd[G_SETPRIMDEPTH]    = RDP_G_SETPRIMDEPTH;
	RDP_UcodeCmd[G_SETSCISSOR]      = RDP_G_SETSCISSOR;
	RDP_UcodeCmd[G_SETCONVERT]      = RDP_G_SETCONVERT;
	RDP_UcodeCmd[G_SETKEYR]         = RDP_G_SETKEYR;
	RDP_UcodeCmd[G_SETKEYGB]        = RDP_G_SETKEYGB;
	RDP_UcodeCmd[G_RDPFULLSYNC]     = RDP_G_RDPFULLSYNC;
	RDP_UcodeCmd[G_RDPTILESYNC]     = RDP_G_RDPTILESYNC;
	RDP_UcodeCmd[G_RDPPIPESYNC]     = RDP_G_RDPPIPESYNC;
	RDP_UcodeCmd[G_RDPLOADSYNC]     = RDP_G_RDPLOADSYNC;
	RDP_UcodeCmd[G_TEXRECTFLIP]     = RDP_G_TEXRECTFLIP;
	RDP_UcodeCmd[G_TEXRECT]         = RDP_G_TEXRECT;

	switch(UcodeID) {
		case F3D:		RDP_F3D_Init(); break;
		case F3DEX:		RDP_F3DEX_Init(); break;
		case F3DEX2:	RDP_F3DEX2_Init(); break;
		default:		break;
	}

	Matrix.ModelStackSize = G_MTX_STACKSIZE;

	Gfx.Update |= CHANGED_MULT_MAT;

	crc_GenerateTable();

	Matrix.UseMatrixHack = false;
}

void RDP_LoadToSegment(unsigned char Segment, unsigned char * Buffer, unsigned int Offset, unsigned int Size)
{
	if(Segment >= MAX_SEGMENTS) return;

	RAM[Segment].SourceOffset = Offset;
	RAM[Segment].SourceCompType = 0;

	RAM[Segment].Data = (unsigned char*) malloc (sizeof(char) * Size);

	unsigned int ID = Read32(Buffer, Offset);
	if(ID == 0x59617A30) {
		RAM[Segment].SourceCompType = 1;
		RDP_Yaz0Decode(&Buffer[Offset], RAM[Segment].Data, Size);
	} else if(ID == 0x4D494F30) {
		RAM[Segment].SourceCompType = 2;
		RDP_MIO0Decode(&Buffer[Offset], RAM[Segment].Data, Size);
	} else {
		memcpy(RAM[Segment].Data, &Buffer[Offset], Size);
	}

	RAM[Segment].IsSet = true;
	RAM[Segment].Size = Size;
}

void RDP_LoadToRDRAM(unsigned char * Buffer, unsigned int Size)
{
	if(Size >= 0x800000) Size = 0x800000;
	RDRAM.Data = (unsigned char*) malloc (sizeof(char) * Size);
	memcpy(RDRAM.Data, &Buffer[0], Size);
	RDRAM.IsSet = true;
	RDRAM.Size = Size;
}

bool RDP_SaveSegment(unsigned char Segment, unsigned char * Buffer)
{
	// check if the segment is invalid
	if((Segment >= MAX_SEGMENTS) || (RAM[Segment].IsSet == false)) return false;

	// check if the segment's source was compressed
	if(RAM[Segment].SourceCompType > 0) return false;

	// store the segment's data back in the source buffer
	memcpy(&Buffer[RAM[Segment].SourceOffset], RAM[Segment].Data, RAM[Segment].Size);

	return true;
}

// Yaz0 decompression code from yaz0dec by thakis - http://www.amnoid.de/gc/
void RDP_Yaz0Decode(unsigned char * Input, unsigned char * Output, unsigned int DecSize)
{
	unsigned int Size = Read32(Input, 4);
	if(DecSize != Size) return;

	int SrcPlace = 0x10, DstPlace = 0;	//current read/write positions

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

	return;
}

// MIO0 decompression code by HyperHacker
void RDP_MIO0Decode(unsigned char * Input, unsigned char * Output, unsigned int DecSize)
{
	unsigned char MapByte = 0x80, CurMapByte, Length;
	unsigned short SData, Dist;
	unsigned int NumBytesOutput = 0;
	unsigned int MapLoc = 0;
	unsigned int CompLoc = Read32(Input, 8);	// current compressed data position
	unsigned int RawLoc = Read32(Input, 12);	// current raw data position
	unsigned int OutLoc = 0;	// current output position

	int i;

	unsigned int OutputSize = Read32(Input, 4);
	if(DecSize != OutputSize) return;

	MapLoc = 0x10;
	CurMapByte = Input[MapLoc];

	while(NumBytesOutput < OutputSize) {
		// raw
		if(CurMapByte & MapByte) {
			Output[OutLoc] = Input[RawLoc]; // copy a byte to output.
			OutLoc++;
			RawLoc++;
			NumBytesOutput++;
		}

		// compressed
		else {
			SData = Read16(Input, CompLoc); // get compressed data
			Length = (SData >> 12) + 3;
			Dist = (SData & 0xFFF) + 1;

			// sanity check: can't copy from before first byte
			if(((int)OutLoc - Dist) < 0) {
				free(Output);
				return;
			}

			// copy from output
			for(i = 0; i < Length; i++) {
				Output[OutLoc] = Output[OutLoc - Dist];
				OutLoc++;
				NumBytesOutput++;
				if(NumBytesOutput >= OutputSize) break;
			}
			CompLoc += 2;
		}

		MapByte >>= 1; // next map bit

		// if we did them all, get the next map byte
		if(!MapByte) {
			MapByte = 0x80;
			MapLoc++;
			CurMapByte = Input[MapLoc];

			// sanity check: map pointer should never reach this
			int Check = CompLoc;
			if(RawLoc < CompLoc) Check = RawLoc;

			if(MapLoc > Check) {
				free(Output);
				return;
			}
		}
	}

	return;
}

bool RDP_CheckAddressValidity(unsigned int Address)
{
	unsigned char Segment = Address >> 24;
	unsigned int Offset = (Address & 0x00FFFFFF);

	if((Segment >= MAX_SEGMENTS) && (Segment != 0x80)) return false;

	if(Segment != 0x80) {
		if((RAM[Segment].IsSet == false) || (RAM[Segment].Size < Offset)) return false;
	} else {
		if((RDRAM.IsSet == false) || (RDRAM.Size < Offset)) return false;
	}

	return true;
}

void RDP_ClearSegment(unsigned char Segment)
{
	if(RAM[Segment].IsSet == true) {
		free(RAM[Segment].Data);
		RAM[Segment].IsSet = false;
		RAM[Segment].Size = 0;
	}
}

void RDP_ClearRDRAM()
{
	if(RDRAM.IsSet == true) {
		free(RDRAM.Data);
		RDRAM.IsSet = false;
		RDRAM.Size = 0;
	}
}

void RDP_ClearStructures(bool Full)
{
	int i = 0;

	static const __Vertex Vertex_Empty = { { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0.0f, 0.0f, 0.0f, 0.0f };
	for(i = 0; i < ArraySize(Vertex); i++) Vertex[i] = Vertex_Empty;

	static const __Palette Palette_Empty = { 0, 0, 0, 0 };
	for(i = 0; i < ArraySize(Palette); i++) Palette[i] = Palette_Empty;

	static const __Texture Texture_Empty = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {{0, 0, 0, 0, 0, 0}}, 0.0f, 0.0f, 0.0f, 0.0f };
	Texture[0] = Texture_Empty;
	Texture[1] = Texture_Empty;

	Gfx.BlendColor =	(__RGBA){ 0.0f, 0.0f, 0.0f, 0.0f };
	Gfx.FogColor =		(__RGBA){ 0.0f, 0.0f, 0.0f, 0.0f };
	Gfx.EnvColor =		(__RGBA){ 0.5f, 0.5f, 0.5f, 0.5f };
	Gfx.FillColor =		(__FillColor){ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	Gfx.PrimColor =		(__PrimColor){ 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, 0 };

	if(OpenGL.Ext_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, Gfx.EnvColor.R, Gfx.EnvColor.G, Gfx.EnvColor.B, Gfx.EnvColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, Gfx.PrimColor.R, Gfx.PrimColor.G, Gfx.PrimColor.B, Gfx.PrimColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L);
	}

	Gfx.DLStackPos = 0;

	Gfx.Update = 0xFFFFFFFF;
	Gfx.GeometryMode = 0;
	Gfx.OtherMode.L = 0;
	Gfx.OtherMode.H = 0;
	Gfx.Store_RDPHalf1 = 0; Gfx.Store_RDPHalf2 = 0;
	Gfx.Combiner0 = 0; Gfx.Combiner1 = 0;

	Gfx.CurrentTexture = 0;
	Gfx.IsMultiTexture = false;

	Texture[0].ScaleS = 1.0f;
	Texture[0].ScaleT = 1.0f;
	Texture[1].ScaleS = 1.0f;
	Texture[1].ScaleT = 1.0f;

	Texture[0].ShiftScaleS = 1.0f;
	Texture[0].ShiftScaleT = 1.0f;
	Texture[1].ShiftScaleS = 1.0f;
	Texture[1].ShiftScaleT = 1.0f;

	if(Full) {
		static const __FragmentCache FragmentCache_Empty = { 0, 0, -1 };
		for(i = 0; i < ArraySize(FragmentCache); i++) {
			if(OpenGL.Ext_FragmentProgram) glDeleteProgramsARB(1, &FragmentCache[i].ProgramID);
			FragmentCache[i] = FragmentCache_Empty;
		}
		System.FragCachePosition = 0;

		static const __TextureCache TextureCache_Empty = { 0, 0, 0, -1 };
		for(i = 0; i < ArraySize(TextureCache); i++) TextureCache[i] = TextureCache_Empty;
		System.TextureCachePosition = 0;
	}
}

void RDP_ParseDisplayList(unsigned int Address, bool ResetStack)
{
	if(!RDP_CheckAddressValidity(Address)) return;

	DListAddress = Address;

	RDP_SetRenderMode(0, 0);
//	Gfx.OtherMode.cycleType = G_CYC_2CYCLE;

	if(ResetStack) Gfx.DLStackPos = 0;

	glPolygonMode(GL_FRONT_AND_BACK, (System.Options & BRDP_WIREFRAME) ? GL_LINE : GL_FILL);

	glDisable(GL_TEXTURE_2D);
	if(OpenGL.Ext_MultiTexture) {
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
	}

	if(OpenGL.Ext_FragmentProgram) glDisable(GL_FRAGMENT_PROGRAM_ARB);

	while(Gfx.DLStackPos >= 0) {
		unsigned int OldAddr = DListAddress;
		DListAddress = RDP_Macro_DetectMacro(DListAddress);
		if(OldAddr == DListAddress) {
			Segment = DListAddress >> 24;
			Offset = (DListAddress & 0x00FFFFFF);

			if(Segment != 0x80) {
				w0 = Read32(RAM[Segment].Data, Offset);
				w1 = Read32(RAM[Segment].Data, Offset + 4);

				wp0 = Read32(RAM[Segment].Data, Offset - 8);
				wp1 = Read32(RAM[Segment].Data, Offset - 4);

				wn0 = Read32(RAM[Segment].Data, Offset + 8);
				wn1 = Read32(RAM[Segment].Data, Offset + 12);
			} else {
				w0 = Read32(RDRAM.Data, Offset);
				w1 = Read32(RDRAM.Data, Offset + 4);

				wp0 = Read32(RDRAM.Data, Offset - 8);
				wp1 = Read32(RDRAM.Data, Offset - 4);

				wn0 = Read32(RDRAM.Data, Offset + 8);
				wn1 = Read32(RDRAM.Data, Offset + 12);
			}

			DListAddress += 8;

			RDP_UcodeCmd[w0 >> 24] ();
		}
	}
}

void RDP_DrawTriangle(int Vtxs[])
{
	if(Gfx.Update) RDP_UpdateGLStates();

	glBegin(GL_TRIANGLES);

	int i = 0;
	for(i = 0; i < 3; i++) {
		Vertex[Vtxs[i]].RealS0 = _FIXED2FLOAT(Vertex[Vtxs[i]].Vtx.S, 16) * (Texture[0].ScaleS * Texture[0].ShiftScaleS) / 32.0f / _FIXED2FLOAT(Texture[0].RealWidth, 16);
		Vertex[Vtxs[i]].RealT0 = _FIXED2FLOAT(Vertex[Vtxs[i]].Vtx.T, 16) * (Texture[0].ScaleT * Texture[0].ShiftScaleT) / 32.0f / _FIXED2FLOAT(Texture[0].RealHeight, 16);
		Vertex[Vtxs[i]].RealS1 = _FIXED2FLOAT(Vertex[Vtxs[i]].Vtx.S, 16) * (Texture[1].ScaleS * Texture[1].ShiftScaleS) / 32.0f / _FIXED2FLOAT(Texture[1].RealWidth, 16);
		Vertex[Vtxs[i]].RealT1 = _FIXED2FLOAT(Vertex[Vtxs[i]].Vtx.T, 16) * (Texture[1].ScaleT * Texture[1].ShiftScaleT) / 32.0f / _FIXED2FLOAT(Texture[1].RealHeight, 16);

		if(isnan(Vertex[Vtxs[i]].RealS0)) Vertex[Vtxs[i]].RealS0 = 0.0f;
		if(isnan(Vertex[Vtxs[i]].RealT0)) Vertex[Vtxs[i]].RealT0 = 0.0f;
		if(isnan(Vertex[Vtxs[i]].RealS1)) Vertex[Vtxs[i]].RealS1 = 0.0f;
		if(isnan(Vertex[Vtxs[i]].RealT1)) Vertex[Vtxs[i]].RealT1 = 0.0f;

		if(!(Gfx.GeometryMode & G_TEXTURE_GEN_LINEAR)) {
			if(OpenGL.Ext_MultiTexture) {
				glMultiTexCoord2fARB(GL_TEXTURE0_ARB, Vertex[Vtxs[i]].RealS0, Vertex[Vtxs[i]].RealT0);
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB, Vertex[Vtxs[i]].RealS1, Vertex[Vtxs[i]].RealT1);
			} else {
				glTexCoord2f(Vertex[Vtxs[i]].RealS0, Vertex[Vtxs[i]].RealT0);
			}
		}

		glNormal3b(Vertex[Vtxs[i]].Vtx.R, Vertex[Vtxs[i]].Vtx.G, Vertex[Vtxs[i]].Vtx.B);
//		glColor4f(1.0f,1.0f,1.0f,1.0f);
		if(!(Gfx.GeometryMode & G_LIGHTING)) glColor4ub(Vertex[Vtxs[i]].Vtx.R, Vertex[Vtxs[i]].Vtx.G, Vertex[Vtxs[i]].Vtx.B, Vertex[Vtxs[i]].Vtx.A);

		glVertex3d(Vertex[Vtxs[i]].Vtx.X, Vertex[Vtxs[i]].Vtx.Y, Vertex[Vtxs[i]].Vtx.Z);
	}

	RDP_Dump_DumpTriangle(Vertex, Vtxs);

	glEnd();
}

void RDP_SetRenderMode(unsigned int Mode1, unsigned int Mode2)
{
	Gfx.OtherMode.L &= 0x00000007;
	Gfx.OtherMode.L |= Mode1 | Mode2;

	Gfx.Update |= CHANGED_RENDERMODE;
}

void RDP_SetCycleType(unsigned int Type)
{
	Gfx.OtherMode.cycleType = Type;
}

void RDP_SetPrimColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
{
	unsigned int Color = (R << 24) | (G << 16) | (B << 8) | A;
	gDP_SetPrimColor(0, Color);
}

void RDP_SetEnvColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A)
{
	unsigned int Color = (R << 24) | (G << 16) | (B << 8) | A;
	gDP_SetEnvColor(0, Color);
}

void RDP_ChangeTileSize(unsigned int Tile, unsigned int ULS, unsigned int ULT, unsigned int LRS, unsigned int LRT)
{
	Texture[Gfx.CurrentTexture].Tile = Tile;
	Texture[Gfx.CurrentTexture].ULS = _SHIFTR(ULS, 2, 10);
	Texture[Gfx.CurrentTexture].ULT = _SHIFTR(ULT, 2, 10);
	Texture[Gfx.CurrentTexture].LRS = _SHIFTR(LRS, 2, 10);
	Texture[Gfx.CurrentTexture].LRT = _SHIFTR(LRT, 2, 10);
}

void RDP_CalcTextureSize(int TextureID)
{
	unsigned int MaxTexel = 0, Line_Shift = 0;

	switch(Texture[TextureID].Format) {
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

	unsigned int Tile_Width = (Texture[TextureID].LRS - Texture[TextureID].ULS) + 1;
	unsigned int Tile_Height = (Texture[TextureID].LRT - Texture[TextureID].ULT) + 1;

	unsigned int Mask_Width = 1 << Texture[TextureID].MaskS;
	unsigned int Mask_Height = 1 << Texture[TextureID].MaskT;

	unsigned int Line_Height = 0;
	if(Line_Width > 0) Line_Height = min(MaxTexel / Line_Width, Tile_Height);

	if(Texture[TextureID].IsTexRect) {
		unsigned short TexRect_Width = Texture[TextureID].TexRectW - Texture[TextureID].ULS;
		unsigned short TexRect_Height = Texture[TextureID].TexRectH - Texture[TextureID].ULT;

		if((Texture[TextureID].MaskS > 0) && ((Mask_Width * Mask_Height) <= MaxTexel)) {
			Texture[TextureID].Width = Mask_Width;

		} else if((Tile_Width * Tile_Height) <= MaxTexel) {
			Texture[TextureID].Width = Tile_Width;

		} else if((Tile_Width * TexRect_Height) <= MaxTexel) {
			Texture[TextureID].Width = Tile_Width;

		} else if((TexRect_Width * Tile_Height) <= MaxTexel) {
			Texture[TextureID].Width = Texture[TextureID].TexRectW;

		} else if((TexRect_Width * TexRect_Height) <= MaxTexel) {
			Texture[TextureID].Width = Texture[TextureID].TexRectW;

		} else {
			Texture[TextureID].Width = Line_Width;
		}

		if((Texture[TextureID].MaskT > 0) && ((Mask_Width * Mask_Height) <= MaxTexel)) {
			Texture[TextureID].Height = Mask_Height;

		} else if((Tile_Width * Tile_Height) <= MaxTexel) {
			Texture[TextureID].Height = Tile_Height;

		} else if((Tile_Width * TexRect_Height) <= MaxTexel) {
			Texture[TextureID].Height = Tile_Height;

		} else if((TexRect_Width * Tile_Height) <= MaxTexel) {
			Texture[TextureID].Height = Texture[TextureID].TexRectH;

		} else if((TexRect_Width * TexRect_Height) <= MaxTexel) {
			Texture[TextureID].Height = Texture[TextureID].TexRectH;

		} else {
			Texture[TextureID].Height = Line_Height;
		}
	} else {
		if((Texture[TextureID].MaskS > 0) && ((Mask_Width * Mask_Height) <= MaxTexel)) {
			Texture[TextureID].Width = Mask_Width;
		} else if((Tile_Width * Tile_Height) <= MaxTexel) {
			Texture[TextureID].Width = Tile_Width;
		} else {
			Texture[TextureID].Width = Line_Width;
		}

		if((Texture[TextureID].MaskT > 0) && ((Mask_Width * Mask_Height) <= MaxTexel)) {
			Texture[TextureID].Height = Mask_Height;
		} else if((Tile_Width * Tile_Height) <= MaxTexel) {
			Texture[TextureID].Height = Tile_Height;
		} else {
			Texture[TextureID].Height = Line_Height;
		}
	}

	unsigned int Clamp_Width = Texture[TextureID].clamps ? Tile_Width : Texture[TextureID].Width;
	unsigned int Clamp_Height = Texture[TextureID].clampt ? Tile_Height : Texture[TextureID].Height;

	if(Clamp_Width > 256) Texture[TextureID].clamps = 0;
	if(Clamp_Height > 256) Texture[TextureID].clampt = 0;

	if(Mask_Width > Texture[TextureID].Width) {
		Texture[TextureID].MaskS = RDP_PowOf(Texture[TextureID].Width);
		Mask_Width = 1 << Texture[TextureID].MaskS;
	}
	if(Mask_Height > Texture[TextureID].Height) {
		Texture[TextureID].MaskT = RDP_PowOf(Texture[TextureID].Height);
		Mask_Height = 1 << Texture[TextureID].MaskT;
	}

	if(Texture[TextureID].clamps) {
		Texture[TextureID].RealWidth = RDP_Pow2(Clamp_Width);
	} else if(Texture[TextureID].mirrors) {
		Texture[TextureID].RealWidth = RDP_Pow2(Mask_Width);
	} else {
		Texture[TextureID].RealWidth = RDP_Pow2(Texture[TextureID].Width);
	}

	if(Texture[TextureID].clampt) {
		Texture[TextureID].RealHeight = RDP_Pow2(Clamp_Height);
	} else if(Texture[TextureID].mirrort) {
		Texture[TextureID].RealHeight = RDP_Pow2(Mask_Height);
	} else {
		Texture[TextureID].RealHeight = RDP_Pow2(Texture[TextureID].Height);
	}

	Texture[TextureID].ShiftScaleS = 1.0f;
	Texture[TextureID].ShiftScaleT = 1.0f;

	if(Texture[TextureID].ShiftS > 10) {
		Texture[TextureID].ShiftScaleS = (1 << (16 - Texture[TextureID].ShiftS));
	} else if(Texture[TextureID].ShiftS > 0) {
		Texture[TextureID].ShiftScaleS /= (1 << Texture[TextureID].ShiftS);
	}

	if(Texture[TextureID].ShiftT > 10) {
		Texture[TextureID].ShiftScaleT = (1 << (16 - Texture[TextureID].ShiftT));
	} else if(Texture[TextureID].ShiftT > 0) {
		Texture[TextureID].ShiftScaleT /= (1 << Texture[TextureID].ShiftT);
	}
}

inline unsigned long RDP_Pow2(unsigned long dim)
{
	unsigned long i = 1;

	while (i < dim) i <<= 1;

	return i;
}

inline unsigned long RDP_PowOf(unsigned long dim)
{
	unsigned long num = 1;
	unsigned long i = 0;

	while (num < dim) {
		num <<= 1;
		i++;
	}

	return i;
}

void RDP_InitLoadTexture()
{
/*	if(OpenGL.Ext_FragmentProgram && ((System.Options & BRDP_COMBINER) == 0)) {
		glDisable(GL_FRAGMENT_PROGRAM_ARB);
	}
*/
	if(OpenGL.Ext_MultiTexture) {
		if(Texture[0].Offset != 0x00) {
			RDP_CalcTextureSize(0);
			glEnable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, RDP_CheckTextureCache(0));
		}

		if(Gfx.IsMultiTexture && (Texture[1].Offset != 0x00)) {
			RDP_CalcTextureSize(1);
			glEnable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_2D, RDP_CheckTextureCache(1));
		}

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
	} else {
		if(Texture[0].Offset != 0x00) {
			RDP_CalcTextureSize(0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, RDP_CheckTextureCache(0));
		}
	}
}

GLuint RDP_CheckTextureCache(unsigned int TexID)
{
	GLuint GLID = 0;

	int CacheCheck = 0; bool SearchingCache = true; bool NewTexture = false;
	while(SearchingCache) {
		if((TextureCache[CacheCheck].Offset == Texture[TexID].Offset) &&
			(TextureCache[CacheCheck].RealWidth == Texture[TexID].RealWidth) &&
			(TextureCache[CacheCheck].RealHeight == Texture[TexID].RealHeight) &&
			(TextureCache[CacheCheck].CRC32 == Texture[TexID].CRC32)) {
			SearchingCache = false;
			NewTexture = false;
		} else {
			if(CacheCheck != CACHE_TEXTURES) {
				CacheCheck++;
			} else {
				SearchingCache = false;
				NewTexture = true;
			}
		}
	}

	if(NewTexture) {
		GLID = RDP_LoadTexture(TexID);
		RDP_Dump_SelectMaterial(TextureCache[System.TextureCachePosition].MaterialID);
		TextureCache[System.TextureCachePosition].Offset = Texture[TexID].Offset;
		TextureCache[System.TextureCachePosition].RealWidth = Texture[TexID].RealWidth;
		TextureCache[System.TextureCachePosition].RealHeight = Texture[TexID].RealHeight;
		TextureCache[System.TextureCachePosition].CRC32 = Texture[TexID].CRC32;
		TextureCache[System.TextureCachePosition].TextureID = GLID;
		System.TextureCachePosition++;
	} else {
		GLID = TextureCache[CacheCheck].TextureID;
		RDP_Dump_SelectMaterial(TextureCache[CacheCheck].MaterialID);
	}

	if(System.TextureCachePosition > CACHE_TEXTURES) {
		int i = 0;
		static const __TextureCache TextureCache_Empty;
		for(i = 0; i < CACHE_TEXTURES; i++) TextureCache[i] = TextureCache_Empty;
		System.TextureCachePosition = 0;
	}

	return GLID;
}

GLuint RDP_LoadTexture(int TextureID)
{
	unsigned char TexSegment = Texture[TextureID].Offset >> 24;
	unsigned int TexOffset = (Texture[TextureID].Offset & 0x00FFFFFF);

	int i = 0, j = 0;

	unsigned int BytesPerPixel = 0x08;
	switch(Texture[TextureID].Format) {
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

	unsigned int BufferSize = (Texture[TextureID].RealHeight * Texture[TextureID].RealWidth) * BytesPerPixel;
	unsigned char * TextureData = (unsigned char *) malloc (BufferSize);

	memset(TextureData, 0xFF, BufferSize);

	unsigned int GLTexPosition = 0;

	unsigned char * SourceBuffer = NULL;
	unsigned int SourceSize = 0;

	if(TexSegment != 0x80) {
		SourceBuffer = RAM[TexSegment].Data;
		SourceSize = RAM[TexSegment].Size;
	} else {
		SourceBuffer = RDRAM.Data;
		SourceSize = RDRAM.Size;
	}

	if(System.Options & BRDP_TEXCRC) {
		Texture[TextureID].CRC32 = crc_GenerateCRC(SourceBuffer, SourceSize);
	} else {
		Texture[TextureID].CRC32 = 0;
	}

	if(!RDP_CheckAddressValidity(Texture[TextureID].Offset)) {
		while(i < BufferSize) { TextureData[i++] = 0xFF; TextureData[i++] = 0x00; TextureData[i++] = 0x00; TextureData[i++] = 0xFF; }

	} else if((System.Options & BRDP_TEXTURES) == 0) {
		memset(TextureData, 0xFF, BufferSize);

	} else {
		switch(Texture[TextureID].Format) {
			case 0x00:
			case 0x08:
			case 0x10: {
				unsigned short Raw;
				unsigned int RGBA = 0;

				for(j = 0; j < Texture[TextureID].Height; j++) {
					for(i = 0; i < Texture[TextureID].Width; i++) {
						Raw = (SourceBuffer[TexOffset] << 8) | SourceBuffer[TexOffset + 1];

						RGBA = ((Raw & 0xF800) >> 8) << 24;
						RGBA |= (((Raw & 0x07C0) << 5) >> 8) << 16;
						RGBA |= (((Raw & 0x003E) << 18) >> 16) << 8;
						if((Raw & 0x0001)) RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition, RGBA);

						TexOffset += 2;
						GLTexPosition += 4;

						if(TexOffset > SourceSize) break;
					}
					TexOffset += Texture[TextureID].LineSize * 4 - Texture[TextureID].Width;
				}
				break; }

			case 0x18: {
				memcpy(TextureData, &SourceBuffer[TexOffset], (Texture[TextureID].Height * Texture[TextureID].Width * 4));
				break; }

			case 0x40:
			case 0x50: {
				unsigned int CI1, CI2;
				unsigned int RGBA = 0;

				for(j = 0; j < Texture[TextureID].Height; j++) {
					for(i = 0; i < Texture[TextureID].Width / 2; i++) {
						CI1 = (SourceBuffer[TexOffset] & 0xF0) >> 4;
						CI2 = (SourceBuffer[TexOffset] & 0x0F);

						RGBA = (Palette[CI1].R << 24);
						RGBA |= (Palette[CI1].G << 16);
						RGBA |= (Palette[CI1].B << 8);
						RGBA |= Palette[CI1].A;
						Write32(TextureData, GLTexPosition, RGBA);

						RGBA = (Palette[CI2].R << 24);
						RGBA |= (Palette[CI2].G << 16);
						RGBA |= (Palette[CI2].B << 8);
						RGBA |= Palette[CI2].A;
						Write32(TextureData, GLTexPosition + 4, RGBA);

						TexOffset += 1;
						GLTexPosition += 8;
					}
					TexOffset += Texture[TextureID].LineSize * 8 - (Texture[TextureID].Width / 2);
				}
				break; }

			case 0x48: {
				unsigned short Raw;
				unsigned int RGBA = 0;

				for(j = 0; j < Texture[TextureID].Height; j++) {
					for(i = 0; i < Texture[TextureID].Width; i++) {
						Raw = SourceBuffer[TexOffset];

						RGBA = (Palette[Raw].R << 24);
						RGBA |= (Palette[Raw].G << 16);
						RGBA |= (Palette[Raw].B << 8);
						RGBA |= Palette[Raw].A;
						Write32(TextureData, GLTexPosition, RGBA);

						TexOffset += 1;
						GLTexPosition += 4;
					}
					TexOffset += Texture[TextureID].LineSize * 8 - Texture[TextureID].Width;
				}
				break; }

			case 0x60: {
				unsigned short Raw;
				unsigned int RGBA = 0;

				for(j = 0; j < Texture[TextureID].Height; j++) {
					for(i = 0; i < Texture[TextureID].Width / 2; i++) {
						Raw = (SourceBuffer[TexOffset] & 0xF0) >> 4;
						RGBA = (((Raw & 0x0E) << 4) << 24);
						RGBA |= (((Raw & 0x0E) << 4) << 16);
						RGBA |= (((Raw & 0x0E) << 4) << 8);
						if((Raw & 0x01)) RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition, RGBA);

						Raw = (SourceBuffer[TexOffset] & 0x0F);
						RGBA = (((Raw & 0x0E) << 4) << 24);
						RGBA |= (((Raw & 0x0E) << 4) << 16);
						RGBA |= (((Raw & 0x0E) << 4) << 8);
						if((Raw & 0x01)) RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition + 4, RGBA);

						TexOffset += 1;
						GLTexPosition += 8;
					}
					TexOffset += Texture[TextureID].LineSize * 8 - (Texture[TextureID].Width / 2);
				}
				break; }

			case 0x68: {
				unsigned short Raw;
				unsigned int RGBA = 0;

				for(j = 0; j < Texture[TextureID].Height; j++) {
					for(i = 0; i < Texture[TextureID].Width; i++) {
						Raw = SourceBuffer[TexOffset];
						RGBA = (((Raw & 0xF0) + 0x0F) << 24);
						RGBA |= (((Raw & 0xF0) + 0x0F) << 16);
						RGBA |= (((Raw & 0xF0) + 0x0F) << 8);
						RGBA |= ((Raw & 0x0F) << 4);
						Write32(TextureData, GLTexPosition, RGBA);

						TexOffset += 1;
						GLTexPosition += 4;
					}
					TexOffset += Texture[TextureID].LineSize * 8 - Texture[TextureID].Width;
				}
				break; }

			case 0x70: {
				for(j = 0; j < Texture[TextureID].Height; j++) {
					for(i = 0; i < Texture[TextureID].Width; i++) {
						TextureData[GLTexPosition]     = SourceBuffer[TexOffset];
						TextureData[GLTexPosition + 1] = SourceBuffer[TexOffset];
						TextureData[GLTexPosition + 2] = SourceBuffer[TexOffset];
						TextureData[GLTexPosition + 3] = SourceBuffer[TexOffset + 1];

						TexOffset += 2;
						GLTexPosition += 4;
					}
					TexOffset += Texture[TextureID].LineSize * 4 - Texture[TextureID].Width;
				}
				break; }

			case 0x80:
			case 0x90: {
				unsigned short Raw;
				unsigned int RGBA = 0;

				for(j = 0; j < Texture[TextureID].Height; j++) {
					for(i = 0; i < Texture[TextureID].Width / 2; i++) {
						Raw = (SourceBuffer[TexOffset] & 0xF0) >> 4;
						RGBA = (((Raw & 0x0F) << 4) << 24);
						RGBA |= (((Raw & 0x0F) << 4) << 16);
						RGBA |= (((Raw & 0x0F) << 4) << 8);
						RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition, RGBA);

						Raw = (SourceBuffer[TexOffset] & 0x0F);
						RGBA = (((Raw & 0x0F) << 4) << 24);
						RGBA |= (((Raw & 0x0F) << 4) << 16);
						RGBA |= (((Raw & 0x0F) << 4) << 8);
						RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition + 4, RGBA);

						TexOffset += 1;
						GLTexPosition += 8;
					}
					TexOffset += Texture[TextureID].LineSize * 8 - (Texture[TextureID].Width / 2);
				}
				break; }

			case 0x88: {
				for(j = 0; j < Texture[TextureID].Height; j++) {
					for(i = 0; i < Texture[TextureID].Width; i++) {
						TextureData[GLTexPosition]     = SourceBuffer[TexOffset];
						TextureData[GLTexPosition + 1] = SourceBuffer[TexOffset];
						TextureData[GLTexPosition + 2] = SourceBuffer[TexOffset];
						TextureData[GLTexPosition + 3] = 0xFF;

						TexOffset += 1;
						GLTexPosition += 4;
					}
					TexOffset += Texture[TextureID].LineSize * 8 - Texture[TextureID].Width;
				}
				break; }

			default:
				memset(TextureData, 0xFF, BufferSize);
				break;
		}
	}

	glBindTexture(GL_TEXTURE_2D, Gfx.GLTextureID[Gfx.GLTextureCount]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Texture[TextureID].clamps ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Texture[TextureID].clampt ? GL_CLAMP_TO_EDGE : GL_REPEAT);

	if((Texture[TextureID].mirrors) && (OpenGL.Ext_TexMirroredRepeat)) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB);
	if((Texture[TextureID].mirrort) && (OpenGL.Ext_TexMirroredRepeat)) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Texture[TextureID].RealWidth, Texture[TextureID].RealHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureData);
/*
	switch(Texture[TextureID].CMS) {
		case G_TX_CLAMP:
		case 3:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			break;
		case G_TX_MIRROR:
			if(OpenGL.Ext_TexMirroredRepeat) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB);
			break;
		case G_TX_WRAP:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			break;
		default:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			break;
	}

	switch(Texture[TextureID].CMT) {
		case G_TX_CLAMP:
		case 3:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			break;
		case G_TX_MIRROR:
			if(OpenGL.Ext_TexMirroredRepeat) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB);
			break;
		case G_TX_WRAP:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			break;
		default:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			break;
	}
*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*if(TextureID == 0)*/ TextureCache[System.TextureCachePosition].MaterialID = RDP_Dump_CreateMaterial(TextureData, Texture[TextureID].Format, Texture[TextureID].Offset, Texture[TextureID].RealWidth, Texture[TextureID].RealHeight, (Texture[TextureID].CMS & G_TX_MIRROR), (Texture[TextureID].CMT & G_TX_MIRROR));

	free(TextureData);

	return Gfx.GLTextureID[Gfx.GLTextureCount++];
}

void RDP_SetRendererOptions(unsigned char Options)
{
	System.Options = Options;
}

unsigned char RDP_GetRendererOptions()
{
	return System.Options;
}

void RDP_ToggleMatrixHack()
{
	Matrix.UseMatrixHack ^= 1;
}
