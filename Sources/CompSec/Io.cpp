#include "Io.h"

/***************************************************************
 *
 * Reads binary data from a file
 *
 **************************************************************/
byte_array read_file(std::wstring filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		throw std::ifstream::failure(strerror(errno));
	}

	// Seeks end of file to get content size
	file.seekg(0, std::ios::beg);
	auto buf_begin = file.tellg();
	file.seekg(0, std::ios::end);
	auto buf_end = file.tellg();

	long buf_size = buf_end - buf_begin;

	// Rewind cursor to the beginning of file
	file.seekg(0, std::ios::beg);

	// Allocates 
	char * buffer = new char[buf_size];

	file.read(buffer, buf_size);

	file.close();

	return to_byte_array(buffer, buf_size);
}

/***************************************************************
 *
 * Reads binary data from a file
 *
 **************************************************************/
byte_array read_file(std::string filename) {
	return read_file(std::wstring(filename.begin(), filename.end()));
}

/***************************************************************
 *
 * Writes binary content to a file
 *
 **************************************************************/
void write_file(std::wstring filename, byte_array data) {
	std::ofstream file(filename, std::ios::binary);
	if (!file) {
		throw std::ofstream::failure(strerror(errno));
	}

	file.write(data.pointer, data.size);

	file.close();
}

/***************************************************************
 *
 * Writes binary content to a file
 *
 **************************************************************/
void write_file(std::string filename, byte_array data) {
	write_file(std::wstring(filename.begin(), filename.end()), data);
}