CC= g++
CFLAGS= -g -std=c++17 -Wall -Werror
LINKER= -lSDL2 -lSDL2_ttf
GAME= SnakeGame

all: Main

Main: Main.o $(GAME).o
	$(CC) $(CFLAGS) $^ $(LINKER) -o $@

Main.o: Main.cc
	$(CC) $(CFLAGS) $^ -c

$(GAME).o: $(GAME).cc
	$(CC) $(CFLAGS) $^ -c

clean:
	rm *.o Main
