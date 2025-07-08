main.bin:
	clang *.c ivy/*.c  -Wall -Wextra -pedantic -lm -lX11 -g -ggdb -O0 -o main.bin
	
run: main.bin
	./main.bin

.PHONY: main.bin run
