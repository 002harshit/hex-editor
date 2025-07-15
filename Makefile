main.bin:
	clang ivy/ivy_x11.c main.c  -Wall -Wextra -pedantic -lm -lX11 -g -ggdb -O0 -o main.bin
	
run: main.bin
	./main.bin

.PHONY: main.bin run
