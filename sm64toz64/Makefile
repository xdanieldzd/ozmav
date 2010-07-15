# Possible targets:
# make 			Standard build on non-windows
# make DBG=1		Debug build on non-windows
# make WIN32=1		Standard build on windows
# make WIN32=1 DBG=1	Debug build on windows

PROJECT = SM64toZ64

# Target
ifeq ($(WIN32), 1)
	TARGET = $(PROJECT).exe
else
	TARGET = $(PROJECT)
endif

# Environment
CC = gcc

# Utils flags
CFLAGS = -O3 -U_FORTIFY_SOURCE

ifeq ($(DBG),1)
  CFLAGS += -DDEBUG
endif

# Win32 options
ifeq ($(WIN32), 1)
	CFLAGS += -mwindows -mms-bitfields -DWIN32
endif

# Application pieces
PIECES	= main.o actors.o script.o dlist.o

# Make
all: $(PIECES)
	$(CC) $(CFLAGS) -o $(TARGET) $(PIECES)

# Cleanup
clean:
	rm -vf $(TARGET) *.o 
