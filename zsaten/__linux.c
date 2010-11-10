#include "globals.h"

#ifndef WIN32
static int snglBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
static int dblBuf[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

Display *dpy;
Window win;

XVisualInfo *vi;
Colormap cmap;
XSetWindowAttributes swa;
GLXContext cx;
XEvent event;

int dummy;

int XInit(char * WndTitle, int Width, int Height)
{
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not open display\n");
		return EXIT_FAILURE; }
	if(!glXQueryExtension(dpy, &dummy, &dummy)) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: X server has no OpenGL GLX extension\n");
		return EXIT_FAILURE; }

	vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf);
	if (vi == NULL) {
		vi = glXChooseVisual(dpy, DefaultScreen(dpy), snglBuf);
		if (vi == NULL) {
			dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: No RGB visual with depth buffer\n");
			return EXIT_FAILURE; }
	}
	if(vi->class != TrueColor) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: TrueColor visual required for this program\n");
		return EXIT_FAILURE; }

	cx = glXCreateContext(dpy, vi, None, GL_TRUE);
	if (cx == NULL) {
		dbgprintf(0, MSK_COLORTYPE_ERROR, "- Error: Could not create rendering context\n");
		return EXIT_FAILURE; }

	cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
	swa.colormap = cmap;
	swa.border_pixel = 0;
	swa.event_mask = KeyPressMask | ExposureMask | ButtonPressMask | StructureNotifyMask;
	win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, Width, Height, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
	XSetStandardProperties(dpy, win, WndTitle, WndTitle, None, NULL, 0, NULL);

	glXMakeCurrent(dpy, win, cx);

	XMapWindow(dpy, win);

	XSelectInput(dpy, win, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | Button1MotionMask | StructureNotifyMask);

	return EXIT_SUCCESS;
}

