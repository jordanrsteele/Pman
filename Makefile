.phony all:
all: PMan

PMan: PMan.c
	gcc PMan.c -o PMan

.PHONY clean:
clean:
	-rm -rf *.o *.exe
	-rm -rf ./PMan