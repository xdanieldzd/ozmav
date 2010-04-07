/*
	"'This is the project's dialog data. You should not change any of the dialog and object definitions
	if you are not sure about your actions.', says MISAKA in an indifferent tone."
*/

static __MSK_UI_Dialog DlgOptions =
{
	"Options", 16, 40,
	{
		{ MSK_UI_DLGOBJ_LABEL,		1,	1,	-1,	"Following options can be changed:",	NULL },
		{ MSK_UI_DLGOBJ_NUMBERSEL,	7,	1,	2,	"Debug Level|4|0",						&zOptions.DebugLevel },
		{ MSK_UI_DLGOBJ_CHECKBOX,	3,	1,	0,	"Enable Textures",						&zOptions.EnableTextures },
		{ MSK_UI_DLGOBJ_CHECKBOX,	5,	1,	1,	"Enable Combiner",						&zOptions.EnableCombiner },
		{ MSK_UI_DLGOBJ_BUTTON,		9,	1,	3,	"Dump Scene to .obj|0",					NULL },
		{ MSK_UI_DLGOBJ_LINE,		11,	1,	-1,	"-1",									NULL },
		{ MSK_UI_DLGOBJ_BUTTON,		-1,	-1,	4,	"OK|1",									NULL }
	}
};

static __MSK_UI_Dialog DlgAbout =
{
	"About", 14, 62,
	{
		{ MSK_UI_DLGOBJ_LABEL,		1,	1,	-1,	zProgram.Title,													NULL },
		{ MSK_UI_DLGOBJ_LABEL,		2,	1,	-1,	"OpenGL Zelda Map Viewer",										NULL },
		{ MSK_UI_DLGOBJ_LABEL,		4,	1,	-1,	"Written in 2010 by xdaniel - http://ozmav.googlecode.com/",	NULL },
		{ MSK_UI_DLGOBJ_LABEL,		6,	1,	-1,	"Using PDCurses and libMISAKA Console & UI Library",			NULL },
		{ MSK_UI_DLGOBJ_LABEL,		7,	1,	-1,	"libMISAKA - Written in 2010 by xdaniel",						NULL },
		{ MSK_UI_DLGOBJ_LINE,		9,	1,	-1,	"-1",															NULL },
		{ MSK_UI_DLGOBJ_BUTTON,		-1,	-1,	0,	"OK|1",															NULL }
	}
};
