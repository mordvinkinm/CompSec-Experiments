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
 * @returns byte_array Byte array with file content
 *
 **************************************************************/
byte_array read_file(std::string filename);

/***************************************************************
 *
 * Reads binary data from a file
 *
 * @param filename path to file
 *
 * @returns byte_array Byte array with file content
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

/***************************************************************
 *
 * Get folder name from a full file path (without trailing slash)
 * For input "C:\temp\example.exe" it returns "C:\temp"
 *
 * @param file_path absolute file path
 * 
 * @returns std::wstring 
 *
 **************************************************************/
std::wstring get_folder_name(std::wstring file_path);

/***************************************************************
 *
 * Get file name from a full file path
 * For input "C:\temp\example.exe" it returns "example.exe"
 *
 * @param file_path absolute file path
 *
 * @returns std::wstring
 *
 **************************************************************/
std::wstring get_file_name(std::wstring file_path);

#endif /* IO_H */