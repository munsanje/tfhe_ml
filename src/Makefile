CC=g++
CCFLAGS=--std=c++11
LDFLAGS=-ltfhe-spqlios-fma

all: test

test: test.o encryption.o arithmetic.o
	$(CC) $(CCFLAGS) -o test test.o arithmetic.o $(LDFLAGS)

test.o: test.cpp encryption.hpp arithmetic.o
	$(CC) $(CCFLAGS) -c test.cpp encryption.hpp arithmetic.cpp $(LDFLAGS)

arithmetic.o: arithmetic.cpp arithmetic.hpp
	$(CC) $(CCFLAGS) -c arithmetic.cpp $(LDFLAGS)

encryption.o: encryption.hpp
	$(CC) $(CCFLAGS) -o encryption.o -c encryption.hpp $(LDFLAGS)

clean:
	rm -f test
	rm -f *.o *.gch
