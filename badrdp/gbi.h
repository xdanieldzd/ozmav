#define	G_SETCIMG				0xFF
#define	G_SETZIMG				0xFE
#define	G_SETTIMG				0xFD
#define	G_SETCOMBINE			0xFC
#define	G_SETENVCOLOR			0xFB
#define	G_SETPRIMCOLOR			0xFA
#define	G_SETBLENDCOLOR			0xF9
#define	G_SETFOGCOLOR			0xF8
#define	G_SETFILLCOLOR			0xF7
#define	G_FILLRECT				0xF6
#define	G_SETTILE				0xF5
#define	G_LOADTILE				0xF4
#define	G_LOADBLOCK				0xF3
#define	G_SETTILESIZE			0xF2
#define	G_LOADTLUT				0xF0
#define	G_RDPSETOTHERMODE		0xEF
#define	G_SETPRIMDEPTH			0xEE
#define	G_SETSCISSOR			0xED
#define	G_SETCONVERT			0xEC
#define	G_SETKEYR				0xEB
#define	G_SETKEYGB				0xEA
#define	G_RDPFULLSYNC			0xE9
#define	G_RDPTILESYNC			0xE8
#define	G_RDPPIPESYNC			0xE7
#define	G_RDPLOADSYNC			0xE6
#define G_TEXRECTFLIP			0xE5
#define G_TEXRECT				0xE4

// ----------------------------------------

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
/*
#define	ZMODE_OPA				0x00000000
#define	ZMODE_INTER				0x00000400
#define	ZMODE_XLU				0x00000800
#define	ZMODE_DEC				0x00000C00
*/
#define	CVG_X_ALPHA				0x00001000
#define	ALPHA_CVG_SEL			0x00002000
#define	FORCE_BL				0x00004000

#define G_MWO_POINT_RGBA		0x10
#define G_MWO_POINT_ST			0x14

#define	ZMODE_OPA				0
#define	ZMODE_INTER				1
#define	ZMODE_XLU				2
#define	ZMODE_DEC				3

#define	G_CYC_1CYCLE			0
#define	G_CYC_2CYCLE			1
#define	G_CYC_COPY				2
#define	G_CYC_FILL				3

#define G_MW_MATRIX			0x00
#define G_MW_NUMLIGHT		0x02
#define G_MW_CLIP			0x04
#define G_MW_SEGMENT		0x06
#define G_MW_FOG			0x08
#define G_MW_LIGHTCOL		0x0A
#define G_MW_FORCEMTX		0x0C
#define G_MW_POINTS			0x0C
#define	G_MW_PERSPNORM		0x0E
