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
 *
 * @param pointer pointer to byte array
 * @param size number of bytes in the array
 *
 * @returns byte_array Structure that holds both original pointer
 *			and size of data. Does NOT copy source data
 *
 **************************************************************/
byte_array to_byte_array(char* pointer, size_t size);

/***************************************************************
 *
 * Copies string into byte array
 *
 * @param data input string
 *
 * @returns byte_array Structure that holds pointer to copied
 *			buffer and size of string.
 *
 **************************************************************/
byte_array str_to_bytearray(const std::string data);

#endif BYTE_ARRAY