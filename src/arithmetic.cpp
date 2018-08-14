#include "arithmetic.hpp"
#include <cstdio>
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
    bootsXOR(tmp, &a[i], &b[i], bk);
    bootsXOR(&sum[i], tmp, carry, bk);
    bootsAND(carry, carry, tmp, bk);
    bootsAND(tmp, &a[i], &b[i], bk);
    bootsOR(carry, carry, tmp, bk);
  }
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
}


void mult(LweSample* result, LweSample* a, LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {

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
