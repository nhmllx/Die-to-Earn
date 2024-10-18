CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: walk

walk: walk.cpp fmateolazo.cpp
	g++ $(CFLAGS) walk.cpp libggfonts.a fmateolazo.cpp -Wall -Wextra $(LFLAGS) -owalk

clean:
	rm -f walk
	rm -f *.o

