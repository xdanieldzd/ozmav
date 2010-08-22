extern int hud_Init(unsigned char FontPath[]);
extern bool hud_LoadFontBMP(unsigned char Path[]);
extern void hud_BuildFont();
extern void hud_KillFont();
extern void hud_Print(int Index, GLint X, GLint Y, int W, int H, float BGColor[4], float FGColor[4], char * String);
extern int hud_GetFreeObjectIndex();
extern void hud_ToggleObjectVisibility(int Index);
extern void hud_ClearObject(int Index);
extern void hud_ClearAllObjects();

struct __Font {
	unsigned char * Image;
	GLuint TexID;
	GLuint BaseDL;
	int Width, Height, Plane, BPP;
	char CharWidths[256];
	float BGC[4];
	float FGC[4];
};

struct __HUD {
	bool IsUsed;
	bool IsHidden;
	GLuint DL;
};

struct __Font Font;
struct __HUD HUD[256];
