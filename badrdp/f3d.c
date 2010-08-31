#include "globals.h"

// ----------------------------------------

void RDP_F3D_Init()
{
	RDP_InitFlags(F3D);

	RDP_UcodeCmd[F3D_SPNOOP]            = RDP_F3D_SPNOOP;
	RDP_UcodeCmd[F3D_MTX]               = RDP_F3D_MTX;
	RDP_UcodeCmd[F3D_RESERVED0]         = RDP_F3D_RESERVED0;
	RDP_UcodeCmd[F3D_MOVEMEM]           = RDP_F3D_MOVEMEM;
	RDP_UcodeCmd[F3D_VTX]               = RDP_F3D_VTX;
	RDP_UcodeCmd[F3D_RESERVED1]         = RDP_F3D_RESERVED1;
	RDP_UcodeCmd[F3D_DL]                = RDP_F3D_DL;
	RDP_UcodeCmd[F3D_RESERVED2]         = RDP_F3D_RESERVED2;
	RDP_UcodeCmd[F3D_RESERVED3]         = RDP_F3D_RESERVED3;
	RDP_UcodeCmd[F3D_SPRITE2D_BASE]     = RDP_F3D_SPRITE2D_BASE;
	RDP_UcodeCmd[F3D_TRI4]              = RDP_F3D_TRI4;
	RDP_UcodeCmd[F3D_RDPHALF_CONT]      = RDP_F3D_RDPHALF_CONT;
	RDP_UcodeCmd[F3D_RDPHALF_2]         = RDP_F3D_RDPHALF_2;
	RDP_UcodeCmd[F3D_RDPHALF_1]         = RDP_F3D_RDPHALF_1;
	RDP_UcodeCmd[F3D_QUAD]              = RDP_F3D_QUAD;
	RDP_UcodeCmd[F3D_CLEARGEOMETRYMODE] = RDP_F3D_CLEARGEOMETRYMODE;
	RDP_UcodeCmd[F3D_SETGEOMETRYMODE]   = RDP_F3D_SETGEOMETRYMODE;
	RDP_UcodeCmd[F3D_ENDDL]             = RDP_F3D_ENDDL;
	RDP_UcodeCmd[F3D_SETOTHERMODE_L]    = RDP_F3D_SETOTHERMODE_L;
	RDP_UcodeCmd[F3D_SETOTHERMODE_H]    = RDP_F3D_SETOTHERMODE_H;
	RDP_UcodeCmd[F3D_TEXTURE]           = RDP_F3D_TEXTURE;
	RDP_UcodeCmd[F3D_MOVEWORD]          = RDP_F3D_MOVEWORD;
	RDP_UcodeCmd[F3D_POPMTX]            = RDP_F3D_POPMTX;
	RDP_UcodeCmd[F3D_CULLDL]            = RDP_F3D_CULLDL;
	RDP_UcodeCmd[F3D_TRI1]              = RDP_F3D_TRI1;
}

// ----------------------------------------

void RDP_F3D_SPNOOP()
{
	//
}

void RDP_F3D_MTX()
{
	int i = 0, j = 0;
	signed long MtxTemp1 = 0, MtxTemp2 = 0;

	unsigned char Segment = w1 >> 24;
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

void RDP_F3D_RESERVED0()
{
	//
}

void RDP_F3D_MOVEMEM()
{
	//
}

void RDP_F3D_VTX()
{
	gSP_Vertex(w1, _SHIFTR(w0, 20, 4) + 1, _SHIFTR(w0, 16, 4));
}

void RDP_F3D_RESERVED1()
{
	//
}

void RDP_F3D_RESERVED2()
{
	//
}

void RDP_F3D_DL()
{
	if(!RDP_CheckAddressValidity(w1)) return;

	Gfx.DLStack[Gfx.DLStackPos] = DListAddress;
	Gfx.DLStackPos++;

	RDP_ParseDisplayList(w1, false);
}

void RDP_F3D_RESERVED3()
{
	//
}

void RDP_F3D_SPRITE2D_BASE()
{
	//
}

void RDP_F3D_TRI4()
{
	//
}

void RDP_F3D_RDPHALF_CONT()
{
	//
}

void RDP_F3D_RDPHALF_2()
{
	Gfx.Store_RDPHalf2 = w1;
}

void RDP_F3D_RDPHALF_1()
{
	Gfx.Store_RDPHalf1 = w1;
}

void RDP_F3D_QUAD()
{
	//
}

void RDP_F3D_CLEARGEOMETRYMODE()
{
	Gfx.GeometryMode &= ~w1;

	Gfx.Update |= CHANGED_GEOMETRYMODE;
}

void RDP_F3D_SETGEOMETRYMODE()
{
	Gfx.GeometryMode |= w1;

	Gfx.Update |= CHANGED_GEOMETRYMODE;
}

void RDP_F3D_ENDDL()
{
	Gfx.DLStackPos--;
	DListAddress = Gfx.DLStack[Gfx.DLStackPos];
}

void RDP_F3D_SETOTHERMODE_L()
{
	switch(_SHIFTR( w0, 8, 8 )) {
		case G_MDSFT_RENDERMODE:
			RDP_SetRenderMode(w1 & 0xCCCCFFFF, w1 & 0x3333FFFF);
			break;
		default: {
			unsigned int shift = _SHIFTR( w0, 8, 8 );
			unsigned int length = _SHIFTR( w0, 0, 8 );
			unsigned int mask = ((1 << length) - 1) << shift;

			Gfx.OtherModeL &= ~mask;
			Gfx.OtherModeL |= w1 & mask;

			Gfx.Update |= CHANGED_RENDERMODE | CHANGED_ALPHACOMPARE;
			break;
		}
	}
}

void RDP_F3D_SETOTHERMODE_H()
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

void RDP_F3D_TEXTURE()
{
	static const __Texture Texture_Empty = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, 0.0f };
	Texture[0] = Texture_Empty;
	Texture[1] = Texture_Empty;

	Gfx.CurrentTexture = 0;
	Gfx.IsMultiTexture = false;

	Texture[0].ScaleS = _FIXED2FLOAT(_SHIFTR(w1, 16, 16), 16);
	Texture[0].ScaleT = _FIXED2FLOAT(_SHIFTR(w1, 0, 16), 16);

	if(Texture[0].ScaleS == 0.0f) Texture[0].ScaleS = 1.0f;
	if(Texture[0].ScaleT == 0.0f) Texture[0].ScaleT = 1.0f;

	Texture[1].ScaleS = Texture[0].ScaleS;
	Texture[1].ScaleT = Texture[0].ScaleT;
}

void RDP_F3D_MOVEWORD()
{
	//
}

void RDP_F3D_POPMTX()
{
	glPopMatrix();
}

void RDP_F3D_CULLDL()
{
	//
}

void RDP_F3D_TRI1()
{
	if(Gfx.Update) RDP_UpdateGLStates();

	int Vtxs[] = { _SHIFTR( w1, 17, 7 ), _SHIFTR( w1, 9, 7 ), _SHIFTR( w1, 1, 7 ) };
	RDP_DrawTriangle(Vtxs);
}
