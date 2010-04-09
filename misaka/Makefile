PROJECT = libMISAKA

# Environment
CC = gcc
AR = ar

# Utils flags
CFLAGS = -Wall -U_FORTIFY_SOURCE

# Application pieces
PIECES	= msk_base.o msk_console.o msk_ui.o

#Platfor specific options
ifeq ($(WIN32), 1)
  CFLAGS += -DWIN32
  LDFLAGS = C:/Programme/PDCurses-SO/win32/panel.a	\
            C:/Programme/PDCurses-SO/win32/pdcurses.a
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
