CC=gcc
FLAGS= -g -Wall 

all: run

run: main.o helpFunc.o
	$(CC) $(FLAGS) -o run main.o helpFunc.o 

helpFunc.o: helpFunc.c helpFunc.h
	$(CC) $(FLAGS) -c helpFunc.c

main.o: main.c helpFunc.h
	$(CC) $(FLAGS) -c main.c 


.PHONY: clean all

clean:
	rm -f *.o run