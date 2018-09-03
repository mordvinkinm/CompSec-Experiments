#include <string>

#include "Io.h"
#include "ByteArray.h"
#include "Injector.h"
#include "Container.h"
#include "StringConverter.h"

#include "Host.h"

const std::string patch_mode = std::string("patch");
const std::string after_patching_mode = std::string("after_patching");

const std::string Sign = "VLM";
const byte_array Signature = str_to_bytearray(Sign);

int get_volume_id(std::wstring path) {
	int startIndex = path.find('\\');
	DWORD volume_id;
	std::wstring volume_label = std::wstring(path.begin(), path.begin() + startIndex + 1);
	GetVolumeInformation(volume_label.c_str(), NULL, 0, &volume_id, NULL, NULL, NULL, 0);

	return volume_id;
}

void run_application() {
	std::cout << "Program works!" << std::endl;
}

void log(std::string message) {
	std::ofstream outfile;
	outfile.open("C:\\Users\\Mikhail\\Repositories\\Public\\CompSec-Experiments\\Output\\Release\\log.txt", std::ios_base::app);
	outfile << message << std::endl;
	outfile.close();
}

void log(std::wstring message) {
	std::wofstream outfile;
	outfile.open("C:\\Users\\Mikhail\\Repositories\\Public\\CompSec-Experiments\\Output\\Release\\log.txt", std::ios_base::app);
	outfile << message << std::endl;
	outfile.close();
}

int main_routine(std::wstring self_path) {
	log("Main routine: started");

	byte_array self = read_file(self_path);
	byte_array self_unwrapped = container_get_host(self, Signature);

	// Pointer == NULL - initial run, otherwise application have been run at least once
	if (self_unwrapped.pointer == NULL) {
		// Determine filename for the new host app
		std::wstring host_filename = get_folder_name(self_path) + L"\\Host.exe";

		// Write host app to the hard drive
		int host_data_length = sizeof(host_app_data) / sizeof(*host_app_data);
		byte_array host_data = to_byte_array(host_app_data, host_data_length);

		try {
			write_file(host_filename, host_data);
		}
		catch(std::ofstream::failure e) {
			std::cerr << "Failure occurred while writing host executable to disk, shutting down..." << std::endl << e.what() << std::endl;
			return EXIT_FAILURE;
		}

		// Run host app
		std::wstring cmdline_args = host_filename + L" " + string_to_wstring(patch_mode) + L" \"" + get_file_name(self_path) + L"\"";
		run_pe(self, host_filename, cmdline_args, CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP | CREATE_NO_WINDOW, get_folder_name(self_path));
	}
	else {
		// Application was run once
		byte_array data_unwrapped = container_unwrap(self, Signature);

		// Check that volume id stored in executable equal to actual volume id
		int actual_volume_id = get_volume_id(self_path);
		int stored_volume_id = *(int*)(data_unwrapped.pointer);

		if (stored_volume_id != actual_volume_id) {
			std::cout << "You must run this application from the same drive you originally ran it from." << std::endl;
		}
		else {
			run_application();
		}

		system("pause");
	}

	return EXIT_SUCCESS;
}

std::wstring get_self_path(char* argv_path) {
	std::string cmd_s(argv_path);
	std::wstring cmd_ws = string_to_wstring(std::string(argv_path));
	LPCWSTR cmd_lws = (LPCWSTR)(cmd_ws.c_str());

	LPTSTR lpBuffer = new TCHAR[2048];
	GetFullPathName(cmd_lws, 2048, lpBuffer, NULL);

	return std::wstring(lpBuffer);
}

int patch_self(std::wstring self_path, std::wstring original_file) {
	log(L"Patch: started");
	log(L"Patch: " + self_path);
	log(L"Patch: " + original_file);


	// Read binary volume id
	int current_volume_id = get_volume_id(self_path);
	byte_array volume_id_serialized = to_byte_array((char*)(&current_volume_id), sizeof(int));

	// Modify executable, storing binary volume id in container
	std::wstring original_file_path = get_folder_name(self_path) + L"\\" + original_file;
	byte_array self = read_file(original_file_path);
	byte_array self_modified = container_wrap(self, volume_id_serialized, Signature);

	write_file(original_file_path, self_modified);

	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION process_info;

	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));

	auto cmd_line = original_file_path + L" " + string_to_wstring(after_patching_mode);

	log(L"original_file_path: " + original_file_path);
	log(L"CMD_Line: " + cmd_line);

	if (!CreateProcess(
		(LPCWSTR)(original_file_path.c_str()), 
		(LPWSTR)(cmd_line.c_str()), 
		NULL, 
		NULL, 
		false, 
		CREATE_NEW_PROCESS_GROUP | CREATE_NO_WINDOW, 
		NULL, 
		NULL, 
		&StartupInfo, 
		&process_info)
	) {
		std::cerr << "Failed to run modified executable, task terminated. Error code: " << GetLastError() << std::endl;
		log("Failed to run modified executable: ");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int after_patching(std::wstring self_path) {
	log("After Patching: started");

	std::wstring host_filename = get_folder_name(self_path) + L"\\Host.exe";

	log(L"Self_path: " + self_path);
	log(L"Host_filename: " + host_filename);

	if (std::remove(wstring_to_string(host_filename).c_str()) != 0) {
		std::cerr << "Failed to remove temporary host file, please remove it manually. Error code: " << GetLastError() << std::endl;
		log("Failed to remove temporary host file");
	}

	main_routine(self_path);

	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
	log("-------------");
	log("Application started with following parameters:");
	for (int i = 0; i < argc; i++) {
		log(std::string(argv[i]));
	}
	log("------------");

	if (argc < 1) {
		printf("Cannot get app path, something went wrong");
		log("Failure. Cannot get app path");
		return EXIT_FAILURE;
	}

	std::wstring self_path = get_self_path(argv[0]);
	log(self_path);

	if (argc < 2) {
		return main_routine(self_path);
	}
	else {
		std::string mode = argv[1];

		log(mode);

		if (patch_mode.compare(mode) == 0) {
			std::string original_file = argv[2];
			log(original_file);

			return patch_self(self_path, string_to_wstring(original_file));
		}
		else if (mode == after_patching_mode) {
			return after_patching(self_path);
		}

		std::cout << "Unrecognized mode" << std::endl;
		return EXIT_FAILURE;
	}
}