extern void dl_InitCombiner();
extern void dl_ParseDisplayList(unsigned int Address);

extern void dl_UnemulatedCmd();
extern void dl_F3DEX_MTX();					//0x01
extern void dl_F3DEX_MOVEMEM();				//0x03
extern void dl_F3DEX_VTX();					//0x04
extern void dl_F3DEX_DL();					//0x06
extern void dl_F3DEX_LOAD_UCODE();			//0xAF
extern void dl_F3DEX_BRANCH_Z();			//0xB0
extern void dl_F3DEX_TRI2();				//0xB1
extern void dl_F3DEX_MODIFYVTX();			//0xB2
extern void dl_F3DEX_RDPHALF_2();			//0xB3
extern void dl_F3DEX_RDPHALF_1();			//0xB4
extern void dl_F3DEX_CLEARGEOMETRYMODE();	//0xB6
extern void dl_F3DEX_SETGEOMETRYMODE();		//0xB7
extern void dl_F3DEX_ENDDL();				//0xB8
extern void dl_F3DEX_SETOTHERMODE_L();		//0xB9
extern void dl_F3DEX_SETOTHERMODE_H();		//0xBA
extern void dl_F3DEX_TEXTURE();				//0xBB
extern void dl_F3DEX_MOVEWORD();			//0xBC
extern void dl_F3DEX_POPMTX();				//0xBD
extern void dl_F3DEX_CULLDL();				//0xBE
extern void dl_F3DEX_TRI1();				//0xBF
extern void dl_G_TEXRECT();					//0xE4
extern void dl_G_TEXRECTFLIP();				//0xE5
extern void dl_G_RDPLOADSYNC();				//0xE6
extern void dl_G_RDPPIPESYNC();				//0xE7
extern void dl_G_RDPTILESYNC();				//0xE8
extern void dl_G_RDPFULLSYNC();				//0xE9
extern void dl_G_SETKEYGB();				//0xEA
extern void dl_G_SETKEYR();					//0xEB
extern void dl_G_SETCONVERT();				//0xEC
extern void dl_G_SETSCISSOR();				//0xED
extern void dl_G_SETPRIMDEPTH();			//0xEE
extern void dl_G_RDPSETOTHERMODE();			//0xEF
extern void dl_G_LOADTLUT();				//0xF0
extern void dl_G_SETTILESIZE();				//0xF2
extern void dl_G_LOADBLOCK();				//0xF3
extern void dl_G_LOADTILE();				//0xF4
extern void dl_G_SETTILE();					//0xF5
extern void dl_G_FILLRECT();				//0xF6
extern void dl_G_SETFILLCOLOR();			//0xF7
extern void dl_G_SETFOGCOLOR();				//0xF8
extern void dl_G_SETBLENDCOLOR();			//0xF9
extern void dl_G_SETPRIMCOLOR();			//0xFA
extern void dl_G_SETENVCOLOR();				//0xFB
extern void dl_G_SETCOMBINE();				//0xFC
extern void dl_G_SETTIMG();					//0xFD
extern void dl_G_SETZIMG();					//0xFE
extern void dl_G_SETCIMG();					//0xFF

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

#define F3DEX_TEXTURE_ENABLE	0x00000002
#define F3DEX_SHADING_SMOOTH	0x00000200
#define F3DEX_CULL_FRONT		0x00001000
#define F3DEX_CULL_BACK			0x00002000
#define F3DEX_CULL_BOTH			0x00003000
#define F3DEX_CLIPPING			0x00800000

#define G_ZBUFFER				0x00000001
#define G_SHADE					0x00000004
#define G_FOG					0x00010000
#define G_LIGHTING				0x00020000
#define G_TEXTURE_GEN			0x00040000
#define G_TEXTURE_GEN_LINEAR	0x00080000
#define G_LOD					0x00100000

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
