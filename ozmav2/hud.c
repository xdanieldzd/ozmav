#include "globals.h"

char TempString[8192];

const unsigned char FontWidths[] = {
//	   !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /
	4, 2, 4, 6, 6, 6, 6, 2, 4, 4, 6, 6, 3, 5, 2, 4,
//	0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 2, 3, 5, 5, 5, 6,
//	@  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
//	P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 6, 5,
//	`  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o
	3, 6, 6, 6, 6, 6, 6, 6, 6, 2, 4, 6, 2, 6, 6, 6,
//	p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~  (tab)
	6, 6, 5, 6, 6, 6, 6, 6, 6, 6, 6, 4, 2, 4, 7, 16,
//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

float TextBG[] = { 0.1f, 0.1f, 0.1f, 0.7f };
float TextFG[] = { 1.0, 1.0f, 1.0f, 1.0f };

int hud_Init(unsigned char FontPath[])
{
	if(!hud_LoadFontBMP(FontPath)) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Could not load font image!\n");
		return EXIT_FAILURE;
	}

	if(glIsTexture(Font.TexID)) glDeleteTextures(1, &Font.TexID);
	glGenTextures(1, &Font.TexID);

	glBindTexture(GL_TEXTURE_2D, Font.TexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Font.Width, Font.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Font.Image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	memcpy(Font.CharWidths, FontWidths, ArraySize(FontWidths));
	memcpy(Font.BGC, TextBG, ArraySize(TextBG));
	memcpy(Font.FGC, TextFG, ArraySize(TextFG));

	hud_BuildFont();

	if(Font.Image != NULL) free(Font.Image);

	return EXIT_SUCCESS;
}

bool hud_LoadFontBMP(unsigned char Path[])
{
	int ColorKey[3] = { 0xFF, 0x00, 0xFF };

	FILE * File;
	if((File = fopen(Path, "rb")) == NULL) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Could not open BMP file '%s'!\n", Path);
		return false;
	}

	char TempID[] = { 0, 0, 0 };
	fread(&TempID, 2, 1, File);
	if(strcmp(TempID, "BM")) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Font image not a BMP file!\n");
		return false;
	}

	fseek(File, 18, SEEK_SET);
	fread(&Font.Width, 1, sizeof(int), File);
	fread(&Font.Height, 1, sizeof(int), File);
	fread(&Font.Plane, 1, sizeof(short), File);
	fread(&Font.BPP, 1, sizeof(short), File);

	if(Font.BPP != 24) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: BMP file is not 24bpp!\n");
		return false;
	}

	Font.Image = (unsigned char *)malloc(sizeof(char) * Font.Width * Font.Height * 4);

	int Size = Font.Width * Font.Height * (Font.BPP / 8);
	unsigned char * TempImage = (unsigned char *)malloc(sizeof(char) * Size);

	fseek(File, 24, SEEK_CUR);
	fread(TempImage, Size, sizeof(char), File);

	fclose(File);

	int BytesPx = (Font.BPP / 8);

	int X, Y;
	int SrcOffset = 0;

	for(Y = Font.Height; Y > 0; Y--) {
		for(X = 0; X < Font.Width * BytesPx; X += BytesPx) {
			// check for transparency color key
            if(	TempImage[((Y - 1) * Font.Width) * BytesPx + X + 2] == ColorKey[0] &&
				TempImage[((Y - 1) * Font.Width) * BytesPx + X + 1] == ColorKey[1] &&
				TempImage[((Y - 1) * Font.Width) * BytesPx + X + 0] == ColorKey[2])
			{
				Font.Image[SrcOffset + 3]	= 0;
			} else {
				Font.Image[SrcOffset + 3]	= 0xFF;
			}

			// copy image
			Font.Image[SrcOffset + 2]	= TempImage[((Y - 1) * Font.Width) * BytesPx + X + 0];
			Font.Image[SrcOffset + 1]	= TempImage[((Y - 1) * Font.Width) * BytesPx + X + 1];
			Font.Image[SrcOffset + 0]	= TempImage[((Y - 1) * Font.Width) * BytesPx + X + 2];
			SrcOffset += 4;
		}
	}

	return true;
}

void hud_BuildFont()
{
	int i;
	float CharX, CharY;

	Font.BaseDL = glGenLists(128);
	glBindTexture(GL_TEXTURE_2D, Font.TexID);

	for(i = 0; i < 128; i++) {
		CharX = ((float)(i % 16)) / 16.0f;
		CharY = ((float)(i / 16)) / 8.0f;

		glNewList(Font.BaseDL + i, GL_COMPILE);
			glBegin(GL_QUADS);
				glTexCoord2f(CharX, CharY);
				glVertex2i(0, 0);
				glTexCoord2f(CharX + 0.0625f, CharY);
				glVertex2i(8, 0);
				glTexCoord2f(CharX + 0.0625f, CharY + 0.125f);
				glVertex2i(8, 8);
				glTexCoord2f(CharX, CharY + 0.125f);
				glVertex2i(0, 8);
			glEnd();
			glTranslated(Font.CharWidths[i], 0, 0);
		glEndList();
	}
}

