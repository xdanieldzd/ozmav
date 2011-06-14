#include "globals.h"
#include "font.h"

char TempString[8192];

unsigned char FontWidths[128];

float BGColor[4] = { 0.1f, 0.1f, 0.1f, 0.5f };
float FGColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

int hud_Init()
{
	memset(FontWidths, 2, sizeof(FontWidths));

	if(!hud_LoadFontBuffer(fontdata)) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Could not load font data!\n");
		return EXIT_FAILURE;
	}

	if(glIsTexture(zHUD.TexID)) glDeleteTextures(1, &zHUD.TexID);
	glGenTextures(1, &zHUD.TexID);

	glBindTexture(GL_TEXTURE_2D, zHUD.TexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, zHUD.Width, zHUD.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, zHUD.Image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	memcpy(zHUD.CharWidths, FontWidths, ArraySize(FontWidths));

	hud_BuildFont();

	if(zHUD.Image != NULL) free(zHUD.Image);

	return EXIT_SUCCESS;
}

bool hud_LoadFontBuffer(unsigned char * Buffer)
{
	int ColorKey[3] = { 0xFF, 0x00, 0xFF };
	int WidthKey[3] = { 0xFF, 0xFF, 0x00 };

	char TempID[] = { 0, 0, 0 };
	memcpy(&TempID, &Buffer[0], 2);
	if(strcmp(TempID, "BM")) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: Font data not in BMP format!\n");
		return false;
	}

	memcpy(&zHUD.Width, &Buffer[18], sizeof(int));
	memcpy(&zHUD.Height, &Buffer[22], sizeof(int));
	memcpy(&zHUD.Plane, &Buffer[26], sizeof(short));
	memcpy(&zHUD.BPP, &Buffer[28], sizeof(short));

	if(zHUD.BPP != 24) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: BMP data is not 24bpp!\n");
		return false;
	}

	zHUD.Image = (unsigned char *)malloc(sizeof(char) * zHUD.Width * zHUD.Height * 4);

	int Size = zHUD.Width * zHUD.Height * (zHUD.BPP / 8);
	unsigned char * TempImage = (unsigned char *)malloc(sizeof(char) * Size);

	memcpy(TempImage, &Buffer[54], sizeof(char) * Size);

	int BytesPx = (zHUD.BPP / 8);

	int X, Y, Y2 = 0;
	int SrcOffset = 0;

	for(Y = zHUD.Height; Y > 0; Y--, Y2++) {
		for(X = 0; X < zHUD.Width * BytesPx; X += BytesPx) {
			// check for transparency color key
            if(	TempImage[((Y - 1) * zHUD.Width) * BytesPx + X + 2] == ColorKey[0] &&
				TempImage[((Y - 1) * zHUD.Width) * BytesPx + X + 1] == ColorKey[1] &&
				TempImage[((Y - 1) * zHUD.Width) * BytesPx + X + 0] == ColorKey[2])
			{
				zHUD.Image[SrcOffset + 3]	= 0;

			// check for width marker color key (255, 255, 0)
			} else if(
				TempImage[((Y - 1) * zHUD.Width) * BytesPx + X + 2] == WidthKey[0] &&
				TempImage[((Y - 1) * zHUD.Width) * BytesPx + X + 1] == WidthKey[1] &&
				TempImage[((Y - 1) * zHUD.Width) * BytesPx + X + 0] == WidthKey[2])
			{
				int CharNo = ((Y2 * zHUD.Width / 8) / 8) + ((X / BytesPx) / 8);
				int Width = ((X / BytesPx) - (((X / BytesPx) / 8) * 8)) + 1;
				FontWidths[CharNo] = Width;

				zHUD.Image[SrcOffset + 3]	= 0;

			// if not a special marker, use full alpha
			} else {
				zHUD.Image[SrcOffset + 3]	= 0xFF;
			}

			// copy image
			zHUD.Image[SrcOffset + 2]	= TempImage[((Y - 1) * zHUD.Width) * BytesPx + X + 0];
			zHUD.Image[SrcOffset + 1]	= TempImage[((Y - 1) * zHUD.Width) * BytesPx + X + 1];
			zHUD.Image[SrcOffset + 0]	= TempImage[((Y - 1) * zHUD.Width) * BytesPx + X + 2];
			SrcOffset += 4;
		}
	}

	free(TempImage);

	return true;
}

void hud_BuildFont()
{
	int i;
	float CharX, CharY;

	zHUD.BaseDL = glGenLists(128);
	glBindTexture(GL_TEXTURE_2D, zHUD.TexID);

	for(i = 0; i < 128; i++) {
		CharX = ((float)(i % 16)) / 16.0f;
		CharY = ((float)(i / 16)) / 8.0f;

		glNewList(zHUD.BaseDL + i, GL_COMPILE);
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
			glTranslated(zHUD.CharWidths[i], 0, 0);
		glEndList();
	}
}

