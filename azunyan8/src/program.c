/*
	program.c - Azunyan-8 program code (initialization, configuration and ROM loader, SDL event management, etc.)
*/

#include "globals.h"

typedef struct {
	int checksum;
	int numOps;
} __imageData;
__imageData imageData[256];

int eventFilter(const SDL_Event * event)
{
	// intercept quit event and create a keydown->escape instead
	if(event->type == SDL_QUIT) {
		SDL_Event new_event;
		new_event.type = SDL_KEYDOWN;
		new_event.key.keysym.sym = SDLK_ESCAPE;
		SDL_PushEvent(&new_event);
	}

	return 1;
}

int initProgram()
{
	program.screen = NULL;
	program.ttff = NULL;
	program.colWhite.r = 255;
	program.colWhite.g = 255;
	program.colWhite.b = 255;
	program.colGreen.r = 128;
	program.colGreen.g = 255;
	program.colGreen.b = 0;
	program.colRed.r = 255;
	program.colRed.g = 64;
	program.colRed.b = 0;

	program.scrWidth = 640;
	program.scrHeight = 480;

	program.isPaused = 1;
	program.isChip8Paused = 1;

	program.func_keydown = doMainKeydown;
	program.func_keyup = doMainKeyup;
	program.func_mousedown = NULL;
	program.func_mousemove = NULL;
	program.func_draw_main = doMainDrawing;
	program.func_draw_gui = NULL;
	program.func_draw_box = NULL;

	program.inpReturn = -1;

	unsigned int InitFlags = 0;
	#ifdef HW_RVL
	InitFlags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK;
	#else
	InitFlags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
	#endif
	if(SDL_Init(InitFlags) < 0) {
		printf("Unable to init SDL: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	atexit(SDL_Quit);

	program.screen = SDL_SetVideoMode(
		program.scrWidth, program.scrHeight,
		16, SDL_HWSURFACE | SDL_DOUBLEBUF);

	if(!program.screen) {
		printf("Unable to set video mode: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	#ifdef HW_RVL
	SDL_JoystickOpen(0);
	SDL_ShowCursor(1);
	#endif

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	if(TTF_Init()) {
		printf("Unable to init SDL_ttf (%s)\n", TTF_GetError());
		return EXIT_FAILURE;
	}

	program.ttff = TTF_OpenFont(parseFormat("%s%cVeraMono.ttf", program.apppath, FILESEP), FONTHEIGHT_SMALL);
	if(program.ttff == NULL) {
		printf("Unable to open font (%s)\n", TTF_GetError());
		return EXIT_FAILURE;
	}
	program.ttff_big = TTF_OpenFont(parseFormat("%s%cVeraMono.ttf", program.apppath, FILESEP), FONTHEIGHT_BIG);
	if(program.ttff_big == NULL) {
		printf("Unable to open font (%s)\n", TTF_GetError());
		return EXIT_FAILURE;
	}

	SDL_SetEventFilter(eventFilter);

	return EXIT_SUCCESS;
}

int calcROMChecksum()
{
	int i = 0; unsigned short chk = 0;
	for(i = 0x200; i < 0x1000; i++) chk += interpreter.memory[i];

	return chk;
}

void setOpsPerFrame(int quiet)
{
	interpreter.numFrames = interpreter.numOps / TARGET_FPS;
	if(!quiet) addStatMessage(parseFormat("Opcodes/frame set to %d.", interpreter.numOps));
}

void setDefaultConfig()
{
	// default zoom factor
	program.zoomFactor = 10;

	// pause execution when inactive
	program.pauseWhenInactive = 1;

	// default color mode to 0 (green)
	program.colorMode = 0;

	// enable sound by default
	program.enableSound = 1;

	// don't enter disassembler on start
	program.startIntoDisasm = 0;

	// default num/ops
	interpreter.numOps = 400;

	// default ops to exec in disasm
	program.disasmDefRunOps = 10;

	// clear image data struct
	int i = 0;
	for(i = 0; i < 256; i++) {
		imageData[i].checksum = -1;
		imageData[i].numOps = -1;
	}
}

int loadConfig(char * path)
{
	setDefaultConfig();

	char line[BUFSIZ];
	char var[64], val[64];
	int mode = 0;

	FILE* fp;
	fp = fopen(path, "r");
	if(fp == NULL) return EXIT_FAILURE;

	while(fgets(line, sizeof(line), fp) != NULL) {
		int i = strspn(line, "\t\n\v");
		// skip comments
		if(line[i] == '#') continue;

		// check if section header
		if(line[i] == '[') {
			// default to mode 0 ("VARIABLE=VALUE")
			mode = 0;
			// if num/ops header, use mode 1 ("CHECKSUM/HEX=NUMOPS")
			if(strcmp(line + 1, "NumOps]\n") == 0) mode = 1;
			continue;
		}

		switch(mode) {
			case 0: {
				// ("VARIABLE=VALUE")
				sscanf(line, "%64[^#=]=%64s", var, val);
				if(strcmp(var, "NumOpsDefault") == 0) {
					sscanf(val, "%i", &program.defNumOps);
					setOpsPerFrame(1);
				} else if(strcmp(var, "ZoomFactor") == 0) {
					sscanf(val, "%i", &program.zoomFactor);
					program.zoomFactor += 6;
					if(program.zoomFactor > MAX_ZOOMFACTOR) program.zoomFactor = MAX_ZOOMFACTOR;
				} else if(strcmp(var, "PauseWhenInactive") == 0) {
					sscanf(val, "%i", &program.pauseWhenInactive);
				} else if(strcmp(var, "ColorMode") == 0) {
					sscanf(val, "%i", &program.colorMode);
				} else if(strcmp(var, "EnableSound") == 0) {
					sscanf(val, "%i", &program.enableSound);
				} else if(strcmp(var, "StartIntoDisasm") == 0) {
					sscanf(val, "%i", &program.startIntoDisasm);
				} else if(strcmp(var, "DisasmOpsToRun") == 0) {
					sscanf(val, "%i", &program.disasmDefRunOps);
					if(program.disasmDefRunOps == 0) program.disasmDefRunOps = 10;
				}
				break; }
			case 1: {
				// ("CHECKSUM/HEX=NUMOPS")
				int chk, ops;
				sscanf(line, "0x%04X=%64i", &chk, &ops);
				// look for empty entry in imagedata
				for(i = 0; i < 256; i++) {
					if(imageData[i].numOps == -1) {
						// if empty, write data pair there
						imageData[i].checksum = chk;
						imageData[i].numOps = ops;
						break;
					}
				}
				break; }
		}
	}

	return EXIT_SUCCESS;
}

int loadROM(char * path)
{
	FILE* fp;
	fp = fopen(path, "rb");
	if(fp == NULL) return EXIT_FAILURE;
	memset(&interpreter.memory[0x200], 0x00, 0xE00);
	fread(&interpreter.memory[0x200], 0x1000, 1, fp);
	fclose(fp);

	int checksum = calcROMChecksum();

	char temp[BUFSIZ];
	strcpy(temp, getFilename(path));
	addStatMessage(parseFormat("ROM loaded: '%s' (Chk: 0x%04X)", temp, checksum));

	setWindowTitle();

	int i = 0;
	for(i = 0; i < 256; i++) {
		if(imageData[i].checksum == checksum) {
			interpreter.numOps = imageData[i].numOps;
			break;
		}
	}
	if(i == 256) interpreter.numOps = program.defNumOps;

	setOpsPerFrame(0);

	return EXIT_SUCCESS;
}

void initInterpreter()
{
	prepareChars();
	cpuReset();
	memset(interpreter.screen, 0x00, sizeof(interpreter.screen));

	program.isPaused = 0;
	program.isChip8Paused = 0;

	if(loadROM(program.filename)) {
		program.isPaused = 1;
		program.isChip8Paused = 1;
		addStatMessage(parseFormat("Couldn't load ROM '%s'!", program.filename));
	}

	addStatMessage("Interpreter reset.");

	if(program.startIntoDisasm) showDisassembly();
}

void doExitCallback()
{
	switch(program.inpReturn) {
		case 2: {
			exitProgram(EXIT_SUCCESS);
			break; }

		default: {
			break; }
	}
}

int doSDLEvents()
{
	SDL_Event event;
	#ifdef HW_RVL
	int JoyButton;
	#endif

	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT: {
				break; }

			case SDL_ACTIVEEVENT: {
				if((program.pauseWhenInactive) && (event.active.state != SDL_APPMOUSEFOCUS)) {
					if(event.active.gain == 0) {
						program.isChip8Paused = 1;
					} else {
						program.isChip8Paused = 0;
					}
				}
				break; }

			case SDL_KEYDOWN: {
				if(program.func_keydown != NULL) {
					program.func_keydown(&event.key);
					if(program.inpReturn != -1) return program.inpReturn;
				}
				break; }

			case SDL_KEYUP: {
				if(program.func_keyup != NULL) program.func_keyup(&event.key);
			}

			case SDL_MOUSEBUTTONDOWN: {
				if(program.func_mousedown != NULL) {
					program.func_mousedown(&event.button);
					if(program.inpReturn != -1) return program.inpReturn;
				}
				break; }

			case SDL_MOUSEMOTION: {
				if(program.func_mousemove != NULL) {
					program.func_mousemove(&event.motion);
				}
				break; }

			#ifdef HW_RVL
			case SDL_JOYBUTTONDOWN: {
				switch((JoyButton = event.jbutton.button)) {
					case 0: // A (comes back as mouse event)
					case 1: // B (comes back as mouse event)
						break;
					case 3: // 2
						break;
					case 2: // 1
						program.isChip8Paused ^= 1;
						break;
					case 6: // Home
						messageBox("Do you really want to exit?", MB_YESNO, (void*)doYesNoKeydown, (void*)doYesNoMousedown, (void*)doYesNoMousemove, doExitCallback);
						break;
					case 4: // -
					case 5: // +
						break;
				}
				break; }
			#endif

			case SDL_VIDEORESIZE: {
				program.screen = SDL_SetVideoMode(
					program.scrWidth, program.scrHeight,
					16, SDL_HWSURFACE | SDL_DOUBLEBUF);

				if(!program.screen) {
					printf("Surface lost while resizing: %s\n", SDL_GetError());
					return EXIT_FAILURE;
				}
				break; }
		}
	}

	SDL_FillRect(program.screen, 0, SDL_MapRGB(program.screen->format, 0, 0, 0));

	return 0;
}

void doMainKeydown(SDL_KeyboardEvent * ev)
{
	// exit
	if(ev->keysym.sym == SDLK_ESCAPE) messageBox("Do you really want to exit?",
		MB_YESNO, (void*)doYesNoKeydown, (void*)doYesNoMousedown, (void*)doYesNoMousemove, doExitCallback);

	// about
	if(ev->keysym.sym == SDLK_F12) messageBox(
		APP_TITLE " " APP_VERSION " - Chip-8 Interpreter| |Written in September 2010 by xdaniel|http://ozmav.googlecode.com/",
		MB_OKAY, (void*)doOkayKeydown, (void*)doOkayMousedown, (void*)doOkayMousemove, NULL);

	// help
	if(ev->keysym.sym == SDLK_F11) messageBox(
		"Controls & Help:| |- 1234/QWER/ASDF/ZXCV: Chip-8 Keypad|- F1: Reset interpreter|- F2: Pause/unpause interpreter|"
		"- F3: Load new image| |- F4: Toggle color mode, F5: Toggle sound|- Numpad -/+: Decrease/increase zoom factor| |"
		"- F9/F10: Decrease/increase opcodes/frame| |- Escape: Exit program",
		MB_OKAY, (void*)doOkayKeydown, (void*)doOkayMousedown, (void*)doOkayMousemove, NULL);

	if(!program.isPaused) {
		// reset interpreter
		if(ev->keysym.sym == SDLK_F1) initInterpreter();

		// pause/unpause interpreter
		if(ev->keysym.sym == SDLK_F2) {
			program.isChip8Paused ^= 1;
			addStatMessage(parseFormat("Interpreter %s.", (program.isChip8Paused ? "paused" : "unpaused")));
		}

		// load new image
		if(ev->keysym.sym == SDLK_F3) {
			char newfile[MAX_PATH];
			strcpy(newfile, fileBrowser(program.lastpath));

			if(strcmp(program.filename, newfile)) {
				strcpy(program.filename, newfile);
				if(strcmp(program.filename, "\0")) initInterpreter();
			}
		}

		// toggle color mode
		if(ev->keysym.sym == SDLK_F4) {
			program.colorMode ^= 1;
			addStatMessage(parseFormat("Switched to 'black/%s' color mode.", (program.colorMode ? "white" : "green")));
		}

		// toggle sound
		if(ev->keysym.sym == SDLK_F5) {
			program.enableSound ^= 1;
			addStatMessage(parseFormat("Sound has been %s.", (program.enableSound ? "enabled" : "disabled")));
		}

		// show disassembler
		if(ev->keysym.sym == SDLK_F6) showDisassembly();

		// toggle starting into disasm
		if(ev->keysym.sym == SDLK_F7) {
			program.startIntoDisasm ^= 1;
			addStatMessage(parseFormat("Will %s disassembler on ROM load/reset.", (program.startIntoDisasm ? "enter" : "not enter")));
		}

		// show memview
		if(ev->keysym.sym == SDLK_F8) showMemview();

		// increase num/ops
		if(ev->keysym.sym == SDLK_F9) {
			if(interpreter.numOps > 100) {
				interpreter.numOps-=40;
				setOpsPerFrame(0);
			}
		}
		// decrease num/ops
		if(ev->keysym.sym == SDLK_F10) {
			if(interpreter.numOps < 2000) {
				interpreter.numOps+=40;
				setOpsPerFrame(0);
			}
		}

		// increase zoom factor
		if(ev->keysym.sym == SDLK_KP_PLUS) {
			if(program.zoomFactor < MAX_ZOOMFACTOR) program.zoomFactor++;
		}
		// decrease zoom factor
		if(ev->keysym.sym == SDLK_KP_MINUS) {
			if(program.zoomFactor > 6) program.zoomFactor--;
		}

		// chip-8 input
		int key = -1;
		switch(ev->keysym.sym) {
			case SDLK_x: key = 0; break;
			case SDLK_1: key = 1; break;
			case SDLK_2: key = 2; break;
			case SDLK_3: key = 3; break;
			case SDLK_q: key = 4; break;
			case SDLK_w: key = 5; break;
			case SDLK_e: key = 6; break;
			case SDLK_a: key = 7; break;
			case SDLK_s: key = 8; break;
			case SDLK_d: key = 9; break;
			case SDLK_z: key = 10; break;
			case SDLK_c: key = 11; break;
			case SDLK_4: key = 12; break;
			case SDLK_r: key = 13; break;
			case SDLK_f: key = 14; break;
			case SDLK_v: key = 15; break;
			default: break;
		}
		if(key != -1) keyPressed(key);
	}
}

void doMainKeyup(SDL_KeyboardEvent * ev)
{
	if(!program.isPaused) {
		int key = -1;
		switch(ev->keysym.sym) {
			case SDLK_x: key = 0; break;
			case SDLK_1: key = 1; break;
			case SDLK_2: key = 2; break;
			case SDLK_3: key = 3; break;
			case SDLK_q: key = 4; break;
			case SDLK_w: key = 5; break;
			case SDLK_e: key = 6; break;
			case SDLK_a: key = 7; break;
			case SDLK_s: key = 8; break;
			case SDLK_d: key = 9; break;
			case SDLK_z: key = 10; break;
			case SDLK_c: key = 11; break;
			case SDLK_4: key = 12; break;
			case SDLK_r: key = 13; break;
			case SDLK_f: key = 14; break;
			case SDLK_v: key = 15; break;
			default: break;
		}
		if(key != -1) keyReleased(key);
	}
}

void setWindowTitle()
{
	sprintf(program.title, APP_TITLE " " APP_VERSION " (" APP_OS ") - %s", getFilename(program.filename));
	SDL_WM_SetCaption(program.title, program.title);
}

void doMainDrawing()
{
	drawScreen();
	drawStatMessages();

	#ifdef HW_RVL
	boxRGBA(program.screen, 0, SCREEN_OVERSCAN, program.scrWidth, SCREEN_OVERSCAN + FONTHEIGHT_BIG + 4, 0, 0, 0, 192);
	fontPrint(2, SCREEN_OVERSCAN + 1, 0, program.colWhite, program.ttff_big, program.title);
	hlineRGBA(program.screen, SCREEN_OVERSCAN, program.scrWidth, SCREEN_OVERSCAN + FONTHEIGHT_BIG + 4, 0, 128, 0, 128);
	#endif
}

void endDrawing()
{
	SDL_Flip(program.screen);
}

void exitProgram(int code)
{
	endAudio();
	#ifdef HW_RVL
	SDL_JoystickClose(0);
	#endif
	exit(code);
}
