#include "globals.h"

// ----------------------------------------

void RDP_F3DEX2_Init()
{
	RDP_InitFlags(F3DEX2);

	RDP_UcodeCmd[F3DEX2_VTX]            = RDP_F3DEX2_VTX;
	RDP_UcodeCmd[F3DEX2_MODIFYVTX]      = RDP_F3DEX2_MODIFYVTX;
	RDP_UcodeCmd[F3DEX2_CULLDL]         = RDP_F3DEX2_CULLDL;
	RDP_UcodeCmd[F3DEX2_BRANCH_Z]       = RDP_F3DEX2_BRANCH_Z;
	RDP_UcodeCmd[F3DEX2_TRI1]           = RDP_F3DEX2_TRI1;
	RDP_UcodeCmd[F3DEX2_TRI2]           = RDP_F3DEX2_TRI2;
	RDP_UcodeCmd[F3DEX2_QUAD]           = RDP_F3DEX2_QUAD;
	RDP_UcodeCmd[F3DEX2_SPECIAL_3]      = RDP_F3DEX2_SPECIAL_3;
	RDP_UcodeCmd[F3DEX2_SPECIAL_2]      = RDP_F3DEX2_SPECIAL_2;
	RDP_UcodeCmd[F3DEX2_SPECIAL_1]      = RDP_F3DEX2_SPECIAL_1;
	RDP_UcodeCmd[F3DEX2_DMA_IO]         = RDP_F3DEX2_DMA_IO;
	RDP_UcodeCmd[F3DEX2_TEXTURE]        = RDP_F3DEX2_TEXTURE;
	RDP_UcodeCmd[F3DEX2_POPMTX]         = RDP_F3DEX2_POPMTX;
	RDP_UcodeCmd[F3DEX2_GEOMETRYMODE]   = RDP_F3DEX2_GEOMETRYMODE;
	RDP_UcodeCmd[F3DEX2_MTX]            = RDP_F3DEX2_MTX;
	RDP_UcodeCmd[F3DEX2_MOVEWORD]       = RDP_F3DEX2_MOVEWORD;
	RDP_UcodeCmd[F3DEX2_MOVEMEM]        = RDP_F3DEX2_MOVEMEM;
	RDP_UcodeCmd[F3DEX2_LOAD_UCODE]     = RDP_F3DEX2_LOAD_UCODE;
	RDP_UcodeCmd[F3DEX2_DL]             = RDP_F3DEX2_DL;
	RDP_UcodeCmd[F3DEX2_ENDDL]          = RDP_F3DEX2_ENDDL;
	RDP_UcodeCmd[F3DEX2_SPNOOP]         = RDP_F3DEX2_SPNOOP;
	RDP_UcodeCmd[F3DEX2_RDPHALF_1]      = RDP_F3DEX2_RDPHALF_1;
	RDP_UcodeCmd[F3DEX2_SETOTHERMODE_L] = RDP_F3DEX2_SETOTHERMODE_L;
	RDP_UcodeCmd[F3DEX2_SETOTHERMODE_H] = RDP_F3DEX2_SETOTHERMODE_H;
	RDP_UcodeCmd[F3DEX2_RDPHALF_2]      = RDP_F3DEX2_RDPHALF_2;
}

// ----------------------------------------

void RDP_F3DEX2_VTX()
{
	unsigned int N = ((w0 >> 12) & 0xFF);

	// crude hack to remove "bad" polygons on actors in zelda, due to crappy matrix support
	if(((wn0 >> 24) == F3DEX2_CULLDL) || ((wn0 >> 24) == F3DEX2_MTX)) {
/*		unsigned int Search, Mtx;
		// go back up to 10 cmds to find MTX cmd
		for(Search = DListAddress; Search > DListAddress - 0x50; Search -= 8) {
			// is current search offset valid?
			if(RDP_CheckAddressValidity(Search)) {
				// get cmd from offset and check if its MTX w/ load from seg 0x0D
				Mtx = Read32(RAM[Search >> 24].Data, (Search & 0x00FFFFFF));
				if(Mtx == 0xDA380003) {
					// get offset up to MTX w1
					Search += 4;
					// read MTX offset from its w1
					Mtx = Read32(RAM[Search >> 24].Data, (Search & 0x00FFFFFF));
					// call VTX/MTX combo function
					gSP_VertexMtxHack(w1, N, (((w0 >> 1) & 0x7F) - N), Mtx);
					DListAddress += 8;
					return;
				}
			}
		}*/
	} else {
		gSP_Vertex(w1, N, (((w0 >> 1) & 0x7F) - N));
	}
}

