struct __zHUD {
	unsigned char * Image;
	GLuint TexID;
	GLuint BaseDL;
	int Width, Height, Plane, BPP;
	char CharWidths[256];
};

struct __zHUD zHUD;

extern int hud_Init(unsigned char FontPath[]);
extern bool hud_LoadFontBMP(unsigned char Path[]);
extern void hud_BuildFont();
extern void hud_KillFont();
extern void hud_Print(GLint X, GLint Y, int W, int H, char * String);
