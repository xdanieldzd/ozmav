#define APP_TITLE 			"Azunyan-8"
#define APP_VERSION			"v3"

#ifdef HW_RVL
 #define APP_OS				"Wii"
#else
 #ifdef WIN32
  #define APP_OS			"Win32"
 #else
  #define APP_OS			"Unknown"
 #endif
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef HW_RVL
#include <gccore.h>
#include <wiiuse/wpad.h> 
#include <fat.h>

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_gfxPrimitives.h>
#else
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_ttf.h>
#include <SDL_gfxPrimitives.h>
#endif

#include "shared.h"
#include "program.h"
#include "chip8.h"
#include "draw.h"
#include "audio.h"
#include "gui.h"
#include "filebrowse.h"
#include "disasm.h"
#include "memview.h"

#define TARGET_FPS			60
#define STATMSG_MAXMSG		12
#define STATMSG_DURATION	2500

#ifdef HW_RVL
#define FILESEP '/'
#define FONTHEIGHT 10 //18
#define FONTWIDTH 6 //11
#define ZOOMMAX 10
#else
#define FILESEP '\\'
#define FONTHEIGHT 10
#define FONTWIDTH 6
#define ZOOMMAX 14
#endif

#define getX(op)	(op & 0x0F00) >> 8
#define getY(op)	(op & 0x00F0) >> 4
#define getN(op)	(op & 0x000F)
#define getKK(op)	(op & 0x00FF)
#define getNNN(op)	(op & 0x0FFF)

#define arraySize(x)	(sizeof((x)) / sizeof((x)[0]))

#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif

char * tempstr;

typedef struct {
	char text[BUFSIZ];
	int timer;
} __statmsg;

typedef struct {
	SDL_Surface* screen;
	TTF_Font* ttff;
	SDL_Color colWhite, colGreen, colRed;

	int scrWidth, scrHeight;
	int zoomFactor;

	int defNumOps;

	int isPaused;
	int isChip8Paused;
	int colorMode;
	int pauseWhenInactive;
	int enableSound;
	int startIntoDisasm;
	int disasmDefRunOps;

	char apppath[MAX_PATH];
	char filename[MAX_PATH];
	char title[BUFSIZ];

	char lastpath[MAX_PATH];

	void (*func_keydown)(SDL_KeyboardEvent *);
	void (*func_keyup)(SDL_KeyboardEvent *);
	void (*func_mousedown)(SDL_MouseButtonEvent *);
	void (*func_mousemove)(SDL_MouseMotionEvent *);
	void (*func_draw_main)();
	void (*func_draw_gui)();
	void (*func_draw_box)();

	__statmsg statmsg[STATMSG_MAXMSG];

	int inpReturn;
} __program;

typedef struct {
	unsigned char memory[0x1000];
	unsigned char regs[16];
	unsigned char hpf[16];
	unsigned short addri;
	unsigned short pc;

	unsigned short stack[16];
	int sp;

	unsigned char tDelay;
	unsigned char tSound;

	unsigned char screen[64][32];
	unsigned char keys[16];

	int numOps, numFrames;
} __interpreter;

extern __program program;
extern __interpreter interpreter;
