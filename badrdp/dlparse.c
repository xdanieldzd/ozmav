#include "globals.h"

// ----------------------------------------

#ifndef min
#define min(a, b)				((a) < (b) ? (a) : (b))
#endif

RDPInstruction RDP_UcodeCmd[256];

unsigned int DListAddress;
unsigned char Segment; unsigned int Offset;

unsigned int w0, w1;
unsigned int wp0, wp1;
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

struct __System System;
struct __Matrix Matrix;
struct __Gfx Gfx;
struct __Palette Palette[256];
struct __Vertex Vertex[32];
struct __Texture Texture[2];
struct __FragmentCache FragmentCache[CACHE_FRAGMENT];
struct __TextureCache TextureCache[CACHE_TEXTURES];
struct __OpenGL OpenGL;

// ----------------------------------------

void RDP_SetupOpenGL()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
	glClearDepth(5.0f);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	int i = 0;
	for(i = 0; i < 4; i++) {
		Gfx.LightAmbient[i] = 1.0f;
		Gfx.LightDiffuse[i] = 1.0f;
		Gfx.LightSpecular[i] = 1.0f;
		Gfx.LightPosition[i] = 1.0f;
	}

	glLightfv(GL_LIGHT0, GL_AMBIENT, Gfx.LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Gfx.LightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, Gfx.LightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, Gfx.LightPosition);
	glEnable(GL_LIGHT0);

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	RDP_InitGLExtensions();

	if(OpenGL.Ext_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, Gfx.EnvColor.R, Gfx.EnvColor.G, Gfx.EnvColor.B, Gfx.EnvColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, Gfx.PrimColor.R, Gfx.PrimColor.G, Gfx.PrimColor.B, Gfx.PrimColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, Gfx.BlendColor.R, Gfx.BlendColor.G, Gfx.BlendColor.B, Gfx.BlendColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 3, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L);
	}
}

