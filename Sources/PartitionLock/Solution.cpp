#include <string>

#include "Io.h"
#include "ByteArray.h"
#include "Injector.h"
#include "Container.h"
#include "StringConverter.h"

#include "Host.h"

const std::string patch_mode = std::string("patch");

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

int main_routine(std::wstring self_path) {
	byte_array self = read_file(self_path);
	byte_array self_unwrapped = container_get_host(self, Signature);

	// Pointer == NULL - initial run, otherwise application have been run at least once
	if (self_unwrapped.pointer == NULL) {
		std::wstring host_filename = L"C:\\Windows\\System32\\svchost.exe";

		// Run host application and inject self to patch executable on disk
		std::wstring cmdline_args = host_filename + L" " + string_to_wstring(patch_mode) + L" \"" + self_path + L"\"";
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

int patch_self(std::wstring original_file) {
	// Read binary volume id
	int current_volume_id = get_volume_id(original_file);
	byte_array volume_id_serialized = to_byte_array((char*)(&current_volume_id), sizeof(int));

	// Modify executable, storing binary volume id in container
	byte_array self = read_file(original_file);
	byte_array self_modified = container_wrap(self, volume_id_serialized, Signature);

	write_file(original_file, self_modified);

	// Start otiginal process
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION process_info;

	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));

	if (!CreateProcess(
		original_file.c_str(),
		(LPWSTR)(original_file.c_str()),
		NULL,
		NULL,
		false,
		CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP,
		NULL,
		get_folder_name(original_file).c_str(),
		&StartupInfo,
		&process_info)
		) {
		std::cerr << "Failed to run modified executable, task terminated. Error code: " << GetLastError() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
	if (argc < 1) {
		printf("Cannot get app path, something went wrong");
		return EXIT_FAILURE;
	}

	std::wstring self_path = get_self_path(argv[0]);

	if (argc < 2) {
		return main_routine(self_path);
	}
	else {
		std::string mode = argv[1];

		if (patch_mode.compare(mode) == 0) {
			std::string original_file = argv[2];

			return patch_self(string_to_wstring(original_file));
		}
		else {
			std::cout << "Unrecognized mode" << std::endl;

			return EXIT_FAILURE;
		}
	}
}