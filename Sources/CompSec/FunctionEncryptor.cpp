#include "FunctionEncryptor.h"

/***************************************************************
 *
 * Encrypts in-memory function using provided key, using XOR
 *	as an encryption algorithm.
 *
 * To do that, reads process memory, encrypts function and writes
 *	modified process memory back
 *
 **************************************************************/
void encrypt(void* func_ptr, std::string secret_key){
    const int default_read_size = 1024;
    byte_array data = read_current_proc_memory(func_ptr, default_read_size);

    for (size_t i = 0, key_index = 0; i < data.size; i++, key_index = (key_index + 1) % secret_key.size()){
        if (*(data.pointer + i) == PreRetVal && *(data.pointer + i + 1) == RetVal)
            break;

        *(data.pointer + i) = *(data.pointer + i) ^ secret_key[key_index];
    }

    write_current_proc_memory(func_ptr, data);
}

/***************************************************************
 *
 * Decrypts in-memory function using provided key.
 *
 * Since encryption algorithm is XOR, decryption algorithm equals
 *	to encryption algorithm.
 *
 **************************************************************/
void decrypt(void* func_ptr, std::string secret_key){
    encrypt(func_ptr, secret_key);
}