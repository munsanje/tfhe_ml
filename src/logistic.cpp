#include <iostream>
#include <vector>
#include "logistic.hpp"
#include "io.hpp"
#include "numeric.hpp"
#include "alu.hpp"
#include "matrix.hpp"

using namespace std;


ApproxLogRegression::ApproxLogRegression(char* weight_path, char* coefs_path, int dim, const TFheGateBootstrappingCloudKeySet* ck, size_t size, bool mode_clip)
  : weight_path(weight_path), coefs_path(coefs_path), dim(dim), ck(ck), size(size), mode_clip(mode_clip) {
  // load weights from text file and convert to fixed precision integer
  vector<int> plaintext_weights = float_to_fixed<int>(readFile(weight_path)[0], size, mode_clip);  // FIXME opaque code
  // FIXME make more standardised weight initialization subroutine
  weights = new LweSample*[dim];
  cout << "Converting weights:";
  for(int i = 0; i < dim; i++) {
    weights[i] = new_gate_bootstrapping_ciphertext_array(size, ck->params);
    // convert to constant function
    CONSTANT(weights[i], plaintext_weights[i], ck, size);
    cout << " " << plaintext_weights[i];
  }
  cout << endl;
  // load polynomial coefficients
  vector<int16_t> plaintext_coefs = float_to_fixed<int16_t>(readFile(coefs_path)[0], size, mode_clip);
  degree = plaintext_coefs.size() - 1;
  coefs = new LweSample*[degree + 1];
  cout << "Converting coefficients:";
  for(int i = 0; i < degree + 1; i++) {
    coefs[i] = new_gate_bootstrapping_ciphertext_array(size, ck->params);
    CONSTANT(coefs[i], plaintext_coefs[i], ck, size);
    cout << " " << plaintext_coefs[i];
  }
  cout << endl;


}

// TODO implement destructor

/**
  Original algo: y = 1 / (1 + exp(-WX))
*/
void ApproxLogRegression::predict(LweSample* y, LweSample** X) {
  forward(y, X);
}

/**
  Implements Honier's algorithm:
  b_k = a_k + b_{k+1}*x for k in {0, 1, ... n-1}
  b_n = a_n
  The result is then b_0 = a_0 + b_1 * x
  Reference: https://en.wikipedia.org/wiki/Horner%27s_method
  f(X) = c_0 + c_1 * X + ... + c_n * X^n
  NOTE X is a scalar here
*/
void ApproxLogRegression::approxSigmoid(LweSample* y, LweSample* X) {
  LweSample *temp = new_gate_bootstrapping_ciphertext_array(size, ck->params);
  copy(y, coefs[degree], ck, size);
  for(int i = degree-1; i >= 0; i--) {
    mult(temp, y, X, ck, size);
    add(y, coefs[i], temp, ck, size);
  }
}

void ApproxLogRegression::forward(LweSample* y, LweSample** X) {
  preactivation(y, X);
  approxSigmoid(y, y);
}

/**
Dot product of weights and X: a = X * W, where X is of shape (d, 1) and W is of shape (d, 1)
*/
void ApproxLogRegression::preactivation(LweSample* y, LweSample** X) {
  dot(y, weights, X, dim, ck, size);
}
