#include "globals.h"

#ifdef WIN32
#define KEY_HUDMENU_UP			VK_UP
#define KEY_HUDMENU_DOWN		VK_DOWN
#define KEY_HUDMENU_LEFT		VK_LEFT
#define KEY_HUDMENU_RIGHT		VK_RIGHT
#define KEY_HUDMENU_TOGGLE		VK_SPACE
#define KEY_HUDMENU_SKIP1		VK_CONTROL
#define KEY_HUDMENU_SKIP2		VK_SHIFT
#else
#define KEY_HUDMENU_UP			XK_Up&0xFF
#define KEY_HUDMENU_DOWN		XK_Down&0xFF
#define KEY_HUDMENU_LEFT		XK_Left&0xFF
#define KEY_HUDMENU_RIGHT		XK_Right&0xFF
#define KEY_HUDMENU_TOGGLE		XK_space
#define KEY_HUDMENU_SKIP1		XK_Control_L&0xFF
#define KEY_HUDMENU_SKIP2		XK_Shift_L&0xFF
#endif

int MenuItem, LastMenuItem, FirstTrueMenuItem, MaxMenuItem;

void hudMenu_Init()
{
	MenuItem = 0, LastMenuItem = MenuItem, FirstTrueMenuItem = 0, MaxMenuItem = 0;
}

void hudMenu_HandleInput(__zHUDMenuEntry * Menu, int Len)
{
	// temp storage
	int MenuVal;

	// define menu dimensions/bounds
	FirstTrueMenuItem = 0;
	while(Menu[++FirstTrueMenuItem].Type == -1) { }

	MaxMenuItem = Len;
	while(Menu[--MaxMenuItem].Type == -1) { }

	if(LastMenuItem == MenuItem && Menu[MenuItem].Type == -1) MenuItem++;
	LastMenuItem = MenuItem;

	// go up
	if(zProgram.Key[KEY_HUDMENU_UP]) {
		if(MenuItem > FirstTrueMenuItem) {
			MenuItem--;
		} else {
			MenuItem = MaxMenuItem;
		}
		zProgram.Key[KEY_HUDMENU_UP] = false;
		goto __sanitycheck;
	}

	// go down
	if(zProgram.Key[KEY_HUDMENU_DOWN]) {
		if(MenuItem < MaxMenuItem) {
			MenuItem++;
		} else {
			MenuItem = FirstTrueMenuItem;
		}
		zProgram.Key[KEY_HUDMENU_DOWN] = false;
		goto __sanitycheck;
	}

	// decrease value
	if((zProgram.Key[KEY_HUDMENU_LEFT]) && (Menu[MenuItem].Type == 2)) {
		MenuVal = *Menu[MenuItem].Value;
		// modifiers held down?
		if(zProgram.Key[KEY_HUDMENU_SKIP1]) {
			MenuVal-=16;
			if(zProgram.Key[KEY_HUDMENU_SKIP2]) MenuVal-=16;
		} else {
			MenuVal--;
		}
		*Menu[MenuItem].Value = MenuVal;
		zProgram.Key[KEY_HUDMENU_LEFT] = false;
		return;
	}

	// increase value
	if((zProgram.Key[KEY_HUDMENU_RIGHT]) && (Menu[MenuItem].Type == 2)) {
		MenuVal = *Menu[MenuItem].Value;
		// modifiers held down?
		if(zProgram.Key[KEY_HUDMENU_SKIP1]) {
			MenuVal+=16;
			if(zProgram.Key[KEY_HUDMENU_SKIP2]) MenuVal+=16;
		} else {
			MenuVal++;
		}
		*Menu[MenuItem].Value = MenuVal;
		zProgram.Key[KEY_HUDMENU_RIGHT] = false;
		return;
	}

	// toggle value on/off
	if((zProgram.Key[KEY_HUDMENU_TOGGLE]) && (Menu[MenuItem].Type < 2)) {
		MenuVal = *Menu[MenuItem].Value;
		MenuVal ^= 1;
		*Menu[MenuItem].Value = MenuVal;
		zProgram.Key[KEY_HUDMENU_TOGGLE] = false;
		return;
	}

__sanitycheck:
	// skip labels when selecting
	if(Menu[MenuItem].Type == -1) {
		// if we're going down in the menu
		if(LastMenuItem < MenuItem) {
			// is there another entry below?
			if(MenuItem + 1 < Len) {
				// go there
				MenuItem++;
			} else {
				// if not, reset
				MenuItem = LastMenuItem;
			}
			return;
		} else if(LastMenuItem > MenuItem) {
			// is another entry above?
			if(MenuItem - 1 > 0) {
				MenuItem--;
			} else {
				MenuItem = LastMenuItem;
			}
			return;
		}
	}
}

void hudMenu_Render(char Title[], int X, int Y, __zHUDMenuEntry * Menu, int Len)
{
	int i;
	char Message[256];
	char TempString[256];

	// output title
	sprintf(Message, "\x90[ %s ]\n", Title);

	// parse menu entries
	for(i = 0; i < Len; i++) {
		// if entry is selected, color it
		if(i == MenuItem) strcat(Message, "\x90");

		// check the entry type
		switch(Menu[i].Type) {
			case -1:	// label
				sprintf(TempString, "- %s:\n", Menu[i].Name);
				break;
			case 1:		// switch
				sprintf(TempString, "  %s [%s] %s\n", (i == MenuItem ? " " : " "), (*Menu[i].Value ? "X" : " "), Menu[i].Name);
				break;
			case 2:		// value select
				switch(Menu[i].Disp) {
					default:
					case 0: // signed dec
						sprintf(TempString, "  %s %s = %i\n", (i == MenuItem ? " " : " "), Menu[i].Name, (short)(*Menu[i].Value));
						break;
					case 1: // unsigned dec
						sprintf(TempString, "  %s %s = %i\n", (i == MenuItem ? " " : " "), Menu[i].Name, (unsigned short)(*Menu[i].Value));
						break;
					case 2: // signed hex
						sprintf(TempString, "  %s %s = 0x%04X\n", (i == MenuItem ? " " : " "), Menu[i].Name, (short)(*Menu[i].Value));
						break;
					case 3: // unsigned hex
						sprintf(TempString, "  %s %s = 0x%04X\n", (i == MenuItem ? " " : " "), Menu[i].Name, (unsigned short)(*Menu[i].Value));
						break;
				}
				break;
			case 0:		// nothing
			default:
				sprintf(TempString, "  %s %s\n", (i == MenuItem ? " " : " "), Menu[i].Name);
				break;
		}
		strcat(Message, TempString);
	}

	// print via HUD
	hud_Print(X, Y, -1, -1, 1, 1.0f, true, Message);
}
