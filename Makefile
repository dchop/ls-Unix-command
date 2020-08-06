all: clean output
output: myLs.o main.o
	gcc -g -W -Wall -Wpedantic myLs.o main.o -o myls
main.o: main.c
	gcc -c -g -W -Wall -Wpedantic main.c
myLs.o: myLs.c
	gcc -c -g -W -Wall -Wpedantic myLs.c
clean:
	rm -f *.o* myls