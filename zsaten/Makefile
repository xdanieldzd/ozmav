PROJECT = zsaten

#Install dir
ifeq ($(PREFIX),)
	ifeq ($(WIN32), 1)
		PREVIX = C:
		CP = copy
		RM = del
	else
		PREFIX = /usr/bin
		CP = cp
		RM = rm
	endif
endif

# Environment
CC = gcc

# Utils flags
CFLAGS = -Wall -U_FORTIFY_SOURCE 

# Application pieces
PIECES	= camera.o  hud.o  __linux.o  main.o  memwatch.o  mips-eval.o  oz.o  render.o  shared.o  __win32.o  zactors.o  zelda.o

LDFLAGS = -Xlinker -zmuldefs

#Platfor specific options
ifeq ($(WIN32), 1)
  CFLAGS += -DWIN32
  TARGET = $(PROJECT).exe
  #Holy shit. Are these all really needed? - ...nope.
  LDFLAGS += -lopengl32 -lglu32 -lglaux -lkernel32 -luser32 -lgdi32	\
           D:\libpng-1.2.37-lib\lib\libpng12.dll.a				\
           D:\zlib-1.2.3-lib\lib\libz.a						\
           C:\Programme\PDCurses-SO\win32\panel.a				\
           C:\Programme\PDCurses-SO\win32\pdcurses.a
  PIECES += __win32.o
else
  TARGET = $(PROJECT)
  LDFLAGS += -lGL -lpng -lcurses -lX11
  PIECES += __linux.o
endif

LDFLAGS += ../misaka/libMISAKA.a ../badrdp/libbadRDP.a
#Debug? 
ifeq ($(DBG),1)
  CFLAGS += -DDEBUG -g
#  LDFLAGS += ../misaka/libMISAKA_debug.a ../badrdp/libbadRDP_debug.a
else
  CFLAGS += -O2
endif

# Make
all: $(PIECES)
	$(CC) $(CFLAGS) -o $(TARGET) $(PIECES) $(LDFLAGS)

# Install
install:
	$(CP) $(TARGET) $(PREFIX)/$(TARGET)

# Uninstall
uninstall:
	$(RM) $(PREFIX)/$(TARGET)

# Cleanup
clean:
	rm -vrf $(TARGET) *.o dump extr log.txt
