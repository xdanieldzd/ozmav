typedef void (*RDPInstruction)();

static RDPInstruction dl_UcodeCmd[256] =
{
	/* 00-3F */
	dl_UnemulatedCmd,			dl_F3DEX_MTX,				dl_UnemulatedCmd,			dl_F3DEX_MOVEMEM,
	dl_F3DEX_VTX,				dl_UnemulatedCmd,			dl_F3DEX_DL,				dl_UnemulatedCmd,
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
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_F3DEX_LOAD_UCODE,

	dl_F3DEX_BRANCH_Z,			dl_F3DEX_TRI2,				dl_F3DEX_MODIFYVTX,			dl_F3DEX_RDPHALF_2,
	dl_F3DEX_RDPHALF_1,			dl_UnemulatedCmd,			dl_F3DEX_CLEARGEOMETRYMODE,	dl_F3DEX_SETGEOMETRYMODE,
	dl_F3DEX_ENDDL,				dl_F3DEX_SETOTHERMODE_L,	dl_F3DEX_SETOTHERMODE_H,	dl_F3DEX_TEXTURE,
	dl_F3DEX_MOVEWORD,			dl_F3DEX_POPMTX,			dl_F3DEX_CULLDL,			dl_F3DEX_TRI1,

	/* C0-FF */
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,

	dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,			dl_UnemulatedCmd,
	dl_G_TEXRECT,				dl_G_TEXRECTFLIP,			dl_G_RDPLOADSYNC,			dl_G_RDPPIPESYNC,
	dl_G_RDPTILESYNC,			dl_G_RDPFULLSYNC,			dl_G_SETKEYGB,				dl_G_SETKEYR,
	dl_G_SETCONVERT,			dl_G_SETSCISSOR,			dl_G_SETPRIMDEPTH,			dl_G_RDPSETOTHERMODE,

	dl_G_LOADTLUT,				dl_UnemulatedCmd,			dl_G_SETTILESIZE,			dl_G_LOADBLOCK,
	dl_G_LOADTILE,				dl_G_SETTILE,				dl_G_FILLRECT,				dl_G_SETFILLCOLOR,
	dl_G_SETFOGCOLOR,			dl_G_SETBLENDCOLOR,			dl_G_SETPRIMCOLOR,			dl_G_SETENVCOLOR,
	dl_G_SETCOMBINE,			dl_G_SETTIMG,				dl_G_SETZIMG,				dl_G_SETCIMG
};
