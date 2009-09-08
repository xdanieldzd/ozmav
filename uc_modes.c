/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	uc_modes.c - F3DEX2 GeometryMode, SetOtherMode(X) functions
	------------------------------------------------------------ */

#include "globals.h"

/*	------------------------------------------------------------ */

int F3DEX2_Cmd_GEOMETRYMODE()
{
	unsigned int Mode_Clear = (Readout_CurrentByte2 * 0x10000) + (Readout_CurrentByte3 * 0x100) + Readout_CurrentByte4;
	unsigned int Mode_Set = (Readout_CurrentByte6 * 0x10000) + (Readout_CurrentByte7 * 0x100) + Readout_CurrentByte8;

	N64_GeometryMode &= Mode_Clear;
	N64_GeometryMode |= Mode_Set;

	memset(ErrorMsg, 0x00, sizeof(ErrorMsg));
	sprintf(ErrorMsg, "GEOMETRYMODE | CLEAR: %s%s%s%s%s%s%s%s%s%s\nGEOMETRYMODE | SET:   %s%s%s%s%s%s%s%s%s%s\n\n",
		Mode_Clear & G_SHADE ? "G_SHADE | " : "",
		Mode_Clear & G_LIGHTING ? "G_LIGHTING | " : "",
		Mode_Clear & G_SHADING_SMOOTH ? "G_SHADING_SMOOTH | " : "",
		Mode_Clear & G_ZBUFFER ? "G_ZBUFFER | " : "",
		Mode_Clear & G_TEXTURE_GEN ? "G_TEXTURE_GEN | " : "",
		Mode_Clear & G_TEXTURE_GEN_LINEAR ? "G_TEXTURE_GEN_LINEAR | " : "",
		Mode_Clear & G_CULL_FRONT ? "G_CULL_FRONT | " : "",
		Mode_Clear & G_CULL_BACK ? "G_CULL_BACK | " : "",
		Mode_Clear & G_FOG ? "G_FOG | " : "",
		Mode_Clear & G_CLIPPING ? "G_CLIPPING" : "",
		Mode_Set & G_SHADE ? "G_SHADE | " : "",
		Mode_Set & G_LIGHTING ? "G_LIGHTING | " : "",
		Mode_Set & G_SHADING_SMOOTH ? "G_SHADING_SMOOTH | " : "",
		Mode_Set & G_ZBUFFER ? "G_ZBUFFER | " : "",
		Mode_Set & G_TEXTURE_GEN ? "G_TEXTURE_GEN | " : "",
		Mode_Set & G_TEXTURE_GEN_LINEAR ? "G_TEXTURE_GEN_LINEAR | " : "",
		Mode_Set & G_CULL_FRONT ? "G_CULL_FRONT | " : "",
		Mode_Set & G_CULL_BACK ? "G_CULL_BACK | " : "",
		Mode_Set & G_FOG ? "G_FOG | " : "",
		Mode_Set & G_CLIPPING ? "G_CLIPPING" : "" );
//	Helper_LogMessage(2, ErrorMsg);

	return 0;
}

int F3DEX2_UpdateGeoMode()
{
	/* culling */
	if(N64_GeometryMode & G_CULL_BOTH) {
		glEnable(GL_CULL_FACE);
		if(N64_GeometryMode & G_CULL_BACK) {
			glCullFace(GL_BACK);
		} else {
			glCullFace(GL_FRONT);
		}
	} else {
		glDisable(GL_CULL_FACE);
	}

	/* fog */
	if(N64_GeometryMode & G_FOG) {
//		glEnable(GL_FOG);
	} else {
//		glDisable(GL_FOG);
	}
	/* commented out in favor of manual fog en-/disable via menu*/

	/* zbuffer */
	if(N64_GeometryMode & G_ZBUFFER) {
		glEnable(GL_DEPTH_TEST);
	} else {
//		glDisable(GL_DEPTH_TEST);
	}

	/* shading */
	if((N64_GeometryMode & G_TEXTURE_GEN_LINEAR)) {
		glShadeModel(GL_FLAT);
	} else {
		glShadeModel(GL_SMOOTH);
	}

	/* lighting */
	if(N64_GeometryMode & G_LIGHTING) {
		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
	} else {
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);
	}

	return 0;
}

int F3DEX2_Cmd_SETOTHERMODE_H()
{
	unsigned long W0 =	(Readout_CurrentByte1 * 0x1000000) +
						(Readout_CurrentByte2 * 0x10000) +
						(Readout_CurrentByte3 * 0x100) +
						Readout_CurrentByte4;

	unsigned long W1 =	(Readout_CurrentByte5 * 0x1000000) +
						(Readout_CurrentByte6 * 0x10000) +
						(Readout_CurrentByte7 * 0x100) +
						Readout_CurrentByte8;

	int Mode = (32 - _SHIFTR( W0, 8, 8 ) - (_SHIFTR( W0, 0, 8 ) + 1));
	switch(Mode) {
		case G_MDSFT_CYCLETYPE:
			RDPCycleMode = (W1 >> G_MDSFT_CYCLETYPE);
			break;
		case G_MDSFT_TEXTLUT:
			//fprintf(FileSystemLog, "SETOTHERMODE_H: Mode G_MDSFT_TEXTLUT, Val 0x%02X\n\n", (unsigned int)(W1 >> Mode));
			break;
		default:
			//unknown mode
			//fprintf(FileSystemLog, "SETOTHERMODE_H: Mode 0x%02X, Val 0x%02X\n\n", Mode, (unsigned int)(W1 >> Mode));
			break;
	}

	return 0;
}

