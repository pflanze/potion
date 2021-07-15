none:
	@echo "To make a local binary for testing, type:"
	@echo "   make local"
	@echo "To make a C64 target binary, type:"
	@echo "   make c64"
	@echo "To make a C64 small target binary, type:"
	@echo "   make c64small"

all: local c64 c64small

clean:
	-rm potion potion.prg potion.small.prg
	-rm -rf local target smalltarget

local:
	make -f Makefile.linux

c64:
	make -f Makefile.c64

c64small:
	make -f Makefile.c64.small

.PHONY: all none c64 c64small local
