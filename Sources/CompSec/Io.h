#ifndef IO
#define IO

#include <string>
#include <iostream>
#include <fstream>

#include "ByteArray.h"

/***************************************************************
 *
 * Reads binary data from a file
 *
 * @param filename path to file
 *
 * @returns byte array with file content
 *
 **************************************************************/
byte_array read_file(std::string filename);

/***************************************************************
 *
 * Reads binary data from a file
 *
 * @param filename path to file
 *
 * @returns byte array with file content
 *
 **************************************************************/
byte_array read_file(std::wstring filename);

/***************************************************************
 *
 * Writes binary content to a file
 *
 * @param filename path to file
 * @param data binary data to write
 *
 **************************************************************/
void write_file(std::string filename, byte_array data);

/***************************************************************
 *
 * Writes binary content to a file
 *
 * @param filename path to file
 * @param data binary data to write
 *
 **************************************************************/
void write_file(std::wstring filename, byte_array data);

#endif /* IO_H */