TARGET = sf64tk

PARTS = main.o n64sums.o sf64.o
CC = gcc
CFLAGS = -Wall -O2 -U_FORTIFY_SOURCE
LDFLAGS = -lcurses

ifeq ($(PREFIX),)
	PREFIX = /usr/bin
endif

ifeq ($(WIN32), 1)
	TARGET += .exe
	CFLAGS += -DWIN32
	LDFLAGS += ..\misaka\libMISAKA.a
else
	LDFLAGS += ../misaka/libMISAKA.a
endif

all: $(PARTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(PARTS) $(LDFLAGS)

install:
	cp $(TARGET) $(PREFIX)/$(TARGET)

clean:
	rm -vf *.o $(TARGET)