int XMain()
{
	while(XPending(dpy) > 0) {
		XNextEvent(dpy, &event);
		switch (event.type) {
			case Expose: {
				XFlush(dpy);
				break; }
			case KeyPress: {
				KeySym     keysym;
				XKeyEvent *kevent;
				char       buffer[1];
				kevent = (XKeyEvent *) &event;
				if((XLookupString((XKeyEvent *)&event,buffer,1,&keysym,NULL) == 1) && (keysym == (KeySym)XK_Escape)) vProgram.isRunning = false;
				vProgram.key[keysym & 0xFF] = true;
				dbgprintf(0, MSK_COLORTYPE_INFO, "key: %i : %i %i %i %i", (keysym & 0xFF), XK_Up, XK_Down, XK_Left, XK_Right);
				break; }
			case KeyRelease: {
				KeySym     keysym;
				char       buffer[1];
				XLookupString((XKeyEvent *)&event,buffer,1,&keysym,NULL);
				vProgram.key[keysym & 0xFF] = false;
				break; }

			case ButtonPress: {
				vProgram.mouseCenterX = event.xbutton.x;
				vProgram.mouseCenterY = event.xbutton.y;
				break; }

			case MotionNotify: {
				if(event.xmotion.state & Button1Mask) {
					vProgram.mousePosX = event.xbutton.x;
					vProgram.mousePosY = event.xbutton.y;
					ca_MouseMove(vProgram.mousePosX, vProgram.mousePosY);
					ca_Orientation(vCamera.angleX, vCamera.angleY);
				}

				break; }

			case ConfigureNotify: {
				vProgram.windowWidth = event.xconfigure.width;
				vProgram.windowHeight = event.xconfigure.height;
				gl_SetupScene3D(vProgram.windowWidth, vProgram.windowHeight);
				break; }
		}
	}

	if(vProgram.key[XK_w]) ca_Movement(false, 2.0f);
	if(vProgram.key[XK_s]) ca_Movement(false, -2.0f);
	if(vProgram.key[XK_a]) ca_Movement(true, -2.0f);
	if(vProgram.key[XK_d]) ca_Movement(true, 2.0f);

	if(vProgram.key[XK_t]) ca_Movement(false, 0.5f);
	if(vProgram.key[XK_g]) ca_Movement(false, -0.5f);
	if(vProgram.key[XK_f]) ca_Movement(true, -0.5f);
	if(vProgram.key[XK_h]) ca_Movement(true, 0.5f);

	if(vProgram.key[XK_Up&0xFF]) vCamera.actorRotX -= 2.0f;
	if(vProgram.key[XK_Down&0xFF]) vCamera.actorRotX += 2.0f;
	if(vProgram.key[XK_Left&0xFF]) vCamera.actorRotY -= 2.0f;
	if(vProgram.key[XK_Right&0xFF]) vCamera.actorRotY += 2.0f;

	if(vProgram.key[XK_Tab&0xFF]) {
		vProgram.showBones ^= 1;
		vProgram.key[XK_Tab&0xFF] = false;
	}

	if(vProgram.key[XK_space&0xFF]) {
		vProgram.animPlay ^= 1;
		vProgram.key[XK_space&0xFF] = false;
	}

	if(vProgram.key[XK_F1&0xFF]) {
		vProgram.key[XK_F1&0xFF] = false;
		if(vCurrentActor.actorNumber > 0) {
			vCurrentActor.actorNumber--;
			vCurrentActor.animCurrent = 0;
			vCurrentActor.frameCurrent = 0;
			initActorParsing(-1);
		}
	}

	if(vProgram.key[XK_F2&0xFF]) {
		vProgram.key[XK_F2&0xFF] = false;
		if(vCurrentActor.actorNumber < vZeldaInfo.actorCount - 1) {
			vCurrentActor.actorNumber++;
			vCurrentActor.animCurrent = 0;
			vCurrentActor.frameCurrent = 0;
			initActorParsing(-1);
		}
	}

	if(vProgram.key[XK_F3&0xFF]) {
		if(vCurrentActor.animCurrent > 0) {
			vCurrentActor.animCurrent--;
			vCurrentActor.frameCurrent = 0;
		}
		vProgram.key[XK_F3&0xFF] = false;
	}

	if(vProgram.key[XK_F4&0xFF]) {
		if(vCurrentActor.animCurrent < vCurrentActor.animTotal) {
			vCurrentActor.animCurrent++;
			vCurrentActor.frameCurrent = 0;
		}
		vProgram.key[XK_F4&0xFF] = false;
	}

	if(vProgram.key[XK_F5&0xFF]) {
		if(vCurrentActor.boneSetupCurrent > 0) {
			vCurrentActor.boneSetupCurrent--;
			vCurrentActor.frameCurrent = 0;
		}
		vProgram.key[XK_F5&0xFF] = false;
	}

	if(vProgram.key[XK_F6&0xFF]) {
		if(vCurrentActor.boneSetupCurrent < vCurrentActor.boneSetupTotal) {
			vCurrentActor.boneSetupCurrent++;
			vCurrentActor.frameCurrent = 0;
		}
		vProgram.key[XK_F6&0xFF] = false;
	}

	if(vProgram.key[XK_F9&0xFF]) {
		ca_Reset();
		vProgram.key[XK_F9&0xFF] = false;
	}

	if(vProgram.key[XK_F12&0xFF]) {
		vProgram.enableHUD ^= 1;
		vProgram.key[XK_F12&0xFF] = false;
	}

	if(vProgram.key[XK_KP_Divide&0xFF]) {
		if(vProgram.targetFPS > 15.0f) vProgram.targetFPS -= 15.0f;
		vProgram.key[XK_KP_Divide&0xFF] = false;
	}

	if(vProgram.key[XK_KP_Multiply&0xFF]) {
		if(vProgram.targetFPS <= 60.0f) vProgram.targetFPS += 15.0f;
		vProgram.key[XK_KP_Multiply&0xFF] = false;
	}

	if(vProgram.key[XK_KP_Add&0xFF]) {
		if(vProgram.animPlay) vProgram.animPlay = false;
		if(vCurrentActor.frameCurrent < vCurrentActor.frameTotal) vCurrentActor.frameCurrent++;
		else vCurrentActor.frameCurrent = 0;
		vProgram.key[XK_KP_Add&0xFF] = false;
	}

	if(vProgram.key[XK_KP_Subtract&0xFF]) {
		if(vProgram.animPlay) vProgram.animPlay = false;
		if(vCurrentActor.frameCurrent > 0) vCurrentActor.frameCurrent--;
		else vCurrentActor.frameCurrent = vCurrentActor.frameTotal;
		vProgram.key[XK_KP_Subtract&0xFF] = false;
	}

	return EXIT_SUCCESS;
}

int XExit()
{
	glXMakeCurrent(dpy, None, NULL);
	glXDestroyContext(dpy, cx);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);

	return EXIT_SUCCESS;
}

int XSetWindowTitle(char * WndTitle)
{
	XSetStandardProperties(dpy, win, WndTitle, WndTitle, None, NULL, 0, NULL);

	return EXIT_SUCCESS;
}
#endif