void hud_KillFont()
{
	if(glIsList(zHUD.BaseDL)) glDeleteLists(zHUD.BaseDL, 256);
}

void hud_Print(GLint X, GLint Y, int W, int H, int Scale, float Vis, bool Border, char * String, ...)
{
	// NOTES:
	//  - if X or Y == -1, text appears at (window width/height - text width/height)
	//  - if W or H == -1, text background is sized via text width/height

	char Text[256];
	va_list argp;
	if(String == NULL) return;
	va_start(argp, String);
	vsprintf(Text, String, argp);
	va_end(argp);

	int i, j;

	strcpy(TempString, Text);
	unsigned char LineText[512][MAX_PATH];
	int Lines = 0, LineWidths[512], Width = 0;

	memset(LineText, 0x00, ArraySize(LineText));
	memset(LineWidths, 0x00, ArraySize(LineWidths));

	char * pch;
	pch = strtok(TempString, "\n");
	while(pch != NULL) {
		strcpy((char*)LineText[Lines], pch);
		Lines++;
		pch = strtok(NULL, "\n");
	}

	for(i = 0; i < Lines; i++) {
		for(j = 0; j < strlen((char*)LineText[i]); j++) {
			if(LineText[i][j] == '\t') LineText[i][j] = 0x7F;
			if(LineText[i][j] < 0x80) LineWidths[i] += zHUD.CharWidths[LineText[i][j] - 32];
		}
		if(LineWidths[i] > Width) Width = LineWidths[i];
	}

	int RectWidth = Width + 5;
	int RectHeight = (Lines * 10) + 3;

	if(W != -1) RectWidth = W;
	if(H != -1) RectHeight = H;

	if(X == -1) X = zProgram.WindowWidth - RectWidth;
	if(Y == -1) Y = zProgram.WindowHeight - RectHeight;

	if(X + RectWidth > zProgram.WindowWidth) X -= RectWidth;
	if(Y + RectHeight > zProgram.WindowHeight) Y -= RectHeight;

	{
		glPushMatrix();
		glLoadIdentity();
		glScaled(Scale, Scale, Scale);
		glTranslated(X, Y, 0);

		if(RDP_OpenGL_ExtFragmentProgram()) glDisable(GL_FRAGMENT_PROGRAM_ARB);

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_ALPHA_TEST);

		glDisable(GL_TEXTURE_2D);
		glColor4f(BGColor[0], BGColor[1], BGColor[2], BGColor[3] * Vis);
		glRectd(0, 0, RectWidth, RectHeight);
		glEnable(GL_TEXTURE_2D);

		// text
		glPushMatrix();
		glTranslated(3, 3, 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, zHUD.TexID);
		glListBase(zHUD.BaseDL - 32);

		for(i = 0; i < Lines; i++) {
			int HorzCenter = 0;
			switch(LineText[i][0]) {
				case 0x80: {
					HorzCenter = (zProgram.WindowWidth / 2) - (LineWidths[i] / 2);
					break;
				}
				case 0x90: {
					glColor4f(0.0f, 1.0f, 0.0f, FGColor[3] * Vis);
					break;
				}
				case 0x91: {
					glColor4f(1.0f, 0.5f, 0.0f, FGColor[3] * Vis);
					break;
				}
				case 0x92: {
					glColor4f(0.0f, 0.75f, 1.0f, FGColor[3] * Vis);
					break;
				}
				case 0xA0: {
					HorzCenter = (zProgram.WindowWidth / 2) - (LineWidths[i] / 2);
					glColor4f(0.0f, 1.0f, 0.0f, FGColor[3] * Vis);
					break;
				}
				case 0xA1: {
					HorzCenter = (zProgram.WindowWidth / 2) - (LineWidths[i] / 2);
					glColor4f(1.0f, 0.5f, 0.0f, FGColor[3] * Vis);
					break;
				}
				case 0xA2: {
					HorzCenter = (zProgram.WindowWidth / 2) - (LineWidths[i] / 2);
					glColor4f(0.0f, 0.75f, 1.0f, FGColor[3] * Vis);
					break;
				}

				default: {
					glColor4f(FGColor[0], FGColor[1], FGColor[2], FGColor[3] * Vis);
					break;
				}
			}
			glPushMatrix();
			glTranslated(HorzCenter, (i * 10), 0);
			glCallLists(strlen((char*)LineText[i]), GL_BYTE, LineText[i]);
			glPopMatrix();
		}
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();

		if(Border) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glColor4f(0.0f, 0.0f, 0.0f, Vis);
			glRectd(0, 0, RectWidth, RectHeight);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glEnable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);

		glPopMatrix();
	}
}
