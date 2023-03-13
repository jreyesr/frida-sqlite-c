main: main.c
	clang -g main.c -o main -lsqlite3

run: main
	./main