#include "globals.h"

#include "macro_def.h"

unsigned char Segment; unsigned int Offset;
unsigned char NextCmds[32];
unsigned int nw0[32], nw1[32];

unsigned int RDP_Macro_DetectMacro(unsigned int Addr)
{
	int i, j, MaxMacroLen = 0;

	for(i = 0; i < ArraySize(GfxMacros); i++) if(GfxMacros[i].Len > MaxMacroLen) MaxMacroLen = GfxMacros[i].Len;

	if(!RDP_CheckAddressValidity(Addr + (MaxMacroLen * 8))) return Addr;

	Segment = Addr >> 24;
	Offset = (Addr & 0x00FFFFFF);

	for(i = 0; i < MaxMacroLen; i++) {
		if(Segment != 0x80) NextCmds[i] = RAM[Segment].Data[Offset + (i * 8)];
		else NextCmds[i] = RDRAM.Data[Offset + (i * 8)];
	}

	for(i = 0; i < ArraySize(GfxMacros); i++) {
		if(!memcmp(GfxMacros[i].Cmd, NextCmds, GfxMacros[i].Len)) {
			for(j = 0; j < GfxMacros[i].Len + 1; j++) {
				if(Segment != 0x80) {
					nw0[j] = Read32(RAM[Segment].Data, Offset);
					nw1[j] = Read32(RAM[Segment].Data, Offset + 4);
				} else {
					nw0[j] = Read32(RDRAM.Data, Offset);
					nw1[j] = Read32(RDRAM.Data, Offset + 4);
				}
				Offset += 8;
			}
			isMacro = true;

			GfxMacros[i].Func();

			Addr += GfxMacros[i].Len * 8;
			return Addr;
		}
	}

	isMacro = false;

	return Addr;
}

void RDP_Macro_LoadTextureBlock()
{
	Gfx.CurrentTexture = ((nw0[1] & 0x0F00) ? 1 : 0);
	Gfx.IsMultiTexture = Gfx.CurrentTexture;

	gDP_SetTImg(nw0[0], nw1[0]);
	gDP_SetTile(nw0[5], nw1[5]);
	gDP_SetTileSize(nw0[6], nw1[6]);

	// texture is CI and palette will be loaded after this!
	if((Texture[Gfx.CurrentTexture].Format == 0x40) || (Texture[Gfx.CurrentTexture].Format == 0x48) || (Texture[Gfx.CurrentTexture].Format == 0x50)) {
		if((nw0[7] >> 24) == G_SETTIMG) return;
	}
/*
	if(Gfx.IsMultiTexture) {
		Gfx.EnvColor = (__RGBA){ 0.5f, 0.5f, 0.5f, 0.5f };
	} else {
		Gfx.EnvColor = (__RGBA){ 1.0f, 1.0f, 1.0f, 0.75f };
	}
	if(OpenGL.Ext_FragmentProgram) glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, Gfx.EnvColor.R, Gfx.EnvColor.G, Gfx.EnvColor.B, Gfx.EnvColor.A);
*/
	RDP_InitLoadTexture();
}

void RDP_Macro_LoadTLUT()
{
	gDP_SetTImg(nw0[0], nw1[0]);
	gDP_LoadTLUT(nw0[4], nw1[4]);

	RDP_InitLoadTexture();
}

void RDP_Macro_LoadTextureSF64()
{
	Gfx.CurrentTexture = ((nw0[1] & 0x0F00) ? 1 : 0);
	Gfx.IsMultiTexture = Gfx.CurrentTexture;

	gDP_SetTImg(nw0[3], nw1[3]);
	gDP_SetTile(nw0[1], nw1[1]);
	gDP_SetTileSize(nw0[2], nw1[2]);

	if(Gfx.IsMultiTexture) {
		Gfx.EnvColor = (__RGBA){ 0.5f, 0.5f, 0.5f, 0.5f };
	} else {
		Gfx.EnvColor = (__RGBA){ 1.0f, 1.0f, 1.0f, 0.75f };
	}
	if(OpenGL.Ext_FragmentProgram) glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, Gfx.EnvColor.R, Gfx.EnvColor.G, Gfx.EnvColor.B, Gfx.EnvColor.A);

	RDP_InitLoadTexture();
}
