#include "Container.h"

/***************************************************************
 *
 * Hides one byte array in another
 *
 * Added bytes from `data` and bytes from `signature` to 
 * the end of `host` bytes
 *
 **************************************************************/
byte_array container_wrap(const byte_array host, const byte_array data, const byte_array signature) {
	size_t container_size = host.size + data.size + signature.size + sizeof(int);
	char * container_data = new char[container_size];

	memcpy(container_data, host.pointer, host.size);
	memcpy(container_data + host.size, data.pointer, data.size);
	memcpy(container_data + host.size + data.size, &(data.size), sizeof(int));
	memcpy(container_data + host.size + data.size + sizeof(int), signature.pointer, signature.size);

	return to_byte_array(container_data, container_size);
}


/*****************************************************************
 * 
 * Unwraps container, reclaiming data from it.
 *
 * Searches for signature, reads size of hidden array and copies
 * hidden data into an allocated buffer
 *
 * @returns byte_array(NULL, 0) if container's signature doesn't 
 *			 match given signature
 * 
 ****************************************************************/
byte_array container_unwrap(const byte_array data, const byte_array signature) {
	if (memcmp(data.pointer + (data.size - signature.size), signature.pointer, signature.size) != 0)
		return to_byte_array(NULL, 0);

	size_t contained_data_size;
	memcpy(&contained_data_size, data.pointer + (data.size - signature.size - sizeof(int)), sizeof(int));

	char * contained_data = new char[contained_data_size];
	memcpy(contained_data, data.pointer + data.size - signature.size - sizeof(int) - contained_data_size, contained_data_size);

	return to_byte_array(contained_data, contained_data_size);
}

/****************************************************************
 *
 * Unwraps container, reclaiming host from it.
 *
 * Searches for signature, reads size of hidden array and copies
 * host data into an allocated buffer
 *
 * @eturns byte_array(NULL, 0) if container's signature doesn't 
 *			match given signature
 *
 ****************************************************************/
byte_array container_get_host(const byte_array data, const byte_array signature) {
	// Check signature
	if (memcmp(data.pointer + (data.size - signature.size), signature.pointer, signature.size) != 0)
		return to_byte_array(NULL, 0);

	size_t contained_data_size;
	memcpy(&contained_data_size, data.pointer + (data.size - signature.size - sizeof(int)), sizeof(int));

	size_t host_size = data.size - signature.size - sizeof(int) - contained_data_size;
	char * host_data = new char[host_size];
	memcpy(host_data, data.pointer, host_size);

	return to_byte_array(host_data, host_size);
}