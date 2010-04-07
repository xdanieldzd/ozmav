#include "globals.h"
#include "f3dex2_cmd.h"

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
	if(!zl_CheckAddressValidity(Address)) return;

	DListAddress = Address;

	dl_SetRenderMode(0, 0);

	while(zGfx.DLStackPos >= 0) {
		Segment = (DListAddress & 0xFF000000) >> 24;
		Offset = (DListAddress & 0x00FFFFFF);

		w0 = Read32(zRAM[Segment].Data, Offset);
		w1 = Read32(zRAM[Segment].Data, Offset + 4);

		wp0 = Read32(zRAM[Segment].Data, Offset - 8);
		wp1 = Read32(zRAM[Segment].Data, Offset - 4);

		wn0 = Read32(zRAM[Segment].Data, Offset + 8);
		wn1 = Read32(zRAM[Segment].Data, Offset + 12);

		dl_UcodeCmd[w0 >> 24] ();

		DListAddress += 8;
	}
}

void dl_UnemulatedCmd()
{
	//
}

void dl_F3DEX2_VTX()
{
	if(!zl_CheckAddressValidity(w1)) return;

	unsigned char TempSegment = (w1 & 0xFF000000) >> 24;
	unsigned int TempOffset = (w1 & 0x00FFFFFF);

	unsigned int N = ((w0 >> 12) & 0xFF);
	unsigned int V = ((w0 >> 1) & 0x7F) - N;

	if((N > 32) || (V > 32)) return;

	int i = 0;
	for(i = 0; i < (N << 4); i += 16) {
		zVertex[V].X = ((zRAM[TempSegment].Data[TempOffset + i] << 8) | zRAM[TempSegment].Data[TempOffset + i + 1]);
		zVertex[V].Y = ((zRAM[TempSegment].Data[TempOffset + i + 2] << 8) | zRAM[TempSegment].Data[TempOffset + i + 3]);
		zVertex[V].Z = ((zRAM[TempSegment].Data[TempOffset + i + 4] << 8) | zRAM[TempSegment].Data[TempOffset + i + 5]);
		zVertex[V].S = ((zRAM[TempSegment].Data[TempOffset + i + 8] << 8) | zRAM[TempSegment].Data[TempOffset + i + 9]);
		zVertex[V].T = ((zRAM[TempSegment].Data[TempOffset + i + 10] << 8) | zRAM[TempSegment].Data[TempOffset + i + 11]);
		zVertex[V].R = zRAM[TempSegment].Data[TempOffset + i + 12];
		zVertex[V].G = zRAM[TempSegment].Data[TempOffset + i + 13];
		zVertex[V].B = zRAM[TempSegment].Data[TempOffset + i + 14];
		zVertex[V].A = zRAM[TempSegment].Data[TempOffset + i + 15];

		V++;
	}

	dl_InitLoadTexture();
}

void dl_F3DEX2_MODIFYVTX()
{
	//
}

void dl_F3DEX2_CULLDL()
{
	//
}

void dl_F3DEX2_BRANCH_Z()
{
	if(!zl_CheckAddressValidity(zGfx.Store_RDPHalf1)) return;

	int Vtx = _SHIFTR(w0, 1, 11);
	short ZVal = (short)w1;

	if(zVertex[Vtx].Z < ZVal) {
		zGfx.DLStack[zGfx.DLStackPos] = DListAddress;
		zGfx.DLStackPos++;

		dl_ParseDisplayList(zGfx.Store_RDPHalf1);
	}
}

void dl_F3DEX2_TRI1()
{
	if(zGfx.ChangedModes) gl_UpdateStates();

	int Vtxs[] = { ((w0 & 0x00FF0000) >> 16) / 2, ((w0 & 0x0000FF00) >> 8) / 2, (w0 & 0x000000FF) / 2 };
	dl_DrawTriangle(Vtxs);
	md_DumpTriangle(Vtxs);
}

void dl_F3DEX2_TRI2()
{
	if(zGfx.ChangedModes) gl_UpdateStates();

	int Vtxs1[] = { ((w0 & 0x00FF0000) >> 16) / 2, ((w0 & 0x0000FF00) >> 8) / 2, (w0 & 0x000000FF) / 2 };
	dl_DrawTriangle(Vtxs1);
	md_DumpTriangle(Vtxs1);

	if(zGfx.ChangedModes) gl_UpdateStates();

	int Vtxs2[] = { ((w1 & 0x00FF0000) >> 16) / 2, ((w1 & 0x0000FF00) >> 8) / 2, (w1 & 0x000000FF) / 2 };
	dl_DrawTriangle(Vtxs2);
	md_DumpTriangle(Vtxs2);
}

