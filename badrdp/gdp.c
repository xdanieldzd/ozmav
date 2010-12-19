#include "globals.h"

static unsigned int TexAddr = 0;

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

	Texture[0].IsTexRect = true;
	Texture[1].IsTexRect = true;

	RDP_InitLoadTexture();

	float ULS = _FIXED2FLOAT((short)_SHIFTR(w2, 16, 16), 5);
	float ULT = _FIXED2FLOAT((short)_SHIFTR(w2,  0, 16), 5);
	float DSDX = _FIXED2FLOAT((short)_SHIFTR(w3, 16, 16), 10);
	float DTDY = _FIXED2FLOAT((short)_SHIFTR(w3,  0, 16), 10);

	float LRS = ULS + (LRX - ULX - 1) * DSDX;
	float LRT = ULT + (LRY - ULY - 1) * DTDY;

	float Width = max(LRS, ULS) + DSDX;
	float Height = max(LRT, ULT) + DTDY;

	// 0
	float S0_0, T0_0, S1_0, T1_0;
	S0_0 = ULS * Texture[0].ShiftScaleS - _FIXED2FLOAT(Texture[0].ULS, 2);
	T0_0 = ULT * Texture[0].ShiftScaleT - _FIXED2FLOAT(Texture[0].ULT, 2);
	S1_0 = (LRS + 1.0f) * Texture[0].ShiftScaleS - _FIXED2FLOAT(Texture[0].ULS, 2);
	T1_0 = (LRT + 1.0f) * Texture[0].ShiftScaleT - _FIXED2FLOAT(Texture[0].ULT, 2);
	if(Texture[0].MaskS && (fmod(S0_0, Texture[0].RealWidth) == 0.0f) && !Texture[0].mirrors) {
		S1_0 -= S0_0;
		S0_0 = 0.0f;
	}
	if(Texture[0].MaskT && (fmod(T0_0, Texture[0].RealHeight) == 0.0f) && !Texture[0].mirrort) {
		T1_0 -= T0_0;
		T0_0 = 0.0f;
	}
	if(OpenGL.Ext_MultiTexture) glActiveTextureARB(GL_TEXTURE0_ARB);
	if((S0_0 >= 0.0f) && (S1_0 <= Texture[0].RealWidth)) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	}
	if((T0_0 >= 0.0f) && (T1_0 <= Texture[0].RealHeight)) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	S0_0 = (S0_0 * Texture[0].ScaleS) / Width;
	T0_0 = (T0_0 * Texture[0].ScaleT) / Height;
	S1_0 = (S1_0 * Texture[0].ScaleS) / Width;
	T1_0 = (T1_0 * Texture[0].ScaleT) / Height;

	if(isnan(S0_0)) S0_0 = 0.0f;
	if(isnan(T0_0)) T0_0 = 0.0f;
	if(isnan(S1_0) || S1_0 == 0.0f) S1_0 = 1.0f;
	if(isnan(T1_0) || T1_0 == 0.0f) T1_0 = 1.0f;

	// 1
	float S0_1, T0_1, S1_1, T1_1;
	if(OpenGL.Ext_MultiTexture) {
		S0_1 = ULS * Texture[1].ShiftScaleS - _FIXED2FLOAT(Texture[1].ULS, 2);
		T0_1 = ULT * Texture[1].ShiftScaleT - _FIXED2FLOAT(Texture[1].ULT, 2);
		S1_1 = (LRS + 1.0f) * Texture[1].ShiftScaleS - _FIXED2FLOAT(Texture[1].ULS, 2);
		T1_1 = (LRT + 1.0f) * Texture[1].ShiftScaleT - _FIXED2FLOAT(Texture[1].ULT, 2);
		if(Texture[1].MaskS && (fmod(S0_1, Texture[1].RealWidth) == 0.0f) && !Texture[1].mirrors) {
			S1_1 -= S0_1;
			S0_1 = 0.0f;
		}
		if(Texture[1].MaskT && (fmod(T0_1, Texture[1].RealHeight) == 0.0f) && !Texture[1].mirrort) {
			T1_1 -= T0_1;
			T0_1 = 0.0f;
		}
		glActiveTextureARB(GL_TEXTURE1_ARB);
		if((S0_1 >= 0.0f) && (S1_1 <= Texture[1].RealWidth)) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		}
		if((T0_1 >= 0.0f) && (T1_1 <= Texture[1].RealHeight)) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		S0_1 = (S0_1 * Texture[1].ScaleS) / Width;
		T0_1 = (T0_1 * Texture[1].ScaleT) / Height;
		S1_1 = (S1_1 * Texture[1].ScaleS) / Width;
		T1_1 = (T1_1 * Texture[1].ScaleT) / Height;

		if(isnan(S0_1)) S0_1 = 0.0f;
		if(isnan(T0_1)) T0_1 = 0.0f;
		if(isnan(S1_1) || S1_1 == 0.0f) S1_1 = 1.0f;
		if(isnan(T1_1) || T1_1 == 0.0f) T1_1 = 1.0f;
	}

	//dbgprintf(0,0,"%4.2f %4.2f %4.2f %4.2f", S0_0, T0_0, S1_0, T1_0);

	// ->
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
