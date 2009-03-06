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

	N64_GeometryMode = (N64_GeometryMode & Mode_Clear) | Mode_Set;

	memset(ErrorMsg, 0x00, sizeof(ErrorMsg));
	sprintf(ErrorMsg, "GEOMETRYMODE | CLEAR: %s%s%s%s%s%s%s%s%s%s\nGEOMETRYMODE | SET:   %s%s%s%s%s%s%s%s%s%s\n",
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
		glEnable(GL_FOG);
	} else {
		glDisable(GL_FOG);
	}

	/* zbuffer */
	if(N64_GeometryMode & G_ZBUFFER) {
		glDisable(GL_DEPTH_TEST);
	} else {
		glEnable(GL_DEPTH_TEST);
	}

	/* shading */
	if((N64_GeometryMode & G_SHADE) && (N64_GeometryMode & G_SHADING_SMOOTH)) {
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
	unsigned long TempExtract0 =	(Readout_CurrentByte1 * 0x1000000) +
									(Readout_CurrentByte2 * 0x10000) +
									(Readout_CurrentByte3 * 0x100) +
									Readout_CurrentByte4;

	unsigned long TempExtract1 =	(Readout_CurrentByte5 * 0x1000000) +
									(Readout_CurrentByte6 * 0x10000) +
									(Readout_CurrentByte7 * 0x100) +
									Readout_CurrentByte8;

	switch((TempExtract0 >> 8) & 0xFF) {
		case 0x14:
			RDPCycleMode = ((TempExtract1 >> 0x14) & 0x03);
			break;
		default:
			//unknown mode
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

	byte MDSFT = 32 - ((LowBits >> 8) & 0xFF) - (LowBits & 0xFF) - 1;

	bool AA_EN			= (HighBits & 0x00000008) != 0;		/* anti-aliasing */
	bool Z_CMP			= (HighBits & 0x00000010) != 0;		/* zbuffer compare */
	bool Z_UPD			= (HighBits & 0x00000020) != 0;		/* zbuffer update */
	bool IM_RD			= (HighBits & 0x00000040) != 0;		/* reduced anti-aliasing ?? */
	bool CLR_ON_CVG		= (HighBits & 0x00000080) != 0;
	bool CVG_DST_WRAP	= (HighBits & 0x00000100) != 0;
	bool CVG_DST_FULL	= (HighBits & 0x00000200) != 0;
	bool CVG_DST_SAVE	= (HighBits & 0x00000300) != 0;
	bool ZMODE_INTER	= (HighBits & 0x00000400) != 0;
	bool ZMODE_XLU		= (HighBits & 0x00000800) != 0;
	bool ZMODE_DEC		= (HighBits & 0x00000C00) != 0;
	bool CVG_X_ALPHA	= (HighBits & 0x00001000) != 0;
	bool ALPHA_CVG_SEL	= (HighBits & 0x00002000) != 0;
	bool FORCE_BL		= (HighBits & 0x00004000) != 0;		/* force blending */

/*	sprintf(ErrorMsg, \
		"\nSETOTHERMODE_L - RENDERMODE:\n" \
		" - Display List #%d\n" \
		" ------------------\n" \
		" - AA_EN %d\n" \
		" - Z_CMP %d, Z_UPD %d, IM_RD %d, CLR_ON_CVG %d\n" \
		" - CVG_DST_WRAP %d, CVG_DST_FULL %d, CVG_DST_SAVE %d, ZMODE_INTER %d\n" \
		" - ZMODE_XLU %d, ZMODE_DEC %d\n" \
		" - CVG_X_ALPHA %d, ALPHA_CVG_SEL %d, FORCE_BL %d\n\n",
			DLToRender,
			AA_EN,
			Z_CMP, Z_UPD, IM_RD, CLR_ON_CVG,
			CVG_DST_WRAP, CVG_DST_FULL, CVG_DST_SAVE, ZMODE_INTER,
			ZMODE_XLU, ZMODE_DEC,
			CVG_X_ALPHA, ALPHA_CVG_SEL, FORCE_BL);
	Helper_LogMessage(2, ErrorMsg);
*/
	GLenum BlendSrcFactor = GL_ZERO;
	GLenum BlendDstFactor = GL_ZERO;

	switch (MDSFT)
	{
		case 0: // alphacompare
			MessageBox(hwnd, "alphacompare", "", 0);
			break;
		case 2: // zsrcsel
			MessageBox(hwnd, "zsrcsel", "", 0);
			break;
		case 3: // rendermode
			if(Z_CMP)
				{ glEnable(GL_DEPTH_TEST); } else { glDisable(GL_DEPTH_TEST); }

			if((Z_UPD) && !(ZMODE_INTER && ZMODE_XLU))
				{ glDepthMask(GL_TRUE); } else { glDepthMask(GL_FALSE); }

			if(ZMODE_DEC)
				{ glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset(-1.0f, -1.0f); } else { glDisable(GL_POLYGON_OFFSET_FILL); }

/* ??? */	if(ZMODE_XLU)
				{ glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glAlphaFunc(GL_GREATER, 0.0f); } else { glBlendFunc(GL_ONE, GL_ZERO); glAlphaFunc(GL_GEQUAL, 0.5f); }

			if((FORCE_BL) && !(ALPHA_CVG_SEL)) {
				glEnable(GL_BLEND);

				switch(HighBits >> 16)
				{
					case 0x0C18:
					case 0xC810:
					case 0xC811:
						BlendSrcFactor = GL_SRC_ALPHA;
						BlendDstFactor = GL_ONE_MINUS_SRC_ALPHA;
						break;
					default:
						sprintf(SystemLogMsg, "blender dbg test - %04x\n", HighBits >> 16);
						Helper_LogMessage(2, SystemLogMsg);

						BlendSrcFactor = GL_SRC_ALPHA;
						BlendDstFactor = GL_ONE_MINUS_SRC_ALPHA;
				}

				glBlendFunc(BlendSrcFactor, BlendDstFactor);
			} else {
				glDisable(GL_BLEND);
			}

			break;
	}

	//FOR CORRECTNESS WE WILL NEED TO EVENTUALLY MIGRATE ALL BELOW FUNCTIONS TO THE ABOVE SWITCH BLOCK!!!

/*	Blender_Cycle1 = 0;
	Blender_Cycle2 = (HighBits << 16) >> 16;

	GLenum Blender_SrcFactor =		GL_SRC_ALPHA;
	GLenum Blender_DstFactor =		GL_ONE_MINUS_SRC_ALPHA;
	GLenum Blender_AlphaFunc =		GL_GEQUAL;
	GLclampf Blender_AlphaRef =		0.5f;

	switch (Blender_Cycle1 + Blender_Cycle2) {
		case 0x0055 + 0x2048:
		case 0xC811 + 0x2078:								//no blending
		case 0xC811 + 0x3078:
		case 0x0C19 + 0x2078:
		case 0x0C19 + 0x3078:
			Blender_SrcFactor = GL_ONE;
			Blender_DstFactor = GL_ZERO;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.5f;
			break;

		case 0xC810 + 0x3478:								//syotes2 - everything
			Blender_SrcFactor = GL_ONE;
			Blender_DstFactor = GL_ZERO;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.5f;
			break;
		case 0xC810 + 0x49D8:								//spot00 + most other maps - water (does not always work)
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GREATER;
			Blender_AlphaRef = 0.0f;
			break;
		case 0x0C18 + 0x49D8:								//spot03 - water at waterfall
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_DST_COLOR;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.5f;
			break;
		case 0xC810 + 0x4A50:								//MAJORA north clocktown etc - misc things (nct: greenery on walls)
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GREATER;
			Blender_AlphaRef = 0.0f;
			break;
		case 0x0050 + 0x4B50:
		case 0x0C18 + 0x4B50:								//
		case 0xC810 + 0x4B50:								//spot00 - death mountain plane, spot04 - drawing at link's house
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.4f;
			break;
		case 0xC810 + 0x4DD8:								//spot00 - (used near path to gerudo valley?)
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GREATER;
			Blender_AlphaRef = 0.0f;
			break;
		case 0x0C18 + 0x4DD8:								//spot11 - around oasis
			Blender_SrcFactor = GL_ONE;
			Blender_DstFactor = GL_ZERO;
			Blender_AlphaFunc = GL_GEQUAL;
			Blender_AlphaRef = 0.5f;
			break;*/
/*		case 0x0C18 + 0x4F50:								//
		case 0xC810 + 0x4F50:								//spot00, spot02, spot04 - pathways
		case 0xC818 + 0x4F50:								//
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_NOTEQUAL;
			Blender_AlphaRef = 0.0f;
			break;
		case 0xC811 + 0x2D58:								//spot01 - doorways
			Blender_SrcFactor = GL_SRC_ALPHA;
			Blender_DstFactor = GL_ONE_MINUS_SRC_ALPHA;
			Blender_AlphaFunc = GL_GREATER;
			Blender_AlphaRef = 0.0f;
			break;
		default:
			break;
	}

//	glAlphaFunc(Blender_AlphaFunc, Blender_AlphaRef);
*/
	return 0;
}