void dl_F3DEX2_QUAD()
{
	//
}

void dl_F3DEX2_SPECIAL_3()
{
	//
}

void dl_F3DEX2_SPECIAL_2()
{
	//
}

void dl_F3DEX2_SPECIAL_1()
{
	//
}

void dl_F3DEX2_DMA_IO()
{
	//
}

void dl_F3DEX2_TEXTURE()
{
	memset(zTexture, 0x00, sizeof(zTexture));

	zTexture[0].ScaleS = _FIXED2FLOAT(_SHIFTR(w1, 16, 16), 16);
	zTexture[0].ScaleT = _FIXED2FLOAT(_SHIFTR(w1, 0, 16), 16);

	if(zTexture[0].ScaleS == 0.0f) zTexture[0].ScaleS = 1.0f;
	if(zTexture[0].ScaleT == 0.0f) zTexture[0].ScaleT = 1.0f;

	zTexture[1].ScaleS = zTexture[0].ScaleS;
	zTexture[1].ScaleT = zTexture[0].ScaleT;
}

void dl_F3DEX2_POPMTX()
{
	// from OZMAV, wrong but works...

	glPopMatrix();
}

void dl_F3DEX2_GEOMETRYMODE()
{
	zGfx.GeometryMode &= (w0 & 0x00FFFFFF);
	zGfx.GeometryMode |= (w1 & 0x00FFFFFF);

	zGfx.ChangedModes |= CHANGED_GEOMETRYMODE;
}

void dl_F3DEX2_MTX()
{
	// from OZMAV, wrong but works...

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
			MtxTemp1 = ((zRAM[Segment].Data[Offset		] * 0x100) + zRAM[Segment].Data[Offset + 1		]);
			MtxTemp2 = ((zRAM[Segment].Data[Offset + 32	] * 0x100) + zRAM[Segment].Data[Offset + 33	]);
			Matrix[i][j] = ((MtxTemp1 << 16) | MtxTemp2) * fRecip;

			Offset += 2;
		}
	}

	glPushMatrix();
	glMultMatrixf(*Matrix);
}

void dl_F3DEX2_MOVEWORD()
{
	//
}

void dl_F3DEX2_MOVEMEM()
{
	//
}

void dl_F3DEX2_LOAD_UCODE()
{
	//
}

void dl_F3DEX2_DL()
{
	if(!zl_CheckAddressValidity(w1)) return;

	zGfx.DLStack[zGfx.DLStackPos] = DListAddress;
	zGfx.DLStackPos++;

	dl_ParseDisplayList(w1);
}

void dl_F3DEX2_ENDDL()
{
	zGfx.DLStackPos--;
	DListAddress = zGfx.DLStack[zGfx.DLStackPos];
}

void dl_F3DEX2_SPNOOP()
{
	//
}

void dl_F3DEX2_RDPHALF_1()
{
	zGfx.Store_RDPHalf1 = w1;
}

void dl_F3DEX2_SETOTHERMODE_L()
{
	switch(32 - _SHIFTR( w0, 8, 8 ) - (_SHIFTR( w0, 0, 8 ) + 1)) {
		case G_MDSFT_RENDERMODE:
			dl_SetRenderMode(w1 & 0xCCCCFFFF, w1 & 0x3333FFFF);
			break;
		default: {
			unsigned int length = _SHIFTR( w0, 0, 8 ) + 1;
			unsigned int shift = 32 - _SHIFTR( w0, 8, 8 ) - length;
			unsigned int mask = ((1 << length) - 1) << shift;

			zGfx.OtherModeL &= ~mask;
			zGfx.OtherModeL |= w1 & mask;

			zGfx.ChangedModes |= CHANGED_RENDERMODE | CHANGED_ALPHACOMPARE;
			break;
		}
	}
}

