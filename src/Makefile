CC=g++
CCFLAGS=--std=c++11 -fopenmp
LDFLAGS=-ltfhe-spqlios-avx

all: test

run: test
	./test

test: test.o encryption.o alu.o matrix.o logistic.o io.o
	$(CC) $(CCFLAGS) -o test test.o alu.o matrix.o logistic.o io.o $(LDFLAGS)


test.o: test.cpp encryption.hpp alu.o matrix.o logistic.o io.o
	$(CC) $(CCFLAGS) -c test.cpp encryption.hpp alu.cpp matrix.cpp logistic.cpp io.cpp $(LDFLAGS)

test_arithmetic: test_arithmetic.o encryption.o alu.o matrix.o logistic.o io.o metrics.o
	$(CC) $(CCFLAGS) -o test_arithmetic test_arithmetic.o alu.o matrix.o logistic.o io.o metrics.o $(LDFLAGS)

test_arithmetic.o: test_arithmetic.cpp encryption.hpp alu.o matrix.o logistic.o io.o metrics.o tfhe_params.hpp
	$(CC) $(CCFLAGS) -c test_arithmetic.cpp encryption.hpp alu.cpp matrix.cpp logistic.cpp io.cpp metrics.cpp tfhe_params.hpp $(LDFLAGS)

test_logistic: test_logistic.o encryption.o alu.o matrix.o logistic.o io.o metrics.o
	$(CC) $(CCFLAGS) -o test_logistic test_logistic.o alu.o matrix.o logistic.o io.o metrics.o $(LDFLAGS)

test_logistic.o: test_logistic.cpp encryption.hpp alu.o matrix.o logistic.o io.o metrics.o tfhe_params.hpp
	$(CC) $(CCFLAGS) -c test_logistic.cpp encryption.hpp alu.cpp matrix.cpp logistic.cpp io.cpp metrics.cpp tfhe_params.hpp $(LDFLAGS)

metrics.o: metrics.cpp metrics.hpp
	$(CC) $(CCFLAGS) -c metrics.cpp

logistic.o: logistic.cpp numeric.hpp io.hpp encryption.hpp
	$(CC) $(CCFLAGS) -c logistic.cpp numeric.hpp io.cpp encryption.hpp $(LDFLAGS)

io.o: io.cpp
	$(CC) $(CCFLAGS) -c io.cpp

matrix.o: matrix.cpp matrix.hpp alu.o
	$(CC) $(CCFLAGS) -c matrix.cpp alu.cpp $(LDFLAGS)


alu.o: alu.cpp alu.hpp omp_constants.hpp
	$(CC) $(CCFLAGS) -c alu.cpp $(LDFLAGS)

encryption.o: encryption.hpp
	$(CC) $(CCFLAGS) -o encryption.o -c encryption.hpp $(LDFLAGS)

clean:
	rm -f test
	rm -f *.o *.gch
