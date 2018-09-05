#include "alu.hpp"
/*
Implements bitwise full-adder circuit on two n-bit integers
Parallel implementation gives ~0.65x speedup, which close to theoretical circuit speedup of 0.6
Reference: https://en.wikibooks.org/wiki/Microprocessor_Design/Add_and_Subtract_Blocks
Pseudocode

module full_adder(a, b, cin, cout, s);
   input a, b, cin;
   output cout, s;
   wire temp;
   temp = a ^ b;
   s = temp ^ cin;
   cout = (cin & temp) | (a & b);
endmodule
*/
void add(LweSample* sum, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  LweSample *carry = new_gate_bootstrapping_ciphertext(ck->params),
            *tmp_s = new_gate_bootstrapping_ciphertext(ck->params),
            *tmp_c = new_gate_bootstrapping_ciphertext(ck->params);

  // first iteration
  bootsXOR(&sum[0], &a[0], &b[0], ck);
  bootsAND(carry, &a[0], &b[0], ck);
  for(int i = 1; i < size; i++) {

    #pragma omp parallel sections num_threads(2)
    {
      #pragma omp section
      bootsXOR(tmp_s, &a[i], &b[i], ck);
      #pragma omp section
      bootsAND(tmp_c, &a[i], &b[i], ck);
    }
    #pragma omp parallel sections num_threads(2)
    {
      #pragma omp section
      bootsXOR(&sum[i], tmp_s, carry, ck);
      #pragma omp section
      bootsAND(carry, carry, tmp_s, ck);
    }
    bootsOR(carry, carry, tmp_c, ck);
  }

  // clean up
  delete_gate_bootstrapping_ciphertext(carry);
  delete_gate_bootstrapping_ciphertext(tmp_s);
  delete_gate_bootstrapping_ciphertext(tmp_c);
}


/**
  Sequential array sum implementation. Included for completeness and testing
*/
void seq_add(LweSample* result, LweSample** arrays, int num_arrays, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  zero(result, ck, size);
  for(int i = 0; i < num_arrays; i++) {
    add(result, result, arrays[i], ck, size);
  }
}


/**
Parallel reduce sum implementation. Gives ~1.6x speedup over sequential
TODO: use std::vector

*/
void reduce_add(LweSample* result, LweSample** arrays, int num_arrays, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {

  if(num_arrays == 1) {
    copy(result, arrays[0], ck, size);
    return;
  }
  if(num_arrays == 2) {
    add(result, arrays[0], arrays[1], ck, size);
    return;
  }
  int mid_point = num_arrays / 2;
  LweSample *result1 = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  #pragma omp parallel sections num_threads(2)
  {
    #pragma omp section
    reduce_add(result, arrays, mid_point, ck, size);
    #pragma omp section
    reduce_add(result1, &arrays[mid_point], num_arrays-mid_point, ck, size);
  }
  add(result, result, result1, ck, size);
  delete_gate_bootstrapping_ciphertext_array(size, result1);
}

/**
Python code: lr = lambda x, n, amnt: ((x << amnt) | (x >> (n-amnt)))&(2**n-1)
*/
void leftRotate(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt) {
  leftShift(result, a, ck, size, amnt);
  copy(result, &a[size-amnt], ck, amnt);
}

/*
Numbers are assumed to be encoded in little-endian. I.e, with the LSB at the lowest position. In terms of bit arrays, this means
element 0 is the LSB. A left shift then corresponds to moving i to i+1, etc.
Note the left most bit is set to 0
*/
void leftShift(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt) {
  zero(&result[0], ck, size);
  copy(&result[amnt], a, ck, size-amnt);
}

/**
Python code: rr = lambda x, n, amnt: ((x >> amnt) | (x << (n-amnt)))&(2**n-1)
*/
void rightRotate(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt) {
  rightShift(result, a, ck, size, amnt);
  copy(&result[size-amnt], a, ck, amnt);
}

/***/
void rightShift(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size, int amnt) {
  zero(result, ck, size);
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = size-1; i > (amnt-1); i--) {
    bootsCOPY(&result[i-amnt], &a[i], ck);
  }
}

/***/
void sub(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  LweSample *c = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  twosComplement(c, b, ck, size);
  add(result, a, c, ck, size);

  // clean up
  delete_gate_bootstrapping_ciphertext_array(size, c);
}

/**
Implements simple shift and add algorithm:
Let A and B be the operands, s.t P = AxB
Then P = Axb_0 << 0 + Axb_1 << 1 + ... + Axb_{n-1} << {n-1}
1. Multiply each bit of A by each bit of B, to get n n-bit arrays.
  - Note that for full precision, we would need n 2n-bit arrays. But we are dealing with fixed precision.
2. Reduce sum each of the n arrays

Space complexity is O(n^2)

*/
void mult(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  // to store the intermediate results of final result. Note intermediate result has 2n bits
  // p = p_0 * 2^0 + .. + p_{n-1} * 2^{n-1}
  // Create n n-bit arrays
  LweSample **p = new LweSample*[size];
  for(int i = 0; i < size; i++) {
    p[i] = new_gate_bootstrapping_ciphertext_array(size, ck->params);
    zero(p[i], ck, size);
  }
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    for(int j = 0; j < size-i; j++) {
      bootsAND(&p[i][i+j], &a[j], &b[i], ck);
    }
  }
  reduce_add(result, p, size, ck, size);

  // clean up
  for(int i = 0; i < size; i++)
    delete_gate_bootstrapping_ciphertext_array(size, p[i]);
  delete[] p;
}

/* Implements two's complement*/
void twosComplement(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  LweSample *one = new_gate_bootstrapping_ciphertext_array(size, ck->params),
            *c = new_gate_bootstrapping_ciphertext_array(size, ck->params);;
  zero(one, ck, size);
  bootsCONSTANT(&one[0], 1, ck);

  NOT(c, a, ck, size);
  add(result, c, one, ck, size);

  // clean up
  delete_gate_bootstrapping_ciphertext_array(size, one);
  delete_gate_bootstrapping_ciphertext_array(size, c);
}

void NOT(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsNOT(&result[i], &a[i], ck);
  }
}

void copy(LweSample* dest, const LweSample* source, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsCOPY(&dest[i], &source[i], ck);
  }
}

void zero(LweSample* result, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsCONSTANT(&result[i], 0, ck);
  }
}

void OR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsOR(&result[i], &a[i], &b[i], ck);
  }
}

void AND(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsAND(&result[i], &a[i], &b[i], ck);
  }
}

void NAND(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsNAND(&result[i], &a[i], &b[i], ck);
  }
}

void NOR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsNOR(&result[i], &a[i], &b[i], ck);
  }
}

void XOR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsXOR(&result[i], &a[i], &b[i], ck);
  }
}

void XNOR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsXNOR(&result[i], &a[i], &b[i], ck);
  }
}
void ANDNY(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsANDNY(&result[i], &a[i], &b[i], ck);
  }
}
void ANDYN(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsANDYN(&result[i], &a[i], &b[i], ck);
  }
}
void ORNY(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsORNY(&result[i], &a[i], &b[i], ck);
  }
}
void ORYN(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsORYN(&result[i], &a[i], &b[i], ck);
  }
}
void MUX(LweSample* result, const LweSample* a, const LweSample* b, const LweSample* c, const TFheGateBootstrappingCloudKeySet* ck, const size_t size) {
  #pragma omp parallel for num_threads(NUM_THREADS)
  for(int i = 0; i < size; i++) {
    bootsMUX(&result[i], &a[i], &b[i], &c[i], ck);
  }
}
