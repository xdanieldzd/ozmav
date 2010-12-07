#include "globals.h"

// ----------------------------------------

void RDP_F3DEX_Init()
{
	RDP_InitFlags(F3DEX);

	RDP_UcodeCmd[F3D_MTX]               = RDP_F3D_MTX;
	RDP_UcodeCmd[F3D_MOVEMEM]           = RDP_F3D_MOVEMEM;
	RDP_UcodeCmd[F3D_VTX]               = RDP_F3DEX_VTX;
	RDP_UcodeCmd[F3D_DL]                = RDP_F3D_DL;
	RDP_UcodeCmd[F3DEX_LOAD_UCODE]      = RDP_F3DEX_LOAD_UCODE;
	RDP_UcodeCmd[F3DEX_BRANCH_Z]        = RDP_F3DEX_BRANCH_Z;
	RDP_UcodeCmd[F3DEX_TRI2]            = RDP_F3DEX_TRI2;
	RDP_UcodeCmd[F3DEX_MODIFYVTX]       = RDP_F3DEX_MODIFYVTX;
	RDP_UcodeCmd[F3D_RDPHALF_2]         = RDP_F3D_RDPHALF_2;
	RDP_UcodeCmd[F3D_RDPHALF_1]         = RDP_F3D_RDPHALF_1;
	RDP_UcodeCmd[F3D_CLEARGEOMETRYMODE] = RDP_F3D_CLEARGEOMETRYMODE;
	RDP_UcodeCmd[F3D_SETGEOMETRYMODE]   = RDP_F3D_SETGEOMETRYMODE;
	RDP_UcodeCmd[F3D_ENDDL]             = RDP_F3D_ENDDL;
	RDP_UcodeCmd[F3D_SETOTHERMODE_L]    = RDP_F3D_SETOTHERMODE_L;
	RDP_UcodeCmd[F3D_SETOTHERMODE_H]    = RDP_F3D_SETOTHERMODE_H;
	RDP_UcodeCmd[F3D_TEXTURE]           = RDP_F3D_TEXTURE;
	RDP_UcodeCmd[F3D_MOVEWORD]          = RDP_F3D_MOVEWORD;
	RDP_UcodeCmd[F3D_POPMTX]            = RDP_F3D_POPMTX;
	RDP_UcodeCmd[F3D_CULLDL]            = RDP_F3DEX_CULLDL;
	RDP_UcodeCmd[F3D_TRI1]              = RDP_F3DEX_TRI1;
}

void RDP_F3DEX_VTX()
{
	gSP_Vertex(w1, _SHIFTR(w0, 10, 6), _SHIFTR(w0, 17, 7));
}

void RDP_F3DEX_LOAD_UCODE()
{
	//
}

void RDP_F3DEX_BRANCH_Z()
{
	if(!RDP_CheckAddressValidity(Gfx.Store_RDPHalf1)) return;

	int Vtx = _SHIFTR(w0, 1, 11);
	int ZVal = (int)w1;

	if(Vertex[Vtx].Vtx.Z < ZVal) {
		Gfx.DLStack[Gfx.DLStackPos] = DListAddress;
		Gfx.DLStackPos++;

		RDP_ParseDisplayList(Gfx.Store_RDPHalf1, false);
	}
}

void RDP_F3DEX_TRI2()
{
	int Vtxs1[] = { _SHIFTR( w0, 17, 7 ), _SHIFTR( w0, 9, 7 ), _SHIFTR( w0, 1, 7 ) };
	RDP_DrawTriangle(Vtxs1);

	int Vtxs2[] = { _SHIFTR( w1, 17, 7 ), _SHIFTR( w1, 9, 7 ), _SHIFTR( w1, 1, 7 ) };
	RDP_DrawTriangle(Vtxs2);
}

void RDP_F3DEX_MODIFYVTX()
{
	//
}

void RDP_F3DEX_CULLDL()
{
	//
}

void RDP_F3DEX_TRI1()
{
	int Vtxs[] = { _SHIFTR( w1, 17, 7 ), _SHIFTR( w1, 9, 7 ), _SHIFTR( w1, 1, 7 ) };
	RDP_DrawTriangle(Vtxs);
}