int F3DEX2_Cmd_SETOTHERMODE_L()
{
    unsigned int LowBits =	        (Readout_CurrentByte2 * 0x10000) +
									(Readout_CurrentByte3 * 0x100) +
									(Readout_CurrentByte4);

    unsigned int HighBits  =	    (Readout_CurrentByte5 * 0x1000000) +
									(Readout_CurrentByte6 * 0x10000) +
									(Readout_CurrentByte7 * 0x100) +
									 Readout_CurrentByte8;

	unsigned char MDSFT = (32 - _SHIFTR( LowBits, 8, 8 ) - (_SHIFTR( LowBits, 0, 8 ) + 1));

	bool AA_EN			= (HighBits & 0x00000008) != 0;		/* anti-aliasing */
	bool Z_CMP			= (HighBits & 0x00000010) != 0;		/* zbuffer compare */
	bool Z_UPD			= (HighBits & 0x00000020) != 0;		/* zbuffer update */
	bool IM_RD			= (HighBits & 0x00000040) != 0;
	bool CLR_ON_CVG		= (HighBits & 0x00000080) != 0;
	bool CVG_DST_WRAP	= (HighBits & 0x00000100) != 0;
	bool CVG_DST_FULL	= (HighBits & 0x00000200) != 0;
	bool CVG_DST_SAVE	= (HighBits & 0x00000300) != 0;
	bool ZMODE_INTER	= (HighBits & 0x00000400) != 0;
	bool ZMODE_XLU		= (HighBits & 0x00000800) != 0;
	bool ZMODE_DEC		= (HighBits & 0x00000C00) != 0;		/* decal */
	bool CVG_X_ALPHA	= (HighBits & 0x00001000) != 0;
	bool ALPHA_CVG_SEL	= (HighBits & 0x00002000) != 0;
	bool FORCE_BL		= (HighBits & 0x00004000) != 0;		/* force blending */

	switch(MDSFT) {
		case G_MDSFT_ALPHACOMPARE:
			MessageBox(hwnd, "alphacompare", "", 0);
			break;
		case G_MDSFT_ZSRCSEL:
			MessageBox(hwnd, "zsrcsel", "", 0);
			break;
		case G_MDSFT_RENDERMODE: {
			sprintf(ErrorMsg, \
				"SETOTHERMODE_L - RENDERMODE:\n" \
				" ------------------\n" \
				" - AA_EN %d\n" \
				" - Z_CMP %d, Z_UPD %d, IM_RD %d, CLR_ON_CVG %d\n" \
				" - CVG_DST_WRAP %d, CVG_DST_FULL %d, CVG_DST_SAVE %d, ZMODE_INTER %d\n" \
				" - ZMODE_XLU %d, ZMODE_DEC %d\n" \
				" - CVG_X_ALPHA %d, ALPHA_CVG_SEL %d, FORCE_BL %d\n\n",
					AA_EN,
					Z_CMP, Z_UPD, IM_RD, CLR_ON_CVG,
					CVG_DST_WRAP, CVG_DST_FULL, CVG_DST_SAVE, ZMODE_INTER,
					ZMODE_XLU, ZMODE_DEC,
					CVG_X_ALPHA, ALPHA_CVG_SEL, FORCE_BL);
			Helper_LogMessage(2, ErrorMsg);

			if(Z_CMP)
				{ glEnable(GL_DEPTH_BUFFER); } else { glDisable(GL_DEPTH_BUFFER); }

			if((Z_UPD) && !(ZMODE_INTER && ZMODE_XLU))
				{ glDepthMask(GL_TRUE); } else { glDepthMask(GL_FALSE); }

			if(ZMODE_DEC)
				{ glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset(-1.0f, -1.0f); } else { glDisable(GL_POLYGON_OFFSET_FILL); }

/* ??? */	if(ZMODE_XLU)
				{ glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glAlphaFunc(GL_GREATER, 0.0f); } else { /*glBlendFunc(GL_ONE, GL_ZERO);*/ glAlphaFunc(GL_GEQUAL, 0.5f); }

			if(!(ALPHA_CVG_SEL)) {
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc((BlendColor[3] > 0.0f) ? GL_GEQUAL : GL_GREATER, BlendColor[3]);
			} else if(CVG_X_ALPHA) {
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GEQUAL, 0.5f);
			} else {
				glDisable(GL_ALPHA_TEST);
			}

			RDPOtherMode_ForceBlender = FORCE_BL;
			RDPOtherMode_BlendMode = (HighBits >> 16);

			F3DEX2_ForceBlender();

			break; }
	}

	return 0;
}

int F3DEX2_ForceBlender()
{
	if((RDPOtherMode_ForceBlender) && (RDPCycleMode < 2)) {
		glEnable(GL_BLEND);

		switch(RDPOtherMode_BlendMode) {
			case 0xC810:
			case 0xC811:
			case 0x0C18:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case 0xAF50:
				glBlendFunc(GL_ZERO, GL_ONE);
				break;
			default:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				sprintf(SystemLogMsg, "blender dbg test - %04x\n", RDPOtherMode_BlendMode);
				Helper_LogMessage(2, SystemLogMsg);
				break;
		}
	} else {
		glBlendFunc(GL_ONE, GL_ZERO);
	}

	return 0;
}
