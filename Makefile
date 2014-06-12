CXXFLAGS=-g3 -Wall
test: cycle_writer.o test.o
	g++ $(CXXFLAGS) test.o cycle_writer.o   -o test
clean:
	rm *.o test
