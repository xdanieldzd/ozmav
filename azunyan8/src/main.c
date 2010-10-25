/*
	main.c - Azunyan-8 startup code & main loop
*/

#include "globals.h"

char * tempstr = NULL;

__program program;
__interpreter interpreter;

int i;

int main(int argc, char** argv)
{
	#ifdef HW_RVL
	// Wii: init libfat and set application path
	fatInitDefault();
	strcpy(program.apppath, "sd:/apps/wii_azunyan8");
	#else
	// others: get application path
	getcwd(program.apppath, MAX_PATH);
	#endif
	
	// set filebrowser's last path to application path
	strcpy(program.lastpath, program.apppath);
	strcat(program.lastpath, parseFormat("%c", FILESEP));

	// load config file
	if(loadConfig(parseFormat("%s%cconfig.txt", program.apppath, FILESEP))) {
		addStatMessage("Could not load configuration!");
	}

	// force Wii to use its max zoom factor
	#ifdef HW_RVL
	program.zoomFactor = ZOOMMAX;
	#endif

	// initialize program
	if(initProgram()) return EXIT_FAILURE;
	if(initAudio()) return EXIT_FAILURE;

	// check if ROM filename has been specified
	if(argc >= 2) {
		strcpy(program.filename, argv[1]);
	} else {
		sprintf(program.filename, "no ROM loaded");
		setWindowTitle();
		// try to open a ROM using filebrowser
		strcpy(program.filename, fileBrowser(program.lastpath));
	}

	// if we've got a filename, initialize the interpreter
	if(strcmp(program.filename, "\0")) initInterpreter();

	float interval = 1000 / TARGET_FPS;
	unsigned int time2 = SDL_GetTicks();

	// main loop!
	while(!doSDLEvents()) {
		if(!program.isPaused) {
			if(!program.isChip8Paused) {
				unsigned int current = SDL_GetTicks();
				srand(current);

				if((time2 + interval) < current) {
					if(decreaseTimers()) break;
					for(i = 0; i < interpreter.numFrames; i++) cpuRun();
					time2 = current;
				}
			}
		}

		if(program.func_draw_main != NULL) program.func_draw_main();

		endDrawing();
	}

	exitProgram(EXIT_SUCCESS);

	// this shouldn't be reached!
	return EXIT_FAILURE;
}
