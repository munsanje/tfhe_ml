#include <cstdlib>
#include <iostream>
#include "encryption.hpp"
#include "alu.hpp"
#include "matrix.hpp"
#include "logistic.hpp"
#include "io.hpp"

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
  typedef int16_t num_type;
  size_t size = sizeof(num_type) * 8;
  num_type pt = 1;
  num_type pt2 = -25;
  printf("%d\n", pt);
  printf("sizeof %ld\n", size);

  /*logistic regression stuff*/
  char *coefs_path = "/home/munsanje/Uni/Thesis/code/wiscons_state/polycoefs.csv",
       *weights_path = "/home/munsanje/Uni/Thesis/code/wiscons_state/logweights_wiscon_real_0.01.csv",
       *data_path = "/home/munsanje/Uni/Thesis/data/cleaned_scaled_int16_breast-cancer-wisconsin.data";
  int dim = 9;

  ApproxLogRegression lr(weights_path, coefs_path, dim, ck, size);
  vector<vector<double>> data = readFile(data_path);
  int which = 4;
  for(int i = 0; i < dim; i++)
    cout << data[which][i] << " ";
  cout << endl;
  size_t N = data.size();
  size_t d = dim;

  LweSample ***enc_data = new LweSample**[N];
  for(int i = 0; i < N; i++) {
    enc_data[i] = new LweSample*[d];
    for(int j = 0; j < d; j++) {
      enc_data[i][j] = new_gate_bootstrapping_ciphertext_array(size, params);
      encrypt<num_type>(enc_data[i][j], data[i][j], sk);
    }
  }
  LweSample *y = new_gate_bootstrapping_ciphertext_array(size, params);

  cout << "running pred" << endl;
  lr.predict(y, enc_data[which]);
  cout << "finished pred" << endl;

  // LweSample* a = new_gate_bootstrapping_ciphertext_array(size, params);
  // encrypt<num_type>(a, pt, sk);
  // lr.approxSigmoid(y, a);
  num_type res = decrypt<num_type>(y, sk) >= 0.5 ? 1 : 0;
  cout << res << endl;
// */
  /**************************/
/* arithmetic stuff
  LweSample* a = new_gate_bootstrapping_ciphertext_array(size, params);
  encrypt<num_type>(a, pt, sk);

  LweSample* s = new_gate_bootstrapping_ciphertext_array(size, params);
  // zero(s, ck, size);
  // bootsCONSTANT(&s[0], 1, ck);
  // CONSTANT(s, pt2, ck, size);
  encrypt<num_type>(s, pt2, sk);
  LweSample* res = new_gate_bootstrapping_ciphertext_array(size, params);
  // sub(res, a, s, ck, size);
  add(res, a, s, ck, size);
  num_type dec_a = decrypt<num_type>(a, sk);
  num_type dec_s = decrypt<num_type>(s, sk);
  num_type dec_res = decrypt<num_type>(res, sk);
  cout << "pt: " << pt << endl;
  cout << "dec_a: " << dec_a << endl;
  cout << "dec_s: " << dec_s << endl;
  cout << "dec_res: " << dec_res << endl;
  */
  /*
 /*
  // encryption variable
  int array_size = 2;
  LweSample ***arrays = new LweSample**[array_size];
  for(int i = 0; i < array_size; i++) {
    arrays[i] = new LweSample*[array_size];
    for(int j = 0; j < array_size; j++) {
      arrays[i][j] = new_gate_bootstrapping_ciphertext_array(size, params);
      encrypt<num_type>(arrays[i][j], pt, sk);
    }
  }

  LweSample ***sum = new LweSample**[array_size];
  for(int i = 0; i < array_size; i++) {
    sum[i] = new LweSample*[array_size];
    for(int j = 0; j < array_size; j++) {
      sum[i][j] = new_gate_bootstrapping_ciphertext_array(size, params);
    }
  }
  mat_add(sum, arrays, arrays, array_size, array_size, ck, size);
  cout << "added" << endl;*/
  // LweSample *enc = new_gate_bootstrapping_ciphertext_array(size, params);
  // encrypt<num_type>(enc, &pt, sk);
  // // encryption variable 2
  // LweSample *b = new_gate_bootstrapping_ciphertext_array(size, params);
  // num_type pt2 = 7;
  // encrypt<num_type>(b, &pt2, sk);
  //
  // LweSample *sum = new_gate_bootstrapping_ciphertext_array(size, params);
  // sub(sum, enc, b, ck, 16);
  // twosComplement(sum, enc, ck, size);
  // rightRotate(sum, enc, ck, size, 3);
  // leftRotate(sum, enc, ck, size, 16);
  // zero(enc, ck, size);
  // mult(sum, enc, b, ck, size);
  // int times = 3;
  // double stt = omp_get_wtime();
  // for(int i = 0; i < times; i++)
    // reduce_add(sum, arrays, array_size, ck, size);
    // seq_add(sum, arrays, array_size, ck, size);
    // invert(sum, enc, ck, size);
    // bootsOR(sum, enc, b, ck);
    // OR(sum, enc, b, ck, size);
    // mult(sum, enc, b, ck, size);
    // add(sum, enc, b, ck, size);
    // add(sum, enc, b, ck, size);
    // copy(sum, enc, ck, size);
/********/
  // stt = (omp_get_wtime() - stt) / times;
  // printf("time: %f seconds\n", stt);
/*
  num_type recovered[5][5];
  for(int i = 0; i < array_size; i++) {
    for(int j = 0; j < array_size; j++) {
      recovered[i][j] = decrypt<num_type>(sum[i][j], sk);
      cout << recovered[i][j] << " ";
    }
    cout << endl;
  }*/
  // num_type recovered = decrypt<num_type>(enc, sk);
  // printf("recovered: %d\n", recovered);


  // num_type recovered_sum = decrypt<num_type>(sum, sk);
  // printf("recovered_sum: %d\n", recovered_sum);


}
