
CFLAGS=$(shell pkg-config --cflags gtkmm-3.0) $(shell pkg-config --cflags sdl2)
LIBS=$(shell pkg-config --libs gtkmm-3.0) $(shell pkg-config --libs sdl2) -lSDL2_mixer

all:
	g++ -o wildcat *.cpp -std=c++14 $(CFLAGS) $(LIBS)