void dl_F3DEX2_SETOTHERMODE_H()
{
	switch(32 - _SHIFTR( w0, 8, 8 ) - (_SHIFTR( w0, 0, 8 ) + 1)) {
		default: {
			unsigned int length = _SHIFTR( w0, 0, 8 ) + 1;
			unsigned int shift = 32 - _SHIFTR( w0, 8, 8 ) - length;
			unsigned int mask = ((1 << length) - 1) << shift;

			zGfx.OtherModeH &= ~mask;
			zGfx.OtherModeH |= w1 & mask;
			break;
		}
	}
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
	if(!zl_CheckAddressValidity(zTexture[zGfx.CurrentTexture].PalOffset)) return;

	memset(zPalette, 0xFF, sizeof(zPalette));

	unsigned char PalSegment = (zTexture[zGfx.CurrentTexture].PalOffset & 0xFF000000) >> 24;
	unsigned int PalOffset = (zTexture[zGfx.CurrentTexture].PalOffset & 0x00FFFFFF);

	unsigned int PalSize = ((w1 & 0x00FFF000) >> 14) + 1;

	unsigned short Raw;
	unsigned int R, G, B, A;

	unsigned int PalLoop;

	for(PalLoop = 0; PalLoop < PalSize; PalLoop++) {
		Raw = (zRAM[PalSegment].Data[PalOffset] << 8) | zRAM[PalSegment].Data[PalOffset + 1];

		R = (Raw & 0xF800) >> 8;
		G = ((Raw & 0x07C0) << 5) >> 8;
		B = ((Raw & 0x003E) << 18) >> 16;

		if((Raw & 0x0001)) { A = 0xFF; } else { A = 0x00; }

		zPalette[PalLoop].R = R;
		zPalette[PalLoop].G = G;
		zPalette[PalLoop].B = B;
		zPalette[PalLoop].A = A;

		PalOffset += 2;

		if(PalOffset > zRAM[PalSegment].Size) break;
	}
}

void dl_F3DEX2_RDPHALF_2()
{
	zGfx.Store_RDPHalf2 = w1;
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

	zTexture[zGfx.CurrentTexture].Format = (w0 & 0x00FF0000) >> 16;
	zTexture[zGfx.CurrentTexture].CMT = _SHIFTR(w1, 18, 2);
	zTexture[zGfx.CurrentTexture].CMS = _SHIFTR(w1, 8, 2);
	zTexture[zGfx.CurrentTexture].LineSize = _SHIFTR(w0, 9, 9);
	zTexture[zGfx.CurrentTexture].Palette = _SHIFTR(w1, 20, 4);
	zTexture[zGfx.CurrentTexture].ShiftT = _SHIFTR(w1, 10, 4);
	zTexture[zGfx.CurrentTexture].ShiftS = _SHIFTR(w1, 0, 4);
	zTexture[zGfx.CurrentTexture].MaskT = _SHIFTR(w1, 14, 4);
	zTexture[zGfx.CurrentTexture].MaskS = _SHIFTR(w1, 4, 4);
}

void dl_G_FILLRECT()
{
	//
}

void dl_G_SETFILLCOLOR(void)
{
	zGfx.FillColor.R = _SHIFTR(w1, 11, 5) * 0.032258064f;
	zGfx.FillColor.G = _SHIFTR(w1, 6, 5) * 0.032258064f;
	zGfx.FillColor.B = _SHIFTR(w1, 1, 5) * 0.032258064f;
	zGfx.FillColor.A = _SHIFTR(w1, 0, 1);

	zGfx.FillColor.Z = _SHIFTR(w1, 2, 14);
	zGfx.FillColor.DZ = _SHIFTR(w1, 0, 2);
}

void dl_G_SETFOGCOLOR(void)
{
	zGfx.FogColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	zGfx.FogColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	zGfx.FogColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	zGfx.FogColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;
}

void dl_G_SETBLENDCOLOR(void)
{
	zGfx.BlendColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	zGfx.BlendColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	zGfx.BlendColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	zGfx.BlendColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	if(zOpenGL.Ext_FragmentProgram && zOptions.EnableCombiner) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, zGfx.BlendColor.R, zGfx.BlendColor.G, zGfx.BlendColor.B, zGfx.BlendColor.A);
	}
}

void dl_G_SETPRIMCOLOR(void)
{
	zGfx.PrimColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	zGfx.PrimColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	zGfx.PrimColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	zGfx.PrimColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	zGfx.PrimColor.M = _SHIFTL(w0, 8, 8);
	zGfx.PrimColor.L = _SHIFTL(w0, 0, 8) * 0.0039215689f;

	if(zOpenGL.Ext_FragmentProgram && zOptions.EnableCombiner) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, zGfx.PrimColor.R, zGfx.PrimColor.G, zGfx.PrimColor.B, zGfx.PrimColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 3, zGfx.PrimColor.L, zGfx.PrimColor.L, zGfx.PrimColor.L, zGfx.PrimColor.L);
	}
}

void dl_G_SETENVCOLOR(void)
{
	zGfx.EnvColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	zGfx.EnvColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	zGfx.EnvColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	zGfx.EnvColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	if(zOpenGL.Ext_FragmentProgram && zOptions.EnableCombiner) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, zGfx.EnvColor.R, zGfx.EnvColor.G, zGfx.EnvColor.B, zGfx.EnvColor.A);
	}
}

