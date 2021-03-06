/******************************************************************
 *
 * The main idea is to protect arbitrary executable with 
 * password without modifying target executable itself.
 *
 * From technical perspective, it performs following steps:
 *  1. Reads self byte content, checks for signature
 *  2. Reads target file, with both exe headers and content
 *  3. Encrypts byte content of target file
 *  4. Concatenates self byte content with encrypted content of
 *		a target file
 *  5. Copies concatenated content into new executable file
 *
 * When user runs file from #5 following steps are performed:
 *  1. Application reads self byte content
 *  2. If signature found, then extracts data stored in container
 *		inside of self byte content
 *  3. Decrypts extracted byte array
 *  4. Runs decrypted executable, creating a new process and
		injecting decrypted executable into it
 *
 * This PoC must be compiled for x86 platform and without
 * optimizations. Target applications must be x86.
 *
 ******************************************************************/
#include <stdlib.h>
#include <string>

#include "Io.h"
#include "ByteArray.h"
#include "Container.h"
#include "Injector.h"
#include "StringConverter.h"

/***********************************************************
 *
 * Encrypts byte array, using XOR with provided password
 *
 ***********************************************************/
byte_array encrypt(byte_array data, std::string password) {
	for (size_t i = 0; i < data.size; i++)
		*(data.pointer + i) ^= password[i % password.size()];

	return data;
}

int main(int argc, char* argv[]) {
	if (argc < 1) {
		std::cerr << "Failure occurred while getting app path. " << "Shutting down" << std::endl;
		return EXIT_FAILURE;
	}

	const std::string Sign = "INJECTOR";
	byte_array Signature = str_to_bytearray(Sign);

	std::string self_path = argv[0];

	// Read self content
	byte_array self;
	byte_array self_unwrapped;
	
	try {
		self = read_file(self_path);
		self_unwrapped = container_unwrap(self, Signature);
	}
	catch (std::ifstream::failure e) {
		std::cerr << "Failure occurred while reading self, shutting down..." << std::endl << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	// Doesn't contains container
	if (self_unwrapped.pointer == NULL) {
		std::cout << "Countainer not found. Creating..." << std::endl;

		// Prompts user for a target filename and reads it
		std::string filename;
		byte_array raw_data;

		bool read_successful = false;

		do {
			std::cout << "Enter path to file: " << std::endl;

			try {
				std::cin >> filename;
				raw_data = read_file(filename);
				read_successful = true;
			}
			catch (std::ifstream::failure e) {
				std::cerr << "Failure occurred while reading file: " << e.what() << std::endl;
			}
		} while (read_successful != true);

		// Reads password
		std::string password;
		std::cout << "Enter password: ";
		std::cin >> password;

		// Encrypts & injects target file into host
		byte_array data = encrypt(raw_data, password);
		byte_array target = container_wrap(self, data, Signature);

		// Saves a passwordified copy
		int dotpos = filename.find_last_of('.');
		std::string filename_without_ext = filename.substr(0, dotpos);
		std::string filename_ext = filename.substr(dotpos, 999999);

		try {
			write_file(filename_without_ext + "_copy" + filename_ext, target);
		}
		catch (std::ofstream::failure e) {
			std::cerr << "Failure occurred while writing result, shutting down..." << std::endl << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}
	else {
		std::cout << "Container found. Decrypting..." << std::endl;
		std::cout << "Password to decrypt: ";
		std::string password;
		std::cin >> password;

		// Decrypts self content and runs it
		byte_array decrypted = encrypt(self_unwrapped, password);
		std::wstring wself_path = string_to_wstring(self_path);

		// Runs decrypted executable
		if (run_pe(decrypted, wself_path, L"") == false) {
			std::cerr << "Failed to run application" << std::endl;
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}