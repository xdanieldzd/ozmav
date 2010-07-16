PROJECT = libbadRDP

# Environment
CC = gcc
AR = ar

# Utils flags
CFLAGS = -Wall -U_FORTIFY_SOURCE

# Application pieces
PIECES	= dump.o  f3dex2.o  dlparse.o  f3d.o   f3dex.o  rdp.o

#Platfor specific options
ifeq ($(WIN32), 1)
  CFLAGS += -DWIN32
  LDFLAGS = -lopengl32 -lglu32 -lglaux -lkernel32 -luser32 -lgdi32\
  	D:/libpng-1.2.37-lib/lib/libpng12.dll.a\
  	D:/zlib-1.2.3-lib/lib/libz.a
else
  LDFLAGS = -L/usr/X11R6/lib -L/usr/lib -L/lib -lglut -lXext -lX11 -lXmu -lXi -lGL -lGLU -lm -lpng -lz
endif

#Debug?
ifeq ($(DBG),1)
  CFLAGS += -DDEBUG -ggdb3 -g
  TARGET = $(PROJECT)_debug.a
else
  CFLAGS += -O2
  TARGET = $(PROJECT).a
endif

# Make
all: $(PIECES)
	$(AR) rcs $(TARGET) $(PIECES)

# Cleanup
clean:
	rm -vf *.a *.o
