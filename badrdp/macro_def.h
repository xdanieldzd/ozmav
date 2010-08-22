typedef struct {
	int Len;
	RDPInstruction Func;
	char Cmd[32];
} __GfxMacro;

__GfxMacro GfxMacros[] =
{
	// gDPLoadTextureBlock
	{
		7, RDP_Macro_LoadTextureBlock, { G_SETTIMG, G_SETTILE, G_RDPLOADSYNC, G_LOADBLOCK, G_RDPPIPESYNC, G_SETTILE, G_SETTILESIZE }
	},

	// gDPLoadTLUT_pal16, gDPLoadTLUT_pal256
	{
		6, RDP_Macro_LoadTLUT, { G_SETTIMG, G_RDPTILESYNC, G_SETTILE, G_RDPLOADSYNC, G_LOADTLUT, G_RDPPIPESYNC }
	}
};
