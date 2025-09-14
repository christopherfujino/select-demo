.PHONY: run
run: a.out test.rb
	./a.out

a.out: main.o
	$(CC) main.o -o a.out

main.o: main.c
	$(CC) -c main.c -o main.o
