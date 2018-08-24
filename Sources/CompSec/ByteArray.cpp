/***************************************************************
 *
 * Purpose: Operations to manipulate byte_array abstraction
 *
 * @author Mikhail Mordvinkin
 *
 **************************************************************/

#include "ByteArray.h"

/***************************************************************
 *
 * Creates new byte_array structure using given pointer to data 
 * and data size
 *
 * @param pointer pointer to byte array 
 * @param size number of bytes in the array
 *
 * @returns structure that encapsulates byte array and its size
 *
 **************************************************************/
byte_array to_byte_array(char* pointer, size_t size) {
	byte_array result = { pointer, size };

	return result;
}

/***************************************************************
 *
 * Copies string into newly allocated char* array and 
 * wraps it in byte_array structure
 *
 * @param data pointer to source string
 * 
 * @returns structure that encapsulates copied byte array and 
 *			its size
 *
 **************************************************************/
byte_array str_to_bytearray(const std::string data) {
	char * pointer = new char[data.size()];
	strcpy(pointer, data.c_str());

	return to_byte_array(pointer, data.size());
}