/**
Implements tests on HE arithmetic functions

Scope:

1. Precision vs runtime

2. Parallelism vs runtime

3. Correctness of functions

Basic approach is to encapsulate each "test" in a function and when function is run, it outputs
whatever metric and also saves it to disk
*/

#include <omp.h>
#include <cstdlib>

#include "tfhe_params.hpp"
#include "encryption.hpp"
#include "alu.hpp"
#include "io.hpp"

void test_precision() {

}

void test_parallelism() {

}

/* Tests basic execution times of all arithmetic functions */
template<typename T>
void test_execution_times(T a, T b) {


}

int main() {
  omp_set_nested(1);
  omp_set_num_threads(NUM_THREADS);

  TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(MINIMUM_LAMBDA);
  const TFheGateBootstrappingSecretKeySet* sk = new_random_gate_bootstrapping_secret_keyset(params);

  typedef int8_t num_type;
  size_t size = sizeof(num_type) * 8;

  num_type a = rand() % size, b = rand() % size;  // random operands




}
