/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	uc_comb.c - F3DEX2 coloring & combiner functions
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

int F3DEX2_Cmd_SETCOMBINE()
{
    unsigned int COMBINE0 =	        (Readout_CurrentByte2 * 0x10000) +
									(Readout_CurrentByte3 * 0x100) +
									(Readout_CurrentByte4);

    unsigned int COMBINE1  =	    (Readout_CurrentByte5 * 0x1000000) +
									(Readout_CurrentByte6 * 0x10000) +
									(Readout_CurrentByte7 * 0x100) +
									 Readout_CurrentByte8;
    cA0=(COMBINE0>>20)&0xF;
    cB0=(COMBINE1>>28)&0xF;
    cC0=(COMBINE0>>15)&0x1F;
    cD0=(COMBINE1>>15)&0x7;

    aA0=(COMBINE0>>12)&0x7;
    aB0=(COMBINE1>>12)&0x7;
    aC0=(COMBINE0>>9)&0x7;
    aD0=(COMBINE1>>9)&0x7;

    cA1=(COMBINE0>>5)&0xF;
    cB1=(COMBINE1>>24)&0xF;
    cC1=(COMBINE0>>0)&0x1F;
    cD1=(COMBINE1>>6)&0x7;

    aA1=(COMBINE1>>21)&0x7;
    aB1=(COMBINE1>>3)&0x7;
    aC1=(COMBINE1>>18)&0x7;
    aD1=(COMBINE1>>0)&0x7;

	F3DEX2_BuildFragmentShader();

    return 0;
}

