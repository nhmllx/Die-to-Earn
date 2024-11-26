CFLAGS = -I ./include
LFLAGS = -lrt -lX11 -lGLU -lGL -pthread -lm #-lXrandr

all: walk

walk: walk.cpp fmateolazo.cpp imalleaux.cpp nmalleaux.cpp
	g++ $(CFLAGS) walk.cpp libggfonts.a fmateolazo.cpp imalleaux.cpp nmalleaux.cpp -Wall -Wextra $(LFLAGS) -owalk

clean:
	rm -f walk
	rm -f *.o

