#include "globals.h"

extern bool do_arb;
static unsigned int TexAddr = 0;

void gDP_TexRect(unsigned int w0, unsigned int w1, unsigned int w2, unsigned int w3)
{
//	dbgprintf(0,0,"%s(%08x, %08x, %08x, %08x);", __FUNCTION__,w0,w1,w2,w3);

	// CALCULATE!
	unsigned int ULX = _SHIFTR(w1, 12, 12) / 2;
	unsigned int ULY = _SHIFTR(w1,  0, 12) / 2;
	unsigned int LRX = _SHIFTR(w0, 12, 12) / 2;
	unsigned int LRY = _SHIFTR(w0,  0, 12) / 2;

	float ULS[2], ULT[2], LRS[2], LRT[2];

	float Off_S = (float)((short)((w2 & 0xFFFF0000) >> 16)) / 32.0f;
	float Off_T = (float)((short)(w2 & 0x0000FFFF)) / 32.0f;
	float DSDX = (float)((short)((w3 & 0xFFFF0000) >> 16)) / 1024.0f;
	float DTDY = (float)((short)(w3 & 0x0000FFFF)) / 1024.0f;

	if(Gfx.OtherMode.cycleType >= 2) {
		DSDX = 1.0f;
		LRX++;
		LRY++;
	}

//	dbgprintf(0,0,"Coords: %i, %i -> %i, %i", ULX, ULY, LRX, LRY);

	if(Gfx.OtherMode.cycleType == G_CYC_COPY) DSDX /= 4.0f;

	float Off_X = (float)Off_S;
	float Off_Y = (float)Off_T;
	float Off_Size_X = (float)((LRX - ULX) * DSDX);
	float Off_Size_Y = (float)((LRY - ULY) * DTDY);

	RDP_InitLoadTexture();

	int i = 0;
	for(i = 0; i < 2; i++) {
		Texture[i].TexRectW = Off_Size_X;
		Texture[i].TexRectH = Off_Size_Y;
		Texture[i].IsTexRect = true;

		RDP_CalcTextureSize(i);

//		dbgprintf(0,0,"Tex%i: RW:%i, RH:%i", i, Texture[i].RealWidth, Texture[i].RealHeight);

		float SX = 1.0f, SY = 1.0f;

		if(Texture[i].ShiftS > 10) {
			SX = (1 << (16 - Texture[i].ShiftS));
		} else if(Texture[i].ShiftS > 0) {
			SX /= (1 << Texture[i].ShiftS);
		}

		if(Texture[i].ShiftT > 10) {
			SY = (1 << (16 - Texture[i].ShiftT));
		} else if(Texture[i].ShiftT > 0) {
			SY /= (1 << Texture[i].ShiftT);
		}

		ULS[i] = (Off_X * SX);
		ULT[i] = (Off_Y * SY);

		ULS[i] -= Texture[i].ULS + Texture[i].ScaleS - 0.5f;
		ULT[i] -= Texture[i].ULT + Texture[i].ScaleT - 0.5f;

		LRS[i] = (ULS[i] + Off_Size_X * SX - 1.0f);
		LRT[i] = (ULT[i] + Off_Size_Y * SY - 1.0f);

		ULS[i] /= Texture[i].RealWidth * 2.0f;
		ULT[i] /= Texture[i].RealHeight * 2.0f;
		LRS[i] /= Texture[i].RealWidth * 2.0f;
		LRT[i] /= Texture[i].RealHeight * 2.0f;

//		dbgprintf(0,0,"Tex%i: ULS:%4.2f, ULT:%4.2f, LRS:%4.2f, LRS:%4.2f", i, ULS[i], ULT[i], LRS[i], LRT[i]);
	}

	// RENDER!
	if(Gfx.Update) RDP_UpdateGLStates();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, System.DrawWidth, System.DrawHeight, 0, -2048, 2048);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(Gfx.OtherMode.cycleType == G_CYC_FILL) {
		glColor4f(Gfx.FillColor.R, Gfx.FillColor.G, Gfx.FillColor.B, Gfx.FillColor.A);
	} else {
		glColor4f(Gfx.BlendColor.R, Gfx.BlendColor.G, Gfx.BlendColor.B, Gfx.BlendColor.A);
	}

	glBegin(GL_QUADS);
		if(OpenGL.Ext_MultiTexture) {
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, ULS[0], ULT[0]);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, ULS[1], ULT[1]);
			glVertex2d(ULX, ULY);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, LRS[0], ULT[0]);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, LRS[1], ULT[1]);
			glVertex2d(LRX, ULY);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, LRS[0], LRT[0]);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, LRS[1], LRT[1]);
			glVertex2d(LRX, LRY);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, ULS[0], LRT[0]);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, ULS[1], LRT[1]);
			glVertex2d(ULX, LRY);
		} else {
			glTexCoord2f(ULS[0], ULT[0]); glVertex2d(ULX, ULY);
			glTexCoord2f(LRS[0], ULT[0]); glVertex2d(LRX, ULY);
			glTexCoord2f(LRS[0], LRT[0]); glVertex2d(LRX, LRY);
			glTexCoord2f(ULS[0], LRT[0]); glVertex2d(ULX, LRY);
		}
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

