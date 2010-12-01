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

	if(glIsTexture(vHUD.TexID)) glDeleteTextures(1, &vHUD.TexID);
	glGenTextures(1, &vHUD.TexID);

	glBindTexture(GL_TEXTURE_2D, vHUD.TexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, vHUD.Width, vHUD.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, vHUD.Image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	memcpy(vHUD.CharWidths, FontWidths, arraySize(FontWidths));

	hud_BuildFont();

	if(vHUD.Image != NULL) free(vHUD.Image);

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

	memcpy(&vHUD.Width, &Buffer[18], sizeof(int));
	memcpy(&vHUD.Height, &Buffer[22], sizeof(int));
	memcpy(&vHUD.Plane, &Buffer[26], sizeof(short));
	memcpy(&vHUD.BPP, &Buffer[28], sizeof(short));

	if(vHUD.BPP != 24) {
		MSK_ConsolePrint(MSK_COLORTYPE_ERROR, "- Error: BMP data is not 24bpp!\n");
		return false;
	}

	vHUD.Image = (unsigned char *)malloc(sizeof(char) * vHUD.Width * vHUD.Height * 4);

	int Size = vHUD.Width * vHUD.Height * (vHUD.BPP / 8);
	unsigned char * TempImage = (unsigned char *)malloc(sizeof(char) * Size);

	memcpy(TempImage, &Buffer[54], sizeof(char) * Size);

	int BytesPx = (vHUD.BPP / 8);

	int X, Y, Y2 = 0;
	int SrcOffset = 0;

	for(Y = vHUD.Height; Y > 0; Y--, Y2++) {
		for(X = 0; X < vHUD.Width * BytesPx; X += BytesPx) {
			// check for transparency color key (255, 0, 255)
            if(	TempImage[((Y - 1) * vHUD.Width) * BytesPx + X + 2] == ColorKey[0] &&
				TempImage[((Y - 1) * vHUD.Width) * BytesPx + X + 1] == ColorKey[1] &&
				TempImage[((Y - 1) * vHUD.Width) * BytesPx + X + 0] == ColorKey[2])
			{
				vHUD.Image[SrcOffset + 3]	= 0;

			// check for width marker color key (255, 255, 0)
			} else if(
				TempImage[((Y - 1) * vHUD.Width) * BytesPx + X + 2] == WidthKey[0] &&
				TempImage[((Y - 1) * vHUD.Width) * BytesPx + X + 1] == WidthKey[1] &&
				TempImage[((Y - 1) * vHUD.Width) * BytesPx + X + 0] == WidthKey[2]) {

				int CharNo = ((Y2 * vHUD.Width / 8) / 8) + ((X / BytesPx) / 8);
				int Width = ((X / BytesPx) - (((X / BytesPx) / 8) * 8)) + 1;
				FontWidths[CharNo] = Width;

				vHUD.Image[SrcOffset + 3]	= 0;
			} else {
				vHUD.Image[SrcOffset + 3]	= 0xFF;
			}

			// copy image
			vHUD.Image[SrcOffset + 2]	= TempImage[((Y - 1) * vHUD.Width) * BytesPx + X + 0];
			vHUD.Image[SrcOffset + 1]	= TempImage[((Y - 1) * vHUD.Width) * BytesPx + X + 1];
			vHUD.Image[SrcOffset + 0]	= TempImage[((Y - 1) * vHUD.Width) * BytesPx + X + 2];
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

	vHUD.BaseDL = glGenLists(128);
	glBindTexture(GL_TEXTURE_2D, vHUD.TexID);

	for(i = 0; i < 128; i++) {
		CharX = ((float)(i % 16)) / 16.0f;
		CharY = ((float)(i / 16)) / 8.0f;

		glNewList(vHUD.BaseDL + i, GL_COMPILE);
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
			glTranslated(vHUD.CharWidths[i], 0, 0);
		glEndList();
	}
}

void hud_KillFont()
{
	if(glIsList(vHUD.BaseDL)) glDeleteLists(vHUD.BaseDL, 256);
}

void hud_Print(GLint X, GLint Y, int W, int H, char * String)
{
	// NOTES:
	//  - if X or Y == -1, text appears at (window width/height - text width/height)
	//  - if W or H == -1, text background is sized via text width/height

	int i, j;

	strcpy(TempString, String);
	unsigned char LineText[512][MAX_PATH];
	int Lines = 0, LineWidths[512], Width = 0;

	memset(LineText, 0x00, arraySize(LineText));
	memset(LineWidths, 0x00, arraySize(LineWidths));

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
			LineWidths[i] += vHUD.CharWidths[LineText[i][j] - 32];
		}
		if(LineWidths[i] > Width) Width = LineWidths[i];
	}

	int RectWidth = Width + 5;
	int RectHeight = (Lines * 10) + 3;

	if(W != -1) RectWidth = W;
	if(H != -1) RectHeight = H;

	if(X == -1) X = vProgram.windowWidth - RectWidth;
	if(Y == -1) Y = vProgram.windowHeight - RectHeight;

	if(X + RectWidth > vProgram.windowWidth) X -= RectWidth;
	if(Y + RectHeight > vProgram.windowHeight) Y -= RectHeight;

	{
		glPushMatrix();
		glLoadIdentity();
		glTranslated(X, Y, 0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(BGColor[0], BGColor[1], BGColor[2], BGColor[3]);
		glRectf(0, 0, RectWidth, RectHeight);

		// text
		glTranslated(3, 3, 0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, vHUD.TexID);
		glListBase(vHUD.BaseDL - 32);

		for(i = 0; i < Lines; i++) {
			int HorzCenter = 0;
			switch(LineText[i][0]) {
				case 0x80: {
					HorzCenter = (vProgram.windowWidth / 2) - (LineWidths[i] / 2);
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
					HorzCenter = (vProgram.windowWidth / 2) - (LineWidths[i] / 2);
					glColor4f(0.0f, 1.0f, 0.0f, FGColor[3]);
					break;
				}
				case 0xA1: {
					HorzCenter = (vProgram.windowWidth / 2) - (LineWidths[i] / 2);
					glColor4f(1.0f, 0.5f, 0.0f, FGColor[3]);
					break;
				}
				case 0xA2: {
					HorzCenter = (vProgram.windowWidth / 2) - (LineWidths[i] / 2);
					glColor4f(0.0f, 0.75f, 1.0f, FGColor[3]);
					break;
				}

				default: {
					glColor4f(FGColor[0], FGColor[1], FGColor[2], FGColor[3]);
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
	}
}
