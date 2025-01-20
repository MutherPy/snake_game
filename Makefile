
compile: snake.c
	@clang $? -o snake.o

run:
	@./snake.o

build: compile run

.PHONY: build
