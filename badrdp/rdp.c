#include "globals.h"

void RDP_UnemulatedCmd()
{
	// simulate ENDDL to break out of DList
	Gfx.DLStackPos--;
	DListAddress = Gfx.DLStack[Gfx.DLStackPos];
}

void RDP_G_TEXRECT()
{
	// RDP_G_TEXRECT - EVENTUALLY NEEDED SOMEDAY
}

void RDP_G_TEXRECTFLIP()
{
	// RDP_G_TEXRECTFLIP - EVENTUALLY NEEDED SOMEDAY
}

void RDP_G_RDPLOADSYNC()
{
	//
}

void RDP_G_RDPPIPESYNC()
{
	//
}

void RDP_G_RDPTILESYNC()
{
	//
}

void RDP_G_RDPFULLSYNC()
{
	//
}

void RDP_G_SETKEYGB()
{
	// RDP_G_SETKEYGB - EVENTUALLY NEEDED SOMEDAY
}

void RDP_G_SETKEYR()
{
	// RDP_G_SETKEYR - EVENTUALLY NEEDED SOMEDAY
}

void RDP_G_SETCONVERT()
{
	// RDP_G_SETCONVERT - EVENTUALLY NEEDED SOMEDAY
}

void RDP_G_SETSCISSOR()
{
	// RDP_G_SETSCISSOR - EVENTUALLY NEEDED SOMEDAY
}

void RDP_G_SETPRIMDEPTH()
{
	// RDP_G_SETPRIMDEPTH - EVENTUALLY NEEDED SOMEDAY
}

void RDP_G_RDPSETOTHERMODE()
{
	// RDP_G_RDPSETOTHERMODE - EVENTUALLY NEEDED SOMEDAY
}

void RDP_G_LOADTLUT()
{
	gDP_LoadTLUT(w0, w1);
}

void RDP_G_SETTILESIZE()
{
	gDP_SetTileSize(w0, w1);
}

void RDP_G_LOADBLOCK()
{
	// RDP_G_LOADBLOCK - EVENTUALLY NEEDED SOMEDAY
}

void RDP_G_LOADTILE()
{
	// RDP_G_LOADTILE - EVENTUALLY NEEDED SOMEDAY
}

void RDP_G_SETTILE()
{
	gDP_SetTile(w0, w1);
}

void RDP_G_FILLRECT()
{
	// RDP_G_FILLRECT - EVENTUALLY NEEDED SOMEDAY
}

void RDP_G_SETFILLCOLOR()
{
	gDP_SetFillColor(w0, w1);
}

void RDP_G_SETFOGCOLOR()
{
	gDP_SetFogColor(w0, w1);
}

void RDP_G_SETBLENDCOLOR()
{
	gDP_SetBlendColor(w0, w1);
}

void RDP_G_SETPRIMCOLOR()
{
	gDP_SetPrimColor(w0, w1);
}

void RDP_G_SETENVCOLOR()
{
	gDP_SetEnvColor(w0, w1);
}

void RDP_G_SETCOMBINE()
{
	gDP_SetCombine(w0, w1);
}

void RDP_G_SETTIMG()
{
	gDP_SetTImg(w0, w1);
}

void RDP_G_SETZIMG()
{
	// RDP_G_SETZIMG - EVENTUALLY NEEDED SOMEDAY
}

void RDP_G_SETCIMG()
{
	// RDP_G_SETCIMG - EVENTUALLY NEEDED SOMEDAY
}