void hud_KillFont()
{
	if(glIsList(Font.BaseDL)) glDeleteLists(Font.BaseDL, 256);
}

void hud_Print(int Index, GLint X, GLint Y, int W, int H, float BGColor[4], float FGColor[4], char * String)
{
	// NOTES:
	//  - if X or Y == -1, text appears at (window width/height - text width/height)
	//  - if W or H == -1, text background is sized via text width/height

	int i, j;

	if(HUD[Index].IsUsed) glDeleteLists(HUD[Index].DL, 1);
	HUD[Index].DL = glGenLists(1);

	HUD[Index].IsUsed = true;

	strcpy(TempString, String);
	unsigned char LineText[512][MAX_PATH];
	int Lines = 0, LineWidths[512], Width = 0;

	memset(LineText, 0x00, ArraySize(LineText));
	memset(LineWidths, 0x00, ArraySize(LineWidths));

	char * pch;
	pch = strtok(TempString, "\n");
	while(pch != NULL) {
		strcpy(LineText[Lines], pch);
		Lines++;
		pch = strtok(NULL, "\n");
	}

	for(i = 0; i < Lines; i++) {
		for(j = 0; j < strlen(LineText[i]); j++) {
			if(LineText[i][j] == '\t') LineText[i][j] = 0x7F;
			LineWidths[i] += Font.CharWidths[LineText[i][j] - 32];
		}
		if(LineWidths[i] > Width) Width = LineWidths[i];
	}

	int RectWidth = Width + 5;
	int RectHeight = (Lines * 10) + 3;

	if(W != -1) RectWidth = W;
	if(H != -1) RectHeight = H;

	if(X == -1) X = WINDOW_WIDTH - RectWidth;
	if(Y == -1) Y = WINDOW_HEIGHT - RectHeight;

	if(X + RectWidth > WINDOW_WIDTH) X -= RectWidth;
	if(Y + RectHeight > WINDOW_HEIGHT) Y -= RectHeight;

	glNewList(HUD[Index].DL, GL_COMPILE);
		glPushMatrix();
		glLoadIdentity();
		glTranslated(X, Y, 0);

		glColor4f(BGColor[0], BGColor[1], BGColor[2], BGColor[3]);
		glRectf(0, 0, RectWidth, RectHeight);

		// text
		glTranslated(3, 3, 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Font.TexID);
		glListBase(Font.BaseDL - 32);

		for(i = 0; i < Lines; i++) {
			int HorzCenter = 0;
			glColor4f(FGColor[0], FGColor[1], FGColor[2], FGColor[3]);
			switch(LineText[i][0]) {
				case 0x80: {
					HorzCenter = (WINDOW_WIDTH / 2) - (LineWidths[i] / 2);
					break;
				}
				case 0x90: {
					glColor4f(0.0f, 1.0f, 0.0f, FGColor[3]);
					break;
				}
				case 0x91: {
					glColor4f(1.0f, 0.5f, 0.0f, FGColor[3]);
					break;
				}
				case 0x92: {
					glColor4f(0.0f, 0.75f, 1.0f, FGColor[3]);
					break;
				}
				case 0xA0: {
					HorzCenter = (WINDOW_WIDTH / 2) - (LineWidths[i] / 2);
					glColor4f(0.0f, 1.0f, 0.0f, FGColor[3]);
					break;
				}
				case 0xA1: {
					HorzCenter = (WINDOW_WIDTH / 2) - (LineWidths[i] / 2);
					glColor4f(1.0f, 0.5f, 0.0f, FGColor[3]);
					break;
				}
				case 0xA2: {
					HorzCenter = (WINDOW_WIDTH / 2) - (LineWidths[i] / 2);
					glColor4f(0.0f, 0.75f, 1.0f, FGColor[3]);
					break;
				}
			}
			glPushMatrix();
			glTranslated(HorzCenter, (i * 10), 0);
			glCallLists(strlen(LineText[i]), GL_BYTE, LineText[i]);
			glPopMatrix();
		}

		glDisable(GL_TEXTURE_2D);

		glPopMatrix();
	glEndList();
}

int hud_GetFreeObjectIndex()
{
	int i;
	for(i = 0; i < ArraySize(HUD); i++) {
		if(HUD[i].IsUsed == false) return i;
	}

	return -1;
}

void hud_ToggleObjectVisibility(int Index)
{
	HUD[Index].IsHidden ^= 1;
}

void hud_ClearObject(int Index)
{
	if(HUD[Index].IsUsed) glDeleteLists(HUD[Index].DL, 1);
	HUD[Index].IsUsed = false;
}

void hud_ClearAllObjects()
{
	int i;
	for(i = 0; i < ArraySize(HUD); i++) hud_ClearObject(i);
}
