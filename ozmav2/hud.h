struct __zHUD {
	unsigned char * Image;
	GLuint TexID;
	GLuint BaseDL;
	int Width, Height, Plane, BPP;
	char CharWidths[256];
};

struct __zHUD zHUD;

extern int hud_Init();
extern bool hud_LoadFontBuffer(unsigned char * Buffer);
extern void hud_BuildFont();
extern void hud_KillFont();
extern void hud_Print(GLint X, GLint Y, int W, int H, int Scale, char * String, ...);
