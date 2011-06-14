#include <sys/stat.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/keysym.h>
#include <limits.h>

#define MAX_PATH	PATH_MAX

#define FILESEP '/'

#define KEY_CAMERA_UP_FAST			XK_w
#define KEY_CAMERA_DOWN_FAST		XK_s
#define KEY_CAMERA_LEFT_FAST		XK_a
#define KEY_CAMERA_RIGHT_FAST		XK_d
#define KEY_CAMERA_UP_SLOW			XK_t
#define KEY_CAMERA_DOWN_SLOW		XK_g
#define KEY_CAMERA_LEFT_SLOW		XK_f
#define KEY_CAMERA_RIGHT_SLOW		XK_h
#define KEY_SCENE_PREVIOUS			XK_F1&0xFF
#define KEY_SCENE_NEXT				XK_F2&0xFF
#define KEY_MAP_PREVIOUS			XK_F3&0xFF
#define KEY_MAP_NEXT				XK_F4&0xFF
#define KEY_MOUSE_MODESWITCH		XK_F5&0xFF
#define KEY_ACTOR_PREVIOUS			XK_KP_Subtract&0xFF
#define KEY_ACTOR_NEXT				XK_KP_Add&0xFF
#define VK_UP         XK_Up&0xFF
#define VK_DOWN          XK_Down&0xFF
#define VK_LEFT         XK_Left&0xFF
#define VK_RIGHT          XK_Right&0xFF
#define VK_TAB            XK_Tab&0xFF
#define VK_SPACE                     XK_space&0xFF
#define VK_CONTROL			XK_Control_L&0xFF
#define VK_SHIFT			XK_Shift_L&0xFF
#define KEY_GUI_TOGGLEHUD			XK_F12&0xFF

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