void RDP_InitGLExtensions()
{
	OpenGL.IsExtUnsupported = false;

	memset(OpenGL.ExtSupported, 0x00, sizeof(OpenGL.ExtSupported));

	OpenGL.ExtensionList = strdup((const char*)glGetString(GL_EXTENSIONS));
	int i;
	for(i = 0; i < strlen(OpenGL.ExtensionList); i++) {
		if(OpenGL.ExtensionList[i] == ' ') OpenGL.ExtensionList[i] = '\n';
	}

	if(strstr(OpenGL.ExtensionList, "GL_ARB_texture_mirrored_repeat")) {
		OpenGL.Ext_TexMirroredRepeat = true;
		sprintf(OpenGL.ExtSupported, "%sGL_ARB_texture_mirrored_repeat\n", OpenGL.ExtSupported);
	} else {
		OpenGL.IsExtUnsupported = true;
		OpenGL.Ext_TexMirroredRepeat = false;
		sprintf(OpenGL.ExtUnsupported, "%sGL_ARB_texture_mirrored_repeat\n", OpenGL.ExtUnsupported);
	}

	if(strstr(OpenGL.ExtensionList, "GL_ARB_multitexture")) {
		OpenGL.Ext_MultiTexture = true;
        #ifdef WIN32
		glMultiTexCoord1fARB		= (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");
		glMultiTexCoord2fARB		= (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
		glMultiTexCoord3fARB		= (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");
		glMultiTexCoord4fARB		= (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");
		glActiveTextureARB			= (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		glClientActiveTextureARB	= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
        #endif
		sprintf(OpenGL.ExtSupported, "%sGL_ARB_multitexture\n", OpenGL.ExtSupported);
	} else {
		OpenGL.IsExtUnsupported = true;
		OpenGL.Ext_MultiTexture = false;
		sprintf(OpenGL.ExtUnsupported, "%sGL_ARB_multitexture\n", OpenGL.ExtUnsupported);
	}

	if(strstr(OpenGL.ExtensionList, "GL_ARB_fragment_program")) {
		OpenGL.Ext_FragmentProgram = true;
		#ifdef WIN32
		glGenProgramsARB				= (PFNGLGENPROGRAMSARBPROC) wglGetProcAddress("glGenProgramsARB");
		glBindProgramARB				= (PFNGLBINDPROGRAMARBPROC) wglGetProcAddress("glBindProgramARB");
		glDeleteProgramsARB				= (PFNGLDELETEPROGRAMSARBPROC) wglGetProcAddress("glDeleteProgramsARB");
		glProgramStringARB				= (PFNGLPROGRAMSTRINGARBPROC) wglGetProcAddress("glProgramStringARB");
		glProgramEnvParameter4fARB		= (PFNGLPROGRAMENVPARAMETER4FARBPROC) wglGetProcAddress("glProgramEnvParameter4fARB");
		glProgramLocalParameter4fARB	= (PFNGLPROGRAMLOCALPARAMETER4FARBPROC) wglGetProcAddress("glProgramLocalParameter4fARB");
        #else
		glGenProgramsARB				= (PFNGLGENPROGRAMSARBPROC) glXGetProcAddressARB((const GLubyte*)"glGenProgramsARB");
		glBindProgramARB				= (PFNGLBINDPROGRAMARBPROC) glXGetProcAddressARB((const GLubyte*)"glBindProgramARB");
		glDeleteProgramsARB				= (PFNGLDELETEPROGRAMSARBPROC) glXGetProcAddressARB((const GLubyte*)"glDeleteProgramsARB");
		glProgramStringARB				= (PFNGLPROGRAMSTRINGARBPROC) glXGetProcAddressARB((const GLubyte*)"glProgramStringARB");
		glProgramEnvParameter4fARB		= (PFNGLPROGRAMENVPARAMETER4FARBPROC) glXGetProcAddressARB((const GLubyte*)"glProgramEnvParameter4fARB");
		glProgramLocalParameter4fARB	= (PFNGLPROGRAMLOCALPARAMETER4FARBPROC) glXGetProcAddressARB((const GLubyte*)"glProgramLocalParameter4fARB");
		#endif
		sprintf(OpenGL.ExtSupported, "%sGL_ARB_fragment_program\n", OpenGL.ExtSupported);
	} else {
		OpenGL.IsExtUnsupported = true;
		OpenGL.Ext_FragmentProgram = false;
		sprintf(OpenGL.ExtUnsupported, "%sGL_ARB_fragment_program\n", OpenGL.ExtUnsupported);
	}
}

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
}

void RDP_LoadToSegment(unsigned char Segment, unsigned char * Buffer, unsigned int Offset, unsigned int Size)
{
	if(Segment >= MAX_SEGMENTS) return;

	RAM[Segment].Data = (unsigned char*) malloc (sizeof(char) * Size);

	unsigned int ID = Read32(Buffer, Offset);
	if(ID == 0x59617A30) {
		RDP_Yaz0Decode(&Buffer[Offset], RAM[Segment].Data, Size);
	} else if(ID == 0x4D494F30) {
		RDP_MIO0Decode(&Buffer[Offset], RAM[Segment].Data, Size);
	} else {
		memcpy(RAM[Segment].Data, &Buffer[Offset], Size);
	}

	RAM[Segment].IsSet = true;
	RAM[Segment].Size = Size;
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

	if(Segment >= MAX_SEGMENTS) return false;

	if((RAM[Segment].IsSet == false) || (RAM[Segment].Size < Offset)) return false;

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

void RDP_ClearTextures()
{
	if(Gfx.GLTextureID[0]) glDeleteTextures(Gfx.GLTextureCount, Gfx.GLTextureID);
	Gfx.GLTextureCount = 0;

	glGenTextures(CACHE_TEXTURES, &Gfx.GLTextureID[0]);
}

void RDP_ClearStructures(bool Full)
{
	int i = 0;

	static const struct __Vertex Vertex_Empty = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for(i = 0; i < ArraySize(Vertex); i++) Vertex[i] = Vertex_Empty;

	static const struct __Palette Palette_Empty = { 0, 0, 0, 0 };
	for(i = 0; i < ArraySize(Palette); i++) Palette[i] = Palette_Empty;

	static const struct __Texture Texture_Empty = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f };
	Texture[0] = Texture_Empty;
	Texture[1] = Texture_Empty;

	static const struct __TextureCache TextureCache_Empty = { 0, 0, 0, -1 };
	for(i = 0; i < ArraySize(TextureCache); i++) TextureCache[i] = TextureCache_Empty;
	System.TextureCachePosition = 0;

	static const struct __RGBA RGBA_Empty = { 0.0f, 0.0f, 0.0f, 0.0f };
	Gfx.BlendColor = RGBA_Empty;
	Gfx.EnvColor = RGBA_Empty;
	Gfx.FogColor = RGBA_Empty;
	static const struct __FillColor FillColor_Empty = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	Gfx.FillColor = FillColor_Empty;
	static const struct __PrimColor PrimColor_Empty = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 };
	Gfx.PrimColor = PrimColor_Empty;

	Gfx.DLStackPos = 0;

	Gfx.ChangedModes = 0;
	Gfx.GeometryMode = 0;
	Gfx.OtherModeL = 0;
	Gfx.OtherModeH = 0;
	Gfx.Store_RDPHalf1 = 0; Gfx.Store_RDPHalf2 = 0;
	Gfx.Combiner0 = 0; Gfx.Combiner1 = 0;

	if(Full) {
		static const struct __FragmentCache FragmentCache_Empty = { 0, 0, -1 };
		for(i = 0; i < ArraySize(FragmentCache); i++) FragmentCache[i] = FragmentCache_Empty;
		System.FragCachePosition = 0;
	}
}

void RDP_ParseDisplayList(unsigned int Address, bool ResetStack)
{
	if(!RDP_CheckAddressValidity(Address)) return;

	DListAddress = Address;

	RDP_SetRenderMode(0, 0);

	if(ResetStack) Gfx.DLStackPos = 0;

	while(Gfx.DLStackPos >= 0) {
		Segment = DListAddress >> 24;
		Offset = (DListAddress & 0x00FFFFFF);

		w0 = Read32(RAM[Segment].Data, Offset);
		w1 = Read32(RAM[Segment].Data, Offset + 4);

		wp0 = Read32(RAM[Segment].Data, Offset - 8);
		wp1 = Read32(RAM[Segment].Data, Offset - 4);

		wn0 = Read32(RAM[Segment].Data, Offset + 8);
		wn1 = Read32(RAM[Segment].Data, Offset + 12);

		RDP_UcodeCmd[w0 >> 24] ();

		DListAddress += 8;
	}
}

void RDP_DrawTriangle(int Vtxs[])
{
	glBegin(GL_TRIANGLES);

	int i = 0;
	for(i = 0; i < 3; i++) {
		float TempS0 = _FIXED2FLOAT(Vertex[Vtxs[i]].S, 16) * (Texture[0].ScaleS * Texture[0].ShiftScaleS) / 32.0f / _FIXED2FLOAT(Texture[0].RealWidth, 16);
		float TempT0 = _FIXED2FLOAT(Vertex[Vtxs[i]].T, 16) * (Texture[0].ScaleT * Texture[0].ShiftScaleT) / 32.0f / _FIXED2FLOAT(Texture[0].RealHeight, 16);

		if(OpenGL.Ext_MultiTexture) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempS0, TempT0);
			if(Gfx.IsMultiTexture) {
				float TempS1 = _FIXED2FLOAT(Vertex[Vtxs[i]].S, 16) * (Texture[1].ScaleS * Texture[1].ShiftScaleS) / 32.0f / _FIXED2FLOAT(Texture[1].RealWidth, 16);
				float TempT1 = _FIXED2FLOAT(Vertex[Vtxs[i]].T, 16) * (Texture[1].ScaleT * Texture[1].ShiftScaleT) / 32.0f / _FIXED2FLOAT(Texture[1].RealHeight, 16);
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB, TempS1, TempT1);
			}
		} else {
			glTexCoord2f(TempS0, TempT0);
		}

		glNormal3b(Vertex[Vtxs[i]].R, Vertex[Vtxs[i]].G, Vertex[Vtxs[i]].B);
		if(!(Gfx.GeometryMode & G_LIGHTING)) glColor4ub(Vertex[Vtxs[i]].R, Vertex[Vtxs[i]].G, Vertex[Vtxs[i]].B, Vertex[Vtxs[i]].A);

		glVertex3d(Vertex[Vtxs[i]].X, Vertex[Vtxs[i]].Y, Vertex[Vtxs[i]].Z);
	}

	glEnd();
}

