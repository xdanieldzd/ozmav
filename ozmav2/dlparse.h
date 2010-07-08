typedef void (*RDPInstruction)();
extern RDPInstruction dl_UcodeCmd[256];

extern enum { F3D, F3DEX, F3DEX2 } UcodeIDs;

typedef bool (*CheckAddressValidity)(unsigned int Address);
extern CheckAddressValidity dl_CheckAddressValidity;

// ----------------------------------------

extern unsigned int DListAddress;
extern unsigned char Segment; extern unsigned int Offset;

extern unsigned int w0, w1;
extern unsigned int wp0, wp1;
extern unsigned int wn0, wn1;

extern unsigned int G_TEXTURE_ENABLE;
extern unsigned int G_SHADING_SMOOTH;
extern unsigned int G_CULL_FRONT;
extern unsigned int G_CULL_BACK;
extern unsigned int G_CULL_BOTH;
extern unsigned int G_CLIPPING;

#define dl_InitFlags(Ucode) \
	G_TEXTURE_ENABLE	= Ucode##_TEXTURE_ENABLE; \
	G_SHADING_SMOOTH	= Ucode##_SHADING_SMOOTH; \
	G_CULL_FRONT		= Ucode##_CULL_FRONT; \
	G_CULL_BACK			= Ucode##_CULL_BACK; \
	G_CULL_BOTH			= Ucode##_CULL_BOTH; \
	G_CLIPPING			= Ucode##_CLIPPING;

// ----------------------------------------

extern void dl_InitParser(int UcodeID);
extern void dl_InitCombiner();
extern void dl_ParseDisplayList(unsigned int Address);
extern void dl_DrawTriangle(int Vtxs[]);
extern void dl_UpdateGeometryMode();
extern void dl_SetRenderMode(unsigned int Mode1, unsigned int Mode2);
extern void dl_CheckFragmentCache();
extern void dl_CreateCombinerProgram(unsigned int Cmb0, unsigned int Cmb1);
extern void dl_ChangeTileSize(unsigned int Tile, unsigned int ULS, unsigned int ULT, unsigned int LRS, unsigned int LRT);
extern void dl_CalcTextureSize(int TextureID);
inline unsigned long dl_Pow2(unsigned long dim);
inline unsigned long dl_PowOf(unsigned long dim);
extern void dl_InitLoadTexture();
extern GLuint dl_CheckTextureCache(unsigned int TexID);
extern GLuint dl_LoadTexture(int TextureID);

// ----------------------------------------

#define CHANGED_GEOMETRYMODE	0x01
#define CHANGED_RENDERMODE		0x02
#define CHANGED_ALPHACOMPARE	0x04
