#include <windows.h>
#include <conio.h>

#ifdef MOUSE_MOVED
#undef MOUSE_MOVED
#endif

#define FILESEP '\\'

#define KEY_CAMERA_UP				'W'
#define KEY_CAMERA_DOWN				'S'
#define KEY_CAMERA_LEFT				'A'
#define KEY_CAMERA_RIGHT			'D'
#define KEY_GUI_TOGGLEGRID			VK_F11
#define KEY_GUI_TOGGLEHUD			VK_F12
#define KEY_DLIST_NEXTLIST			VK_ADD
#define KEY_DLIST_PREVLIST			VK_SUBTRACT

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