void RDP_SetRenderMode(unsigned int Mode1, unsigned int Mode2)
{
	Gfx.OtherModeL &= 0x00000007;
	Gfx.OtherModeL |= Mode1 | Mode2;

	Gfx.ChangedModes |= CHANGED_RENDERMODE;
}

void RDP_CheckFragmentCache()
{
	int CacheCheck = 0; bool SearchingCache = true; bool NewProg = false;
	while(SearchingCache) {
		if((FragmentCache[CacheCheck].zCombiner0 == Gfx.Combiner0) && (FragmentCache[CacheCheck].zCombiner1 == Gfx.Combiner1)) {
			SearchingCache = false;
			NewProg = false;
		} else {
			if(CacheCheck != CACHE_FRAGMENT) {
				CacheCheck++;
			} else {
				SearchingCache = false;
				NewProg = true;
			}
		}
	}

	glEnable(GL_FRAGMENT_PROGRAM_ARB);

	if(NewProg) {
		RDP_CreateCombinerProgram(Gfx.Combiner0, Gfx.Combiner1);
	} else {
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, FragmentCache[CacheCheck].ProgramID);
	}

	if(System.FragCachePosition > CACHE_FRAGMENT) {
		int i = 0;
		static const struct __FragmentCache FragmentCache_Empty;
		for(i = 0; i < CACHE_FRAGMENT; i++) FragmentCache[i] = FragmentCache_Empty;
		System.FragCachePosition = 0;
	}
}

