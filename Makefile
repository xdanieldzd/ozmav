#Options to send to other makefiles...
ifeq ($(WIN32), 1)
	OPTIONS += WIN32=1
endif
ifeq ($(DBG), 1)
	OPTIONS += DBG=1
endif

#Prefix
ifeq ($(PREFIX),)
	PREFIX = /usr/bin
endif

ifeq ($(APP),)
	APP = ozmav2
endif

targets:
	@echo "ozmav svn Makefile."
	@echo "  Actions:"
	@echo "    all           == Build ozmav2, zsaten, and sm64toz64"
	@echo "    clean         == Remove all files built from source"
	@echo "    rebuild       == clean and re-build all"
	@echo "    install       == install application x (details below)"
	@echo "    uninstall     == uninstall OZMAV2"
	@echo "    package       == Build package"
	@echo "    ozmav2        == Build ozmav2"
	@echo "    zsaten        == Build ZSaten"
	@echo "    sm64toz64     == Build sm64toz64"
	@echo "    dlviewer      == Build dlviewer"
	@echo "  Options:"
	@echo "    WIN32=1       == mingw build (Makefiles need fixing)"
	@echo "  Debugging Options:"
	@echo "    DBG=1         == Enable debugging symbols. (Rebuild"
	@echo "                     suggested if not used previous build)"
	@echo "  Install Options:"
	@echo "    PREFIX=path   == install/uninstall prefix (default: /usr/bin)"
	@echo "    APP=x         == install application x (ozmav2 (default),zsaten,sm64toz64)"

dummy:
	@echo

libs:
	$(MAKE) -C misaka $(OPTIONS)
	$(MAKE) -C badrdp $(OPTIONS)

ozmav2: libs
	$(MAKE) -C misaka $(OPTIONS)
	$(MAKE) -C badrdp $(OPTIONS)
	$(MAKE) -C ozmav2 $(OPTIONS)

sm64toz64: dummy
	$(MAKE) -C sm64toz64 $(OPTIONS)

zsaten: libs
	$(MAKE) -C zsaten $(OPTIONS)

dlviewer: libs
	$(MAKE) -C dlviewer $(OPTIONS)

all: ozmav2 zsaten sm64toz64 dlviewer

install:
	$(MAKE) -C $(APP) install

uninstall:
	$(MAKE) -C $(APP) uninstall

rebuild: clean all

package:
	rm ozmav2.tgz
	tgz ozmav2.tgz

clean:
	$(MAKE) -C misaka clean $(OPTIONS)
	$(MAKE) -C badrdp clean $(OPTIONS)
	$(MAKE) -C ozmav2 clean $(OPTIONS)
	$(MAKE) -C sm64toz64 clean $(OPTIONS)
	$(MAKE) -C zsaten clean $(OPTIONS)
	$(MAKE) -C dlviewer clean $(OPTIONS)

