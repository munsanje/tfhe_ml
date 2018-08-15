#include "arithmetic.hpp"
/*
Implements bitwise full-adder circuit on two 64-bit integers
TODO: sort out size type convention, perhaps make flexible
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
void add(LweSample* sum, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  LweSample *carry = new_gate_bootstrapping_ciphertext(bk->params),
            *tmp = new_gate_bootstrapping_ciphertext(bk->params);

  // first iteration
  bootsXOR(&sum[0], &a[0], &b[0], bk);
  bootsAND(carry, &a[0], &b[0], bk);
  for(int i = 1; i < size; i++) {
    // sum
    bootsXOR(tmp, &a[i], &b[i], bk);
    bootsXOR(&sum[i], tmp, carry, bk);
    // carry
    bootsAND(carry, carry, tmp, bk);
    bootsAND(tmp, &a[i], &b[i], bk);
    bootsOR(carry, carry, tmp, bk);
  }

  // clean up
  delete_gate_bootstrapping_ciphertext(carry);
  delete_gate_bootstrapping_ciphertext(tmp);
}

/**
Python code: lr = lambda x, n, amnt: ((x << amnt) | (x >> (n-amnt)))&(2**n-1)
*/
void leftRotate(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size, int amnt) {
  leftShift(result, a, bk, size, amnt);
  for(int i = 0; i < amnt; i++) {
    bootsCOPY(&result[i], &a[size-amnt+i], bk);
  }
}

/*
Numbers are assumed to be encoded in little-endian. I.e, with the LSB at the lowest position. In terms of bit arrays, this means
element 0 is the LSB. A left shift then corresponds to moving i to i+1, etc.
Note the left most bit is set to 0
*/
void leftShift(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size, int amnt) {
  zero(&result[0], bk, size);
  for(int i = 0; i < size-amnt; i++) {
    bootsCOPY(&result[i+amnt], &a[i], bk);
  }
}

/**
Python code: rr = lambda x, n, amnt: ((x >> amnt) | (x << (n-amnt)))&(2**n-1)
*/
void rightRotate(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size, int amnt) {
  rightShift(result, a, bk, size, amnt);
  for(int i = 0; i < amnt; i++) {
    bootsCOPY(&result[size-amnt+i], &a[i], bk);
  }
}

void rightShift(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size, int amnt) {
  zero(result, bk, size);
  for(int i = size-1; i > (amnt-1); i--) {
    bootsCOPY(&result[i-amnt], &a[i], bk);
  }
}


void sub(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  LweSample *c = new_gate_bootstrapping_ciphertext_array(size, bk->params);
  twosComplement(c, b, bk, size);
  add(result, a, c, bk, size);

  // clean up
  delete_gate_bootstrapping_ciphertext_array(size, c);
}


/**
Implements simple shift and add algorithm:
Let A and B be the operands, s.t P = AxB
Then P = Axb_0 << 0 + Axb_1 << 1 + ... + Axb_{n-1} << {n-1}

TODO
1. implement BW algorithm
2. Vectorize
Reference:
Baugh-Wooley Multiplication Algorithm.
http://www.ece.uvic.ca/~fayez/courses/ceng465/lab_465/project2/multiplier.pdf
*/
void mult(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  // to store the intermediate results of final result. Note intermediate result has 2n bits
  // p = p_0 * 2^0 + .. + p_{n-1} * 2^{n-1}
  int int_size = 2*size;
  LweSample *p = new_gate_bootstrapping_ciphertext_array(int_size, bk->params);
  // accumulator
  LweSample *acc = new_gate_bootstrapping_ciphertext_array(int_size, bk->params);
  LweSample *acc_cpy = new_gate_bootstrapping_ciphertext_array(int_size, bk->params);
  zero(acc, bk, int_size);
  for(int i = 0; i < size; i++) {
    zero(p, bk, int_size);
    for(int j = 0; j < size; j++) {
      bootsAND(&p[i+j], &a[j], &b[i], bk);
    }
    copy(acc_cpy, acc, bk, int_size);
    add(acc, acc_cpy, p, bk, int_size);
  }
  // copy only lower n bits (precision loss)
  copy(result, acc, bk, size);

  // clean up
  delete_gate_bootstrapping_ciphertext_array(int_size, p);
  delete_gate_bootstrapping_ciphertext_array(int_size, acc);
  delete_gate_bootstrapping_ciphertext_array(int_size, acc_cpy);
}

/* Implements two's complement*/
void twosComplement(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  LweSample *one = new_gate_bootstrapping_ciphertext_array(size, bk->params),
            *c = new_gate_bootstrapping_ciphertext_array(size, bk->params);;
  zero(one, bk, size);
  bootsCONSTANT(&one[0], 1, bk);
  copy(c, a, bk, size);

  for(int i = 0; i < size; i++) {
    bootsNOT(&c[i], &c[i], bk);
  }
  add(result, c, one, bk, size);

  // clean up
  delete_gate_bootstrapping_ciphertext_array(size, one);
  delete_gate_bootstrapping_ciphertext_array(size, c);
}

void copy(LweSample* dest, const LweSample* source, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  for(int i = 0; i < size; i++) {
    bootsCOPY(&dest[i], &source[i], bk);
  }
}

void zero(LweSample* result, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  for(int i = 0; i < size; i++)
    bootsCONSTANT(&result[i], 0, bk);
}
