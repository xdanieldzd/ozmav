#include "globals.h"

static unsigned int TexAddr = 0;

void gDP_LoadTLUT(unsigned int w0, unsigned int w1)
{
	if(!RDP_CheckAddressValidity(TexAddr)) return;

	memset(Palette, 0x00, ArraySize(Palette));

	unsigned char PalSegment = TexAddr >> 24;
	unsigned int PalOffset = (TexAddr & 0x00FFFFFF);

//	unsigned int PalSize = ((w1 & 0x00FFF000) >> 14) + 1;
	unsigned int PalSize = (((w1 & 0x00FF0000) >> 16) + 1) * 4;

	unsigned short Raw;
	unsigned int R, G, B, A;

	unsigned int PalLoop;

	for(PalLoop = 0; PalLoop < PalSize; PalLoop++) {
		Raw = (RAM[PalSegment].Data[PalOffset] << 8) | RAM[PalSegment].Data[PalOffset + 1];

		R = (Raw & 0xF800) >> 8;
		G = ((Raw & 0x07C0) << 5) >> 8;
		B = ((Raw & 0x003E) << 18) >> 16;

		if((Raw & 0x0001)) { A = 0xFF; } else { A = 0x00; }

		Palette[PalLoop].R = R;
		Palette[PalLoop].G = G;
		Palette[PalLoop].B = B;
		Palette[PalLoop].A = A;

		PalOffset += 2;

		if(PalOffset > RAM[PalSegment].Size) break;
	}
}

void gDP_SetTileSize(unsigned int w0, unsigned int w1)
{
	RDP_ChangeTileSize(_SHIFTR(w1, 24, 3), _SHIFTR(w0, 12, 12), _SHIFTR(w0, 0, 12), _SHIFTR(w1, 12, 12), _SHIFTR(w1, 0, 12));
}

void gDP_SetTile(unsigned int w0, unsigned int w1)
{
	if(((w1 & 0xFF000000) >> 24) == 0x07) return;

	Texture[Gfx.CurrentTexture].Offset = TexAddr;

	Texture[Gfx.CurrentTexture].Format = (w0 & 0x00FF0000) >> 16;
	Texture[Gfx.CurrentTexture].CMT = _SHIFTR(w1, 18, 2);
	Texture[Gfx.CurrentTexture].CMS = _SHIFTR(w1, 8, 2);
	Texture[Gfx.CurrentTexture].LineSize = _SHIFTR(w0, 9, 9);
	Texture[Gfx.CurrentTexture].Palette = _SHIFTR(w1, 20, 4);
	Texture[Gfx.CurrentTexture].ShiftT = _SHIFTR(w1, 10, 4);
	Texture[Gfx.CurrentTexture].ShiftS = _SHIFTR(w1, 0, 4);
	Texture[Gfx.CurrentTexture].MaskT = _SHIFTR(w1, 14, 4);
	Texture[Gfx.CurrentTexture].MaskS = _SHIFTR(w1, 4, 4);
}

void gDP_SetFillColor(unsigned int w0, unsigned int w1)
{
	Gfx.FillColor.R = _SHIFTR(w1, 11, 5) * 0.032258064f;
	Gfx.FillColor.G = _SHIFTR(w1, 6, 5) * 0.032258064f;
	Gfx.FillColor.B = _SHIFTR(w1, 1, 5) * 0.032258064f;
	Gfx.FillColor.A = _SHIFTR(w1, 0, 1);

	Gfx.FillColor.Z = _SHIFTR(w1, 2, 14);
	Gfx.FillColor.DZ = _SHIFTR(w1, 0, 2);
}

void gDP_SetFogColor(unsigned int w0, unsigned int w1)
{
	Gfx.FogColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	Gfx.FogColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	Gfx.FogColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	Gfx.FogColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;
}

void gDP_SetBlendColor(unsigned int w0, unsigned int w1)
{
	Gfx.BlendColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	Gfx.BlendColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	Gfx.BlendColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	Gfx.BlendColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;
}

void gDP_SetPrimColor(unsigned int w0, unsigned int w1)
{
	Gfx.PrimColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	Gfx.PrimColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	Gfx.PrimColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	Gfx.PrimColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	Gfx.PrimColor.M = _SHIFTL(w0, 8, 8);
	Gfx.PrimColor.L = _SHIFTL(w0, 0, 8) * 0.0039215689f;

	if(OpenGL.Ext_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, Gfx.PrimColor.R, Gfx.PrimColor.G, Gfx.PrimColor.B, Gfx.PrimColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L);
	}
}

void gDP_SetEnvColor(unsigned int w0, unsigned int w1)
{
	Gfx.EnvColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	Gfx.EnvColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	Gfx.EnvColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	Gfx.EnvColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	if(OpenGL.Ext_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, Gfx.EnvColor.R, Gfx.EnvColor.G, Gfx.EnvColor.B, Gfx.EnvColor.A);
	}
}

void gDP_SetCombine(unsigned int w0, unsigned int w1)
{
	Gfx.Combiner0 = (w0 & 0x00FFFFFF);
	Gfx.Combiner1 = w1;

	if(OpenGL.Ext_FragmentProgram && (System.Options & BRDP_COMBINER)) RDP_CheckFragmentCache();
}

void gDP_SetTImg(unsigned int w0, unsigned int w1)
{
	TexAddr = w1;
}
