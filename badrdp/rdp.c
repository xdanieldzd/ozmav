#include "globals.h"

void RDP_UnemulatedCmd()
{
	//no error message as this shouldn't happen with valid dlists, and because it clutters up the log if it happens
}

void RDP_G_TEXRECT()
{
	//
}

void RDP_G_TEXRECTFLIP()
{
	//
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
	//
}

void RDP_G_SETKEYR()
{
	//
}

void RDP_G_SETCONVERT()
{
	//
}

void RDP_G_SETSCISSOR()
{
	//
}

void RDP_G_SETPRIMDEPTH()
{
	//
}

void RDP_G_RDPSETOTHERMODE()
{
	//
}

void RDP_G_LOADTLUT()
{
	if(!RDP_CheckAddressValidity(Texture[Gfx.CurrentTexture].PalOffset)) return;

	memset(Palette, 0xFF, sizeof(Palette));

	unsigned char PalSegment = Texture[Gfx.CurrentTexture].PalOffset >> 24;
	unsigned int PalOffset = (Texture[Gfx.CurrentTexture].PalOffset & 0x00FFFFFF);

	unsigned int PalSize = ((w1 & 0x00FFF000) >> 14) + 1;

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


void RDP_G_SETTILESIZE()
{
	RDP_ChangeTileSize(_SHIFTR(w1, 24, 3), _SHIFTR(w0, 12, 12), _SHIFTR(w0, 0, 12), _SHIFTR(w1, 12, 12), _SHIFTR(w1, 0, 12));
}

void RDP_G_LOADBLOCK()
{
	RDP_ChangeTileSize(_SHIFTR(w1, 24, 3), _SHIFTR(w0, 12, 12), _SHIFTR(w0, 0, 12), _SHIFTR(w1, 12, 12), _SHIFTR(w1, 0, 12));
}

void RDP_G_LOADTILE()
{
	//
}

void RDP_G_SETTILE()
{
	if(w1 == 0x07000000) return;

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

void RDP_G_FILLRECT()
{
	//
}

void RDP_G_SETFILLCOLOR(void)
{
	Gfx.FillColor.R = _SHIFTR(w1, 11, 5) * 0.032258064f;
	Gfx.FillColor.G = _SHIFTR(w1, 6, 5) * 0.032258064f;
	Gfx.FillColor.B = _SHIFTR(w1, 1, 5) * 0.032258064f;
	Gfx.FillColor.A = _SHIFTR(w1, 0, 1);

	Gfx.FillColor.Z = _SHIFTR(w1, 2, 14);
	Gfx.FillColor.DZ = _SHIFTR(w1, 0, 2);
}

void RDP_G_SETFOGCOLOR(void)
{
	Gfx.FogColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	Gfx.FogColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	Gfx.FogColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	Gfx.FogColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;
}

void RDP_G_SETBLENDCOLOR(void)
{
	Gfx.BlendColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	Gfx.BlendColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	Gfx.BlendColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	Gfx.BlendColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	if(OpenGL.Ext_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, Gfx.BlendColor.R, Gfx.BlendColor.G, Gfx.BlendColor.B, Gfx.BlendColor.A);
	}
}

void RDP_G_SETPRIMCOLOR(void)
{
	Gfx.PrimColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	Gfx.PrimColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	Gfx.PrimColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	Gfx.PrimColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	Gfx.PrimColor.M = _SHIFTL(w0, 8, 8);
	Gfx.PrimColor.L = _SHIFTL(w0, 0, 8) * 0.0039215689f;

	if(OpenGL.Ext_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, Gfx.PrimColor.R, Gfx.PrimColor.G, Gfx.PrimColor.B, Gfx.PrimColor.A);
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 3, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L, Gfx.PrimColor.L);
	}
}

void RDP_G_SETENVCOLOR(void)
{
	Gfx.EnvColor.R = _SHIFTR(w1, 24, 8) * 0.0039215689f;
	Gfx.EnvColor.G = _SHIFTR(w1, 16, 8) * 0.0039215689f;
	Gfx.EnvColor.B = _SHIFTR(w1, 8, 8) * 0.0039215689f;
	Gfx.EnvColor.A = _SHIFTR(w1, 0, 8) * 0.0039215689f;

	if(OpenGL.Ext_FragmentProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, Gfx.EnvColor.R, Gfx.EnvColor.G, Gfx.EnvColor.B, Gfx.EnvColor.A);
	}
}

void RDP_G_SETCOMBINE()
{
	Gfx.Combiner0 = (w0 & 0x00FFFFFF);
	Gfx.Combiner1 = w1;

	if(OpenGL.Ext_FragmentProgram) RDP_CheckFragmentCache();
}

void RDP_G_SETTIMG()
{
	Gfx.CurrentTexture = 0;
	Gfx.IsMultiTexture = false;

	if((wp0 >> 24) == G_SETTILESIZE) {
		Gfx.CurrentTexture = 1;
		Gfx.IsMultiTexture = true;
	}

	if((wn0 >> 24) == G_RDPTILESYNC) {
		Texture[Gfx.CurrentTexture].PalOffset = w1;
		return;
	}

	Texture[Gfx.CurrentTexture].Offset = w1;
}

void RDP_G_SETZIMG()
{
	//
}

void RDP_G_SETCIMG()
{
	//
}
