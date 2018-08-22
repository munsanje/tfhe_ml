#include <cstdlib>
#include <iostream>
#include "encryption.hpp"
#include "alu.hpp"

#include <omp.h>
using namespace std;


int main() {
  omp_set_nested(1);
  omp_set_num_threads(NUM_THREADS);
  // initialize encryption variables
  const int minimum_lambda = 128;
  TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);
  TFheGateBootstrappingSecretKeySet* sk = new_random_gate_bootstrapping_secret_keyset(params);
  const TFheGateBootstrappingCloudKeySet* ck = &sk->cloud;

  // initialize test parameters
  typedef uint16_t num_type;
  size_t size = sizeof(num_type) * 8;
  num_type pt = 10;

  printf("%d\n", pt);
  printf("sizeof %ld\n", size);

  // encryption variable
  int array_size = 10;
  LweSample **arrays = new LweSample*[array_size];
  for(int i = 0; i < array_size; i++) {
    arrays[i] = new_gate_bootstrapping_ciphertext_array(size, params);
    encrypt<num_type>(arrays[i], &pt, sk);
  }
  LweSample *enc = new_gate_bootstrapping_ciphertext_array(size, params);
  encrypt<num_type>(enc, &pt, sk);
  // encryption variable 2
  LweSample *b = new_gate_bootstrapping_ciphertext_array(size, params);
  num_type pt2 = 7;
  encrypt<num_type>(b, &pt2, sk);

  LweSample *sum = new_gate_bootstrapping_ciphertext_array(size, params);
  // sub(sum, enc, b, ck, 16);
  // twosComplement(sum, enc, ck, size);
  // rightRotate(sum, enc, ck, size, 3);
  // leftRotate(sum, enc, ck, size, 16);
  // zero(enc, ck, size);
  // mult(sum, enc, b, ck, size);
  int times = 3;
  double stt = omp_get_wtime();
  for(int i = 0; i < times; i++)
    // reduce_add(sum, arrays, array_size, ck, size);
    // seq_add(sum, arrays, array_size, ck, size);
    // invert(sum, enc, ck, size);
    // bootsOR(sum, enc, b, ck);
    // OR(sum, enc, b, ck, size);
    mult(sum, enc, b, ck, size);
    // add(sum, enc, b, ck, size);
    // add(sum, enc, b, ck, size);
    // copy(sum, enc, ck, size);
/********/
  stt = (omp_get_wtime() - stt) / times;
  printf("time: %f seconds\n", stt);

  num_type recovered = decrypt<num_type>(enc, sk);
  printf("recovered: %d\n", recovered);


  num_type recovered_sum = decrypt<num_type>(sum, sk);
  printf("recovered_sum: %d\n", recovered_sum);


}