int F3DEX2_BuildFragmentShader()
{
	char ShaderString[8192];
	sprintf(ShaderString,"!!ARBfp1.0\n");
	sprintf(ShaderString,"%sTEMP R0;\n",ShaderString);
    sprintf(ShaderString,"%sTEMP R1;\n",ShaderString);
    sprintf(ShaderString,"%sTEMP aR0;\n",ShaderString);
    sprintf(ShaderString,"%sTEMP aR1;\n",ShaderString);
    sprintf(ShaderString,"%sPARAM envcolor = program.env[0];\n",ShaderString);
    sprintf(ShaderString,"%sPARAM primcolor = program.env[1];\n",ShaderString);
    switch(cA0)
    {
        case 1: // cTEXEL0
            sprintf(ShaderString,"%sTEX R0, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // cTEXEL1
            sprintf(ShaderString,"%sTEX R0, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // cPRIM
            sprintf(ShaderString,"%sMOV R0, primcolor;\n",ShaderString);
/* !!! */	HACK_UseColorSource = 0;
            break;
        case 4: // cSHADE
            sprintf(ShaderString,"%sMOV R0, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // cENV
            sprintf(ShaderString,"%sMOV R0, envcolor;\n",ShaderString);
/* !!! */	HACK_UseColorSource = 1;
            break;
        case 6: // 1.0
            sprintf(ShaderString,"%sMOV R0, {1.0,1.0,1.0,1.0};\n",ShaderString);
/* !!! */	HACK_UseColorSource = 2;
            break;
        default:
            sprintf(ShaderString,"%sMOV R0, {0,0,0,0};\n",ShaderString);
            break;
    }
    switch(cB0)
    {
        case 1: // cTEXEL0
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // cTEXEL1
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // cPRIM
            sprintf(ShaderString,"%sMOV R1, primcolor;\n",ShaderString);
            break;
        case 4: // cSHADE
            sprintf(ShaderString,"%sMOV R1, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // cENV
            sprintf(ShaderString,"%sMOV R1, envcolor;\n",ShaderString);
            break;
        default:
            sprintf(ShaderString,"%sMOV R1, {0,0,0,0};\n",ShaderString);
            break;
    }
    sprintf(ShaderString,"%sSUB R0, R0, R1;\n",ShaderString);
    switch(cC0)
    {
        case 1: // cTEXEL0
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // cTEXEL1
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // cPRIM
            sprintf(ShaderString,"%sMOV R1, primcolor;\n",ShaderString);
            break;
        case 4: // cSHADE
            sprintf(ShaderString,"%sMOV R1, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // cENV
            sprintf(ShaderString,"%sMOV R1, envcolor;\n",ShaderString);
            break;
        case 8: // aTEXEL0
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            sprintf(ShaderString,"%sMOV R1, R1.aaaa;\n",ShaderString);
            break;
        case 9: // aTEXEL1
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            sprintf(ShaderString,"%sMOV R1, R1.aaaa;\n",ShaderString);
            break;
        case 10: // aPRIM
            sprintf(ShaderString,"%sMOV R1, primcolor;\n",ShaderString);
            sprintf(ShaderString,"%sMOV R1, R1.aaaa;\n",ShaderString);
            break;
        case 11: // aSHADE
            sprintf(ShaderString,"%sMOV R1, fragment.color.primary;\n",ShaderString);
            sprintf(ShaderString,"%sMOV R1, R1.aaaa;\n",ShaderString);
            break;
        case 12: // aENV
            sprintf(ShaderString,"%sMOV R1, envcolor;\n",ShaderString);
            sprintf(ShaderString,"%sMOV R1, R1.aaaa;\n",ShaderString);
            break;
        default:
            sprintf(ShaderString,"%sMOV R1, {0,0,0,0};\n",ShaderString);
            break;

    }
    sprintf(ShaderString,"%sMUL R0, R0, R1;\n",ShaderString);
    switch(cD0)
    {
        case 1: // cTEXEL0
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // cTEXEL1
            sprintf(ShaderString,"%sTEX R1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // cPRIM
            sprintf(ShaderString,"%sMOV R1, primcolor;\n",ShaderString);
            break;
        case 4: // cSHADE
            sprintf(ShaderString,"%sMOV R1, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // cENV
            sprintf(ShaderString,"%sMOV R1, envcolor;\n",ShaderString);
            break;
        case 6: // 1.0
            sprintf(ShaderString,"%sMOV R1, {1.0,1.0,1.0,1.0};\n",ShaderString);
            break;
        default:
            sprintf(ShaderString,"%sMOV R1, {0,0,0,0};\n",ShaderString);
            break;
    }
    sprintf(ShaderString,"%sADD R0, R0, R1;\n",ShaderString);

    switch(aA0)
    {
        case 1: // aTEXEL0
            sprintf(ShaderString,"%sTEX aR0, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // aTEXEL1
            sprintf(ShaderString,"%sTEX aR0, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // aPRIM
            sprintf(ShaderString,"%sMOV aR0, primcolor;\n",ShaderString);
/* !!! */	HACK_UseAlphaSource = 0;
            break;
        case 4: // aSHADE
            sprintf(ShaderString,"%sMOV aR0, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // aENV
            sprintf(ShaderString,"%sMOV aR0, envcolor;\n",ShaderString);
/* !!! */	HACK_UseAlphaSource = 1;
            break;
        case 6: // 1.0
            sprintf(ShaderString,"%sMOV aR0, {1.0,1.0,1.0,1.0};\n",ShaderString);
/* !!! */	HACK_UseAlphaSource = 2;
            break;
        default:
            sprintf(ShaderString,"%sMOV aR0, {0,0,0,0};\n",ShaderString);
            break;
    }

    switch(aB0)
    {
        case 1: // aTEXEL0
            sprintf(ShaderString,"%sTEX aR1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // aTEXEL1
            sprintf(ShaderString,"%sTEX aR1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // aPRIM
            sprintf(ShaderString,"%sMOV aR1, primcolor;\n",ShaderString);
            break;
        case 4: // aSHADE
            sprintf(ShaderString,"%sMOV aR1, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // aENV
            sprintf(ShaderString,"%sMOV aR1, envcolor;\n",ShaderString);
            break;
        case 6: // 1.0
            sprintf(ShaderString,"%sMOV aR1, {1.0,1.0,1.0,1.0};\n",ShaderString);
            break;
        default:
            sprintf(ShaderString,"%sMOV aR1, {0,0,0,0};\n",ShaderString);
            break;
    }
    sprintf(ShaderString,"%sSUB aR0, aR0, aR1;\n",ShaderString);
    switch(aC0)
    {
        case 1: // aTEXEL0
            sprintf(ShaderString,"%sTEX aR1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // aTEXEL1
            sprintf(ShaderString,"%sTEX aR1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // aPRIM
            sprintf(ShaderString,"%sMOV aR1, primcolor;\n",ShaderString);
            break;
        case 4: // aSHADE
            sprintf(ShaderString,"%sMOV aR1, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // aENV
            sprintf(ShaderString,"%sMOV aR1, envcolor;\n",ShaderString);
            break;
        default:
            sprintf(ShaderString,"%sMOV aR1, {0,0,0,0};\n",ShaderString);
            break;
    }
    sprintf(ShaderString,"%sMUL aR0, aR0, aR1;\n",ShaderString);
    switch(aD0)
    {
        case 1: // aTEXEL0
            sprintf(ShaderString,"%sTEX aR1, fragment.texcoord[0], texture[0], 2D;\n",ShaderString);
            break;
        case 2: // aTEXEL1
            sprintf(ShaderString,"%sTEX aR1, fragment.texcoord[0], texture[1], 2D;\n",ShaderString);
            break;
        case 3: // aPRIM
            sprintf(ShaderString,"%sMOV aR1, primcolor;\n",ShaderString);
            break;
        case 4: // aSHADE
            sprintf(ShaderString,"%sMOV aR1, fragment.color.primary;\n",ShaderString);
            break;
        case 5: // aENV
            sprintf(ShaderString,"%sMOV aR1, envcolor;\n",ShaderString);
            break;
        case 6: // 1.0
            sprintf(ShaderString,"%sMOV aR1, {1.0,1.0,1.0,1.0};\n",ShaderString);
            break;
        default:
            sprintf(ShaderString,"%sMOV aR1, {0,0,0,0};\n",ShaderString);
            break;
    }
    sprintf(ShaderString,"%sADD aR0, aR0, aR1;\n",ShaderString);
    sprintf(ShaderString,"%sMOV R0.a, aR0.a;\n",ShaderString);
    sprintf(ShaderString,"%sMOV result.color, R0;\n",ShaderString);
    sprintf(ShaderString,"%sEND\n",ShaderString);

	/*sprintf(ShaderString, "!!ARBfp1.0\n" \
		"\tTEMP color;\n" \
		"\tMUL color, fragment.texcoord[0].y, 2.0;\n" \
		"\tADD color, 1.0, -color;\n" \
		"\tABS color, color;\n" \
		"\tADD result.color, 1.0, -color;\n" \
		"\tMOV result.color.a, 1.0;\n" \
		"END\n");
*/
	if(GLExtension_FragmentProgram) {
//		sprintf(ErrorMsg, "\nFRAGMENT SHADER!\n%s\n", ShaderString);
//		Helper_LogMessage(2, ErrorMsg);

//		glEnable(GL_FRAGMENT_PROGRAM_ARB);
		glGenProgramsARB(1, &fragProg);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fragProg);
		glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(ShaderString), ShaderString);
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, fragProg);
	}

    return 0;
}

int F3DEX2_Cmd_SETFOGCOLOR()
{
	FogColor[0] = (Readout_CurrentByte5 / 255.0f);
	FogColor[1] = (Readout_CurrentByte6 / 255.0f);
	FogColor[2] = (Readout_CurrentByte7 / 255.0f);
	FogColor[3] = (Readout_CurrentByte8 / 255.0f);

	glFogfv(GL_FOG_COLOR, FogColor);
	glClearColor(FogColor[0], FogColor[1], FogColor[2], FogColor[3]);

	return 0;
}

int F3DEX2_Cmd_SETPRIMCOLOR()
{
	PrimColor[0] = (Readout_CurrentByte5 / 255.0f);
	PrimColor[1] = (Readout_CurrentByte6 / 255.0f);
	PrimColor[2] = (Readout_CurrentByte7 / 255.0f);
	PrimColor[3] = (Readout_CurrentByte8 / 255.0f);

	if(GLExtension_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);
	}

	return 0;
}

int F3DEX2_Cmd_SETENVCOLOR()
{
    EnvColor[0] = (Readout_CurrentByte5 / 255.0f);
	EnvColor[1] = (Readout_CurrentByte6 / 255.0f);
	EnvColor[2] = (Readout_CurrentByte7 / 255.0f);
	EnvColor[3] = (Readout_CurrentByte8 / 255.0f);

	if(GLExtension_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, EnvColor[0], EnvColor[1], EnvColor[2], EnvColor[3]);
	}

	return 0;
}

int F3DEX2_HACKSelectClrAlpSource()
{
	/* this allows selecting which color or alpha values to use without working combiner emulation.
	   it can however only select either prim, env or full 1.0f for either color or alpha, not multiply values or anything...
	   it MIGHT be possible to have it do color combining, but we should rather get the actual combiner emulation to work */

	float TempAlpha = 0.0f;
	switch(HACK_UseAlphaSource) {
		case 0: TempAlpha = PrimColor[3]; break;	/* prim alpha */
		case 1: TempAlpha = EnvColor[3]; break;		/* env alpha */
		case 2: TempAlpha = 1.0f; break;			/* full */

		default: TempAlpha = 1.0f; break;			/* default to full */
	}
	switch(HACK_UseColorSource) {
		case 0: glColor4f(PrimColor[0], PrimColor[1], PrimColor[2], TempAlpha); break;		/* prim color + previous alpha */
		case 1: glColor4f(EnvColor[0], EnvColor[1], EnvColor[2], TempAlpha); break;			/* env color + previous alpha */
		case 2: glColor4f(1.0f, 1.0f, 1.0f, TempAlpha); break;								/* full + previous alpha */

		default: glColor4f(1.0f, 1.0f, 0.0f, TempAlpha); break;								/* full + previous alpha */
	}

	return 0;
}
