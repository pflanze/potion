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

runlocal: local
	./potion

c64:
	make -f Makefile.c64

# "c64" is a script that should start "x64" from Vice with the right
# -kernal, -basic, and -chargen and other options and values if
# necessary
runc64: c64
	c64 -autostartprgmode 1 potion.prg

c64small:
	make -f Makefile.c64.small

.PHONY: all none c64 c64small local