//	dbgprintf(0,0,"TEXRECT done.\n\n");
}
/*
void gDP_TexRect(unsigned int w0, unsigned int w1, unsigned int w2, unsigned int w3)
{
	// HACKISH AND WRONG

	unsigned int ULX = _SHIFTR(w1, 12, 12) / 2;
	unsigned int ULY = _SHIFTR(w1,  0, 12) / 2;
	unsigned int LRX = _SHIFTR(w0, 12, 12) / 2;
	unsigned int LRY = _SHIFTR(w0,  0, 12) / 2;

	if(Gfx.Update) RDP_UpdateGLStates();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, System.DrawWidth, System.DrawHeight, 0, -2048, 2048);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(Gfx.OtherMode.cycleType == G_CYC_FILL) {
		glColor4f(Gfx.FillColor.R, Gfx.FillColor.G, Gfx.FillColor.B, Gfx.FillColor.A);
	} else {
		glColor4f(Gfx.BlendColor.R, Gfx.BlendColor.G, Gfx.BlendColor.B, Gfx.BlendColor.A);
	}

	float ULS = _FIXED2FLOAT((short)_SHIFTR(w2, 16, 16), 5);
	float ULT = _FIXED2FLOAT((short)_SHIFTR(w2,  0, 16), 5);
	float DSDX = _FIXED2FLOAT((short)_SHIFTR(w3, 16, 16), 10);
	float DTDY = _FIXED2FLOAT((short)_SHIFTR(w3,  0, 16), 10);
	float LRS = ULS + (LRX - ULX - 1) * DSDX;
	float LRT = ULT + (LRY - ULY - 1) * DTDY;

//	dbgprintf(0,0,"ULS:%4.2f, ULT:%4.2f, LRS:%4.2f, LRS:%4.2f", ULS, ULT, LRS, LRT);

	float S0_0 = ULS * (Texture[0].ScaleS * Texture[0].ShiftScaleS) / 32.0f / _FIXED2FLOAT(Texture[0].RealWidth, 16);//0.0f;
	float T0_0 = ULT * (Texture[0].ScaleT * Texture[0].ShiftScaleT) / 32.0f / _FIXED2FLOAT(Texture[0].RealHeight, 16);//0.0f;
	float S1_0 = LRS * (Texture[0].ScaleS * Texture[0].ShiftScaleS) / 32.0f / _FIXED2FLOAT(Texture[0].RealWidth, 16);//1.0f;
	float T1_0 = LRT * (Texture[0].ScaleT * Texture[0].ShiftScaleT) / 32.0f / _FIXED2FLOAT(Texture[0].RealHeight, 16);//1.0f;

	float S0_1 = ULS * (Texture[1].ScaleS * Texture[1].ShiftScaleS) / 32.0f / _FIXED2FLOAT(Texture[1].RealWidth, 16);//0.0f;
	float T0_1 = ULT * (Texture[1].ScaleT * Texture[1].ShiftScaleT) / 32.0f / _FIXED2FLOAT(Texture[1].RealHeight, 16);//0.0f;
	float S1_1 = LRS * (Texture[1].ScaleS * Texture[1].ShiftScaleS) / 32.0f / _FIXED2FLOAT(Texture[1].RealWidth, 16);//1.0f;
	float T1_1 = LRT * (Texture[1].ScaleT * Texture[1].ShiftScaleT) / 32.0f / _FIXED2FLOAT(Texture[1].RealHeight, 16);//1.0f;

//	dbgprintf(0,0,"S0_0:%4.2f, T0_0:%4.2f, S1_0:%4.2f, T1_0:%4.2f", S0_0, T0_0, S1_0, T1_0);

	RDP_InitLoadTexture();

	if(OpenGL.Ext_MultiTexture) {
		glBegin(GL_QUADS);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, S0_0, T0_0);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, S0_1, T0_1);
			glVertex2d(ULX, ULY);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, S1_0, T0_0);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, S1_1, T0_1);
			glVertex2d(LRX, ULY);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, S1_0, T1_0);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, S1_1, T1_1);
			glVertex2d(LRX, LRY);
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, S0_0, T1_0);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, S0_1, T1_1);
			glVertex2d(ULX, LRY);
		glEnd();
	} else {
		glBegin(GL_QUADS);
			glTexCoord2f(S0_0, T0_0); glVertex2d(ULX, ULY);
			glTexCoord2f(S1_0, T0_0); glVertex2d(LRX, ULY);
			glTexCoord2f(S1_0, T1_0); glVertex2d(LRX, LRY);
			glTexCoord2f(S0_0, T1_0); glVertex2d(ULX, LRY);
		glEnd();
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
*/
#define MAX_SCREEN_BORDER 32

