#include "globals.h"

// ----------------------------------------

RDPInstruction dl_UcodeCmd[256];
CheckAddressValidity dl_CheckAddressValidity;

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

// ----------------------------------------

void dl_InitParser(int UcodeID)
{
	dl_CheckAddressValidity = zl_CheckAddressValidity;

	int i = 0;
	for(i = 0; i < 256; i++) {
		dl_UcodeCmd[i] = dl_UnemulatedCmd;
	}

	dl_UcodeCmd[G_SETCIMG]         = dl_G_SETCIMG;
	dl_UcodeCmd[G_SETZIMG]         = dl_G_SETZIMG;
	dl_UcodeCmd[G_SETTIMG]         = dl_G_SETTIMG;
	dl_UcodeCmd[G_SETCOMBINE]      = dl_G_SETCOMBINE;
	dl_UcodeCmd[G_SETENVCOLOR]     = dl_G_SETENVCOLOR;
	dl_UcodeCmd[G_SETPRIMCOLOR]    = dl_G_SETPRIMCOLOR;
	dl_UcodeCmd[G_SETBLENDCOLOR]   = dl_G_SETBLENDCOLOR;
	dl_UcodeCmd[G_SETFOGCOLOR]     = dl_G_SETFOGCOLOR;
	dl_UcodeCmd[G_SETFILLCOLOR]    = dl_G_SETFILLCOLOR;
	dl_UcodeCmd[G_FILLRECT]        = dl_G_FILLRECT;
	dl_UcodeCmd[G_SETTILE]         = dl_G_SETTILE;
	dl_UcodeCmd[G_LOADTILE]        = dl_G_LOADTILE;
	dl_UcodeCmd[G_LOADBLOCK]       = dl_G_LOADBLOCK;
	dl_UcodeCmd[G_SETTILESIZE]     = dl_G_SETTILESIZE;
	dl_UcodeCmd[G_LOADTLUT]        = dl_G_LOADTLUT;
	dl_UcodeCmd[G_RDPSETOTHERMODE] = dl_G_RDPSETOTHERMODE;
	dl_UcodeCmd[G_SETPRIMDEPTH]    = dl_G_SETPRIMDEPTH;
	dl_UcodeCmd[G_SETSCISSOR]      = dl_G_SETSCISSOR;
	dl_UcodeCmd[G_SETCONVERT]      = dl_G_SETCONVERT;
	dl_UcodeCmd[G_SETKEYR]         = dl_G_SETKEYR;
	dl_UcodeCmd[G_SETKEYGB]        = dl_G_SETKEYGB;
	dl_UcodeCmd[G_RDPFULLSYNC]     = dl_G_RDPFULLSYNC;
	dl_UcodeCmd[G_RDPTILESYNC]     = dl_G_RDPTILESYNC;
	dl_UcodeCmd[G_RDPPIPESYNC]     = dl_G_RDPPIPESYNC;
	dl_UcodeCmd[G_RDPLOADSYNC]     = dl_G_RDPLOADSYNC;
	dl_UcodeCmd[G_TEXRECTFLIP]     = dl_G_TEXRECTFLIP;
	dl_UcodeCmd[G_TEXRECT]         = dl_G_TEXRECT;

	switch(UcodeID) {
		case F3DEX2:	dl_F3DEX2_Init(); break;
		default:		break;
	}
}

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
	if(!dl_CheckAddressValidity(Address)) return;

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

void dl_DrawTriangle(int Vtxs[])
{
	glBegin(GL_TRIANGLES);

	int i = 0;
	for(i = 0; i < 3; i++) {
		float TempS0 = _FIXED2FLOAT(zVertex[Vtxs[i]].S, 16) * (zTexture[0].ScaleS * zTexture[0].ShiftScaleS) / 32.0f / _FIXED2FLOAT(zTexture[0].RealWidth, 16);
		float TempT0 = _FIXED2FLOAT(zVertex[Vtxs[i]].T, 16) * (zTexture[0].ScaleT * zTexture[0].ShiftScaleT) / 32.0f / _FIXED2FLOAT(zTexture[0].RealHeight, 16);

		if(zOpenGL.Ext_MultiTexture) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, TempS0, TempT0);
			if(zGfx.IsMultiTexture) {
				float TempS1 = _FIXED2FLOAT(zVertex[Vtxs[i]].S, 16) * (zTexture[1].ScaleS * zTexture[1].ShiftScaleS) / 32.0f / _FIXED2FLOAT(zTexture[1].RealWidth, 16);
				float TempT1 = _FIXED2FLOAT(zVertex[Vtxs[i]].T, 16) * (zTexture[1].ScaleT * zTexture[1].ShiftScaleT) / 32.0f / _FIXED2FLOAT(zTexture[1].RealHeight, 16);
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
		dl_CreateCombinerProgram(zGfx.Combiner0, zGfx.Combiner1);
	} else {
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, zFragmentCache[CacheCheck].ProgramID);
	}

	if(zProgram.FragCachePosition > CACHE_FRAGMENT) {
		int i = 0;
		static const struct __zFragmentCache zFragmentCache_Empty;
		for(i = 0; i < CACHE_FRAGMENT; i++) zFragmentCache[i] = zFragmentCache_Empty;
		zProgram.FragCachePosition = 0;
	}
}

