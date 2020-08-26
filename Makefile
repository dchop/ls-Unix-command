CFLAGS = -g -W -Wall -Wpedantic
MAIN = ./src/main.c
MYLS = ./src/myLs.c
INC = -I include
BIN = bin/myls

all: clean build

build: myLs.o main.o
	@mkdir -p bin
	gcc $(CFLAGS) myLs.o main.o -o $(BIN)

main.o: $(MAIN)
	gcc -c $(CFLAGS) $(INC)  $(MAIN)

myLs.o: $(MYLS)
	gcc -c $(CFLAGS) $(INC) $(MYLS)

mem: clean build mem-check

mem-check: 
	@mkdir -p bin
	 valgrind -s --leak-check=full \
			 --show-leak-kinds=all \
			 --track-origins=yes \
			 --show-reachable=yes \
			 ./$(BIN) -ilR
			 
clean:
	rm -f *.o* *.out* ./$(BIN)