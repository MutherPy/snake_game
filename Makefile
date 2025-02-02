
compile: snake.c
	@(clang $? -o snake.o || gcc $? -o snake.o) && echo 'Yo, aka'

run:
	@./snake.o

build: compile run

.PHONY: build

.DEFAULT_GOAL := build
