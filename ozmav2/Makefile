PROJECT = OZMAV2

# Environment
CC = gcc

# Utils flags
CFLAGS = -Wall -U_FORTIFY_SOURCE

# Application pieces
PIECES	= camera.o confunc.o draw.o dump.o f3dex2.o main.o oz.o zelda.o

# In regards to backslashes
# (http://www.netbsd.org/docs/pkgsrc/makefile.html#makefile.variables)
#
#   "	When a variable's value is parsed from a Makefile, the hash
#	character ``#'' and the backslash character ``\'' are handled
#	specially. If a backslash is followed by a newline, any
#	whitespace immediately in front of the backslash, the backslash,
#	the newline, and any whitespace immediately behind the newline
#	are replaced with a single space. A backslash character and an
#	immediately following hash character are replaced with a single
#	hash character. Otherwise, the backslash is passed as is. In a
#	variable assignment, any hash character that is not preceded by
#	a backslash starts a comment that continues upto the end of the
#	logical line.  "
#
# gedit (actually, gtk viewsource) does not seem to handle Makefile
# backslashes properly.

#Platfor specific options
ifeq ($(WIN32), 1)
  CFLAGS += -DWIN32
  TARGET = $(PROJECT).exe
  #Holy shit. Are these all really needed?
  LDFLAGS = -lopengl32 -lglu32 -lglaux -lkernel32 -luser32 -lgdi32 -lwinspool	\
           -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32	\
           -lodbccp32 -lcomctl32						\
           D:\libpng-1.2.37-lib\lib\libpng12.dll.a				\
           D:\zlib-1.2.3-lib\lib\libz.a						\
           C:\Programme\PDCurses-SO\win32\panel.a				\
           C:\Programme\PDCurses-SO\win32\pdcurses.a
  PIECES += __win32.o
else
  TARGET = $(PROJECT)
  LDFLAGS = -lGL -lpng -lcurses -lX11
  PIECES += __linux.o
endif

#Debug? 
ifeq ($(DBG),1)
  CFLAGS += -DDEBUG -g
  ifeq ($(WIN32),1)
    LDFLAGS += ..\..\libMISAKA\libMISAKA_debug.a
  else
    LDFLAGS += ../misaka/libMISAKA_debug.a
  endif
else
  CFLAGS += -O2
  ifeq ($(WIN32),1)
    LDFLAGS += ..\..\libMISAKA\libMISAKA.a -s
  else
    LDFLAGS += ../misaka/libMISAKA.a
  endif
endif

# Make
all: $(PIECES)
	$(CC) $(CFLAGS) -o $(TARGET) $(PIECES) $(LDFLAGS)

# Cleanup
clean:
	rm -vf $(TARGET) *.o
