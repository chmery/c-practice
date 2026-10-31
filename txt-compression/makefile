SOURCE_FILES = main.c encoder.c decoder.c shared.c debug.c ../binary-tree/heap.c
CFLAGS = -Wall -Wextra -pedantic -g 

main: $(SOURCE_FILES)
	gcc $(SOURCE_FILES) -lm -o main $(CFLAGS)
	./main