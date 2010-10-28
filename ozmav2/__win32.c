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
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wcex)) return EXIT_SUCCESS;

	hwnd = CreateWindowEx(0, APPTITLE, WndTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, true);

	EnableOpenGL(hwnd, &hDC, &hRC);

	// resize window for OpenGL area itself to be ex. 640x480, not the actual window
	WinAPISetWindowSize((Width + GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXFRAME)), (Height + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYFRAME)));

	return EXIT_SUCCESS;
}

int WinAPIMain()
{
	if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if(msg.message == WM_QUIT) {
			zProgram.IsRunning = false;
		} else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return -1;

	} else {
		if(zProgram.Key[VK_ESCAPE]) PostQuitMessage(0);

		if(zProgram.Key['W']) ca_Movement(false, 6.0f);
		if(zProgram.Key['S']) ca_Movement(false, -6.0f);
		if(zProgram.Key['A']) ca_Movement(true, -6.0f);
		if(zProgram.Key['D']) ca_Movement(true, 6.0f);

		if(zProgram.Key['T']) ca_Movement(false, 1.0f);
		if(zProgram.Key['G']) ca_Movement(false, -1.0f);
		if(zProgram.Key['F']) ca_Movement(true, -1.0f);
		if(zProgram.Key['H']) ca_Movement(true, 1.0f);

		if(zProgram.Key[VK_F1] && zOptions.SceneNo > 0) {
			zOptions.SceneNo--;
			if(zl_LoadScene(zOptions.SceneNo)) return EXIT_FAILURE;
			zProgram.Key[VK_F1] = false;
		}

		if(zProgram.Key[VK_F2] && zOptions.SceneNo < zGame.SceneCount) {
			zOptions.SceneNo++;
			if(zl_LoadScene(zOptions.SceneNo)) return EXIT_FAILURE;
			zProgram.Key[VK_F2] = false;
		}

		if(zProgram.Key[VK_F3] && zOptions.MapToRender > -1) {
			if(zSHeader[0].MapCount > 1) {
				zOptions.MapToRender--;
				zOptions.SelectedActor = -1;
				zOptions.SelectedActorMap = zOptions.MapToRender;
			} else {
				zOptions.MapToRender = 0;
			}
			zProgram.Key[VK_F3] = false;
		}

		if(zProgram.Key[VK_F4] && zOptions.MapToRender < zSHeader[0].MapCount - 1) {
			zOptions.MapToRender++;
			zOptions.SelectedActor = -1;
			zOptions.SelectedActorMap = zOptions.MapToRender;
			zProgram.Key[VK_F4] = false;
		}

		if(zProgram.Key[VK_F5]) {
			zProgram.MouseMode++;
			if(zProgram.MouseMode == 6) zProgram.MouseMode = 0;
			sprintf(zProgram.WndTitle, "%s - %s - ", APPTITLE, zGame.TitleText);
			switch(zProgram.MouseMode) {
				case 0: sprintf(zProgram.WndTitle, "%sCamera Mode", zProgram.WndTitle); break;
				case 1: sprintf(zProgram.WndTitle, "%sActor Mode (X/Y)", zProgram.WndTitle); break;
				case 2: sprintf(zProgram.WndTitle, "%sActor Mode (X/Z)", zProgram.WndTitle); break;
				case 3: sprintf(zProgram.WndTitle, "%sActor Mode (X)", zProgram.WndTitle); break;
				case 4: sprintf(zProgram.WndTitle, "%sActor Mode (Y)", zProgram.WndTitle); break;
				case 5: sprintf(zProgram.WndTitle, "%sActor Mode (Z)", zProgram.WndTitle); break;
			}
			oz_SetWindowTitle(zProgram.WndTitle);
			zProgram.Key[VK_F5] = false;
		}

		if(zProgram.Key[VK_SUBTRACT] && zOptions.SelectedActor > -1) {
			zOptions.SelectedActor--;
			zProgram.Key[VK_SUBTRACT] = false;
		}

		if(zProgram.Key[VK_ADD] && zOptions.SelectedActor < zMHeader[0][zOptions.MapToRender].ActorCount - 1) {
			zOptions.SelectedActor++;
			zProgram.Key[VK_ADD] = false;
		}

		zOptions.SelectedActorMap = zOptions.MapToRender;

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

int WinAPISetWindowSize(int Width, int Height)
{
	zProgram.WindowWidth = Width;
	zProgram.WindowHeight = Height;
	return SetWindowPos(hwnd, NULL, 0, 0, zProgram.WindowWidth, zProgram.WindowHeight, SWP_NOMOVE);
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
			zProgram.WindowWidth = rcClient.right;
			zProgram.WindowHeight = rcClient.bottom;
			gl_SetupScene3D(zProgram.WindowWidth, zProgram.WindowHeight);
			break; }

		case WM_KEYDOWN:
			zProgram.Key[wParam] = true;
			break;
		case WM_KEYUP:
			zProgram.Key[wParam] = false;
			break;

		case WM_LBUTTONDOWN: {
			SetFocus(hwnd);
			zProgram.MouseButtonLDown = true;
			zProgram.MouseCenterX = (signed int)LOWORD(lParam);
			zProgram.MouseCenterY = (signed int)HIWORD(lParam);
			break; }
		case WM_LBUTTONUP: {
			zProgram.MouseButtonLDown = false;
			break; }
		case WM_RBUTTONDOWN: {
			SetFocus(hwnd);
			zProgram.MouseButtonRDown = true;
			if(zProgram.MouseMode >= 1) {
				int YOffset = GetSystemMetrics(SM_CYSIZE) + GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYSIZEFRAME) + 10;
				zProgram.MouseCenterX = (signed int)LOWORD(lParam);
				zProgram.MouseCenterY = (signed int)HIWORD(lParam) + YOffset;
				zProgram.SceneCoords = ms_GetSceneCoords(zProgram.MouseCenterX, zProgram.MouseCenterY);
				zOptions.SelectedActor = ms_SelectedMapActor();
				//MSK_ConsolePrint(MSK_COLORTYPE_WARNING, "MOUSE COORDS: X:%i, Y:%i (OFS:%i)\nHIT COORDS: X:%i, Y:%i, Z:%i\n", zProgram.MouseCenterX, zProgram.MouseCenterY, YOffset, zProgram.SceneCoords.X, zProgram.SceneCoords.Y, zProgram.SceneCoords.Z);
			}
			break; }
		case WM_RBUTTONUP: {
			zProgram.MouseButtonRDown = false;
			break; }

		case WM_MOUSEMOVE: {
			switch(zProgram.MouseMode) {
				// camera mode
				case 0: {
					if(zProgram.MouseButtonLDown) {
						zProgram.MousePosX = (signed int)LOWORD(lParam);
						zProgram.MousePosY = (signed int)HIWORD(lParam);
						ca_MouseMove(zProgram.MousePosX, zProgram.MousePosY);
						ca_Orientation(zCamera.AngleX, zCamera.AngleY);
					}
					break; }
				// actor mode
				case 1:
				case 2:
				case 3:
				case 4:
				case 5: {
					int YOffset = GetSystemMetrics(SM_CYSIZE) + GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYSIZEFRAME) + 10;
					int OldMouseX = zProgram.MousePosX;
					int OldMouseY = zProgram.MousePosY;
					zProgram.MousePosX = (signed int)LOWORD(lParam);
					zProgram.MousePosY = (signed int)HIWORD(lParam) + YOffset;
					if(zProgram.MouseButtonLDown) {
						if(zOptions.SelectedActor >= 0) {
							int DX = (zProgram.MousePosX - OldMouseX) * sin(zCamera.RotX + 1);
							int DY = (zProgram.MousePosY - OldMouseY) * cos(zCamera.RotY + 1);
							// actor mode, subdivision
							switch(zProgram.MouseMode) {
								// X/Y
								case 1: {
									zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.X += DX;
									zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.Y += -DY;
									break; }
								// X/Z
								case 2: {
									zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.X += DX;
									zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.Z += -DY;
									break; }
								// X
								case 3: zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.X += DX; break;
								// Y
								case 4: zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.Y += -DY; break;
								// Z
								case 5: zMapActor[zOptions.SelectedActorMap][zOptions.SelectedActor].Pos.Z += -DY; break;
							}
						}
					}
					break; }
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