void RDP_F3DEX2_MODIFYVTX()
{
	gSP_ModifyVertex(_SHIFTR(w0, 1, 15), _SHIFTR(w0, 16, 8), w1);
}

void RDP_F3DEX2_CULLDL()
{
	//
}

void RDP_F3DEX2_BRANCH_Z()
{
	if(!RDP_CheckAddressValidity(Gfx.Store_RDPHalf1)) return;

	int Vtx = _SHIFTR(w0, 1, 11);
	short ZVal = (short)w1;

	if(Vertex[Vtx].Vtx.Z < ZVal) {
		Gfx.DLStack[Gfx.DLStackPos] = DListAddress;
		Gfx.DLStackPos++;

		RDP_ParseDisplayList(Gfx.Store_RDPHalf1, false);
	}
}

void RDP_F3DEX2_TRI1()
{
	if(Gfx.Update) RDP_UpdateGLStates();

	int Vtxs[] = { ((w0 & 0x00FF0000) >> 16) / 2, ((w0 & 0x0000FF00) >> 8) / 2, (w0 & 0x000000FF) / 2 };
	RDP_DrawTriangle(Vtxs);
}

void RDP_F3DEX2_TRI2()
{
	if(Gfx.Update) RDP_UpdateGLStates();

	int Vtxs1[] = { ((w0 & 0x00FF0000) >> 16) / 2, ((w0 & 0x0000FF00) >> 8) / 2, (w0 & 0x000000FF) / 2 };
	RDP_DrawTriangle(Vtxs1);

	if(Gfx.Update) RDP_UpdateGLStates();

	int Vtxs2[] = { ((w1 & 0x00FF0000) >> 16) / 2, ((w1 & 0x0000FF00) >> 8) / 2, (w1 & 0x000000FF) / 2 };
	RDP_DrawTriangle(Vtxs2);
}

void RDP_F3DEX2_QUAD()
{
	if(Gfx.Update) RDP_UpdateGLStates();

	int Vtxs1[] = { ((w1 & 0xFF000000) >> 24) / 2, ((w1 & 0x00FF0000) >> 16) / 2, ((w1 & 0x0000FF00) >> 8) / 2 };
	RDP_DrawTriangle(Vtxs1);

	if(Gfx.Update) RDP_UpdateGLStates();

	int Vtxs2[] = { ((w1 & 0xFF000000) >> 24) / 2, ((w1 & 0x0000FF00) >> 8) / 2, (w1 & 0x000000FF) / 2 };
	RDP_DrawTriangle(Vtxs2);
}

void RDP_F3DEX2_SPECIAL_3()
{
	//
}

void RDP_F3DEX2_SPECIAL_2()
{
	//
}

void RDP_F3DEX2_SPECIAL_1()
{
	//
}

void RDP_F3DEX2_DMA_IO()
{
	//
}

void RDP_F3DEX2_TEXTURE()
{
	static const __Texture Texture_Empty = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f };
	Texture[0] = Texture_Empty;
	Texture[1] = Texture_Empty;

	if(_SHIFTR(w1, 16, 16) < 0xFFFF) {
		Texture[0].ScaleS = _FIXED2FLOAT(_SHIFTR(w1, 16, 16), 16);
	} else {
		Texture[0].ScaleS = 1.0f;
	}

	if(_SHIFTR(w1, 0, 16) < 0xFFFF) {
		Texture[0].ScaleT = _FIXED2FLOAT(_SHIFTR(w1, 0, 16), 16);
	} else {
		Texture[0].ScaleT = 1.0f;
	}

	Texture[1].ScaleS = Texture[0].ScaleS;
	Texture[1].ScaleT = Texture[0].ScaleT;
}

void RDP_F3DEX2_POPMTX()
{
	glPopMatrix();

//	RDP_Matrix_ModelviewPop(w1 >> 6);
}

void RDP_F3DEX2_GEOMETRYMODE()
{
	Gfx.GeometryMode &= (w0 & 0x00FFFFFF);
	Gfx.GeometryMode |= w1;

	Gfx.Update |= CHANGED_GEOMETRYMODE;
}

