CXXFLAGS=-g3 -Wall
main: engine.o main.o
	g++ $(CXXFLAGS) main.o engine.o   -o main
clean:
	rm *.o main
