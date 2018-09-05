/**
* Implementation of approximate logistic regression.
* Approximates sigmoid function with n-degree polynomial.
* Coefficients are estimated using Remez's algorithm, as implemented in the lolremez tool (https://github.com/samhocevar/lolremez)
* Assumes weights have already been optimized in the clear

Components:
1. Data
 - Data will be in form of Nxd array
 - Data will be encrypted by the time it gets to this module

2. Outputs
  - Outputs will be in form of bits. Each bit will give target estimate

*/

#pragma once

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <cstddef>


struct logreg {
  // weights
  LweSample **weights;
  // dimension
  int dim;
  // polynomial parameters
  uint16_t coefs[];

  // polynomial degree
  uint8_t n;


  void load_coefs(char* path) {
    
  }
};


void predict(LweSample* y, const LweSample** X, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);

void approx_sigmoid(LweSample* output, const LweSample* X, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);

void forward(LweSample* output, const LweSample** X, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);

void preactivation(LweSample* output, const LweSample** weights, const TFheGateBootstrappingCloudKeySet* ck, const size_t size);