void RDP_CreateCombinerProgram(unsigned int Cmb0, unsigned int Cmb1)
{
	if(!OpenGL.Ext_FragmentProgram) return;

	int cA[2], cB[2], cC[2], cD[2], aA[2], aB[2], aC[2], aD[2];

	cA[0] = ((Cmb0 >> 20) & 0x0F);
	cB[0] = ((Cmb1 >> 28) & 0x0F);
	cC[0] = ((Cmb0 >> 15) & 0x1F);
	cD[0] = ((Cmb1 >> 15) & 0x07);

	aA[0] = ((Cmb0 >> 12) & 0x07);
	aB[0] = ((Cmb1 >> 12) & 0x07);
	aC[0] = ((Cmb0 >>  9) & 0x07);
	aD[0] = ((Cmb1 >>  9) & 0x07);

	cA[1] = ((Cmb0 >>  5) & 0x0F);
	cB[1] = ((Cmb1 >> 24) & 0x0F);
	cC[1] = ((Cmb0 >>  0) & 0x1F);
	cD[1] = ((Cmb1 >>  6) & 0x07);

	aA[1] = ((Cmb1 >> 21) & 0x07);
	aB[1] = ((Cmb1 >>  3) & 0x07);
	aC[1] = ((Cmb1 >> 18) & 0x07);
	aD[1] = ((Cmb1 >>  0) & 0x07);

	char ProgramString[16384];
	memset(ProgramString, 0x00, sizeof(ProgramString));

	char * LeadIn =
		"!!ARBfp1.0\n"
		"\n"
		"TEMP Tex0; TEMP Tex1;\n"
		"TEMP R0; TEMP R1;\n"
		"TEMP aR0; TEMP aR1;\n"
		"TEMP Comb; TEMP aComb;\n"
		"\n"
		"PARAM EnvColor = program.env[0];\n"
		"PARAM PrimColor = program.env[1];\n"
		"PARAM BlendColor = program.env[2];\n"
		"PARAM PrimColorLOD = program.env[3];\n"
		"ATTRIB Shade = fragment.color.primary;\n"
		"\n"
		"OUTPUT Out = result.color;\n"
		"\n"
		"TEX Tex0, fragment.texcoord[0], texture[0], 2D;\n"
		"TEX Tex1, fragment.texcoord[1], texture[1], 2D;\n"
		"\n";

	strcpy(ProgramString, LeadIn);

	int Cycle = 0, NumCycles = 2;
	for(Cycle = 0; Cycle < NumCycles; Cycle++) {
		sprintf(ProgramString, "%s# Color %d\n", ProgramString, Cycle);
		switch(cA[Cycle]) {
			case G_CCMUX_COMBINED:
				strcat(ProgramString, "MOV R0.rgb, Comb;\n");
				break;
			case G_CCMUX_TEXEL0:
				strcat(ProgramString, "MOV R0.rgb, Tex0;\n");
				break;
			case G_CCMUX_TEXEL1:
				strcat(ProgramString, "MOV R0.rgb, Tex1;\n");
				break;
			case G_CCMUX_PRIMITIVE:
				strcat(ProgramString, "MOV R0.rgb, PrimColor;\n");
				break;
			case G_CCMUX_SHADE:
				strcat(ProgramString, "MOV R0.rgb, Shade;\n");
				break;
			case G_CCMUX_ENVIRONMENT:
				strcat(ProgramString, "MOV R0.rgb, EnvColor;\n");
				break;
			case G_CCMUX_1:
				strcat(ProgramString, "MOV R0.rgb, {1.0, 1.0, 1.0, 1.0};\n");
				break;
			case G_CCMUX_COMBINED_ALPHA:
				strcat(ProgramString, "MOV R0.rgb, Comb.a;\n");
				break;
			case G_CCMUX_TEXEL0_ALPHA:
				strcat(ProgramString, "MOV R0.rgb, Tex0.a;\n");
				break;
			case G_CCMUX_TEXEL1_ALPHA:
				strcat(ProgramString, "MOV R0.rgb, Tex1.a;\n");
				break;
			case G_CCMUX_PRIMITIVE_ALPHA:
				strcat(ProgramString, "MOV R0.rgb, PrimColor.a;\n");
				break;
			case G_CCMUX_SHADE_ALPHA:
				strcat(ProgramString, "MOV R0.rgb, Shade.a;\n");
				break;
			case G_CCMUX_ENV_ALPHA:
				strcat(ProgramString, "MOV R0.rgb, EnvColor.a;\n");
				break;
			case G_CCMUX_LOD_FRACTION:
				strcat(ProgramString, "MOV R0.rgb, {0.0, 0.0, 0.0, 0.0};\n");	// unemulated
				break;
			case G_CCMUX_PRIM_LOD_FRAC:
				strcat(ProgramString, "MOV R0.rgb, PrimColorLOD;\n");
				break;
			case 15:	// 0
				strcat(ProgramString, "MOV R0.rgb, {0.0, 0.0, 0.0, 0.0};\n");
				break;
			default:
				strcat(ProgramString, "MOV R0.rgb, {0.0, 0.0, 0.0, 0.0};\n");
				sprintf(ProgramString, "%s# -%d\n", ProgramString, cA[Cycle]);
				break;
		}

		switch(cB[Cycle]) {
			case G_CCMUX_COMBINED:
				strcat(ProgramString, "MOV R1.rgb, Comb;\n");
				break;
			case G_CCMUX_TEXEL0:
				strcat(ProgramString, "MOV R1.rgb, Tex0;\n");
				break;
			case G_CCMUX_TEXEL1:
				strcat(ProgramString, "MOV R1.rgb, Tex1;\n");
				break;
			case G_CCMUX_PRIMITIVE:
				strcat(ProgramString, "MOV R1.rgb, PrimColor;\n");
				break;
			case G_CCMUX_SHADE:
				strcat(ProgramString, "MOV R1.rgb, Shade;\n");
				break;
			case G_CCMUX_ENVIRONMENT:
				strcat(ProgramString, "MOV R1.rgb, EnvColor;\n");
				break;
			case G_CCMUX_1:
				strcat(ProgramString, "MOV R1.rgb, {1.0, 1.0, 1.0, 1.0};\n");
				break;
			case G_CCMUX_COMBINED_ALPHA:
				strcat(ProgramString, "MOV R1.rgb, Comb.a;\n");
				break;
			case G_CCMUX_TEXEL0_ALPHA:
				strcat(ProgramString, "MOV R1.rgb, Tex0.a;\n");
				break;
			case G_CCMUX_TEXEL1_ALPHA:
				strcat(ProgramString, "MOV R1.rgb, Tex1.a;\n");
				break;
			case G_CCMUX_PRIMITIVE_ALPHA:
				strcat(ProgramString, "MOV R1.rgb, PrimColor.a;\n");
				break;
			case G_CCMUX_SHADE_ALPHA:
				strcat(ProgramString, "MOV R1.rgb, Shade.a;\n");
				break;
			case G_CCMUX_ENV_ALPHA:
				strcat(ProgramString, "MOV R1.rgb, EnvColor.a;\n");
				break;
			case G_CCMUX_LOD_FRACTION:
				strcat(ProgramString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");	// unemulated
				break;
			case G_CCMUX_PRIM_LOD_FRAC:
				strcat(ProgramString, "MOV R1.rgb, PrimColorLOD;\n");
				break;
			case 15:	// 0
				strcat(ProgramString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");
				break;
			default:
				strcat(ProgramString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");
				sprintf(ProgramString, "%s# -%d\n", ProgramString, cB[Cycle]);
				break;
		}
		strcat(ProgramString, "SUB R0, R0, R1;\n\n");

		switch(cC[Cycle]) {
			case G_CCMUX_COMBINED:
				strcat(ProgramString, "MOV R1.rgb, Comb;\n");
				break;
			case G_CCMUX_TEXEL0:
				strcat(ProgramString, "MOV R1.rgb, Tex0;\n");
				break;
			case G_CCMUX_TEXEL1:
				strcat(ProgramString, "MOV R1.rgb, Tex1;\n");
				break;
			case G_CCMUX_PRIMITIVE:
				strcat(ProgramString, "MOV R1.rgb, PrimColor;\n");
				break;
			case G_CCMUX_SHADE:
				strcat(ProgramString, "MOV R1.rgb, Shade;\n");
				break;
			case G_CCMUX_ENVIRONMENT:
				strcat(ProgramString, "MOV R1.rgb, EnvColor;\n");
				break;
			case G_CCMUX_1:
				strcat(ProgramString, "MOV R1.rgb, {1.0, 1.0, 1.0, 1.0};\n");
				break;
			case G_CCMUX_COMBINED_ALPHA:
				strcat(ProgramString, "MOV R1.rgb, Comb.a;\n");
				break;
			case G_CCMUX_TEXEL0_ALPHA:
				strcat(ProgramString, "MOV R1.rgb, Tex0.a;\n");
				break;
			case G_CCMUX_TEXEL1_ALPHA:
				strcat(ProgramString, "MOV R1.rgb, Tex1.a;\n");
				break;
			case G_CCMUX_PRIMITIVE_ALPHA:
				strcat(ProgramString, "MOV R1.rgb, PrimColor.a;\n");
				break;
			case G_CCMUX_SHADE_ALPHA:
				strcat(ProgramString, "MOV R1.rgb, Shade.a;\n");
				break;
			case G_CCMUX_ENV_ALPHA:
				strcat(ProgramString, "MOV R1.rgb, EnvColor.a;\n");
				break;
			case G_CCMUX_LOD_FRACTION:
				strcat(ProgramString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");	// unemulated
				break;
			case G_CCMUX_PRIM_LOD_FRAC:
				strcat(ProgramString, "MOV R1.rgb, PrimColorLOD;\n");
				break;
			case G_CCMUX_K5:
				strcat(ProgramString, "MOV R1.rgb, {1.0, 1.0, 1.0, 1.0};\n");	// unemulated
				break;
			case G_CCMUX_0:
				strcat(ProgramString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");
				break;
			default:
				strcat(ProgramString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");
				sprintf(ProgramString, "%s# -%d\n", ProgramString, cC[Cycle]);
				break;
		}
		strcat(ProgramString, "MUL R0, R0, R1;\n\n");

		switch(cD[Cycle]) {
			case G_CCMUX_COMBINED:
				strcat(ProgramString, "MOV R1.rgb, Comb;\n");
				break;
			case G_CCMUX_TEXEL0:
				strcat(ProgramString, "MOV R1.rgb, Tex0;\n");
				break;
			case G_CCMUX_TEXEL1:
				strcat(ProgramString, "MOV R1.rgb, Tex1;\n");
				break;
			case G_CCMUX_PRIMITIVE:
				strcat(ProgramString, "MOV R1.rgb, PrimColor;\n");
				break;
			case G_CCMUX_SHADE:
				strcat(ProgramString, "MOV R1.rgb, Shade;\n");
				break;
			case G_CCMUX_ENVIRONMENT:
				strcat(ProgramString, "MOV R1.rgb, EnvColor;\n");
				break;
			case G_CCMUX_1:
				strcat(ProgramString, "MOV R1.rgb, {1.0, 1.0, 1.0, 1.0};\n");
				break;
			case 7:		// 0
				strcat(ProgramString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");
				break;
			default:
				strcat(ProgramString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");
				sprintf(ProgramString, "%s# -%d\n", ProgramString, cD[Cycle]);
				break;
		}
		strcat(ProgramString, "ADD R0, R0, R1;\n\n");

		sprintf(ProgramString, "%s# Alpha %d\n", ProgramString, Cycle);

		switch(aA[Cycle]) {
			case G_ACMUX_COMBINED:
				strcat(ProgramString, "MOV aR0.a, aComb;\n");
				break;
			case G_ACMUX_TEXEL0:
				strcat(ProgramString, "MOV aR0.a, Tex0;\n");
				break;
			case G_ACMUX_TEXEL1:
				strcat(ProgramString, "MOV aR0.a, Tex1;\n");
				break;
			case G_ACMUX_PRIMITIVE:
				strcat(ProgramString, "MOV aR0.a, PrimColor;\n");
				break;
			case G_ACMUX_SHADE:
				strcat(ProgramString, "MOV aR0.a, Shade;\n");
				break;
			case G_ACMUX_ENVIRONMENT:
				strcat(ProgramString, "MOV aR0.a, EnvColor;\n");
				break;
			case G_ACMUX_1:
				strcat(ProgramString, "MOV aR0.a, {1.0, 1.0, 1.0, 1.0};\n");
				break;
			case G_ACMUX_0:
				strcat(ProgramString, "MOV aR0.a, {0.0, 0.0, 0.0, 0.0};\n");
				break;
			default:
				strcat(ProgramString, "MOV aR0.a, {0.0, 0.0, 0.0, 0.0};\n");
				sprintf(ProgramString, "%s# -%d\n", ProgramString, aA[Cycle]);
				break;
		}

		switch(aB[Cycle]) {
			case G_ACMUX_COMBINED:
				strcat(ProgramString, "MOV aR1.a, aComb.a;\n");
				break;
			case G_ACMUX_TEXEL0:
				strcat(ProgramString, "MOV aR1.a, Tex0.a;\n");
				break;
			case G_ACMUX_TEXEL1:
				strcat(ProgramString, "MOV aR1.a, Tex1.a;\n");
				break;
			case G_ACMUX_PRIMITIVE:
				strcat(ProgramString, "MOV aR1.a, PrimColor.a;\n");
				break;
			case G_ACMUX_SHADE:
				strcat(ProgramString, "MOV aR1.a, Shade.a;\n");
				break;
			case G_ACMUX_ENVIRONMENT:
				strcat(ProgramString, "MOV aR1.a, EnvColor.a;\n");
				break;
			case G_ACMUX_1:
				strcat(ProgramString, "MOV aR1.a, {1.0, 1.0, 1.0, 1.0};\n");
				break;
			case G_ACMUX_0:
				strcat(ProgramString, "MOV aR1.a, {0.0, 0.0, 0.0, 0.0};\n");
				break;
			default:
				strcat(ProgramString, "MOV aR1.a, {0.0, 0.0, 0.0, 0.0};\n");
				sprintf(ProgramString, "%s# -%d\n", ProgramString, aB[Cycle]);
				break;
		}
		strcat(ProgramString, "SUB aR0.a, aR0.a, aR1.a;\n\n");

		switch(aC[Cycle]) {
			case G_ACMUX_COMBINED:
				strcat(ProgramString, "MOV aR1.a, aComb.a;\n");
				break;
			case G_ACMUX_TEXEL0:
				strcat(ProgramString, "MOV aR1.a, Tex0.a;\n");
				break;
			case G_ACMUX_TEXEL1:
				strcat(ProgramString, "MOV aR1.a, Tex1.a;\n");
				break;
			case G_ACMUX_PRIMITIVE:
				strcat(ProgramString, "MOV aR1.a, PrimColor.a;\n");
				break;
			case G_ACMUX_SHADE:
				strcat(ProgramString, "MOV aR1.a, Shade.a;\n");
				break;
			case G_ACMUX_ENVIRONMENT:
				strcat(ProgramString, "MOV aR1.a, EnvColor.a;\n");
				break;
			case G_ACMUX_1:
				strcat(ProgramString, "MOV aR1.a, {1.0, 1.0, 1.0, 1.0};\n");
				break;
			case G_ACMUX_0:
				strcat(ProgramString, "MOV aR1.a, {0.0, 0.0, 0.0, 0.0};\n");
				break;
			default:
				strcat(ProgramString, "MOV aR1.a, {0.0, 0.0, 0.0, 0.0};\n");
				sprintf(ProgramString, "%s# -%d\n", ProgramString, aC[Cycle]);
				break;
		}
		strcat(ProgramString, "MUL aR0.a, aR0.a, aR1.a;\n\n");

		switch(aD[Cycle]) {
			case G_ACMUX_COMBINED:
				strcat(ProgramString, "MOV aR1.a, aComb.a;\n");
				break;
			case G_ACMUX_TEXEL0:
				strcat(ProgramString, "MOV aR1.a, Tex0.a;\n");
				break;
			case G_ACMUX_TEXEL1:
				strcat(ProgramString, "MOV aR1.a, Tex1.a;\n");
				break;
			case G_ACMUX_PRIMITIVE:
				strcat(ProgramString, "MOV aR1.a, PrimColor.a;\n");
				break;
			case G_ACMUX_SHADE:
				strcat(ProgramString, "MOV aR1.a, Shade.a;\n");
				break;
			case G_ACMUX_ENVIRONMENT:
				strcat(ProgramString, "MOV aR1.a, EnvColor.a;\n");
				break;
			case G_ACMUX_1:
				strcat(ProgramString, "MOV aR1.a, {1.0, 1.0, 1.0, 1.0};\n");
				break;
			case G_ACMUX_0:
				strcat(ProgramString, "MOV aR1.a, {0.0, 0.0, 0.0, 0.0};\n");
				break;
			default:
				strcat(ProgramString, "MOV aR1.a, {0.0, 0.0, 0.0, 0.0};\n");
				sprintf(ProgramString, "%s# -%d\n", ProgramString, aD[Cycle]);
				break;
		}
		strcat(ProgramString, "ADD aR0.a, aR0.a, aR1.a;\n\n");

		strcat(ProgramString, "MOV Comb.rgb, R0;\n");
		strcat(ProgramString, "MOV aComb.a, aR0.a;\n\n");
	}

	strcat(ProgramString, "# Finish\n");
	strcat(ProgramString,
			"MOV Comb.a, aComb.a;\n"
			"MOV Out, Comb;\n"
			"END\n");

	glGenProgramsARB(1, &FragmentCache[System.FragCachePosition].ProgramID);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, FragmentCache[System.FragCachePosition].ProgramID);
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(ProgramString), ProgramString);

	FragmentCache[System.FragCachePosition].zCombiner0 = Cmb0;
	FragmentCache[System.FragCachePosition].zCombiner1 = Cmb1;
	System.FragCachePosition++;
}

void RDP_ChangeTileSize(unsigned int Tile, unsigned int ULS, unsigned int ULT, unsigned int LRS, unsigned int LRT)
{
	Texture[Gfx.CurrentTexture].Tile = Tile;
	Texture[Gfx.CurrentTexture].ULS = ULS;
	Texture[Gfx.CurrentTexture].ULT = ULT;
	Texture[Gfx.CurrentTexture].LRS = LRS;
	Texture[Gfx.CurrentTexture].LRT = LRT;

	Texture[Gfx.CurrentTexture].Width = (Texture[Gfx.CurrentTexture].LRS - Texture[Gfx.CurrentTexture].ULS) + 1;
	Texture[Gfx.CurrentTexture].Height = (Texture[Gfx.CurrentTexture].LRT - Texture[Gfx.CurrentTexture].ULT) + 1;

	RDP_CalcTextureSize(Gfx.CurrentTexture);
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

	unsigned int Tile_Width = Texture[TextureID].LRS - Texture[TextureID].ULS + 1;
	unsigned int Tile_Height = Texture[TextureID].LRT - Texture[TextureID].ULT + 1;

	unsigned int Mask_Width = 1 << Texture[TextureID].MaskS;
	unsigned int Mask_Height = 1 << Texture[TextureID].MaskT;

	unsigned int Line_Height = 0;
	if(Line_Width > 0) Line_Height = min(MaxTexel / Line_Width, Tile_Height);

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

	unsigned int Clamp_Width = 0;
	unsigned int Clamp_Height = 0;

	if(Texture[TextureID].CMS == 1) {
		Clamp_Width = Tile_Width;
	} else {
		Clamp_Width = Texture[TextureID].Width;
	}
	if(Texture[TextureID].CMT == 1) {
		Clamp_Height = Tile_Height;
	} else {
		Clamp_Height = Texture[TextureID].Height;
	}

	if(Mask_Width > Texture[TextureID].Width) {
		Texture[TextureID].MaskS = RDP_PowOf(Texture[TextureID].Width);
		Mask_Width = 1 << Texture[TextureID].MaskS;
	}
	if(Mask_Height > Texture[TextureID].Height) {
		Texture[TextureID].MaskT = RDP_PowOf(Texture[TextureID].Height);
		Mask_Height = 1 << Texture[TextureID].MaskT;
	}

	if((Texture[TextureID].CMS == 2) || (Texture[TextureID].CMS == 3)) {
		Texture[TextureID].RealWidth = RDP_Pow2(Clamp_Width);
	} else if(Texture[TextureID].CMS == 1) {
		Texture[TextureID].RealWidth = RDP_Pow2(Mask_Width);
	} else {
		Texture[TextureID].RealWidth = RDP_Pow2(Texture[TextureID].Width);
	}

	if((Texture[TextureID].CMT == 2) || (Texture[TextureID].CMT == 3)) {
		Texture[TextureID].RealHeight = RDP_Pow2(Clamp_Height);
	} else if(Texture[TextureID].CMT == 1) {
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
	if(OpenGL.Ext_MultiTexture) {
		if(Texture[0].Offset != 0x00) {
			glEnable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, RDP_CheckTextureCache(0));
		}

		if(Gfx.IsMultiTexture && (Texture[1].Offset != 0x00)) {
			glEnable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_2D, RDP_CheckTextureCache(1));
		}

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
	} else {
		if(Texture[0].Offset != 0x00) {
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
			(TextureCache[CacheCheck].RealHeight == Texture[TexID].RealHeight)) {
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
		TextureCache[System.TextureCachePosition].Offset = Texture[TexID].Offset;
		TextureCache[System.TextureCachePosition].RealWidth = Texture[TexID].RealWidth;
		TextureCache[System.TextureCachePosition].RealHeight = Texture[TexID].RealHeight;
		TextureCache[System.TextureCachePosition].TextureID = GLID;
		System.TextureCachePosition++;
	} else {
		GLID = TextureCache[CacheCheck].TextureID;
	}

	if(System.TextureCachePosition > CACHE_TEXTURES) {
		int i = 0;
		static const struct __TextureCache TextureCache_Empty;
		for(i = 0; i < CACHE_TEXTURES; i++) TextureCache[i] = TextureCache_Empty;
		System.TextureCachePosition = 0;
	}

	return GLID;
}

GLuint RDP_LoadTexture(int TextureID)
{
	unsigned char TexSegment = Texture[TextureID].Offset >> 24;
	unsigned int TexOffset = (Texture[TextureID].Offset & 0x00FFFFFF);

//	if((wn0 == 0x03000000) || (wn0 == 0xE1000000)) return EXIT_SUCCESS;

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

	if(!RDP_CheckAddressValidity(Texture[TextureID].Offset)) {
		while(i < BufferSize) { TextureData[i++] = 0xFF; TextureData[i++] = 0x00; TextureData[i++] = 0x00; TextureData[i++] = 0xFF; }
	} else {
/*		char Log_PalOffset[256];
		sprintf(Log_PalOffset, "- CI palette offset: 0x%08X\n", (unsigned int)Texture[TextureID].PalOffset);
		char Log_PalNo[256];
		sprintf(Log_PalNo, "- Palette #%d\n", Texture[TextureID].Palette);
		char Log_YParam[256];
		sprintf(Log_YParam, "- CMS: %s%s%s\n",
			((Texture[TextureID].CMS & G_TX_WRAP) ? "G_TX_WRAP | " : ""),
			((Texture[TextureID].CMS & G_TX_MIRROR) ? "G_TX_MIRROR | " : ""),
			((Texture[TextureID].CMS & G_TX_CLAMP) ? "G_TX_CLAMP | " : ""));
		char Log_XParam[256];
		sprintf(Log_XParam, "- CMT: %s%s%s\n",
			((Texture[TextureID].CMT & G_TX_WRAP) ? "G_TX_WRAP" : ""),
			((Texture[TextureID].CMT & G_TX_MIRROR) ? "G_TX_MIRROR" : ""),
			((Texture[TextureID].CMT & G_TX_CLAMP) ? "G_TX_CLAMP" : ""));

		bool IsCITex = false;
		if(Texture[TextureID].PalOffset != 0) IsCITex = true;

		dbgprintf(0, MSK_COLORTYPE_INFO, "Texture unit %d:\n"
			"---------------\n"
			"- Offset: 0x%08X\n"
			"%s"
			"- Size: %d * %d\n"
			"- Format: %s (0x%02X)\n"
			"%s"
			"%s"
			"- S scale %4.2f, T scale %4.2f\n"
			"- S shift scale %4.2f, T shift scale %4.2f\n"
			"- S mask %d, T mask %d\n"
			"- 'LineSize' %d\n"
			"%s\n",
				TextureID,
				(unsigned int)Texture[TextureID].Offset,
				(IsCITex ? Log_PalOffset : ""),
				Texture[TextureID].Width, Texture[TextureID].Height,
				"", Texture[TextureID].Format,
				Log_YParam,
				Log_XParam,
				Texture[TextureID].ScaleS, Texture[TextureID].ScaleT,
				Texture[TextureID].ShiftScaleS, Texture[TextureID].ShiftScaleT,
				Texture[TextureID].MaskS, Texture[TextureID].MaskT,
				Texture[TextureID].LineSize,
				(IsCITex ? Log_PalNo : "")
		);
*/
		switch(Texture[TextureID].Format) {
			case 0x00:
			case 0x08:
			case 0x10: {
				unsigned short Raw;
				unsigned int RGBA = 0;

				for(j = 0; j < Texture[TextureID].Height; j++) {
					for(i = 0; i < Texture[TextureID].Width; i++) {
						Raw = (RAM[TexSegment].Data[TexOffset] << 8) | RAM[TexSegment].Data[TexOffset + 1];

						RGBA = ((Raw & 0xF800) >> 8) << 24;
						RGBA |= (((Raw & 0x07C0) << 5) >> 8) << 16;
						RGBA |= (((Raw & 0x003E) << 18) >> 16) << 8;
						if((Raw & 0x0001)) RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition, RGBA);

						TexOffset += 2;
						GLTexPosition += 4;

						if(TexOffset > RAM[TexSegment].Size) break;
					}
					TexOffset += Texture[TextureID].LineSize * 4 - Texture[TextureID].Width;
				}
				break; }

			case 0x18: {
				memcpy(TextureData, &RAM[TexSegment].Data[TexOffset], (Texture[TextureID].Height * Texture[TextureID].Width * 4));
				break; }

			case 0x40:
			case 0x50: {
				unsigned int CI1, CI2;
				unsigned int RGBA = 0;

				for(j = 0; j < Texture[TextureID].Height; j++) {
					for(i = 0; i < Texture[TextureID].Width / 2; i++) {
						CI1 = (RAM[TexSegment].Data[TexOffset] & 0xF0) >> 4;
						CI2 = (RAM[TexSegment].Data[TexOffset] & 0x0F);

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
						Raw = RAM[TexSegment].Data[TexOffset];

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
						Raw = (RAM[TexSegment].Data[TexOffset] & 0xF0) >> 4;
						RGBA = (((Raw & 0x0E) << 4) << 24);
						RGBA |= (((Raw & 0x0E) << 4) << 16);
						RGBA |= (((Raw & 0x0E) << 4) << 8);
						if((Raw & 0x01)) RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition, RGBA);

						Raw = (RAM[TexSegment].Data[TexOffset] & 0x0F);
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
						Raw = RAM[TexSegment].Data[TexOffset];
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
						TextureData[GLTexPosition]     = RAM[TexSegment].Data[TexOffset];
						TextureData[GLTexPosition + 1] = RAM[TexSegment].Data[TexOffset];
						TextureData[GLTexPosition + 2] = RAM[TexSegment].Data[TexOffset];
						TextureData[GLTexPosition + 3] = RAM[TexSegment].Data[TexOffset + 1];

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
						Raw = (RAM[TexSegment].Data[TexOffset] & 0xF0) >> 4;
						RGBA = (((Raw & 0x0F) << 4) << 24);
						RGBA |= (((Raw & 0x0F) << 4) << 16);
						RGBA |= (((Raw & 0x0F) << 4) << 8);
						RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition, RGBA);

						Raw = (RAM[TexSegment].Data[TexOffset] & 0x0F);
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
						TextureData[GLTexPosition]     = RAM[TexSegment].Data[TexOffset];
						TextureData[GLTexPosition + 1] = RAM[TexSegment].Data[TexOffset];
						TextureData[GLTexPosition + 2] = RAM[TexSegment].Data[TexOffset];
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Texture[TextureID].RealWidth, Texture[TextureID].RealHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureData);

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	free(TextureData);

	return Gfx.GLTextureID[Gfx.GLTextureCount++];
}

void RDP_UpdateGLStates()
{
	if(Gfx.ChangedModes & CHANGED_GEOMETRYMODE) {
		if(Gfx.GeometryMode & G_CULL_BOTH) {
			glEnable(GL_CULL_FACE);

			if(Gfx.GeometryMode & G_CULL_BACK)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);
		} else {
			glDisable(GL_CULL_FACE);
		}

		if((Gfx.GeometryMode & G_SHADING_SMOOTH) || !(Gfx.GeometryMode & G_TEXTURE_GEN_LINEAR)) {
			glShadeModel(GL_SMOOTH);
		} else {
			glShadeModel(GL_FLAT);
		}

		if(Gfx.GeometryMode & G_LIGHTING) {
			glEnable(GL_LIGHTING);
			glEnable(GL_NORMALIZE);
		} else {
			glDisable(GL_LIGHTING);
			glDisable(GL_NORMALIZE);
		}

		Gfx.ChangedModes &= ~CHANGED_GEOMETRYMODE;
	}
/*
	if(Gfx.GeometryMode & G_ZBUFFER)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
*/
	if(Gfx.ChangedModes & CHANGED_RENDERMODE) {
/*		if(Gfx.OtherModeL & Z_CMP)
			glDepthFunc(GL_LEQUAL);
		else
			glDepthFunc(GL_ALWAYS);
*/
/*		if((Gfx.OtherModeL & Z_UPD) && !(Gfx.OtherModeL & ZMODE_INTER && Gfx.OtherModeL & ZMODE_XLU))
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
*/
		if(Gfx.OtherModeL & ZMODE_DEC) {
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(-3.0f, -3.0f);
		} else {
			glDisable(GL_POLYGON_OFFSET_FILL);
		}
	}

	if((Gfx.ChangedModes & CHANGED_ALPHACOMPARE) || (Gfx.ChangedModes & CHANGED_RENDERMODE)) {
		if(!(Gfx.OtherModeL & ALPHA_CVG_SEL)) {
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc((Gfx.BlendColor.A > 0.0f) ? GL_GEQUAL : GL_GREATER, Gfx.BlendColor.A);
		} else if(Gfx.OtherModeL & CVG_X_ALPHA) {
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GEQUAL, 0.2f);
		} else
			glDisable(GL_ALPHA_TEST);
	}

	if(Gfx.ChangedModes & CHANGED_RENDERMODE) {
		if((Gfx.OtherModeL & FORCE_BL) && !(Gfx.OtherModeL & ALPHA_CVG_SEL)) {
			glEnable(GL_BLEND);

			switch(Gfx.OtherModeL >> 16) {
				case 0x0448: // Add
				case 0x055A:
					glBlendFunc(GL_ONE, GL_ONE);
					break;
				case 0x0C08: // 1080 Sky
				case 0x0F0A: // Used LOTS of places
					glBlendFunc(GL_ONE, GL_ZERO);
					break;
				case 0xC810: // Blends fog
				case 0xC811: // Blends fog
				case 0x0C18: // Standard interpolated blend
				case 0x0C19: // Used for antialiasing
				case 0x0050: // Standard interpolated blend
				case 0x0055: // Used for antialiasing
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case 0x0FA5: // Seems to be doing just blend color - maybe combiner can be used for this?
				case 0x5055: // Used in Paper Mario intro, I'm not sure if this is right...
					glBlendFunc(GL_ZERO, GL_ONE);
					break;

				default:
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;
			}
		} else {
			glDisable(GL_BLEND);
		}
	}
}

void RDP_Matrix_MulMatrices(float Src1[4][4], float Src2[4][4], float Target[4][4])
{
	int i = 0, j = 0;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			Target[i][j] =
				Src1[i][0] * Src2[0][j] +
				Src1[i][1] * Src2[1][j] +
				Src1[i][2] * Src2[2][j] +
				Src1[i][3] * Src2[3][j];
		}
	}
}

void RDP_Matrix_ModelviewLoad(float Mat[4][4])
{
	memcpy(Matrix.Model, Mat, 64);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(*Matrix.Model);
}

void RDP_Matrix_ModelviewMul(float Mat[4][4])
{
	float MatTemp[4][4];
	memcpy(MatTemp, Matrix.Model, 64);

	RDP_Matrix_MulMatrices(Mat, MatTemp, Matrix.Model);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(*Matrix.Model);
}

void RDP_Matrix_ModelviewPush()
{
	if(Matrix.ModelIndex == Matrix.ModelStackSize) return;

	memcpy(Matrix.ModelStack[Matrix.ModelIndex], Matrix.Model, 64);
	Matrix.ModelIndex++;
}

void RDP_Matrix_ModelviewPop(int PopTo)
{
	if(Matrix.ModelIndex > PopTo - 1) {
		Matrix.ModelIndex -= PopTo;
		memcpy(Matrix.Model, Matrix.ModelStack[Matrix.ModelIndex], 64);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(*Matrix.Model);
	} else {
		return;
	}
}

void RDP_Matrix_ProjectionLoad(float Mat[4][4])
{
	memcpy(Matrix.Proj, Mat, 64);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(*Matrix.Proj);
}

void RDP_Matrix_ProjectionMul(float Mat[4][4])
{
	float MatTemp[4][4];
	memcpy(MatTemp, Matrix.Proj, 64);

	RDP_Matrix_MulMatrices(Mat, MatTemp, Matrix.Proj);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(*Matrix.Proj);
}
