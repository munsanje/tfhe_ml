#include <iostream>
#include "encryption.hpp"
#include "arithmetic.hpp"

using namespace std;

int main() {
  int16_t pt = 100;
  const int minimum_lambda = 128;
  TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);
  TFheGateBootstrappingSecretKeySet* sk = new_random_gate_bootstrapping_secret_keyset(params);
  const TFheGateBootstrappingCloudKeySet* ck = &sk->cloud;

  cout << "Plaintext:\n" << pt << endl;
  cout << "sizeof " << sizeof(int16_t) << endl;
  LweSample *enc = new_gate_bootstrapping_ciphertext_array(16, params);
  encrypt<int16_t>(enc, &pt, sk);

  LweSample *sum = new_gate_bootstrapping_ciphertext_array(16, params);
  // add(sum, enc, enc, ck, 16);
  rightShift(sum, enc, ck, 16);


  int16_t recovered = decrypt<int16_t>(enc, sk);
  cout << "recovered: " << recovered << endl;


  int16_t recovered_sum = decrypt<int16_t>(sum, sk);
  cout << "recovered_sum: " << recovered_sum << endl;


}
