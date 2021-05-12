.PHONY: test clean

test/main: test/main.c test/car.c
	gcc $^ -o $@ -I . -std=c99 -Wall -Wextra -Wconversion -g -fsanitize=undefined

test: test/main
	./test/main

clean:
	rm -f test/main
