/*
	rvl.c - Wii-specific functions
*/

#include "globals.h"

#ifdef HW_RVL
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void rvlInitFat()
{
	// temporary init video, pad, console
	VIDEO_Init();
	WPAD_Init();
	rmode = VIDEO_GetPreferredMode(NULL);
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	CON_Init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();
	// loop until either Home has been pressed, or an SD card has been found
	while(!fatInitDefault()) {
		WPAD_ScanPads();
		printf("\x1b[2;0HFAT initialization error!");
		printf("\x1b[3;0HPlease insert a SD card to continue or press Home to exit!");
		u32 pressed = WPAD_ButtonsDown(0);
		if(pressed & WPAD_BUTTON_HOME) {
			printf("\x1b[5;0HExiting...");
			exit(0);
		}
		VIDEO_WaitVSync();
	}
	printf("\x1b[2;0H");
	// set the app path
	strcpy(program.apppath, "sd:/apps/wii_azunyan8");
}
#endif
