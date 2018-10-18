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
#include <vector>
#include <ctime>
#include <string>
#include <iostream>

#include "tfhe_params.hpp"
#include "encryption.hpp"
#include "alu.hpp"
#include "io.hpp"


using namespace std;

/* Adds '_epochtime' to input string*/
string toTimestamped(string name) {
  return name + "_" + to_string(time(0));
}

/* Tests basic execution times of all arithmetic functions */
template<typename T>
vector<double> test_execution_times() {
  size_t size = sizeof(T) * 8;
  // setup
  TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(MINIMUM_LAMBDA);
  const TFheGateBootstrappingSecretKeySet* sk = new_random_gate_bootstrapping_secret_keyset(params);
  const TFheGateBootstrappingCloudKeySet* ck = &sk->cloud;

  LweSample *enc_a = new_gate_bootstrapping_ciphertext_array(size, params),
            *enc_b = new_gate_bootstrapping_ciphertext_array(size, params);

  T a = rand() % size, b = rand() % size;

  encrypt<T>(enc_a, a, sk);
  encrypt<T>(enc_b, b, sk);

  double tick;

  int test_iters = 5;  // number of iterations for each operation

  LweSample* res = new_gate_bootstrapping_ciphertext_array(size, params);
  // test addition
  double add_agg = 0;
  for(int i = 0; i < test_iters; i++) {
    tick = omp_get_wtime();
    add(res, enc_a, enc_b, ck, size);
    add_agg += omp_get_wtime() - tick;
  }
  add_agg /= test_iters;  // average execution time in seconds for addition
  cout << "Average addition runtime: " << add_agg << endl;
  // test subtraction
  double sub_agg = 0;
  for(int i = 0; i < test_iters; i++) {
    tick = omp_get_wtime();
    sub(res, enc_a, enc_b, ck, size);
    sub_agg += omp_get_wtime() - tick;
  }

  sub_agg /= test_iters;  // average execution time in seconds for subtraction
  cout << "Average subtraction runtime: " << sub_agg << endl;


  // test multiplication
  double mult_agg = 0;
  for(int i = 0; i < test_iters; i++) {
    tick = omp_get_wtime();
    mult(res, enc_a, enc_b, ck, size);
    mult_agg += omp_get_wtime() - tick;
  }
  mult_agg /= test_iters;  // average execution time in seconds for multiplication
  cout << "Average multiplication runtime: " << mult_agg << endl;

  // string header = "precision,addition,subtraction,multiplication";
  vector<double> runtimes = {size, add_agg, sub_agg, mult_agg};
  delete_gate_bootstrapping_ciphertext_array(size, res);
  delete_gate_bootstrapping_ciphertext_array(size, enc_a);
  delete_gate_bootstrapping_ciphertext_array(size, enc_b);
  return runtimes;
}

int main() {
  omp_set_nested(1);
  omp_set_num_threads(NUM_THREADS);

  typedef int8_t num_type;

  vector<double> runtimes_8bit = test_execution_times<int8_t>();
  vector<double> runtimes_16bit = test_execution_times<int16_t>();
  vector<double> runtimes_32bit = test_execution_times<int32_t>();
  vector<double> runtimes_64bit = test_execution_times<int64_t>();

  vector<vector<double>> all_runtimes = {runtimes_8bit, runtimes_16bit, runtimes_32bit, runtimes_64bit};
  string header = "precision,addition,subtraction,multiplication";
  string exec_time_path = toTimestamped("execution_times_by_precision") + ".csv";
  cout << "Writing execution times to " << exec_time_path << endl;
  writeFile(all_runtimes, exec_time_path, ',', header);
}
