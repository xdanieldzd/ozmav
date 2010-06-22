#include "globals.h"
#include "f3dex_cmd.h"

unsigned int DListAddress;
unsigned char Segment; unsigned int Offset;

unsigned int w0, w1;
unsigned int wp0, wp1;
unsigned int wn0, wn1;

void dl_InitCombiner()
{
	dl_CreateCombinerProgram(0x0011FFFF, 0xFFFFFC38);
	dl_CreateCombinerProgram(0x00127E03, 0xFFFFFDF8);
	dl_CreateCombinerProgram(0x00127E03, 0xFFFFF3F8);
	dl_CreateCombinerProgram(0x00127E03, 0xFFFFF7F8);
	dl_CreateCombinerProgram(0x00121603, 0xFF5BFFF8);
	dl_CreateCombinerProgram(0x00267E04, 0x1F0CFDFF);
	dl_CreateCombinerProgram(0x0041FFFF, 0xFFFFFC38);
	dl_CreateCombinerProgram(0x00127E0C, 0xFFFFFDF8);
	dl_CreateCombinerProgram(0x00267E04, 0x1FFCFDF8);
	dl_CreateCombinerProgram(0x00262A04, 0x1F0C93FF);
	dl_CreateCombinerProgram(0x00121803, 0xFF5BFFF8);
	dl_CreateCombinerProgram(0x00121803, 0xFF0FFFFF);
	dl_CreateCombinerProgram(0x0041FFFF, 0xFFFFF638);
	dl_CreateCombinerProgram(0x0011FFFF, 0xFFFFF238);
	dl_CreateCombinerProgram(0x0041C7FF, 0xFFFFFE38);
	dl_CreateCombinerProgram(0x0041FFFF, 0xFFFFF838);

	dl_CreateCombinerProgram(0x00127E60, 0xFFFFF3F8);
	dl_CreateCombinerProgram(0x00272C04, 0x1F0C93FF);
	dl_CreateCombinerProgram(0x0020AC04, 0xFF0F93FF);
	dl_CreateCombinerProgram(0x0026A004, 0x1FFC93F8);
	dl_CreateCombinerProgram(0x00277E04, 0x1F0CF7FF);
	dl_CreateCombinerProgram(0x0020FE04, 0xFF0FF7FF);
	dl_CreateCombinerProgram(0x00272E04, 0x1F0C93FF);
	dl_CreateCombinerProgram(0x00272C04, 0x1F1093FF);
	dl_CreateCombinerProgram(0x0020A203, 0xFF13FFFF);
	dl_CreateCombinerProgram(0x0011FE04, 0xFFFFF7F8);
	dl_CreateCombinerProgram(0x0020AC03, 0xFF0F93FF);
	dl_CreateCombinerProgram(0x00272C03, 0x1F0C93FF);
	dl_CreateCombinerProgram(0x0011FE04, 0xFF0FF3FF);
	dl_CreateCombinerProgram(0x00119C04, 0xFFFFFFF8);
	dl_CreateCombinerProgram(0x00271204, 0x1F0CFFFF);
	dl_CreateCombinerProgram(0x0011FE04, 0xFFFFF3F8);
	dl_CreateCombinerProgram(0x00272C80, 0x350CF37F);
}

void dl_ParseDisplayList(unsigned int Address)
{
	if(!sv_CheckAddressValidity(Address)) return;

	DListAddress = Address;

	dl_SetRenderMode(0, 0);

	Gfx.GeometryMode = G_LIGHTING | F3DEX_SHADING_SMOOTH;
	Gfx.ChangedModes |= CHANGED_GEOMETRYMODE;

	while(Gfx.DLStackPos >= 0) {
		Segment = (DListAddress & 0xFF000000) >> 24;
		Offset = (DListAddress & 0x00FFFFFF);

		w0 = Read32(RAM[Segment].Data, Offset);
		w1 = Read32(RAM[Segment].Data, Offset + 4);

		wp0 = Read32(RAM[Segment].Data, Offset - 8);
		wp1 = Read32(RAM[Segment].Data, Offset - 4);

		wn0 = Read32(RAM[Segment].Data, Offset + 8);
		wn1 = Read32(RAM[Segment].Data, Offset + 12);

		dl_UcodeCmd[w0 >> 24] ();

		DListAddress += 8;
	}
}

void dl_UnemulatedCmd()
{
	MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "Illegal Ucode command 0x%02X!", w0 >> 24);
}

