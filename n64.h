//ALL UCODES - BASIC
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

//F3D - BASIC
#define F3D_SPNOOP				0x00
#define F3D_MTX					0x01
#define F3D_RESERVED0			0x02
#define F3D_MOVEMEM				0x03
#define F3D_VTX					0x04
#define F3D_RESERVED1			0x05
#define F3D_DL					0x06
#define F3D_RESERVED2			0x07
#define F3D_RESERVED3			0x08
#define F3D_SPRITE2D_BASE		0x09
//F3Dxx - SHARED
#define F3D_TRI1				0xBF
#define F3D_CULLDL				0xBE
#define F3D_POPMTX				0xBD
#define F3D_MOVEWORD			0xBC
#define F3D_TEXTURE				0xBB
#define F3D_SETOTHERMODE_H		0xBA
#define F3D_SETOTHERMODE_L		0xB9
#define F3D_ENDDL				0xB8
#define F3D_SETGEOMETRYMODE		0xB7
#define F3D_CLEARGEOMETRYMODE	0xB6
#define F3D_QUAD				0xB5
#define F3D_RDPHALF_1			0xB4
#define F3D_RDPHALF_2			0xB3
#define F3D_RDPHALF_CONT		0xB2
#define F3D_TRI4				0xB1
//F3DEX - SPECIFIC
#define F3DEX_MODIFYVTX			0xB2
#define F3DEX_TRI2				0xB1
#define F3DEX_BRANCH_Z			0xB0
#define F3DEX_LOAD_UCODE		0xAF

//F3DEX2 - BASIC
#define	F3DEX2_VTX				0x01
#define	F3DEX2_MODIFYVTX		0x02
#define	F3DEX2_CULLDL			0x03
#define	F3DEX2_BRANCH_Z			0x04
#define	F3DEX2_TRI1				0x05
#define F3DEX2_TRI2				0x06
#define F3DEX2_QUAD				0x07
//F3DEX2 - SPECIFIC
#define	F3DEX2_RDPHALF_2		0xF1
#define	F3DEX2_SETOTHERMODE_H	0xE3
#define	F3DEX2_SETOTHERMODE_L	0xE2
#define	F3DEX2_RDPHALF_1		0xE1
#define	F3DEX2_SPNOOP			0xE0
#define	F3DEX2_ENDDL			0xDF
#define	F3DEX2_DL				0xDE
#define	F3DEX2_LOAD_UCODE		0xDD
#define	F3DEX2_MOVEMEM			0xDC
#define	F3DEX2_MOVEWORD			0xDB
#define	F3DEX2_MTX				0xDA
#define F3DEX2_GEOMETRYMODE		0xD9
#define	F3DEX2_POPMTX			0xD8
#define	F3DEX2_TEXTURE			0xD7
#define	F3DEX2_DMA_IO			0xD6
#define	F3DEX2_SPECIAL_1		0xD5
#define	F3DEX2_SPECIAL_2		0xD4
#define	F3DEX2_SPECIAL_3		0xD3

/* for SetGeometryMode */
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

/* for SetTile */
#define	G_TX_WRAP				0x00
#define	G_TX_MIRROR				0x01
#define	G_TX_CLAMP				0x02

/* for SetOtherMode_L */
#define	G_MDSFT_ALPHACOMPARE	0
#define	G_MDSFT_ZSRCSEL			2
#define	G_MDSFT_RENDERMODE		3
#define	G_MDSFT_BLENDER			16

/* for SetCombine */
/* -- color combiner -- */
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

/* -- alpha combiner -- */
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

/* for SetOtherMode_H */
#define	G_MDSFT_BLENDMASK		0
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
#define	G_MDSFT_COLORDITHER		22
#define	G_MDSFT_PIPELINE		23

#define	G_CYC_1CYCLE			0x00
#define	G_CYC_2CYCLE			0x01
#define	G_CYC_COPY				0x02
#define	G_CYC_FILL				0x03
