## OPTIONS ##
# set to anything else to disable DBUS
DBUS = 1
GTK = 1

DEPS = fontconfig freetype2 libtoxav libtoxcore
DEPS += libv4lconvert openal vpx x11 xext xrender
ifeq ($(DBUS), 1)
	DEPS += dbus-1
endif

ifeq ($(GTK), 1)
	DEPS += gtk+-2.0
endif

UNAME_S := $(shell uname -s)

CFLAGS += -g -Wall -Wshadow -pthread -std=gnu99
CFLAGS += $(shell pkg-config --cflags $(DEPS))
LDFLAGS = -pthread -lm
LDFLAGS += $(shell pkg-config --libs $(DEPS))

ifneq ($(DBUS), 1)
	CFLAGS += -DNO_DBUS
endif

ifneq ($(GTK), 1)
	CFLAGS += -DNO_GTK
endif

ifeq ($(UNAME_S), Linux)
	LDFLAGS += -lresolv -ldl
endif

DESTDIR ?=
PREFIX ?= /usr/local

SRC = $(wildcard *.c png/png.c)
OBJ = $(SRC:.c=.o)

all: utox

utox: $(OBJ)
	@echo "  LD    $@"
	@$(CC) $(CFLAGS) -o utox $(OBJ) $(LDFLAGS)

install: utox
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 0755 utox $(DESTDIR)$(PREFIX)/bin/utox

	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/14x14/apps
	install -m 644 icons/utox-14x14.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/14x14/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/16x16/apps
	install -m 644 icons/utox-16x16.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/16x16/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/22x22/apps
	install -m 644 icons/utox-22x22.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/22x22/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/24x24/apps
	install -m 644 icons/utox-24x24.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/24x24/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/32x32/apps
	install -m 644 icons/utox-32x32.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/32x32/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/36x36/apps
	install -m 644 icons/utox-36x36.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/36x36/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/48x48/apps
	install -m 644 icons/utox-48x48.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/48x48/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/64x64/apps
	install -m 644 icons/utox-64x64.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/64x64/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/72x72/apps
	install -m 644 icons/utox-72x72.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/72x72/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/96x96/apps
	install -m 644 icons/utox-96x96.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/96x96/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/128x128/apps
	install -m 644 icons/utox-128x128.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/128x128/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/192x192/apps
	install -m 644 icons/utox-192x192.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/192x192/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/256x256/apps
	install -m 644 icons/utox-256x256.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/256x256/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/512x512/apps
	install -m 644 icons/utox-512x512.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/512x512/apps/utox.png
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps
	install -m 644 icons/utox.svg $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/utox.svg

	mkdir -p $(DESTDIR)$(PREFIX)/share/applications
	install -m 644 utox.desktop $(DESTDIR)$(PREFIX)/share/applications/utox.desktop

main.o: xlib/main.c xlib/keysym2ucs.c

.c.o:
	@echo "  CC    $@"
	@$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f utox *.o png/*.o

.PHONY: all clean
