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
#define KEY_SCENE_PREVIOUS			VK_F1
#define KEY_SCENE_NEXT				VK_F2
#define KEY_MAP_PREVIOUS			VK_F3
#define KEY_MAP_NEXT				VK_F4
#define KEY_MOUSE_MODESWITCH		VK_F5
#define KEY_ACTOR_PREVIOUS			VK_SUBTRACT
#define KEY_ACTOR_NEXT				VK_ADD

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
