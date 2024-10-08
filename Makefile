CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: walk fmateo

walk: walk.cpp
	g++ $(CFLAGS) walk.cpp libggfonts.a -Wall -Wextra $(LFLAGS) -owalk

fmateo: mateo.cpp
	g++ $(CFLAGS) mateo.cpp libggfonts.a -Wall -Wextra $(LFLAGS) -o mateo
clean:
	rm -f walk
	rm -f *.o

