#ifndef CONTAINER
#define CONTAINER

#include <string>

#include "ByteArray.h"

/***************************************************************
 *
 * Hides one byte array in another
 *
 * @param host "Host" byte array
 * @param data Byte array to hide
 * @param signature Signature to determine if host byte array
 *			contains hidden data or not
 *
 * @returns byte_array structure.
 *
 **************************************************************/
byte_array container_wrap(const byte_array host, const byte_array data, const byte_array signature);

/***************************************************************
 *
 * Unwraps container, reclaiming hidden data from it
 *
 * @param data Byte array that contains both 'host' original
 *			data and hidden byte array
 * @param signature Signature to determine if byte array contains
 *			hidden data or not
 *
 * @returns byte_array Structure with actual data or 
 *			byte_array(NULL, 0) if signature wasn't found inside
 *			of container
 *
 **************************************************************/
byte_array container_unwrap(const byte_array data, const byte_array signature);

/***************************************************************
 *
 * Unwraps container, reclaiming host data from it
 *
 * @param data Byte array that contains both 'host' original
 *				data and hidden byte array
 * @param signature Signature to determine if byte array contains 
 *			hidden data or not
 *
 * @returns byte_array Structure with actual data or 
 *			byte_array(NULL, 0) if signature wasn't found inside
 *			of container
 *
 **************************************************************/
byte_array container_get_host(const byte_array data, const byte_array signature);

#endif CONTAINER