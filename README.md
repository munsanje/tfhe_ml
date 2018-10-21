# tfhe_ml
Machine learning on homomorphically encrypted data

This repo relies on the TFHE homomorphic encryption library linked at the bottom of this page. We provide two things:

1. An implementation of basic arithmetic operations using TFHE

2. An application of the above operations to logistic regression

## Example usage

## Package dependencies

NOTE: This code has only been tested on an Ubuntu 16.04 machine.

This code depends on the TFHE library.

### Installing TFHE library

1. Clone the TFHE repo to the directory `$TFHE_DIR` (assuming that's where you've stored the location) and run the following commands:


        cd $TFHE_DIR
        git submodule init
        git submodule update
        mkdir build
        cd build
        cmake .. ${BUILD_FLAGS:-'-DENABLE_FFTW=on -DCMAKE_BUILD_TYPE=optim -DENABLE_TESTS=off'}
        make


The library is now built. See tfhe_ml/src/Makefile to see how to compile the program.


### Encrypting two integers and adding the results, then decrypting

    #include "encryption.hpp"
    #include "alu.hpp"
    #include <iostream>

    int main() {

        typdedef num_type int8_t;
        size_t size = sizeof(num_type) * 8;
        // setup
        const int minimum_lambda = 128;
        TFheGateBootstrappingParameterSet* params = new_default_gate_bootstrapping_parameters(minimum_lambda);
        const TFheGateBootstrappingSecretKeySet* sk = new_random_gate_bootstrapping_secret_keyset(params);
        const TFheGateBootstrappingCloudKeySet* ck = &sk->cloud;

        LweSample *enc_a = new_gate_bootstrapping_ciphertext_array(size, params),
                  *enc_b = new_gate_bootstrapping_ciphertext_array(size, params);

        // define integer
        num_type pt = 8, pt2 = 45;
        encrypt<num_type>(enc_a, pt, sk);
        encrypt<num_type>(enc_b, pt2, sk);

        // create new encrypted object to hold sum
        LweSample *sum = new_gate_bootstrapping_ciphertext_array(size, params);


        // add the two integers homomorphically
        add(sum, enc_a, enc_b, ck, size);

        // decrypt and print out
        num_type plain_sum= decrypt<num_type>(sum, sk);

        std::cout << pt << " + " << pt2 << " = " << plain_sum << std::endl;


# References
1. TFHE: https://github.com/tfhe/tfhe
