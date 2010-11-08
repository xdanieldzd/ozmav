static __MSK_UI_Dialog dlgAbout =
{
	"About", 15, 62,
	{
		{ MSK_UI_DLGOBJ_LABEL,  1,  1,  -1, vProgram.wndTitle,                                       NULL },
		{ MSK_UI_DLGOBJ_LABEL,  2,  1,  -1, "OpenGL-based Zelda OoT Actor Viewer",                   NULL },
		{ MSK_UI_DLGOBJ_LABEL,  4,  1,  -1, "Written in 2010 by xdaniel & contributors",             NULL },
		{ MSK_UI_DLGOBJ_LABEL,  5,  1,  -1, "Using PDCurses and libMISAKA Console & UI Library",     NULL },
		{ MSK_UI_DLGOBJ_LABEL,  6,  1,  -1, "http://ozmav.googlecode.com/",                          NULL },
		{ MSK_UI_DLGOBJ_LABEL,  8,  1,  -1, "libMISAKA - Written in 2010 by xdaniel & contributors", NULL },
		{ MSK_UI_DLGOBJ_LINE,   10, 1,  -1, "-1",                                                    NULL },
		{ MSK_UI_DLGOBJ_BUTTON, -1, -1, 0,  "OK|1",                                                  NULL }
	}
};

static __MSK_UI_Dialog dlgOptions =
{
	"Options", 16, 50,
	{
		{ MSK_UI_DLGOBJ_LINE,      1,  1,  -1, "-1",                  NULL },
		{ MSK_UI_DLGOBJ_LABEL,     1,  2,  -1, "Rendering",           NULL },
		{ MSK_UI_DLGOBJ_CHECKBOX,  3,  1,  0,  "Enable Textures",     (short*)&vProgram.enableTextures },
		{ MSK_UI_DLGOBJ_CHECKBOX,  3,  27,  1, "Enable Wireframe",    (short*)&vProgram.enableWireframe },
		{ MSK_UI_DLGOBJ_CHECKBOX,  5,  1,  2,  "Show bone structure", (short*)&vProgram.showBones },
		{ MSK_UI_DLGOBJ_CHECKBOX,  5,  27,  3, "Play animation",      (short*)&vProgram.animPlay },
		{ MSK_UI_DLGOBJ_LINE,      7,  1,  -1, "-1",                  NULL },
		{ MSK_UI_DLGOBJ_LABEL,     7,  2,  -1, "Misc. Settings",      NULL },
		{ MSK_UI_DLGOBJ_NUMBERSEL, 9,  1,  4,  "Debug Level|4|0|0|0", (short*)&vProgram.debugLevel },
		{ MSK_UI_DLGOBJ_LINE,      11, 1,  -1, "-1",                  NULL },
		{ MSK_UI_DLGOBJ_BUTTON,    -1, -1, 5,  "OK|1",                NULL }
	}
};