void gDP_FillRect(unsigned int w0, unsigned int w1, unsigned int w2, unsigned int w3)
{
	// HACKISH AND WRONG

	unsigned int ULX = _SHIFTR(w1, 12, 12) / 2;
	unsigned int ULY = _SHIFTR(w1,  0, 12) / 2;
	unsigned int LRX = _SHIFTR(w0, 12, 12) / 2;
	unsigned int LRY = _SHIFTR(w0,  0, 12) / 2;

	if(Gfx.OtherMode.cycleType == G_CYC_FILL) {
		LRX++;
		LRY++;
		if(	(ULX <= MAX_SCREEN_BORDER) && (ULY <= MAX_SCREEN_BORDER) &&
			(LRX >= (System.DrawWidth - MAX_SCREEN_BORDER)) && (LRY >= (System.DrawHeight - MAX_SCREEN_BORDER))) {
				glClearColor(Gfx.FillColor.R, Gfx.FillColor.G, Gfx.FillColor.B, Gfx.FillColor.A);
				glClear(GL_COLOR_BUFFER_BIT);
				return;
		}
	}

	if(Gfx.Update) RDP_UpdateGLStates();

	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, System.DrawWidth, System.DrawHeight, 0, -2048, 2048);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(Gfx.OtherMode.cycleType == G_CYC_FILL) {
		glColor4f(Gfx.FillColor.R, Gfx.FillColor.G, Gfx.FillColor.B, Gfx.FillColor.A);
	} else {
		glColor4f(Gfx.BlendColor.R, Gfx.BlendColor.G, Gfx.BlendColor.B, Gfx.BlendColor.A);
	}

	glRectd(ULX, ULY, LRX, LRY);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

//	glEnable(GL_TEXTURE_2D);
}

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
		if(PalSegment != 0x80) Raw = (RAM[PalSegment].Data[PalOffset] << 8) | RAM[PalSegment].Data[PalOffset + 1];
		else Raw = (RDRAM.Data[PalOffset] << 8) | RDRAM.Data[PalOffset + 1];

		R = (Raw & 0xF800) >> 8;
		G = ((Raw & 0x07C0) << 5) >> 8;
		B = ((Raw & 0x003E) << 18) >> 16;

		if((Raw & 0x0001)) { A = 0xFF; } else { A = 0x00; }

		Palette[PalLoop].R = R;
		Palette[PalLoop].G = G;
		Palette[PalLoop].B = B;
		Palette[PalLoop].A = A;

		PalOffset += 2;
	}
}

void gDP_SetTileSize(unsigned int w0, unsigned int w1)
{
	RDP_ChangeTileSize(_SHIFTR(w1, 24, 3), _SHIFTR(w0, 12, 12), _SHIFTR(w0, 0, 12), _SHIFTR(w1, 12, 12), _SHIFTR(w1, 0, 12));
}

void gDP_SetTile(unsigned int w0, unsigned int w1)
{
//	if(((w1 & 0xFF000000) >> 24) == 0x07) return;

	if(isMacro) Texture[Gfx.CurrentTexture].Offset = TexAddr;

	Texture[Gfx.CurrentTexture].Format = (w0 & 0x00FF0000) >> 16;
	Texture[Gfx.CurrentTexture].CMT = _SHIFTR(w1, 18, 2);
	Texture[Gfx.CurrentTexture].CMS = _SHIFTR(w1, 8, 2);
	Texture[Gfx.CurrentTexture].LineSize = _SHIFTR(w0, 9, 9);
	Texture[Gfx.CurrentTexture].Palette = _SHIFTR(w1, 20, 4);
	Texture[Gfx.CurrentTexture].ShiftT = _SHIFTR(w1, 10, 4);
	Texture[Gfx.CurrentTexture].ShiftS = _SHIFTR(w1, 0, 4);
	Texture[Gfx.CurrentTexture].MaskT = _SHIFTR(w1, 14, 4);
	Texture[Gfx.CurrentTexture].MaskS = _SHIFTR(w1, 4, 4);

	if(!Texture[Gfx.CurrentTexture].MaskT) Texture[Gfx.CurrentTexture].CMT &= G_TX_CLAMP;
	if(!Texture[Gfx.CurrentTexture].MaskS) Texture[Gfx.CurrentTexture].CMS &= G_TX_CLAMP;

	Texture[Gfx.CurrentTexture].IsTexRect = false;
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

	if(OpenGL.Ext_FragmentProgram && do_arb) {
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
	if(isMacro) TexAddr = w1;
	else Texture[Gfx.CurrentTexture].Offset = w1;
}
