CXXFLAGS=-g3 -Wall
main: cycle_writer.o main.o
	g++ $(CXXFLAGS) main.o cycle_writer.o   -o main
clean:
	rm *.o main