void dl_F3DEX_MTX()
{
	int i = 0, j = 0;
	signed long MtxTemp1 = 0, MtxTemp2 = 0;

	unsigned char Segment = (w1 & 0xFF000000) >> 24;
	unsigned int Offset = (w1 & 0x00FFFFFF);

	GLfloat Matrix[4][4];
	float fRecip = 1.0f / 65536.0f;

	switch(Segment) {
		case 0x01:
		case 0x0C:
		case 0x0D:
			return;
		case 0x80:
			glPopMatrix();
			return;
	}

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			MtxTemp1 = ((RAM[Segment].Data[Offset		] * 0x100) + RAM[Segment].Data[Offset + 1		]);
			MtxTemp2 = ((RAM[Segment].Data[Offset + 32	] * 0x100) + RAM[Segment].Data[Offset + 33	]);
			Matrix[i][j] = ((MtxTemp1 << 16) | MtxTemp2) * fRecip;

			Offset += 2;
		}
	}

	glPushMatrix();
	glMultMatrixf(*Matrix);
}

void dl_F3DEX_MOVEMEM()
{
	//
}

void dl_F3DEX_VTX()
{
	if(!sv_CheckAddressValidity(w1)) return;

	unsigned char TempSegment = (w1 & 0xFF000000) >> 24;
	unsigned int TempOffset = (w1 & 0x00FFFFFF);

	unsigned int V = _SHIFTR( w0, 17, 7 );
	unsigned int N = _SHIFTR( w0, 10, 6 );

	if((N > 32) || (V > 32)) return;

	int i = 0;
	for(i = 0; i < (N << 4); i += 16) {
		Vertex[V].X = ((RAM[TempSegment].Data[TempOffset + i] << 8) | RAM[TempSegment].Data[TempOffset + i + 1]);
		Vertex[V].Y = ((RAM[TempSegment].Data[TempOffset + i + 2] << 8) | RAM[TempSegment].Data[TempOffset + i + 3]);
		Vertex[V].Z = ((RAM[TempSegment].Data[TempOffset + i + 4] << 8) | RAM[TempSegment].Data[TempOffset + i + 5]);
		Vertex[V].S = ((RAM[TempSegment].Data[TempOffset + i + 8] << 8) | RAM[TempSegment].Data[TempOffset + i + 9]);
		Vertex[V].T = ((RAM[TempSegment].Data[TempOffset + i + 10] << 8) | RAM[TempSegment].Data[TempOffset + i + 11]);
		Vertex[V].R = RAM[TempSegment].Data[TempOffset + i + 12];
		Vertex[V].G = RAM[TempSegment].Data[TempOffset + i + 13];
		Vertex[V].B = RAM[TempSegment].Data[TempOffset + i + 14];
		Vertex[V].A = RAM[TempSegment].Data[TempOffset + i + 15];

		V++;
	}

	dl_InitLoadTexture();
}

void dl_F3DEX_DL()
{
	if(!sv_CheckAddressValidity(w1)) return;

	Gfx.DLStack[Gfx.DLStackPos] = DListAddress;
	Gfx.DLStackPos++;

	dl_ParseDisplayList(w1);
}

void dl_F3DEX_LOAD_UCODE()
{
	//
}

void dl_F3DEX_BRANCH_Z()
{
	if(!sv_CheckAddressValidity(Gfx.Store_RDPHalf1)) return;

	int Vtx = _SHIFTR(w0, 1, 11);
	int ZVal = (int)w1;

	if(Vertex[Vtx].Z < ZVal) {
		Gfx.DLStack[Gfx.DLStackPos] = DListAddress;
		Gfx.DLStackPos++;

		dl_ParseDisplayList(Gfx.Store_RDPHalf1);
	}
}

void dl_F3DEX_TRI2()
{
	if(Gfx.ChangedModes) gl_UpdateStates();

	int Vtxs1[] = { _SHIFTR( w0, 17, 7 ), _SHIFTR( w0, 9, 7 ), _SHIFTR( w0, 1, 7 ) };
	dl_DrawTriangle(Vtxs1);

	if(Gfx.ChangedModes) gl_UpdateStates();

	int Vtxs2[] = { _SHIFTR( w1, 17, 7 ), _SHIFTR( w1, 9, 7 ), _SHIFTR( w1, 1, 7 ) };
	dl_DrawTriangle(Vtxs2);
}

void dl_F3DEX_MODIFYVTX()
{
	//
}

void dl_F3DEX_RDPHALF_2()
{
	Gfx.Store_RDPHalf2 = w1;
}

