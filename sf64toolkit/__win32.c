#include "globals.h"

#ifdef WIN32
WNDCLASSEX wcex;
HWND hwnd;
HDC hDC;
HGLRC hRC;
MSG msg;

int WinAPIInit(char * WndTitle, int Width, int Height)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = APPTITLE;
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;

	if (!RegisterClassEx(&wcex)) return EXIT_SUCCESS;

	hwnd = CreateWindowEx(0, APPTITLE, WndTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, hInstance, NULL);

	EnableOpenGL(hwnd, &hDC, &hRC);

	return EXIT_SUCCESS;
}

int WinAPIMain()
{
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if(msg.message == WM_QUIT) {
			Program.IsRunning = false;
		} else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return -1;

	} else {
		if(Program.Key[VK_ESCAPE]) sv_DeInit();

		if(Program.Key['W']) ca_Movement(false, 6.0f);
		if(Program.Key['S']) ca_Movement(false, -6.0f);
		if(Program.Key['A']) ca_Movement(true, -6.0f);
		if(Program.Key['D']) ca_Movement(true, 6.0f);

		if(Program.Key['T']) ca_Movement(false, 24.0f);
		if(Program.Key['G']) ca_Movement(false, -24.0f);
		if(Program.Key['F']) ca_Movement(true, -24.0f);
		if(Program.Key['H']) ca_Movement(true, 24.0f);

		if(Program.Key[VK_F1] && Viewer.LevelID > 0) {
			Viewer.LevelID--;
			sv_Init();
			Program.Key[VK_F1] = false;
		}

		if(Program.Key[VK_F2] && Viewer.LevelID < 0x14) {
			Viewer.LevelID++;
			sv_Init();
			Program.Key[VK_F2] = false;
		}

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

int WinAPIExit()
{
	DisableOpenGL(hwnd, hDC, hRC);

	DestroyWindow(hwnd);

	return EXIT_SUCCESS;
}

int WinAPISetWindowTitle(char * WndTitle)
{
	return SetWindowText(hwnd, WndTitle);
}

int WinAPIShowWindow(int Stat)
{
	/*if(Stat)*/ SetForegroundWindow(hwnd);
	ShowWindow(hwnd, (Stat ? SW_SHOW : SW_HIDE));

	return EXIT_SUCCESS;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;

		case WM_DESTROY:
			return EXIT_SUCCESS;

		case WM_SIZE: {
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			gl_ResizeScene(rcClient.right, rcClient.bottom);
			break; }

		case WM_KEYDOWN:
			Program.Key[wParam] = true;
			break;
		case WM_KEYUP:
			Program.Key[wParam] = false;
			break;

		case WM_LBUTTONDOWN: {
			SetFocus(hwnd);
			Program.MouseButtonDown = true;
			Program.MouseCenterX = (signed int)LOWORD(lParam);
			Program.MouseCenterY = (signed int)HIWORD(lParam);
			break; }
		case WM_LBUTTONUP: {
			Program.MouseButtonDown = false;
			break; }
		case WM_MOUSEMOVE: {
			if(Program.MouseButtonDown) {
				Program.MousePosX = (signed int)LOWORD(lParam);
				Program.MousePosY = (signed int)HIWORD(lParam);
				ca_MouseMove(Program.MousePosX, Program.MousePosY);
				ca_Orientation(Camera.AngleX, Camera.AngleY);
			}
			break; }

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return EXIT_SUCCESS;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	*hDC = GetDC(hwnd);

	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
				  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}
#endif
