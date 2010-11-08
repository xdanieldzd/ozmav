#include "globals.h"

int oz_InitProgram(char * WndTitle, int Width, int Height)
{
	#ifdef WIN32
	return WinAPIInit(WndTitle, Width, Height);
	#else
	return XInit(WndTitle, Width, Height);
	#endif
}

int oz_APIMain()
{
	#ifdef WIN32
	return WinAPIMain();
	#else
	return XMain();
	#endif
}

int oz_ExitProgram()
{
	#ifdef WIN32
	WinAPIExit();
	#else
	return XExit();
	#endif

	MSK_SetLogging(false);

	return EXIT_SUCCESS;
}

int oz_SetWindowTitle(char * WndTitle)
{
	#ifdef WIN32
	return WinAPISetWindowTitle(WndTitle);
	#else
	return XSetWindowTitle(WndTitle);
	#endif
}

int oz_SetWindowSize(int Width, int Height)
{
	#ifdef WIN32
	return WinAPISetWindowSize(Width, Height);
	#else
	return oz_Unimplemented(__FUNCTION__);
	#endif
}

int oz_CreateFolder(char * Folder)
{
	#ifdef WIN32
	CreateDirectory(Folder, NULL);
	#else
	return mkdir(Folder, 0777);
	#endif

	return EXIT_SUCCESS;
}

int oz_Unimplemented(const char FuncName[])
{
	dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Platform-specific code for %s not implemented\n", FuncName);
	return EXIT_FAILURE;
}
