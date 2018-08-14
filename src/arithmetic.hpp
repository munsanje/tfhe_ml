/**
    * Implements addition, subtraction, and multiplication operations

    * Design philosophy is to rely on as few helper classes/constructs as possible. This means the functions
        accept ordinary C++ types (regular ints) and executes on them
*/

#pragma once


#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <cstddef>


void add(LweSample* sum, LweSample* a, LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
void leftRotate(LweSample* result, LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
void leftShift(LweSample* result, LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
void rightRotate(LweSample* result, LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
void rightShift(LweSample* result, LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
void sub(LweSample* result, LweSample* a, LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
void mult(int64_t* a, int64_t* b);
void twosComplement(LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
void copy(LweSample* dest, const LweSample* source, const TFheGateBootstrappingCloudKeySet* bk);

void leftShift_array(LweSample* result, LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
