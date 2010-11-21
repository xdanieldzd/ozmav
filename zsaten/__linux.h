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
#define KEY_CAMERA_RESET			XK_F9&0xFF
#define KEY_ACTOR_ROTATEX_MINUS		XK_Up&0xFF
#define KEY_ACTOR_ROTATEX_PLUS		XK_Down&0xFF
#define KEY_ACTOR_ROTATEY_MINUS		XK_Left&0xFF
#define KEY_ACTOR_ROTATEY_PLUS		XK_Right&0xFF
#define KEY_SWITCH_SHOWBONES		XK_Tab&0xFF
#define KEY_SWITCH_PLAYANIM			XK_space&0xFF
#define KEY_SWITCH_PREVACTOR		XK_F1&0xFF
#define KEY_SWITCH_NEXTACTOR		XK_F2&0xFF
#define KEY_SWITCH_PREVANIM			XK_F3&0xFF
#define KEY_SWITCH_NEXTANIM			XK_F4&0xFF
#define KEY_SWITCH_PREVBONES		XK_F5&0xFF
#define KEY_SWITCH_NEXTBONES		XK_F6&0xFF
#define KEY_SWITCH_LINKDETAIL		XK_F7&0xFF
#define KEY_SWITCH_LINKOBJECT		XK_F8&0xFF
#define KEY_SWITCH_ENABLEHUD		XK_F12&0xFF
#define KEY_SWITCH_LOWERFPS			XK_KP_Divide&0xFF
#define KEY_SWITCH_RAISEFPS			XK_KP_Multiply&0xFF
#define KEY_SWITCH_PREVFRAME		XK_KP_Add&0xFF
#define KEY_SWITCH_NEXTFRAME		XK_KP_Subtract&0xFF

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
