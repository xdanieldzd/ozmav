#include <sys/stat.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/keysym.h>
#include <limits.h>

#define MAX_PATH	PATH_MAX

#define FILESEP '/'

#define KEY_CAMERA_UP				XK_w
#define KEY_CAMERA_DOWN				XK_s
#define KEY_CAMERA_LEFT				XK_a
#define KEY_CAMERA_RIGHT			XK_d
#define KEY_GUI_TOGGLEGRID			XK_F11&0xFF
#define KEY_GUI_TOGGLEHUD			XK_F12&0xFF
#define KEY_DLIST_NEXTLIST			XK_KP_Add&0xFF
#define KEY_DLIST_PREVLIST			XK_KP_Subtract&0xFF

int XInit(char * WndTitle, int Width, int Height);
int XMain();
int XExit();
int XSetWindowTitle(char * WndTitle);

extern Display *dpy;
extern Window win;

extern XVisualInfo *vi;
extern Colormap cmap;
extern XSetWindowAttributes swa;
extern GLXContext cx;
extern XEvent event;
