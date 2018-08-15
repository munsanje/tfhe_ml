#include <chrono>
#include <cstdlib>
#include <iostream>
#include "encryption.hpp"
#include "alu.hpp"

using namespace std;

typedef std::chrono::steady_clock::time_point chrono_time;

chrono_time get_time() {
  return std::chrono::steady_clock::now();
}

int main() {
  // initialize encryption variables
  const int minimum_lambda = 128;
  TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);
  TFheGateBootstrappingSecretKeySet* sk = new_random_gate_bootstrapping_secret_keyset(params);
  const TFheGateBootstrappingCloudKeySet* ck = &sk->cloud;

  // initialize test parameters
  typedef uint8_t num_type;
  size_t size = sizeof(num_type) * 8;
  num_type pt = 4;

  printf("%d\n", pt);
  printf("sizeof %d\n", size);

  // encryption variable
  LweSample *enc = new_gate_bootstrapping_ciphertext_array(size, params);
  encrypt<num_type>(enc, &pt, sk);
  // encryption variable 2
  LweSample *b = new_gate_bootstrapping_ciphertext_array(size, params);
  num_type pt2 = 20;
  encrypt<num_type>(b, &pt2, sk);

  LweSample *sum = new_gate_bootstrapping_ciphertext_array(size, params);
  // sub(sum, enc, b, ck, 16);
  // twosComplement(sum, enc, ck, size);
  // rightRotate(sum, enc, ck, size, 3);
  // leftRotate(sum, enc, ck, size, 16);
  chrono_time start = get_time();
  // add(sum, enc, b, ck, size);
  // zero(enc, ck, size);
  // mult(sum, enc, b, ck, size);
  copy(sum, enc, ck, size);
  chrono_time end = get_time();
  cout << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << " microseconds" << endl;


  num_type recovered = decrypt<num_type>(enc, sk);
  printf("recovered: %d\n", recovered);


  num_type recovered_sum = decrypt<num_type>(sum, sk);
  printf("recovered_sum: %d\n", recovered_sum);


}
