/**
    * Implements addition, subtraction, and multiplication operations

    * Design philosophy is to rely on as few helper classes/constructs as possible. This means the functions
        accept ordinary C++ types (regular ints) and executes on them
*/

#pragma once


#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <cstddef>
#include "omp_constants.hpp"

void add(LweSample* sum, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void leftRotate(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, const size_t size, int amnt);
void leftShift(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, const size_t size, int amnt);
void rightRotate(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, const size_t size, int amnt);
void rightShift(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, const size_t size, int amnt);
void sub(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void mult(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void twosComplement(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void copy(LweSample* dest, const LweSample* source, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void zero(LweSample* result, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void NOT(LweSample* result, const LweSample* a, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void OR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void AND(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void NAND(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void NOR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void XOR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void XNOR(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void ANDNY(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void ANDYN(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void ORNY(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void ORYN(LweSample* result, const LweSample* a, const LweSample* b, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
void MUX(LweSample* result, const LweSample* a, const LweSample* b, const LweSample* c, const TFheGateBootstrappingCloudKeySet* bk, const size_t size);
