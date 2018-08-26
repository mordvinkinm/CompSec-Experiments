#ifndef FUNCTION_ENCRYPTOR
#define FUNCTION_ENCRYPTOR

#include <stdio.h>

#include "Injector.h"
#include "ByteArray.h"

const char PreRetVal = 0x5D;
const char RetVal = 0xC3;

/***************************************************************
 *
 * Encrypts in-memory function using provided key
 *
 * @param func_ptr pointer to function
 * @param secret_key encryption key
 *
 * @returns none
 *
 **************************************************************/
void encrypt(void* func_ptr, std::string secret_key);

/***************************************************************
 *
 * Decrypts in-memory function using provided key
 *
 * @param func_ptr
 * @param secret_key
 *
 * @returns none
 *
 **************************************************************/
void decrypt(void* func_ptr, std::string secret_key);

#endif FUNCTION_ENCRYPTOR