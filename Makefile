CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: walk

walk: walkF.cpp mateo.cpp
	g++ $(CFLAGS) walkF.cpp libggfonts.a mateo.cpp -Wall -Wextra $(LFLAGS) -owalk

clean:
	rm -f walk
	rm -f *.o

