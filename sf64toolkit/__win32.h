#include <windows.h>
#include <conio.h>

#ifdef MOUSE_MOVED
#undef MOUSE_MOVED
#endif

#define FILESEP '\\'

int WinAPIInit(char * WndTitle, int Width, int Height);
int WinAPIMain();
int WinAPIExit();
int WinAPISetWindowTitle(char * WndTitle);
int WinAPIShowWindow(int Stat);

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

extern HINSTANCE hInstance;
extern WNDCLASSEX wcex;
extern HWND hwnd;
extern HDC hDC;
extern HGLRC hRC;
extern MSG msg;
