extern void dl_InitCombiner();
extern void dl_ParseDisplayList(unsigned int Address);

extern void dl_UnemulatedCmd();
extern void dl_F3DEX2_VTX();
extern void dl_F3DEX2_MODIFYVTX();
extern void dl_F3DEX2_CULLDL();
extern void dl_F3DEX2_BRANCH_Z();
extern void dl_F3DEX2_TRI1();
extern void dl_F3DEX2_TRI2();
extern void dl_F3DEX2_QUAD();
extern void dl_F3DEX2_SPECIAL_3();
extern void dl_F3DEX2_SPECIAL_2();
extern void dl_F3DEX2_SPECIAL_1();
extern void dl_F3DEX2_DMA_IO();
extern void dl_F3DEX2_TEXTURE();
extern void dl_F3DEX2_POPMTX();
extern void dl_F3DEX2_GEOMETRYMODE();
extern void dl_F3DEX2_MTX();
extern void dl_F3DEX2_MOVEWORD();
extern void dl_F3DEX2_MOVEMEM();
extern void dl_F3DEX2_LOAD_UCODE();
extern void dl_F3DEX2_DL();
extern void dl_F3DEX2_ENDDL();
extern void dl_F3DEX2_SPNOOP();
extern void dl_F3DEX2_RDPHALF_1();
extern void dl_F3DEX2_SETOTHERMODE_L();
extern void dl_F3DEX2_SETOTHERMODE_H();
extern void dl_G_TEXRECT();
extern void dl_G_TEXRECTFLIP();
extern void dl_G_RDPLOADSYNC();
extern void dl_G_RDPPIPESYNC();
extern void dl_G_RDPTILESYNC();
extern void dl_G_RDPFULLSYNC();
extern void dl_G_SETKEYGB();
extern void dl_G_SETKEYR();
extern void dl_G_SETCONVERT();
extern void dl_G_SETSCISSOR();
extern void dl_G_SETPRIMDEPTH();
extern void dl_G_RDPSETOTHERMODE();
extern void dl_G_LOADTLUT();
extern void dl_F3DEX2_RDPHALF_2();
extern void dl_G_SETTILESIZE();
extern void dl_G_LOADBLOCK();
extern void dl_G_LOADTILE();
extern void dl_G_SETTILE();
extern void dl_G_FILLRECT();
extern void dl_G_SETFILLCOLOR();
extern void dl_G_SETFOGCOLOR();
extern void dl_G_SETBLENDCOLOR();
extern void dl_G_SETPRIMCOLOR();
extern void dl_G_SETENVCOLOR();
extern void dl_G_SETCOMBINE();
extern void dl_G_SETTIMG();
extern void dl_G_SETZIMG();
extern void dl_G_SETCIMG();

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

#define G_TEXTURE_ENABLE		0x00000000
#define G_ZBUFFER				0x00000001
#define G_SHADE					0x00000004
#define G_CULL_FRONT			0x00000200
#define G_CULL_BACK				0x00000400
#define G_CULL_BOTH				0x00000600
#define G_FOG					0x00010000
#define G_LIGHTING				0x00020000
#define G_TEXTURE_GEN			0x00040000
#define G_TEXTURE_GEN_LINEAR	0x00080000
#define G_LOD					0x00100000
#define G_SHADING_SMOOTH		0x00200000
#define G_CLIPPING				0x00800000

#define	G_MDSFT_ALPHACOMPARE	0
#define	G_MDSFT_ZSRCSEL			2
#define	G_MDSFT_RENDERMODE		3

#define	G_MDSFT_ALPHADITHER		4
#define	G_MDSFT_RGBDITHER		6
#define	G_MDSFT_COMBKEY			8
#define	G_MDSFT_TEXTCONV		9
#define	G_MDSFT_TEXTFILT		12
#define	G_MDSFT_TEXTLUT			14
#define	G_MDSFT_TEXTLOD			16
#define	G_MDSFT_TEXTDETAIL		17
#define	G_MDSFT_TEXTPERSP		19
#define	G_MDSFT_CYCLETYPE		20
#define	G_MDSFT_PIPELINE		23

#define	G_TX_WRAP				0x00
#define	G_TX_MIRROR				0x01
#define	G_TX_CLAMP				0x02

#define G_CCMUX_COMBINED		0
#define G_CCMUX_TEXEL0			1
#define G_CCMUX_TEXEL1			2
#define G_CCMUX_PRIMITIVE		3
#define G_CCMUX_SHADE			4
#define G_CCMUX_ENVIRONMENT		5
#define G_CCMUX_CENTER			6
#define G_CCMUX_SCALE			6
#define G_CCMUX_COMBINED_ALPHA	7
#define G_CCMUX_TEXEL0_ALPHA	8
#define G_CCMUX_TEXEL1_ALPHA	9
#define G_CCMUX_PRIMITIVE_ALPHA	10
#define G_CCMUX_SHADE_ALPHA		11
#define G_CCMUX_ENV_ALPHA		12
#define G_CCMUX_LOD_FRACTION	13
#define G_CCMUX_PRIM_LOD_FRAC	14
#define G_CCMUX_NOISE			7
#define G_CCMUX_K4				7
#define G_CCMUX_K5				15
#define G_CCMUX_1				6
#define G_CCMUX_0				31

#define G_ACMUX_COMBINED		0
#define G_ACMUX_TEXEL0			1
#define G_ACMUX_TEXEL1			2
#define G_ACMUX_PRIMITIVE		3
#define G_ACMUX_SHADE			4
#define G_ACMUX_ENVIRONMENT		5
#define G_ACMUX_LOD_FRACTION	0
#define G_ACMUX_PRIM_LOD_FRAC	6
#define G_ACMUX_1				6
#define G_ACMUX_0				7

#define	AA_EN					0x00000008
#define	Z_CMP					0x00000010
#define	Z_UPD					0x00000020
#define	IM_RD					0x00000040
#define	CLR_ON_CVG				0x00000080
#define	CVG_DST_CLAMP			0x00000000
#define	CVG_DST_WRAP			0x00000100
#define	CVG_DST_FULL			0x00000200
#define	CVG_DST_SAVE			0x00000300
#define	ZMODE_OPA				0x00000000
#define	ZMODE_INTER				0x00000400
#define	ZMODE_XLU				0x00000800
#define	ZMODE_DEC				0x00000C00
#define	CVG_X_ALPHA				0x00001000
#define	ALPHA_CVG_SEL			0x00002000
#define	FORCE_BL				0x00004000

#define CHANGED_GEOMETRYMODE	0x01
#define CHANGED_RENDERMODE		0x02
#define CHANGED_ALPHACOMPARE	0x04
