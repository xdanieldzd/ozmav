typedef void (*RDPInstruction)();
extern RDPInstruction RDP_UcodeCmd[256];

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

extern unsigned int G_MTX_STACKSIZE;
extern unsigned int G_MTX_MODELVIEW;
extern unsigned int G_MTX_PROJECTION;
extern unsigned int G_MTX_MUL;
extern unsigned int G_MTX_LOAD;
extern unsigned int G_MTX_NOPUSH;
extern unsigned int G_MTX_PUSH;

#define RDP_InitFlags(Ucode) \
	G_TEXTURE_ENABLE	= Ucode##_TEXTURE_ENABLE; \
	G_SHADING_SMOOTH	= Ucode##_SHADING_SMOOTH; \
	G_CULL_FRONT		= Ucode##_CULL_FRONT; \
	G_CULL_BACK			= Ucode##_CULL_BACK; \
	G_CULL_BOTH			= Ucode##_CULL_BOTH; \
	G_CLIPPING			= Ucode##_CLIPPING; \
\
	G_MTX_STACKSIZE		= Ucode##_MTX_STACKSIZE; \
	G_MTX_MODELVIEW		= Ucode##_MTX_MODELVIEW; \
	G_MTX_PROJECTION	= Ucode##_MTX_PROJECTION; \
	G_MTX_MUL			= Ucode##_MTX_MUL; \
	G_MTX_LOAD			= Ucode##_MTX_LOAD; \
	G_MTX_NOPUSH		= Ucode##_MTX_NOPUSH; \
	G_MTX_PUSH			= Ucode##_MTX_PUSH;

// ----------------------------------------

extern void RDP_DrawTriangle(int Vtxs[]);
extern void RDP_UpdateGeometryMode();
extern void RDP_SetRenderMode(unsigned int Mode1, unsigned int Mode2);
extern void RDP_ChangeTileSize(unsigned int Tile, unsigned int ULS, unsigned int ULT, unsigned int LRS, unsigned int LRT);
extern void RDP_CalcTextureSize(int TextureID);
inline unsigned long RDP_Pow2(unsigned long dim);
inline unsigned long RDP_PowOf(unsigned long dim);
extern void RDP_InitLoadTexture();
extern GLuint RDP_CheckTextureCache(unsigned int TexID);
extern GLuint RDP_LoadTexture(int TextureID);

//-----------------------------------------

void (*glXGetProcAddressARB(const GLubyte *procName))();

// ----------------------------------------

#define CHANGED_GEOMETRYMODE	0x01
#define CHANGED_RENDERMODE		0x02
#define CHANGED_ALPHACOMPARE	0x04
#define CHANGED_MULT_MAT		0x08