void dl_G_SETCOMBINE()
{
	zGfx.Combiner0 = (w0 & 0x00FFFFFF);
	zGfx.Combiner1 = w1;

	if(zOpenGL.Ext_FragmentProgram && zOptions.EnableCombiner) dl_CheckFragmentCache();
}

void dl_G_SETTIMG()
{
	zGfx.CurrentTexture = 0;
	zGfx.IsMultiTexture = false;

	if((wp0 >> 24) == 0xF2) {
		zGfx.CurrentTexture = 1;
		zGfx.IsMultiTexture = true;
	}

	if((wn0 >> 24) == 0xE8) {
		zTexture[zGfx.CurrentTexture].PalOffset = w1;
		return;
	}

	zTexture[zGfx.CurrentTexture].Offset = w1;
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
		float TempS0 = _FIXED2FLOAT(zVertex[Vtxs[i]].S, 16) * (zTexture[0].ScaleS * zTexture[0].ShiftScaleS) / 32.0f / _FIXED2FLOAT(zTexture[0].Width, 16);
		float TempT0 = _FIXED2FLOAT(zVertex[Vtxs[i]].T, 16) * (zTexture[0].ScaleT * zTexture[0].ShiftScaleT) / 32.0f / _FIXED2FLOAT(zTexture[0].Height, 16);

		if(zOpenGL.Ext_MultiTexture) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempS0, TempT0);
			if(zGfx.IsMultiTexture) {
				float TempS1 = _FIXED2FLOAT(zVertex[Vtxs[i]].S, 16) * (zTexture[1].ScaleS * zTexture[1].ShiftScaleS) / 32.0f / _FIXED2FLOAT(zTexture[1].Width, 16);
				float TempT1 = _FIXED2FLOAT(zVertex[Vtxs[i]].T, 16) * (zTexture[1].ScaleT * zTexture[1].ShiftScaleT) / 32.0f / _FIXED2FLOAT(zTexture[1].Height, 16);
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB, TempS1, TempT1);
			}
		} else {
			glTexCoord2f(TempS0, TempT0);
		}

		glNormal3b(zVertex[Vtxs[i]].R, zVertex[Vtxs[i]].G, zVertex[Vtxs[i]].B);
		if(!(zGfx.GeometryMode & G_LIGHTING)) glColor4ub(zVertex[Vtxs[i]].R, zVertex[Vtxs[i]].G, zVertex[Vtxs[i]].B, zVertex[Vtxs[i]].A);

		glVertex3d(zVertex[Vtxs[i]].X, zVertex[Vtxs[i]].Y, zVertex[Vtxs[i]].Z);
	}

	glEnd();
}

void dl_SetRenderMode(unsigned int Mode1, unsigned int Mode2)
{
	zGfx.OtherModeL &= 0x00000007;
	zGfx.OtherModeL |= Mode1 | Mode2;

	zGfx.ChangedModes |= CHANGED_RENDERMODE;
}

void dl_CheckFragmentCache()
{
	int CacheCheck = 0; bool SearchingCache = true; bool NewProg = false;
	while(SearchingCache) {
		if((zFragmentCache[CacheCheck].zCombiner0 == zGfx.Combiner0) && (zFragmentCache[CacheCheck].zCombiner1 == zGfx.Combiner1)) {
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
		dl_CreateCombinerProgram(zGfx.Combiner0, zGfx.Combiner1);
	} else {
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, zFragmentCache[CacheCheck].ProgramID);
	}

	if(zProgram.FragCachePosition > 256) {
		zProgram.FragCachePosition = 0;
		memset(zFragmentCache, 0x00, sizeof(zFragmentCache));
	}
}

void dl_CreateCombinerProgram(unsigned int Cmb0, unsigned int Cmb1)
{
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

	glGenProgramsARB(1, &zFragmentCache[zProgram.FragCachePosition].ProgramID);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, zFragmentCache[zProgram.FragCachePosition].ProgramID);
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(ProgramString), ProgramString);

	zFragmentCache[zProgram.FragCachePosition].zCombiner0 = Cmb0;
	zFragmentCache[zProgram.FragCachePosition].zCombiner1 = Cmb1;
	zProgram.FragCachePosition++;
}

void dl_ChangeTileSize(unsigned int Tile, unsigned int ULS, unsigned int ULT, unsigned int LRS, unsigned int LRT)
{
	zTexture[zGfx.CurrentTexture].Tile = Tile;
	zTexture[zGfx.CurrentTexture].ULS = ULS;
	zTexture[zGfx.CurrentTexture].ULT = ULT;
	zTexture[zGfx.CurrentTexture].LRS = LRS;
	zTexture[zGfx.CurrentTexture].LRT = LRT;
}

