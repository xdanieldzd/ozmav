#include <windows.h>
#include <conio.h>

#ifdef MOUSE_MOVED
#undef MOUSE_MOVED
#endif

#define FILESEP '\\'

#define KEY_CAMERA_UP_FAST			'W'
#define KEY_CAMERA_DOWN_FAST		'S'
#define KEY_CAMERA_LEFT_FAST		'A'
#define KEY_CAMERA_RIGHT_FAST		'D'
#define KEY_CAMERA_UP_SLOW			'T'
#define KEY_CAMERA_DOWN_SLOW		'G'
#define KEY_CAMERA_LEFT_SLOW		'F'
#define KEY_CAMERA_RIGHT_SLOW		'H'
#define KEY_CAMERA_RESET			VK_F9
#define KEY_ACTOR_ROTATEX_MINUS		VK_UP
#define KEY_ACTOR_ROTATEX_PLUS		VK_DOWN
#define KEY_ACTOR_ROTATEY_MINUS		VK_LEFT
#define KEY_ACTOR_ROTATEY_PLUS		VK_RIGHT
#define KEY_SWITCH_SHOWBONES		VK_TAB
#define KEY_SWITCH_PLAYANIM			VK_SPACE
#define KEY_SWITCH_PREVACTOR		VK_F1
#define KEY_SWITCH_NEXTACTOR		VK_F2
#define KEY_SWITCH_PREVANIM			VK_F3
#define KEY_SWITCH_NEXTANIM			VK_F4
#define KEY_SWITCH_PREVBONES		VK_F5
#define KEY_SWITCH_NEXTBONES		VK_F6
#define KEY_SWITCH_LINKDETAIL		VK_F7
#define KEY_SWITCH_LINKOBJECT		VK_F8
#define KEY_SWITCH_ENABLEHUD		VK_F12
#define KEY_SWITCH_LOWERFPS			VK_DIVIDE
#define KEY_SWITCH_RAISEFPS			VK_MULTIPLY
#define KEY_SWITCH_PREVFRAME		VK_SUBTRACT
#define KEY_SWITCH_NEXTFRAME		VK_ADD

int WinAPIInit(char * WndTitle, int Width, int Height);
int WinAPIMain();
int WinAPIExit();
int WinAPISetWindowTitle(char * WndTitle);
int WinAPISetWindowSize(int Width, int Height);

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

extern HINSTANCE hInstance;
extern WNDCLASSEX wcex;
extern HWND hwnd;
extern HDC hDC;
extern HGLRC hRC;
extern MSG msg;