void RDP_F3DEX2_MTX()
{
//	gSP_Matrix(w1, _SHIFTR(w0, 0, 8) ^ G_MTX_PUSH);
//	return;

	if(!(w0 & 0x00FFFFFF)) return;

	unsigned char Segment = w1 >> 24;
	unsigned int Offset = (w1 & 0x00FFFFFF);

	if(Segment == 0x80) {
		glPopMatrix();
		return;
	} else if(Segment == 0x0D) {
		return;
	}

	if(!RDP_CheckAddressValidity(w1)) return;

	int MtxTemp1 = 0, MtxTemp2 = 0;
	int x = 0, y = 0;

	float TempMatrix[4][4];

//	dbgprintf(0, 2, "RDP_F3DEX2_MTX -> Matrix %08X:", w1);

	for(x = 0; x < 4; x++) {
		for(y = 0; y < 4; y++) {
			MtxTemp1 = Read16(RAM[Segment].Data, Offset);
			MtxTemp2 = Read16(RAM[Segment].Data, Offset + 32);
			TempMatrix[x][y] = ((MtxTemp1 << 16) | MtxTemp2) * (1.0f / 65536.0f);
			Offset += 2;
		}
//		dbgprintf(0, 0, "[% 6.0f] [% 6.0f] [% 6.0f] [% 6.0f]", TempMatrix[x][0], TempMatrix[x][1], TempMatrix[x][2], TempMatrix[x][3]);
	}

	glPushMatrix();
	glMultMatrixf(*TempMatrix);
/*
	// below does not yet work right
	unsigned char MtxCommand = (_SHIFTR(w0, 0, 8) | G_MTX_PUSH);

	if(MtxCommand & G_MTX_PROJECTION) {
		if(MtxCommand & G_MTX_LOAD) {
			RDP_Matrix_ProjectionLoad(TempMatrix);
		} else {
			RDP_Matrix_ProjectionMul(TempMatrix);
		}
	} else {
		if((MtxCommand & G_MTX_PUSH) && (Matrix.ModelIndex < (Matrix.ModelStackSize - 1))) {
			RDP_Matrix_ModelviewPush();
		}

		if(MtxCommand & G_MTX_LOAD) {
			RDP_Matrix_ModelviewLoad(TempMatrix);
		} else {
			RDP_Matrix_ModelviewMul(TempMatrix);
		}
	}*/
}

void RDP_F3DEX2_MOVEWORD()
{
	//
}

void RDP_F3DEX2_MOVEMEM()
{
	//
}

void RDP_F3DEX2_LOAD_UCODE()
{
	//
}

void RDP_F3DEX2_DL()
{
	if(!RDP_CheckAddressValidity(w1)) return;

	Gfx.DLStack[Gfx.DLStackPos] = DListAddress;
	Gfx.DLStackPos++;

	RDP_ParseDisplayList(w1, false);
}

void RDP_F3DEX2_ENDDL()
{
	Gfx.DLStackPos--;
	DListAddress = Gfx.DLStack[Gfx.DLStackPos];
}

void RDP_F3DEX2_SPNOOP()
{
	//
}

void RDP_F3DEX2_RDPHALF_1()
{
	Gfx.Store_RDPHalf1 = w1;
}

void RDP_F3DEX2_SETOTHERMODE_L()
{
	switch(32 - _SHIFTR( w0, 8, 8 ) - (_SHIFTR( w0, 0, 8 ) + 1)) {
		case G_MDSFT_RENDERMODE:
			RDP_SetRenderMode(w1 & 0xCCCCFFFF, w1 & 0x3333FFFF);
			break;
		default: {
			unsigned int length = _SHIFTR( w0, 0, 8 ) + 1;
			unsigned int shift = 32 - _SHIFTR( w0, 8, 8 ) - length;
			unsigned int mask = ((1 << length) - 1) << shift;

			Gfx.OtherModeL &= ~mask;
			Gfx.OtherModeL |= w1 & mask;

			Gfx.Update |= CHANGED_RENDERMODE | CHANGED_ALPHACOMPARE;
			break;
		}
	}
}

void RDP_F3DEX2_SETOTHERMODE_H()
{
	switch(32 - _SHIFTR( w0, 8, 8 ) - (_SHIFTR( w0, 0, 8 ) + 1)) {
		default: {
			unsigned int length = _SHIFTR( w0, 0, 8 ) + 1;
			unsigned int shift = 32 - _SHIFTR( w0, 8, 8 ) - length;
			unsigned int mask = ((1 << length) - 1) << shift;

			Gfx.OtherModeH &= ~mask;
			Gfx.OtherModeH |= w1 & mask;
			break;
		}
	}
}

void RDP_F3DEX2_RDPHALF_2()
{
	Gfx.Store_RDPHalf2 = w1;
}
