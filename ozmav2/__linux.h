#include <sys/stat.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/keysym.h>
#include <limits.h>

#define MAX_PATH	PATH_MAX

#define FILESEP '/'

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
