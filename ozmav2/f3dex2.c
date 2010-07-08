#include "globals.h"

// ----------------------------------------

void dl_F3DEX2_Init()
{
	dl_InitFlags(F3DEX2);

	dl_UcodeCmd[F3DEX2_VTX]            = dl_F3DEX2_VTX;
	dl_UcodeCmd[F3DEX2_MODIFYVTX]      = dl_F3DEX2_MODIFYVTX;
	dl_UcodeCmd[F3DEX2_CULLDL]         = dl_F3DEX2_CULLDL;
	dl_UcodeCmd[F3DEX2_BRANCH_Z]       = dl_F3DEX2_BRANCH_Z;
	dl_UcodeCmd[F3DEX2_TRI1]           = dl_F3DEX2_TRI1;
	dl_UcodeCmd[F3DEX2_TRI2]           = dl_F3DEX2_TRI2;
	dl_UcodeCmd[F3DEX2_QUAD]           = dl_F3DEX2_QUAD;
	dl_UcodeCmd[F3DEX2_SPECIAL_3]      = dl_F3DEX2_SPECIAL_3;
	dl_UcodeCmd[F3DEX2_SPECIAL_2]      = dl_F3DEX2_SPECIAL_2;
	dl_UcodeCmd[F3DEX2_SPECIAL_1]      = dl_F3DEX2_SPECIAL_1;
	dl_UcodeCmd[F3DEX2_DMA_IO]         = dl_F3DEX2_DMA_IO;
	dl_UcodeCmd[F3DEX2_TEXTURE]        = dl_F3DEX2_TEXTURE;
	dl_UcodeCmd[F3DEX2_POPMTX]         = dl_F3DEX2_POPMTX;
	dl_UcodeCmd[F3DEX2_GEOMETRYMODE]   = dl_F3DEX2_GEOMETRYMODE;
	dl_UcodeCmd[F3DEX2_MTX]            = dl_F3DEX2_MTX;
	dl_UcodeCmd[F3DEX2_MOVEWORD]       = dl_F3DEX2_MOVEWORD;
	dl_UcodeCmd[F3DEX2_MOVEMEM]        = dl_F3DEX2_MOVEMEM;
	dl_UcodeCmd[F3DEX2_LOAD_UCODE]     = dl_F3DEX2_LOAD_UCODE;
	dl_UcodeCmd[F3DEX2_DL]             = dl_F3DEX2_DL;
	dl_UcodeCmd[F3DEX2_ENDDL]          = dl_F3DEX2_ENDDL;
	dl_UcodeCmd[F3DEX2_SPNOOP]         = dl_F3DEX2_SPNOOP;
	dl_UcodeCmd[F3DEX2_RDPHALF_1]      = dl_F3DEX2_RDPHALF_1;
	dl_UcodeCmd[F3DEX2_SETOTHERMODE_L] = dl_F3DEX2_SETOTHERMODE_L;
	dl_UcodeCmd[F3DEX2_SETOTHERMODE_H] = dl_F3DEX2_SETOTHERMODE_H;
	dl_UcodeCmd[F3DEX2_RDPHALF_2]      = dl_F3DEX2_RDPHALF_2;
}

// ----------------------------------------

void dl_F3DEX2_VTX()
{
	if(!dl_CheckAddressValidity(w1)) return;

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
	if(!dl_CheckAddressValidity(zGfx.Store_RDPHalf1)) return;

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
	if(!dl_CheckAddressValidity(w1)) return;

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

void dl_F3DEX2_RDPHALF_2()
{
	zGfx.Store_RDPHalf2 = w1;
}
