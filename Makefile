#Options to send to other makefiles...
ifeq ($(WIN32), 1)
	OPTIONS += WIN32=1
else ifeq ($(DBG), 1)
	OPTIONS += DBG=1
endif

#Prefix
ifeq ($(PREFIX),)
	PREFIX = /usr/bin
endif

targets:
	@echo "OZMAV2 Makefile."
	@echo "  Targets:"
	@echo "    all           == Build OZMAV2 and all needed libs"
	@echo "    clean         == Remove all files built from source"
	@echo "    rebuild       == clean and re-build all"
	@echo "    install       == install OZMAV2"
	@echo "    uninstall     == uninstall OZMAV2"
	@echo "    package       == Build package"
	@echo "  Options:"
	@echo "    WIN32=1       == mingw build (Makefiles need fixing)"
	@echo "  Debugging Options:"
	@echo "    DBG=1         == Enable debugging symbols. Rebuild"
	@echo "                     suggested if not used previous build)"
	@echo "  Install Options:"
	@echo "    PREFIX=path   == install/uninstall prefix (default: /usr/bin)"

all:
	$(MAKE) -C misaka $(OPTIONS)
	$(MAKE) -C badrdp $(OPTIONS)
	$(MAKE) -C ozmav2 $(OPTIONS)

install:
	cp ozmav2/OZMAV2 $(PREFIX)/OZMAV2
	ln -f $(PREFIX)/OZMAV2 $(PREFIX)/ozmav2

uninstall:
	rm -vf $(PREFIX)/OZMAV2 $(PREFIX)/ozmav2

rebuild: clean all

package:
	tgz ozmav2.tgz

clean:
	$(MAKE) -C misaka clean
	$(MAKE) -C badrdp clean
	$(MAKE) -C ozmav2 clean