void dl_F3DEX_RDPHALF_1()
{
	Gfx.Store_RDPHalf1 = w1;
}

void dl_F3DEX_CLEARGEOMETRYMODE()
{
	Gfx.GeometryMode &= ~w1;

	Gfx.ChangedModes |= CHANGED_GEOMETRYMODE;
}

void dl_F3DEX_SETGEOMETRYMODE()
{
	Gfx.GeometryMode |= w1;

	Gfx.ChangedModes |= CHANGED_GEOMETRYMODE;
}

void dl_F3DEX_ENDDL()
{
	Gfx.DLStackPos--;
	DListAddress = Gfx.DLStack[Gfx.DLStackPos];
}

void dl_F3DEX_SETOTHERMODE_L()
{
	switch(_SHIFTR( w0, 8, 8 )) {
		case G_MDSFT_RENDERMODE:
			dl_SetRenderMode(w1 & 0xCCCCFFFF, w1 & 0x3333FFFF);
			break;
		default: {
			unsigned int shift = _SHIFTR( w0, 8, 8 );
			unsigned int length = _SHIFTR( w0, 0, 8 );
			unsigned int mask = ((1 << length) - 1) << shift;

			Gfx.OtherModeL &= ~mask;
			Gfx.OtherModeL |= w1 & mask;

			Gfx.ChangedModes |= CHANGED_RENDERMODE | CHANGED_ALPHACOMPARE;
			break;
		}
	}
}

void dl_F3DEX_SETOTHERMODE_H()
{
	switch(_SHIFTR(w0, 8, 8)) {
		default: {
			unsigned int shift = _SHIFTR(w0, 8, 8);
			unsigned int length = _SHIFTR(w0, 0, 8);
			unsigned int mask = ((1 << length) - 1) << shift;

			Gfx.OtherModeH &= ~mask;
			Gfx.OtherModeH |= w1 & mask;
			break;
		}
	}
}

void dl_F3DEX_TEXTURE()
{
	memset(Texture, 0x00, sizeof(Texture));

	Texture[0].ScaleS = _FIXED2FLOAT(_SHIFTR(w1, 16, 16), 16);
	Texture[0].ScaleT = _FIXED2FLOAT(_SHIFTR(w1, 0, 16), 16);

	if(Texture[0].ScaleS == 0.0f) Texture[0].ScaleS = 1.0f;
	if(Texture[0].ScaleT == 0.0f) Texture[0].ScaleT = 1.0f;

	Texture[1].ScaleS = Texture[0].ScaleS;
	Texture[1].ScaleT = Texture[0].ScaleT;
}

void dl_F3DEX_MOVEWORD()
{
	//
}

void dl_F3DEX_POPMTX()
{
	glPopMatrix();
}

void dl_F3DEX_CULLDL()
{
	//
}

void dl_F3DEX_TRI1()
{
	if(Gfx.ChangedModes) gl_UpdateStates();

	int Vtxs[] = { _SHIFTR( w1, 17, 7 ), _SHIFTR( w1, 9, 7 ), _SHIFTR( w1, 1, 7 ) };
	dl_DrawTriangle(Vtxs);
}

void dl_G_TEXRECT()
{
	//
}

void dl_G_TEXRECTFLIP()
{
	//
}

void dl_G_RDPLOADSYNC()
{
	//
}

void dl_G_RDPPIPESYNC()
{
	//
}

void dl_G_RDPTILESYNC()
{
	//
}

void dl_G_RDPFULLSYNC()
{
	//
}

void dl_G_SETKEYGB()
{
	//
}

void dl_G_SETKEYR()
{
	//
}

void dl_G_SETCONVERT()
{
	//
}

void dl_G_SETSCISSOR()
{
	//
}

void dl_G_SETPRIMDEPTH()
{
	//
}

void dl_G_RDPSETOTHERMODE()
{
	//
}

