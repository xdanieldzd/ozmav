typedef void (*RDPInstruction)();

static RDPInstruction dl_UcodeCmd[256] =
{
	/* 00-3F */
	dl_UnemulatedCmd,			dl_F3DEX2_VTX,				dl_F3DEX2_MODIFYVTX,		dl_F3DEX2_CULLDL,
	dl_F3DEX2_BRANCH_Z,			dl_F3DEX2_TRI1,				dl_F3DEX2_TRI2,			dl_F3DEX2_QUAD,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	/* 40-7F */
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	/* 80-BF */
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	/* C0-FF */
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_F3DEX2_SPECIAL_3,
	dl_F3DEX2_SPECIAL_2,		dl_F3DEX2_SPECIAL_1,		dl_F3DEX2_DMA_IO,			dl_F3DEX2_TEXTURE,
	dl_F3DEX2_POPMTX,			dl_F3DEX2_GEOMETRYMODE,		dl_F3DEX2_MTX,				dl_F3DEX2_MOVEWORD,
	dl_F3DEX2_MOVEMEM,			dl_F3DEX2_LOAD_UCODE,		dl_F3DEX2_DL,				dl_F3DEX2_ENDDL,

	dl_F3DEX2_SPNOOP,			dl_F3DEX2_RDPHALF_1,		dl_F3DEX2_SETOTHERMODE_L,	dl_F3DEX2_SETOTHERMODE_H,
	dl_G_TEXRECT,				dl_G_TEXRECTFLIP,			dl_G_RDPLOADSYNC,			dl_G_RDPPIPESYNC,
	dl_G_RDPTILESYNC,			dl_G_RDPFULLSYNC,			dl_G_SETKEYGB,				dl_G_SETKEYR,
	dl_G_SETCONVERT,			dl_G_SETSCISSOR,			dl_G_SETPRIMDEPTH,			dl_G_RDPSETOTHERMODE,

	dl_G_LOADTLUT,				dl_F3DEX2_RDPHALF_2,		dl_G_SETTILESIZE,			dl_G_LOADBLOCK,
	dl_G_LOADTILE,				dl_G_SETTILE,				dl_G_FILLRECT,				dl_G_SETFILLCOLOR,
	dl_G_SETFOGCOLOR,			dl_G_SETBLENDCOLOR,			dl_G_SETPRIMCOLOR,			dl_G_SETENVCOLOR,
	dl_G_SETCOMBINE,			dl_G_SETTIMG,				dl_G_SETZIMG,				dl_G_SETCIMG
};
