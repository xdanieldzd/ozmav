#include "globals.h"

void dl_UnemulatedCmd()
{
	//no error message as this shouldn't happen with valid dlists, and because it clutters up the log if it happens
}

void dl_G_TEXRECT()
{
	//
}

void dl_G_TEXRECTFLIP()
{
	//
}

void dl_G_RDPLOADSYNC()
{
	//
}

void dl_G_RDPPIPESYNC()
{
	//
}

void dl_G_RDPTILESYNC()
{
	//
}

void dl_G_RDPFULLSYNC()
{
	//
}

void dl_G_SETKEYGB()
{
	//
}

void dl_G_SETKEYR()
{
	//
}

void dl_G_SETCONVERT()
{
	//
}

void dl_G_SETSCISSOR()
{
	//
}

void dl_G_SETPRIMDEPTH()
{
	//
}

void dl_G_RDPSETOTHERMODE()
{
	//
}

void dl_G_LOADTLUT()
{
	if(!zl_CheckAddressValidity(zTexture[zGfx.CurrentTexture].PalOffset)) return;

	memset(zPalette, 0xFF, sizeof(zPalette));

	unsigned char PalSegment = (zTexture[zGfx.CurrentTexture].PalOffset & 0xFF000000) >> 24;
	unsigned int PalOffset = (zTexture[zGfx.CurrentTexture].PalOffset & 0x00FFFFFF);

	unsigned int PalSize = ((w1 & 0x00FFF000) >> 14) + 1;

	unsigned short Raw;
	unsigned int R, G, B, A;

	unsigned int PalLoop;

	for(PalLoop = 0; PalLoop < PalSize; PalLoop++) {
		Raw = (zRAM[PalSegment].Data[PalOffset] << 8) | zRAM[PalSegment].Data[PalOffset + 1];

		R = (Raw & 0xF800) >> 8;
		G = ((Raw & 0x07C0) << 5) >> 8;
		B = ((Raw & 0x003E) << 18) >> 16;

		if((Raw & 0x0001)) { A = 0xFF; } else { A = 0x00; }

		zPalette[PalLoop].R = R;
		zPalette[PalLoop].G = G;
		zPalette[PalLoop].B = B;
		zPalette[PalLoop].A = A;

		PalOffset += 2;

		if(PalOffset > zRAM[PalSegment].Size) break;
	}
}


void dl_G_SETTILESIZE()
{
	dl_ChangeTileSize(_SHIFTR(w1, 24, 3), _SHIFTR(w0, 12, 12), _SHIFTR(w0, 0, 12), _SHIFTR(w1, 12, 12), _SHIFTR(w1, 0, 12));
}

void dl_G_LOADBLOCK()
{
	dl_ChangeTileSize(_SHIFTR(w1, 24, 3), _SHIFTR(w0, 12, 12), _SHIFTR(w0, 0, 12), _SHIFTR(w1, 12, 12), _SHIFTR(w1, 0, 12));
}

void dl_G_LOADTILE()
{
	//
}

void dl_G_SETTILE()
{
	if(w1 == 0x07000000) return;

	zTexture[zGfx.CurrentTexture].Format = (w0 & 0x00FF0000) >> 16;
	zTexture[zGfx.CurrentTexture].CMT = _SHIFTR(w1, 18, 2);
	zTexture[zGfx.CurrentTexture].CMS = _SHIFTR(w1, 8, 2);
	zTexture[zGfx.CurrentTexture].LineSize = _SHIFTR(w0, 9, 9);
	zTexture[zGfx.CurrentTexture].Palette = _SHIFTR(w1, 20, 4);
	zTexture[zGfx.CurrentTexture].ShiftT = _SHIFTR(w1, 10, 4);
	zTexture[zGfx.CurrentTexture].ShiftS = _SHIFTR(w1, 0, 4);
	zTexture[zGfx.CurrentTexture].MaskT = _SHIFTR(w1, 14, 4);
	zTexture[zGfx.CurrentTexture].MaskS = _SHIFTR(w1, 4, 4);
}

void dl_G_FILLRECT()
{
	//
}

void dl_G_SETFILLCOLOR(void)
{
	zGfx.FillColor.R = _SHIFTR(w1, 11, 5) * 0.032258064f;
	zGfx.FillColor.G = _SHIFTR(w1, 6, 5) * 0.032258064f;
	zGfx.FillColor.B = _SHIFTR(w1, 1, 5) * 0.032258064f;
	zGfx.FillColor.A = _SHIFTR(w1, 0, 1);

	zGfx.FillColor.Z = _SHIFTR(w1, 2, 14);
	zGfx.FillColor.DZ = _SHIFTR(w1, 0, 2);
}

void dl_G_SETFOGCOLOR(void)
{
	zGfx.FogColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	zGfx.FogColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	zGfx.FogColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	zGfx.FogColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;
}

void dl_G_SETBLENDCOLOR(void)
{
	zGfx.BlendColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	zGfx.BlendColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	zGfx.BlendColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	zGfx.BlendColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	if(zOpenGL.Ext_FragmentProgram && zOptions.EnableCombiner) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, zGfx.BlendColor.R, zGfx.BlendColor.G, zGfx.BlendColor.B, zGfx.BlendColor.A);
	}
}

void dl_G_SETPRIMCOLOR(void)
{
	zGfx.PrimColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	zGfx.PrimColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	zGfx.PrimColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	zGfx.PrimColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	zGfx.PrimColor.M = _SHIFTL(w0, 8, 8);
	zGfx.PrimColor.L = _SHIFTL(w0, 0, 8) * 0.0039215689f;

	if(zOpenGL.Ext_FragmentProgram && zOptions.EnableCombiner) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, zGfx.PrimColor.R, zGfx.PrimColor.G, zGfx.PrimColor.B, zGfx.PrimColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 3, zGfx.PrimColor.L, zGfx.PrimColor.L, zGfx.PrimColor.L, zGfx.PrimColor.L);
	}
}

void dl_G_SETENVCOLOR(void)
{
	zGfx.EnvColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	zGfx.EnvColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	zGfx.EnvColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	zGfx.EnvColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	if(zOpenGL.Ext_FragmentProgram && zOptions.EnableCombiner) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, zGfx.EnvColor.R, zGfx.EnvColor.G, zGfx.EnvColor.B, zGfx.EnvColor.A);
	}
}

void dl_G_SETCOMBINE()
{
	zGfx.Combiner0 = (w0 & 0x00FFFFFF);
	zGfx.Combiner1 = w1;

	if(zOpenGL.Ext_FragmentProgram && zOptions.EnableCombiner) dl_CheckFragmentCache();
}

void dl_G_SETTIMG()
{
	zGfx.CurrentTexture = 0;
	zGfx.IsMultiTexture = false;

	if((wp0 >> 24) == G_SETTILESIZE) {
		zGfx.CurrentTexture = 1;
		zGfx.IsMultiTexture = true;
	}

	if((wn0 >> 24) == G_RDPTILESYNC) {
		zTexture[zGfx.CurrentTexture].PalOffset = w1;
		return;
	}

	zTexture[zGfx.CurrentTexture].Offset = w1;
}

void dl_G_SETZIMG()
{
	//
}

void dl_G_SETCIMG()
{
	//
}
