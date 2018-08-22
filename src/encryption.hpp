/*
Implements higher level encryption functions to make using tfhe easier
*/

#pragma once

#include <cstddef>
#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>


/*Encrypt plaintext sample given secret key*/
template<typename T>
void encrypt(LweSample* cipher, T* plaintext, const TFheGateBootstrappingSecretKeySet* sk) {
  uint8_t type_size = sizeof(T) * 8;
  for(int i = 0; i < type_size; i++) {
    bootsSymEncrypt(&cipher[i], (*plaintext >> i) & 1, sk);
  }
}


/*Decrypt cipher given secret key*/
template<typename T>
T decrypt(LweSample* cipher, const TFheGateBootstrappingSecretKeySet* sk) {
  uint8_t type_size = sizeof(T) * 8;
  T plaintext = 0;
  for(int i = 0; i < type_size; i++) {
    plaintext |= bootsSymDecrypt(&cipher[i], sk) << i;
  }
  return plaintext;
}