void dl_CalcTextureSize(int TextureID)
{
	unsigned int MaxTexel = 0, Line_Shift = 0;

	switch(zTexture[TextureID].Format) {
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

	unsigned int Line_Width = zTexture[TextureID].LineSize << Line_Shift;

	unsigned int Tile_Width = zTexture[TextureID].LRS - zTexture[TextureID].ULS + 1;
	unsigned int Tile_Height = zTexture[TextureID].LRT - zTexture[TextureID].ULT + 1;

	unsigned int Mask_Width = 1 << zTexture[TextureID].MaskS;
	unsigned int Mask_Height = 1 << zTexture[TextureID].MaskT;

	unsigned int Line_Height = 0;
	if(Line_Width) {
		Line_Height = Tile_Height;
		if(MaxTexel / Line_Width < Tile_Height) Line_Height = MaxTexel / Line_Width;
	}

	if(zTexture[TextureID].MaskS && ((Mask_Width * Mask_Height) <= MaxTexel)) {
		zTexture[TextureID].Width = Mask_Width;
	} else if((Tile_Width * Tile_Height) <= MaxTexel) {
		zTexture[TextureID].Width = Tile_Width;
	} else {
		zTexture[TextureID].Width = Line_Width;
	}

	if(zTexture[TextureID].MaskT && ((Mask_Width * Mask_Height) <= MaxTexel)) {
		zTexture[TextureID].Height = Mask_Height;
	} else if((Tile_Width * Tile_Height) <= MaxTexel) {
		zTexture[TextureID].Height = Tile_Height;
	} else {
		zTexture[TextureID].Height = Line_Height;
	}

	unsigned int Clamp_Width = (zTexture[TextureID].CMS & G_TX_CLAMP) ? Tile_Width : zTexture[TextureID].Width;
	unsigned int Clamp_Height = (zTexture[TextureID].CMT & G_TX_CLAMP) ? Tile_Height : zTexture[TextureID].Height;

	if(Clamp_Width > 128) zTexture[TextureID].CMS &= G_TX_MIRROR;
	if(Clamp_Height > 128) zTexture[TextureID].CMT &= G_TX_MIRROR;

	if(Mask_Width > zTexture[TextureID].Width) {
		zTexture[TextureID].MaskS = dl_PowOf(zTexture[TextureID].Width);
		Mask_Width = 1 << zTexture[TextureID].MaskS;
	}
	if(Mask_Height > zTexture[TextureID].Height) {
		zTexture[TextureID].MaskT = dl_PowOf(zTexture[TextureID].Height);
		Mask_Height = 1 << zTexture[TextureID].MaskT;
	}

	if(zTexture[TextureID].CMS & G_TX_CLAMP) {
		zTexture[TextureID].RealWidth = dl_Pow2(Clamp_Width);
	} else if(zTexture[TextureID].CMS & G_TX_MIRROR) {
		zTexture[TextureID].RealWidth = Mask_Width << 1;
	} else {
		zTexture[TextureID].RealWidth = dl_Pow2(zTexture[TextureID].Width);
	}

	if(zTexture[TextureID].CMT & G_TX_CLAMP) {
		zTexture[TextureID].RealHeight = dl_Pow2(Clamp_Height);
	} else if(zTexture[TextureID].CMT & G_TX_MIRROR) {
		zTexture[TextureID].RealHeight = Mask_Height << 1;
	} else {
		zTexture[TextureID].RealHeight = dl_Pow2(zTexture[TextureID].Height);
	}

	zTexture[TextureID].ShiftScaleS = 1.0f;
	zTexture[TextureID].ShiftScaleT = 1.0f;

	if(zTexture[TextureID].ShiftS > 10) {
		zTexture[TextureID].ShiftScaleS = (float)(1 << (16 - zTexture[TextureID].ShiftS));
	} else if(zTexture[TextureID].ShiftS > 0) {
		zTexture[TextureID].ShiftScaleS /= (float)(1 << zTexture[TextureID].ShiftS);
	}

	if(zTexture[TextureID].ShiftT > 10) {
		zTexture[TextureID].ShiftScaleT = (float)(1 << (16 - zTexture[TextureID].ShiftT));
	} else if(zTexture[TextureID].ShiftT > 0) {
		zTexture[TextureID].ShiftScaleT /= (float)(1 << zTexture[TextureID].ShiftT);
	}

	/* NULLIFYING SOME STUFF ABOVE SINCE IT'S STILL BROKEN */
	/* --------------------------------------------------- */
	zTexture[TextureID].RealWidth = dl_Pow2(zTexture[TextureID].Width);
	zTexture[TextureID].RealHeight = dl_Pow2(zTexture[TextureID].Height);
	/* --------------------------------------------------- */
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
	if(zOpenGL.Ext_FragmentProgram && !zOptions.EnableCombiner) {
		glDisable(GL_FRAGMENT_PROGRAM_ARB);
	}

	if(zOpenGL.Ext_MultiTexture) {
		if(zTexture[0].Offset != 0x00) {
			glEnable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glBindTexture(GL_TEXTURE_2D, dl_CheckTextureCache(0));
		}

		if(zGfx.IsMultiTexture && (zTexture[1].Offset != 0x00)) {
			glEnable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_2D, dl_CheckTextureCache(1));
		}

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		glActiveTextureARB(GL_TEXTURE0_ARB);
	} else {
		if(zTexture[0].Offset != 0x00) {
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
		if((zTextureCache[CacheCheck].Offset == zTexture[TexID].Offset) &&
			(zTextureCache[CacheCheck].RealWidth == zTexture[TexID].RealWidth) &&
			(zTextureCache[CacheCheck].RealHeight == zTexture[TexID].RealHeight)) {
			SearchingCache = false;
			NewTexture = false;
		} else {
			if(CacheCheck != 512) {
				CacheCheck++;
			} else {
				SearchingCache = false;
				NewTexture = true;
			}
		}
	}

	if(NewTexture) {
		GLID = dl_LoadTexture(TexID);
		zTextureCache[zProgram.TextureCachePosition].Offset = zTexture[TexID].Offset;
		zTextureCache[zProgram.TextureCachePosition].RealWidth = zTexture[TexID].RealWidth;
		zTextureCache[zProgram.TextureCachePosition].RealHeight = zTexture[TexID].RealHeight;
		zTextureCache[zProgram.TextureCachePosition].TextureID = GLID;
		zProgram.TextureCachePosition++;
	} else {
		GLID = zTextureCache[CacheCheck].TextureID;
	}

	if(zProgram.TextureCachePosition > 512) {
		int i = 0;
		static const struct __zTextureCache zTextureCache_Empty;
		for(i = 0; i < 512; i++) zTextureCache[i] = zTextureCache_Empty;
		zProgram.TextureCachePosition = 0;
	}

	return GLID;
}

GLuint dl_LoadTexture(int TextureID)
{
	unsigned char TexSegment = (zTexture[TextureID].Offset & 0xFF000000) >> 24;
	unsigned int TexOffset = (zTexture[TextureID].Offset & 0x00FFFFFF);

	if((wn0 == 0x03000000) || (wn0 == 0xE1000000)) return EXIT_SUCCESS;

	dl_CalcTextureSize(TextureID);

	int i = 0, j = 0;

	unsigned int BytesPerPixel = 0x08;
	switch(zTexture[TextureID].Format) {
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

	unsigned int BufferSize = (zTexture[TextureID].RealHeight * zTexture[TextureID].RealWidth) * BytesPerPixel;
	unsigned char * TextureData = (unsigned char *) malloc (BufferSize);

	memset(TextureData, 0xFF, BufferSize);

	unsigned int GLTexPosition = 0;

	if(!zl_CheckAddressValidity(zTexture[TextureID].Offset) || (!zOptions.EnableTextures)) {
		memset(TextureData, 0xFF, BufferSize);
	} else {
/*		char Log_PalOffset[256];
		sprintf(Log_PalOffset, "- CI palette offset: 0x%08X\n", (unsigned int)zTexture[TextureID].PalOffset);
		char Log_PalNo[256];
		sprintf(Log_PalNo, "- Palette #%d\n", zTexture[TextureID].Palette);
		char Log_YParam[256];
		sprintf(Log_YParam, "- CMS: %s%s%s\n",
			((zTexture[TextureID].CMS & G_TX_WRAP) ? "G_TX_WRAP | " : ""),
			((zTexture[TextureID].CMS & G_TX_MIRROR) ? "G_TX_MIRROR | " : ""),
			((zTexture[TextureID].CMS & G_TX_CLAMP) ? "G_TX_CLAMP | " : ""));
		char Log_XParam[256];
		sprintf(Log_XParam, "- CMT: %s%s%s\n",
			((zTexture[TextureID].CMT & G_TX_WRAP) ? "G_TX_WRAP" : ""),
			((zTexture[TextureID].CMT & G_TX_MIRROR) ? "G_TX_MIRROR" : ""),
			((zTexture[TextureID].CMT & G_TX_CLAMP) ? "G_TX_CLAMP" : ""));

		bool IsCITex = false;
		if(zTexture[TextureID].PalOffset != 0) IsCITex = true;

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
				(unsigned int)zTexture[TextureID].Offset,
				(IsCITex ? Log_PalOffset : ""),
				zTexture[TextureID].Width, zTexture[TextureID].Height,
				"", zTexture[TextureID].Format,
				Log_YParam,
				Log_XParam,
				zTexture[TextureID].ScaleS, zTexture[TextureID].ScaleT,
				zTexture[TextureID].ShiftScaleS, zTexture[TextureID].ShiftScaleT,
				zTexture[TextureID].MaskS, zTexture[TextureID].MaskT,
				zTexture[TextureID].LineSize,
				(IsCITex ? Log_PalNo : "")
		);
*/
		switch(zTexture[TextureID].Format) {
			case 0x00:
			case 0x08:
			case 0x10: {
				unsigned short Raw;
				unsigned int RGBA = 0;

				for(j = 0; j < zTexture[TextureID].Height; j++) {
					for(i = 0; i < zTexture[TextureID].Width; i++) {
						Raw = (zRAM[TexSegment].Data[TexOffset] << 8) | zRAM[TexSegment].Data[TexOffset + 1];

						RGBA = ((Raw & 0xF800) >> 8) << 24;
						RGBA |= (((Raw & 0x07C0) << 5) >> 8) << 16;
						RGBA |= (((Raw & 0x003E) << 18) >> 16) << 8;
						if((Raw & 0x0001)) RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition, RGBA);

						TexOffset += 2;
						GLTexPosition += 4;

						if(TexOffset > zRAM[TexSegment].Size) break;
					}
					TexOffset += zTexture[TextureID].LineSize * 4 - zTexture[TextureID].Width;
				}
				break; }

			case 0x18: {
				memcpy(TextureData, &zRAM[TexSegment].Data[TexOffset], (zTexture[TextureID].Height * zTexture[TextureID].Width * 4));
				break; }

			case 0x40:
			case 0x50: {
				unsigned int CI1, CI2;
				unsigned int RGBA = 0;

				for(j = 0; j < zTexture[TextureID].Height; j++) {
					for(i = 0; i < zTexture[TextureID].Width / 2; i++) {
						CI1 = (zRAM[TexSegment].Data[TexOffset] & 0xF0) >> 4;
						CI2 = (zRAM[TexSegment].Data[TexOffset] & 0x0F);

						RGBA = (zPalette[CI1].R << 24);
						RGBA |= (zPalette[CI1].G << 16);
						RGBA |= (zPalette[CI1].B << 8);
						RGBA |= zPalette[CI1].A;
						Write32(TextureData, GLTexPosition, RGBA);

						RGBA = (zPalette[CI2].R << 24);
						RGBA |= (zPalette[CI2].G << 16);
						RGBA |= (zPalette[CI2].B << 8);
						RGBA |= zPalette[CI2].A;
						Write32(TextureData, GLTexPosition + 4, RGBA);

						TexOffset += 1;
						GLTexPosition += 8;
					}
					TexOffset += zTexture[TextureID].LineSize * 8 - (zTexture[TextureID].Width / 2);
				}
				break; }

			case 0x48: {
				unsigned short Raw;
				unsigned int RGBA = 0;

				for(j = 0; j < zTexture[TextureID].Height; j++) {
					for(i = 0; i < zTexture[TextureID].Width; i++) {
						Raw = zRAM[TexSegment].Data[TexOffset];

						RGBA = (zPalette[Raw].R << 24);
						RGBA |= (zPalette[Raw].G << 16);
						RGBA |= (zPalette[Raw].B << 8);
						RGBA |= zPalette[Raw].A;
						Write32(TextureData, GLTexPosition, RGBA);

						TexOffset += 1;
						GLTexPosition += 4;
					}
					TexOffset += zTexture[TextureID].LineSize * 8 - zTexture[TextureID].Width;
				}
				break; }

			case 0x60: {
				unsigned short Raw;
				unsigned int RGBA = 0;

				for(j = 0; j < zTexture[TextureID].Height; j++) {
					for(i = 0; i < zTexture[TextureID].Width / 2; i++) {
						Raw = (zRAM[TexSegment].Data[TexOffset] & 0xF0) >> 4;
						RGBA = (((Raw & 0x0E) << 4) << 24);
						RGBA |= (((Raw & 0x0E) << 4) << 16);
						RGBA |= (((Raw & 0x0E) << 4) << 8);
						if((Raw & 0x01)) RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition, RGBA);

						Raw = (zRAM[TexSegment].Data[TexOffset] & 0x0F);
						RGBA = (((Raw & 0x0E) << 4) << 24);
						RGBA |= (((Raw & 0x0E) << 4) << 16);
						RGBA |= (((Raw & 0x0E) << 4) << 8);
						if((Raw & 0x01)) RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition + 4, RGBA);

						TexOffset += 1;
						GLTexPosition += 8;
					}
					TexOffset += zTexture[TextureID].LineSize * 8 - (zTexture[TextureID].Width / 2);
				}
				break; }

			case 0x68: {
				unsigned short Raw;
				unsigned int RGBA = 0;

				for(j = 0; j < zTexture[TextureID].Height; j++) {
					for(i = 0; i < zTexture[TextureID].Width; i++) {
						Raw = zRAM[TexSegment].Data[TexOffset];
						RGBA = (((Raw & 0xF0) + 0x0F) << 24);
						RGBA |= (((Raw & 0xF0) + 0x0F) << 16);
						RGBA |= (((Raw & 0xF0) + 0x0F) << 8);
						RGBA |= ((Raw & 0x0F) << 4);
						Write32(TextureData, GLTexPosition, RGBA);

						TexOffset += 1;
						GLTexPosition += 4;
					}
					TexOffset += zTexture[TextureID].LineSize * 8 - zTexture[TextureID].Width;
				}
				break; }

			case 0x70: {
				for(j = 0; j < zTexture[TextureID].Height; j++) {
					for(i = 0; i < zTexture[TextureID].Width; i++) {
						TextureData[GLTexPosition]     = zRAM[TexSegment].Data[TexOffset];
						TextureData[GLTexPosition + 1] = zRAM[TexSegment].Data[TexOffset];
						TextureData[GLTexPosition + 2] = zRAM[TexSegment].Data[TexOffset];
						TextureData[GLTexPosition + 3] = zRAM[TexSegment].Data[TexOffset + 1];

						TexOffset += 2;
						GLTexPosition += 4;
					}
					TexOffset += zTexture[TextureID].LineSize * 4 - zTexture[TextureID].Width;
				}
				break; }

			case 0x80:
			case 0x90: {
				unsigned short Raw;
				unsigned int RGBA = 0;

				for(j = 0; j < zTexture[TextureID].Height; j++) {
					for(i = 0; i < zTexture[TextureID].Width / 2; i++) {
						Raw = (zRAM[TexSegment].Data[TexOffset] & 0xF0) >> 4;
						RGBA = (((Raw & 0x0F) << 4) << 24);
						RGBA |= (((Raw & 0x0F) << 4) << 16);
						RGBA |= (((Raw & 0x0F) << 4) << 8);
						RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition, RGBA);

						Raw = (zRAM[TexSegment].Data[TexOffset] & 0x0F);
						RGBA = (((Raw & 0x0F) << 4) << 24);
						RGBA |= (((Raw & 0x0F) << 4) << 16);
						RGBA |= (((Raw & 0x0F) << 4) << 8);
						RGBA |= 0xFF;
						Write32(TextureData, GLTexPosition + 4, RGBA);

						TexOffset += 1;
						GLTexPosition += 8;
					}
					TexOffset += zTexture[TextureID].LineSize * 8 - (zTexture[TextureID].Width / 2);
				}
				break; }

			case 0x88: {
				for(j = 0; j < zTexture[TextureID].Height; j++) {
					for(i = 0; i < zTexture[TextureID].Width; i++) {
						TextureData[GLTexPosition]     = zRAM[TexSegment].Data[TexOffset];
						TextureData[GLTexPosition + 1] = zRAM[TexSegment].Data[TexOffset];
						TextureData[GLTexPosition + 2] = zRAM[TexSegment].Data[TexOffset];
						TextureData[GLTexPosition + 3] = 0xFF;

						TexOffset += 1;
						GLTexPosition += 4;
					}
					TexOffset += zTexture[TextureID].LineSize * 8 - zTexture[TextureID].Width;
				}
				break; }

			default:
				memset(TextureData, 0xFF, BufferSize);
				break;
		}
	}

	md_CreateMaterial(TextureID, TextureData);

	glBindTexture(GL_TEXTURE_2D, zGfx.GLTextureID[zGfx.GLTextureCount]);

	if(zTexture[TextureID].CMT & G_TX_CLAMP) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); }
	if(zTexture[TextureID].CMT & G_TX_WRAP) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); }
	if(zTexture[TextureID].CMT & G_TX_MIRROR) { if(zOpenGL.Ext_TexMirroredRepeat) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB); }

	if(zTexture[TextureID].CMS & G_TX_CLAMP) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); }
	if(zTexture[TextureID].CMS & G_TX_WRAP) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); }
	if(zTexture[TextureID].CMS & G_TX_MIRROR) { if(zOpenGL.Ext_TexMirroredRepeat) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB); }

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, zTexture[TextureID].RealWidth, zTexture[TextureID].RealHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	free(TextureData);

	return zGfx.GLTextureID[zGfx.GLTextureCount++];
}