void dl_G_LOADTLUT()
{
	if(!sv_CheckAddressValidity(Texture[Gfx.CurrentTexture].PalOffset)) return;

	memset(Palette, 0xFF, sizeof(Palette));

	unsigned char PalSegment = (Texture[Gfx.CurrentTexture].PalOffset & 0xFF000000) >> 24;
	unsigned int PalOffset = (Texture[Gfx.CurrentTexture].PalOffset & 0x00FFFFFF);

	unsigned int PalSize = ((w1 & 0x00FFF000) >> 14) + 1;

	unsigned short Raw;
	unsigned int R, G, B, A;

	unsigned int PalLoop;

	for(PalLoop = 0; PalLoop < PalSize; PalLoop++) {
		Raw = (RAM[PalSegment].Data[PalOffset] << 8) | RAM[PalSegment].Data[PalOffset + 1];

		R = (Raw & 0xF800) >> 8;
		G = ((Raw & 0x07C0) << 5) >> 8;
		B = ((Raw & 0x003E) << 18) >> 16;

		if((Raw & 0x0001)) { A = 0xFF; } else { A = 0x00; }

		Palette[PalLoop].R = R;
		Palette[PalLoop].G = G;
		Palette[PalLoop].B = B;
		Palette[PalLoop].A = A;

		PalOffset += 2;

		if(PalOffset > RAM[PalSegment].Size) break;
	}
}

void dl_G_SETTILESIZE()
{
	dl_ChangeTileSize(_SHIFTR(w1, 24, 3), _SHIFTR(w0, 12, 12), _SHIFTR(w0, 0, 12), _SHIFTR(w1, 12, 12), _SHIFTR(w1, 0, 12));
}

void dl_G_LOADBLOCK()
{
	dl_ChangeTileSize(_SHIFTR(w1, 24, 3), _SHIFTR(w0, 12, 12), _SHIFTR(w0, 0, 12), _SHIFTR(w1, 12, 12), _SHIFTR(w1, 0, 12));
}

void dl_G_LOADTILE()
{
	//
}

void dl_G_SETTILE()
{
	if(w1 == 0x07000000) return;

	Texture[Gfx.CurrentTexture].Format = (w0 & 0x00FF0000) >> 16;
	Texture[Gfx.CurrentTexture].CMT = _SHIFTR(w1, 18, 2);
	Texture[Gfx.CurrentTexture].CMS = _SHIFTR(w1, 8, 2);
	Texture[Gfx.CurrentTexture].LineSize = _SHIFTR(w0, 9, 9);
	Texture[Gfx.CurrentTexture].Palette = _SHIFTR(w1, 20, 4);
	Texture[Gfx.CurrentTexture].ShiftT = _SHIFTR(w1, 10, 4);
	Texture[Gfx.CurrentTexture].ShiftS = _SHIFTR(w1, 0, 4);
	Texture[Gfx.CurrentTexture].MaskT = _SHIFTR(w1, 14, 4);
	Texture[Gfx.CurrentTexture].MaskS = _SHIFTR(w1, 4, 4);
}

void dl_G_FILLRECT()
{
	//
}

void dl_G_SETFILLCOLOR(void)
{
	Gfx.FillColor.R = _SHIFTR(w1, 11, 5) * 0.032258064f;
	Gfx.FillColor.G = _SHIFTR(w1, 6, 5) * 0.032258064f;
	Gfx.FillColor.B = _SHIFTR(w1, 1, 5) * 0.032258064f;
	Gfx.FillColor.A = _SHIFTR(w1, 0, 1);

	Gfx.FillColor.Z = _SHIFTR(w1, 2, 14);
	Gfx.FillColor.DZ = _SHIFTR(w1, 0, 2);
}

void dl_G_SETFOGCOLOR(void)
{
	Gfx.FogColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	Gfx.FogColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	Gfx.FogColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	Gfx.FogColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;
}

void dl_G_SETBLENDCOLOR(void)
{
	Gfx.BlendColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	Gfx.BlendColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	Gfx.BlendColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	Gfx.BlendColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	if(OpenGL.Ext_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, Gfx.BlendColor.R, Gfx.BlendColor.G, Gfx.BlendColor.B, Gfx.BlendColor.A);
	}
}

void dl_G_SETPRIMCOLOR(void)
{
	Gfx.PrimColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	Gfx.PrimColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	Gfx.PrimColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	Gfx.PrimColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	Gfx.PrimColor.M = _SHIFTL(w0, 8, 8);
	Gfx.PrimColor.L = _SHIFTL(w0, 0, 8) * 0.0039215689f;

	if(OpenGL.Ext_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, Gfx.PrimColor.R, Gfx.PrimColor.G, Gfx.PrimColor.B, Gfx.PrimColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 3, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L);
	}
}

void dl_G_SETENVCOLOR(void)
{
	Gfx.EnvColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	Gfx.EnvColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	Gfx.EnvColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	Gfx.EnvColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	if(OpenGL.Ext_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, Gfx.EnvColor.R, Gfx.EnvColor.G, Gfx.EnvColor.B, Gfx.EnvColor.A);
	}
}

