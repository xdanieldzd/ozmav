PROJECT = ozmav2

#Install dir
ifeq ($(PREFIX),)
	ifeq ($(WIN32), 1)
		PREVIX = C:\
		CP = copy
	else
		PREFIX = /usr/bin
		CP = cp
	endif
endif

# Environment
CC = gcc

# Utils flags
CFLAGS = -Wall -U_FORTIFY_SOURCE

# Application pieces
PIECES	= camera.o draw.o __linux.o mips-eval.o __win32.o zelda.o confunc.o main.o oz.o zactors.o hud.o hud_menu.o mouse.o

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

#Debug? 
ifeq ($(DBG),1)
  CFLAGS += -DDEBUG -g
  LDFLAGS += ../misaka/libMISAKA_debug.a ../badrdp/libbadRDP_debug.a
else
  CFLAGS += -O3
  LDFLAGS += ../misaka/libMISAKA.a ../badrdp/libbadRDP.a
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
