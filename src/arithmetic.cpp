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
void add(LweSample* sum, LweSample* a, LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  LweSample *carry = new_gate_bootstrapping_ciphertext_array(1, bk->params),
            *tmp = new_gate_bootstrapping_ciphertext_array(1, bk->params);

  bootsCONSTANT(carry, 0, bk);  // zero-initialize carry

  //
  for(int i = 0; i < size - 1; i++) {
    bootsXOR(tmp, &a[i], &b[i], bk);
    bootsXOR(&sum[i], tmp, carry, bk);
    bootsAND(carry, carry, tmp, bk);
    bootsAND(tmp, &a[i], &b[i], bk);
    bootsOR(carry, carry, tmp, bk);
  }
}

// NOTE ASSUMING LSB at 0th entry
void leftRotate(LweSample* result, LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  leftShift(result, a, bk, size);
  bootsOR(&result[0], &result[0], &a[size-1], bk);
}

/*
Numbers are assumed to be encoded in little-endian. I.e, with the LSB at the lowest position. In terms of bit arrays, this means
element 0 is the LSB. A left shift then corresponds to moving i to i+1, etc.
Note the left most bit is set to 0
*/
void leftShift(LweSample* result, LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  bootsCONSTANT(result, 0, bk);
  for(int i = 0; i < size-1; i++) {
    bootsOR(&result[i+1], &a[i], &a[i], bk);
  }
}

void rightRotate(LweSample* result, LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  rightShift(result, a, bk, size);
  bootsOR(&result[size-1], &result[size-1], &a[0], bk);
}

void rightShift(LweSample* result, LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  bootsCONSTANT(result, 0, bk);
  for(int i = size - 1; i > 1; i--) {
    bootsOR(&result[i-1], &a[i], &a[i], bk);
  }
}


void sub(LweSample* result, LweSample* a, LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  LweSample *c = new_gate_bootstrapping_ciphertext_array(size, bk->params);
  copy(c, b, bk);
  twosComplement(c, bk, size);
  add(result, a, c, bk, size);
}


void mult(int64_t* a, int64_t* b) {

}

/* Implements two's complement*/
void twosComplement(LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size) {
  LweSample *one = new_gate_bootstrapping_ciphertext_array(size, bk->params);
  bootsCONSTANT(one, 1, bk);
  for(int i = 0; i < size; i++) {
    bootsNOT(&a[i], &a[i], bk);
  }
  add(a, a, one, bk, size);
}

void copy(LweSample* dest, const LweSample* source, const TFheGateBootstrappingCloudKeySet* bk) {
  bootsCONSTANT(dest, 0, bk);
  bootsOR(dest, dest, source, bk);
}