void dl_G_SETCOMBINE()
{
	Gfx.Combiner0 = (w0 & 0x00FFFFFF);
	Gfx.Combiner1 = w1;

	if(OpenGL.Ext_FragmentProgram) dl_CheckFragmentCache();
}

void dl_G_SETTIMG()
{
	Gfx.CurrentTexture = 0;
	Gfx.IsMultiTexture = false;

	Texture[Gfx.CurrentTexture].Offset = w1;
}

void dl_G_SETZIMG()
{
	//
}

void dl_G_SETCIMG()
{
	//
}

void dl_DrawTriangle(int Vtxs[])
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

/*		static bool check = true;
		if(check) {
			MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "%4.2f, %4.2f, %4.2f, %4.2f -> %4.2f",
				_FIXED2FLOAT(Vertex[Vtxs[i]].S, 16), (Texture[0].ScaleS), (Texture[0].ShiftScaleS), _FIXED2FLOAT(Texture[0].RealWidth, 16), TempS0);
			check = false;
		}
*/
		glNormal3b(Vertex[Vtxs[i]].R, Vertex[Vtxs[i]].G, Vertex[Vtxs[i]].B);
		if(!(Gfx.GeometryMode & G_LIGHTING)) glColor4ub(Vertex[Vtxs[i]].R, Vertex[Vtxs[i]].G, Vertex[Vtxs[i]].B, Vertex[Vtxs[i]].A);

		glVertex3d(Vertex[Vtxs[i]].X, Vertex[Vtxs[i]].Y, Vertex[Vtxs[i]].Z);
	}

	glEnd();
}

void dl_SetRenderMode(unsigned int Mode1, unsigned int Mode2)
{
	Gfx.OtherModeL &= 0x00000007;
	Gfx.OtherModeL |= Mode1 | Mode2;

	Gfx.ChangedModes |= CHANGED_RENDERMODE;
}

void dl_CheckFragmentCache()
{
	int CacheCheck = 0; bool SearchingCache = true; bool NewProg = false;
	while(SearchingCache) {
		if((FragmentCache[CacheCheck].Combiner0 == Gfx.Combiner0) && (FragmentCache[CacheCheck].Combiner1 == Gfx.Combiner1)) {
			SearchingCache = false;
			NewProg = false;
		} else {
			if(CacheCheck != 256) {
				CacheCheck++;
			} else {
				SearchingCache = false;
				NewProg = true;
			}
		}
	}

	glEnable(GL_FRAGMENT_PROGRAM_ARB);

	if(NewProg) {
		dl_CreateCombinerProgram(Gfx.Combiner0, Gfx.Combiner1);
	} else {
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, FragmentCache[CacheCheck].ProgramID);
	}

	if(Program.FragCachePosition > 256) {
		Program.FragCachePosition = 0;
		memset(FragmentCache, 0x00, sizeof(FragmentCache));
	}
}

void dl_CreateCombinerProgram(unsigned int Cmb0, unsigned int Cmb1)
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

	glGenProgramsARB(1, &FragmentCache[Program.FragCachePosition].ProgramID);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, FragmentCache[Program.FragCachePosition].ProgramID);
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(ProgramString), ProgramString);

	FragmentCache[Program.FragCachePosition].Combiner0 = Cmb0;
	FragmentCache[Program.FragCachePosition].Combiner1 = Cmb1;
	Program.FragCachePosition++;
}

void dl_ChangeTileSize(unsigned int Tile, unsigned int ULS, unsigned int ULT, unsigned int LRS, unsigned int LRT)
{
	Texture[Gfx.CurrentTexture].Tile = Tile;
	Texture[Gfx.CurrentTexture].ULS = ULS;
	Texture[Gfx.CurrentTexture].ULT = ULT;
	Texture[Gfx.CurrentTexture].LRS = LRS;
	Texture[Gfx.CurrentTexture].LRT = LRT;
}

