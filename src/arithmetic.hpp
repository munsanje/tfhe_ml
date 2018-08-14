/**
    * Implements addition, subtraction, and multiplication operations

    * Design philosophy is to rely on as few helper classes/constructs as possible. This means the functions
        accept ordinary C++ types (regular ints) and executes on them
*/

#pragma once


#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <cstddef>


void add(LweSample* sum, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
void leftRotate(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size, int amnt);
void leftShift(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size, int amnt);
void rightRotate(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size, int amnt);
void rightShift(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size, int amnt);
void sub(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
void mult(LweSample* result, LweSample* a, LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
void twosComplement(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
void copy(LweSample* dest, const LweSample* source, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
void zero(LweSample* result, const TFheGateBootstrappingCloudKeySet* bk, size_t size);
