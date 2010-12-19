#define MAX_SEGMENTS		16

typedef struct {
	bool IsSet;
	unsigned int Size;
	int SourceCompType;
	unsigned int SourceOffset;
	unsigned char * Data;
} __RAM;

extern __RAM RAM[MAX_SEGMENTS];

typedef struct {
	bool IsSet;
	unsigned int Size;
	unsigned char * Data;
} __RDRAM;

extern __RDRAM RDRAM;

extern enum { F3D, F3DEX, F3DEX2 } UcodeIDs;

extern enum {
	BRDP_WIREFRAME	= 1,
	BRDP_TEXTURES	= 1 << 1,
	BRDP_COMBINER	= 1 << 2,
	BRDP_LOGMAX		= 1 << 3,
	BRDP_TEXCRC		= 1 << 4
} RenderOpts;

extern void RDP_SetupOpenGL();
extern void RDP_SetOpenGLDimensions(int Width, int Height);
extern void RDP_InitParser(int UcodeID);
extern void RDP_LoadToSegment(unsigned char Segment, unsigned char * Buffer, unsigned int Offset, unsigned int Size);
extern void RDP_LoadToRDRAM(unsigned char * Buffer, unsigned int Size);
extern bool RDP_SaveSegment(unsigned char Segment, unsigned char * Buffer);
extern void RDP_Yaz0Decode(unsigned char * Input, unsigned char * Output, unsigned int DecSize);
extern void RDP_MIO0Decode(unsigned char * Input, unsigned char * Output, unsigned int DecSize);
extern bool RDP_CheckAddressValidity(unsigned int Address);
extern void RDP_ClearSegment(unsigned char Segment);
extern void RDP_ClearRDRAM();
extern void RDP_ClearTextures();
extern void RDP_ClearStructures(bool Full);
extern void RDP_ParseDisplayList(unsigned int Address, bool ResetStack);
extern void RDP_CreateCombinerProgram(unsigned int Cmb0, unsigned int Cmb1);
extern void RDP_Dump_InitModelDumping(char Path[MAX_PATH], char ObjFilename[MAX_PATH], char MtlFilename[MAX_PATH]);
extern void RDP_Dump_StopModelDumping();
extern bool RDP_OpenGL_ExtFragmentProgram();
extern void RDP_SetRendererOptions(unsigned char Options);
extern unsigned char RDP_GetRendererOptions();
extern void RDP_Matrix_ModelviewLoad(float [4][4]);
extern void RDP_Matrix_ProjectionLoad(float [4][4]);
extern void RDP_Matrix_ModelviewPush();
extern void RDP_SetCycleType(unsigned int Type);
extern void RDP_SetPrimColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A);
extern void RDP_SetEnvColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A);
extern void RDP_ToggleMatrixHack();