void dl_CalcTextureSize(int TextureID)
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

	if((Texture[TextureID].CMS & G_TX_CLAMP) && (!Texture[TextureID].CMS & G_TX_MIRROR)) {
		Clamp_Width = Tile_Width;
	} else {
		Clamp_Width = Texture[TextureID].Width;
	}
	if((Texture[TextureID].CMT & G_TX_CLAMP) && (!Texture[TextureID].CMT & G_TX_MIRROR)) {
		Clamp_Height = Tile_Height;
	} else {
		Clamp_Height = Texture[TextureID].Height;
	}

	if(Mask_Width > Texture[TextureID].Width) {
		Texture[TextureID].MaskS = dl_PowOf(Texture[TextureID].Width);
		Mask_Width = 1 << Texture[TextureID].MaskS;
	}
	if(Mask_Height > Texture[TextureID].Height) {
		Texture[TextureID].MaskT = dl_PowOf(Texture[TextureID].Height);
		Mask_Height = 1 << Texture[TextureID].MaskT;
	}

	if(Texture[TextureID].CMS & G_TX_CLAMP) {
		Texture[TextureID].RealWidth = dl_Pow2(Clamp_Width);
	} else if(Texture[TextureID].CMS & G_TX_MIRROR) {
		Texture[TextureID].RealWidth = dl_Pow2(Mask_Width);
	} else {
		Texture[TextureID].RealWidth = dl_Pow2(Texture[TextureID].Width);
	}

	if(Texture[TextureID].CMT & G_TX_CLAMP) {
		Texture[TextureID].RealHeight = dl_Pow2(Clamp_Height);
	} else if(Texture[TextureID].CMT & G_TX_MIRROR) {
		Texture[TextureID].RealHeight = dl_Pow2(Mask_Height);
	} else {
		Texture[TextureID].RealHeight = dl_Pow2(Texture[TextureID].Height);
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

inline unsigned long dl_Pow2(unsigned long dim) {
	unsigned long i = 1;

	while (i < dim) i <<= 1;

	return i;
}

inline unsigned long dl_PowOf(unsigned long dim) {
	unsigned long num = 1;
	unsigned long i = 0;

	while (num < dim) {
		num <<= 1;
		i++;
	}

	return i;
}

void dl_InitLoadTexture()
{
	if(OpenGL.Ext_MultiTexture) {
		if(Texture[0].Offset != 0x00) {
			glEnable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, dl_CheckTextureCache(0));
		}

		if(Gfx.IsMultiTexture && (Texture[1].Offset != 0x00)) {
			glEnable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_2D, dl_CheckTextureCache(1));
		}

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
	} else {
		if(Texture[0].Offset != 0x00) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, dl_CheckTextureCache(0));
		}
	}
}

GLuint dl_CheckTextureCache(unsigned int TexID)
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
			if(CacheCheck != 2048) {
				CacheCheck++;
			} else {
				SearchingCache = false;
				NewTexture = true;
			}
		}
	}

	if(NewTexture) {
		GLID = dl_LoadTexture(TexID);
		TextureCache[Program.TextureCachePosition].Offset = Texture[TexID].Offset;
		TextureCache[Program.TextureCachePosition].RealWidth = Texture[TexID].RealWidth;
		TextureCache[Program.TextureCachePosition].RealHeight = Texture[TexID].RealHeight;
		TextureCache[Program.TextureCachePosition].TextureID = GLID;
		Program.TextureCachePosition++;
	} else {
		GLID = TextureCache[CacheCheck].TextureID;
	}

	if(Program.TextureCachePosition > 2048) {
		int i = 0;
		static const struct __TextureCache TextureCache_Empty;
		for(i = 0; i < 2048; i++) TextureCache[i] = TextureCache_Empty;
		Program.TextureCachePosition = 0;
	}

	return GLID;
}

GLuint dl_LoadTexture(int TextureID)
{
	unsigned char TexSegment = (Texture[TextureID].Offset & 0xFF000000) >> 24;
	unsigned int TexOffset = (Texture[TextureID].Offset & 0x00FFFFFF);

	dl_CalcTextureSize(TextureID);

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

	if(!sv_CheckAddressValidity(Texture[TextureID].Offset)) {
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

	if(Texture[TextureID].CMT & G_TX_CLAMP) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); }
	if(Texture[TextureID].CMT & G_TX_WRAP) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); }
	if(Texture[TextureID].CMT & G_TX_MIRROR) { if(OpenGL.Ext_TexMirroredRepeat) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB); }

	if(Texture[TextureID].CMS & G_TX_CLAMP) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); }
	if(Texture[TextureID].CMS & G_TX_WRAP) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); }
	if(Texture[TextureID].CMS & G_TX_MIRROR) { if(OpenGL.Ext_TexMirroredRepeat) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB); }

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Texture[TextureID].RealWidth, Texture[TextureID].RealHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	free(TextureData);

	return Gfx.GLTextureID[Gfx.GLTextureCount++];
}
