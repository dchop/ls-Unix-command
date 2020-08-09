all: clean output
output: myLs.o main.o
	gcc -g -W -Wall -Wpedantic myLs.o main.o -o myls
main.o: main.c
	gcc -c -g -W -Wall -Wpedantic main.c
myLs.o: myLs.c
	gcc -c -g -W -Wall -Wpedantic myLs.c
valgrind: output
	 valgrind -s --leak-check=full \
			 --show-leak-kinds=all \
			 --track-origins=yes \
			 --show-reachable=yes \
			 ./myls -ilR Test/
clean:
	rm -f *.o* myls