void dl_CreateCombinerProgram(unsigned int Cmb0, unsigned int Cmb1)
{
	if(!zOpenGL.Ext_FragmentProgram) return;

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
	if(Line_Width > 0) Line_Height = min(MaxTexel / Line_Width, Tile_Height);

	if((zTexture[TextureID].MaskS > 0) && ((Mask_Width * Mask_Height) <= MaxTexel)) {
		zTexture[TextureID].Width = Mask_Width;
	} else if((Tile_Width * Tile_Height) <= MaxTexel) {
		zTexture[TextureID].Width = Tile_Width;
	} else {
		zTexture[TextureID].Width = Line_Width;
	}

	if((zTexture[TextureID].MaskT > 0) && ((Mask_Width * Mask_Height) <= MaxTexel)) {
		zTexture[TextureID].Height = Mask_Height;
	} else if((Tile_Width * Tile_Height) <= MaxTexel) {
		zTexture[TextureID].Height = Tile_Height;
	} else {
		zTexture[TextureID].Height = Line_Height;
	}

	unsigned int Clamp_Width = 0;
	unsigned int Clamp_Height = 0;

	if((zTexture[TextureID].CMS & G_TX_CLAMP) && (!zTexture[TextureID].CMS & G_TX_MIRROR)) {
		Clamp_Width = Tile_Width;
	} else {
		Clamp_Width = zTexture[TextureID].Width;
	}
	if((zTexture[TextureID].CMT & G_TX_CLAMP) && (!zTexture[TextureID].CMT & G_TX_MIRROR)) {
		Clamp_Height = Tile_Height;
	} else {
		Clamp_Height = zTexture[TextureID].Height;
	}

	if(Mask_Width > zTexture[TextureID].Width) {
		zTexture[TextureID].MaskS = dl_PowOf(zTexture[TextureID].Width);
		Mask_Width = 1 << zTexture[TextureID].MaskS;
	}
	if(Mask_Height > zTexture[TextureID].Height) {
		zTexture[TextureID].MaskT = dl_PowOf(zTexture[TextureID].Height);
		Mask_Height = 1 << zTexture[TextureID].MaskT;
	}

	if((zTexture[TextureID].CMS == 2) || (zTexture[TextureID].CMS == 3)) {
		zTexture[TextureID].RealWidth = dl_Pow2(Clamp_Width);
	} else if(zTexture[TextureID].CMS == 1) {
		zTexture[TextureID].RealWidth = dl_Pow2(Mask_Width);
	} else {
		zTexture[TextureID].RealWidth = dl_Pow2(zTexture[TextureID].Width);
	}

	if((zTexture[TextureID].CMT == 2) || (zTexture[TextureID].CMT == 3)) {
		zTexture[TextureID].RealHeight = dl_Pow2(Clamp_Height);
	} else if(zTexture[TextureID].CMT == 1) {
		zTexture[TextureID].RealHeight = dl_Pow2(Mask_Height);
	} else {
		zTexture[TextureID].RealHeight = dl_Pow2(zTexture[TextureID].Height);
	}

	zTexture[TextureID].ShiftScaleS = 1.0f;
	zTexture[TextureID].ShiftScaleT = 1.0f;

	if(zTexture[TextureID].ShiftS > 10) {
		zTexture[TextureID].ShiftScaleS = (1 << (16 - zTexture[TextureID].ShiftS));
	} else if(zTexture[TextureID].ShiftS > 0) {
		zTexture[TextureID].ShiftScaleS /= (1 << zTexture[TextureID].ShiftS);
	}

	if(zTexture[TextureID].ShiftT > 10) {
		zTexture[TextureID].ShiftScaleT = (1 << (16 - zTexture[TextureID].ShiftT));
	} else if(zTexture[TextureID].ShiftT > 0) {
		zTexture[TextureID].ShiftScaleT /= (1 << zTexture[TextureID].ShiftT);
	}
}

inline unsigned long dl_Pow2(unsigned long dim)
{
	unsigned long i = 1;

	while (i < dim) i <<= 1;

	return i;
}

inline unsigned long dl_PowOf(unsigned long dim)
{
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
			if(CacheCheck != CACHE_TEXTURES) {
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

	if(zProgram.TextureCachePosition > CACHE_TEXTURES) {
		int i = 0;
		static const struct __zTextureCache zTextureCache_Empty;
		for(i = 0; i < CACHE_TEXTURES; i++) zTextureCache[i] = zTextureCache_Empty;
		zProgram.TextureCachePosition = 0;
	}

	return GLID;
}

GLuint dl_LoadTexture(int TextureID)
{
	unsigned char TexSegment = (zTexture[TextureID].Offset & 0xFF000000) >> 24;
	unsigned int TexOffset = (zTexture[TextureID].Offset & 0x00FFFFFF);

//	if((wn0 == 0x03000000) || (wn0 == 0xE1000000)) return EXIT_SUCCESS;

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

	if(!dl_CheckAddressValidity(zTexture[TextureID].Offset) || (!zOptions.EnableTextures)) {
		while(i < BufferSize) { TextureData[i++] = 0xFF; TextureData[i++] = 0x00; TextureData[i++] = 0x00; TextureData[i++] = 0xFF; }
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, zTexture[TextureID].RealWidth, zTexture[TextureID].RealHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureData);

	switch(zTexture[TextureID].CMS) {
		case G_TX_CLAMP:
		case 3:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			break;
		case G_TX_MIRROR:
			if(zOpenGL.Ext_TexMirroredRepeat) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT_ARB);
			break;
		case G_TX_WRAP:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			break;
		default:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			break;
	}

	switch(zTexture[TextureID].CMT) {
		case G_TX_CLAMP:
		case 3:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			break;
		case G_TX_MIRROR:
			if(zOpenGL.Ext_TexMirroredRepeat) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT_ARB);
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

	return zGfx.GLTextureID[zGfx.GLTextureCount++];
}
