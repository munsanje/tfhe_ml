CC=g++
CCFLAGS=--std=c++11 -fopenmp
LDFLAGS=-ltfhe-spqlios-avx

all: test

run: test
	./test

test: test.o encryption.o alu.o
	$(CC) $(CCFLAGS) -o test test.o alu.o $(LDFLAGS)

test.o: test.cpp encryption.hpp alu.o
	$(CC) $(CCFLAGS) -c test.cpp encryption.hpp alu.cpp $(LDFLAGS)

alu.o: alu.cpp alu.hpp omp_constants.hpp
	$(CC) $(CCFLAGS) -c alu.cpp $(LDFLAGS)

encryption.o: encryption.hpp
	$(CC) $(CCFLAGS) -o encryption.o -c encryption.hpp $(LDFLAGS)

clean:
	rm -f test
	rm -f *.o *.gch
