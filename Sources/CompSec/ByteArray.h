#ifndef BYTE_ARRAY
#define BYTE_ARRAY

#include <string>

/***************************************************************
 *
 * Encapsulates array of bytes and its size inside one structure 
 *
 **************************************************************/
struct byte_array {
	char* pointer;
	size_t size;
};

/***************************************************************
 *
 * Wraps pointer to byte array and size into one structure.
 * Does not copy source data
 *
 * @param pointer pointer to byte array
 * @param size number of bytes in the array
 *
 **************************************************************/
byte_array to_byte_array(char* pointer, size_t size);

/***************************************************************
 *
 * Copies string into byte array
 *
 * @param data input string
 *
 **************************************************************/
byte_array str_to_bytearray(const std::string data);

#endif BYTE_ARRAY