#include "globals.h"

const char * CombinerTypes32[32] = {
	"COMBINED        ", "TEXEL0          ",
	"TEXEL1          ", "PRIMITIVE       ",
	"SHADE           ", "ENVIRONMENT     ",
	"1               ", "COMBINED_ALPHA  ",
	"TEXEL0_ALPHA    ", "TEXEL1_ALPHA    ",
	"PRIMITIVE_ALPHA ", "SHADE_ALPHA     ",
	"ENV_ALPHA       ", "LOCFRAC         ",
	"PRIMLOCFRAC     ", "K5              ",
	"<unknown>       ", "<unknown>       ",
	"<unknown>       ", "<unknown>       ",
	"<unknown>       ", "<unknown>       ",
	"<unknown>       ", "<unknown>       ",
	"<unknown>       ", "<unknown>       ",
	"<unknown>       ", "<unknown>       ",
	"<unknown>       ", "<unknown>       ",
	"<unknown>       ", "0               "
};

const char * CombinerTypes16[16] = {
	"COMBINED        ", "TEXEL0          ",
	"TEXEL1          ", "PRIMITIVE       ",
	"SHADE           ", "ENVIRONMENT     ",
	"1               ", "COMBINED_ALPHA  ",
	"TEXEL0_ALPHA    ", "TEXEL1_ALPHA    ",
	"PRIMITIVE_ALPHA ", "SHADE_ALPHA     ",
	"ENV_ALPHA       ", "LOCFRAC         ",
	"PRIMLOCFRAC     ", "0               "
};

const char * CombinerTypes8[8] = {
	"COMBINED        ", "TEXEL0          ",
	"TEXEL1          ", "PRIMITIVE       ",
	"SHADE           ", "ENVIRONMENT     ",
	"1               ", "0               "
};

void RDP_CheckFragmentCache()
{
	int CacheCheck = 0; bool SearchingCache = true; bool NewProg = false;
	while(SearchingCache) {
		if((FragmentCache[CacheCheck].Combiner0 == Gfx.Combiner0) && (FragmentCache[CacheCheck].Combiner1 == Gfx.Combiner1)) {
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
		static const __FragmentCache FragmentCache_Empty = { 0, 0, -1 };
		for(i = 0; i < ArraySize(FragmentCache); i++) {
			if(OpenGL.Ext_FragmentProgram) glDeleteProgramsARB(1, &FragmentCache[i].ProgramID);
			FragmentCache[i] = FragmentCache_Empty;
		}
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
		"PARAM PrimColorLOD = program.env[2];\n"
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

	FragmentCache[System.FragCachePosition].Combiner0 = Cmb0;
	FragmentCache[System.FragCachePosition].Combiner1 = Cmb1;
	System.FragCachePosition++;

	#if 0
	char temp[MAX_PATH];
	sprintf(temp, "combiner_%08X_%08X.txt", Cmb0, Cmb1);
	FILE *fp = fopen(temp, "w");

	char CombMsg[1024];
	sprintf(CombMsg, "----------------------------------------------------------------------------------------\n"
		"Color 0 = [(%s - %s) * %s] + %s\n"
		"Alpha 0 = [(%s - %s) * %s] + %s\n"
		"Color 1 = [(%s - %s) * %s] + %s\n"
		"Alpha 1 = [(%s - %s) * %s] + %s\n"
		"----------------------------------------------------------------------------------------\n",
		CombinerTypes16[cA[0]], CombinerTypes16[cB[0]], CombinerTypes32[cC[0]], CombinerTypes8[cD[0]],
		CombinerTypes8[aA[0]], CombinerTypes8[aB[0]], CombinerTypes8[aC[0]], CombinerTypes8[aD[0]],
		CombinerTypes16[cA[1]], CombinerTypes16[cB[1]], CombinerTypes32[cC[1]], CombinerTypes8[cD[1]],
		CombinerTypes8[aA[1]], CombinerTypes8[aB[1]], CombinerTypes8[aC[1]], CombinerTypes8[aD[1]]);
	fprintf(fp, CombMsg);

	fprintf(fp, ProgramString);

	fclose(fp);
	#endif
}
