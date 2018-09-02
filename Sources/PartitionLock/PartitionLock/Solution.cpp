#include <string>

#include "Io.h"
#include "ByteArray.h"
#include "Injector.h"
#include "Container.h"

#include "Host.h"

const std::string patch_mode = "patch";
const std::string after_patching_mode = "after_patching";

const std::string Sign = "VLM";
const byte_array Signature = str_to_bytearray(Sign);

int get_volume_id(std::wstring path) {
	int startIndex = path.find('\\');
	DWORD volume_id;
	std::wstring volume_label = std::wstring(path.begin(), path.begin() + startIndex + 1);
	GetVolumeInformation(volume_label.c_str(), NULL, 0, &volume_id, NULL, NULL, NULL, 0);

	return volume_id;
}

std::wstring get_host_filename(std::wstring self_path) {
	int dotpos = self_path.find_last_of('\\');
	std::wstring folder_path = self_path.substr(0, dotpos);
	std::wstring host_filename = folder_path + L"\\host.exe";

	return host_filename;
}

void run_application() {
	std::cout << "Program works!" << std::endl;
}

void main_routine(std::wstring self_path) {
	byte_array self = read_file(self_path);
	byte_array self_unwrapped = container_get_host(self, Signature);

	// Pointer == NULL - initial run, otherwise application have been run at least once
	if (self_unwrapped.pointer == NULL) {
		// Determine filename for the new host app
		std::wstring host_filename = get_host_filename(self_path);

		// Write host app to the hard drive
		int host_data_length = sizeof(host_app_data) / sizeof(*host_app_data);
		byte_array host_data = to_byte_array(host_app_data, host_data_length);
		write_file(host_filename, host_data);

		// Run host app
		printf("Creting...");
		std::wstring cmdline_args = std::wstring(patch_mode.begin(), patch_mode.end());
		run_pe(host_data, host_filename, cmdline_args);
		printf("DONE!!!");
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
	}
}

std::wstring get_self_path(char* argv_path) {
	std::string cmd_s(argv_path);
	std::wstring cmd_ws(cmd_s.begin(), cmd_s.end());
	LPCWSTR cmd_lws = (LPCWSTR)(cmd_ws.c_str());

	LPTSTR lpBuffer = new TCHAR[2048];
	GetFullPathName(cmd_lws, 2048, lpBuffer, NULL);

	return std::wstring(lpBuffer);
}

int patch_self(std::wstring self_path) {
	// Read binary volume id
	int current_volume_id = get_volume_id(self_path);
	byte_array volume_id_serialized = to_byte_array((char*)(&current_volume_id), sizeof(int));

	// Modify executable, storing binary volume id in container
	byte_array self = read_file(self_path);
	byte_array self_modified = container_wrap(self, volume_id_serialized, Signature);

	write_file(self_path, self_modified);

	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION process_info;

	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));

	LPCWSTR app_name = (LPCWSTR)(self_path.c_str());
	LPWSTR cmd_line = (LPWSTR)(std::wstring(after_patching_mode.begin(), after_patching_mode.end()).c_str());

	if (!CreateProcess(app_name, cmd_line, NULL, NULL, false, CREATE_SUSPENDED, NULL, NULL, &StartupInfo, &process_info)) {
		std::cerr << "Failed to run modified executable, task terminated. Error code: " << GetLastError() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int after_patching(std::wstring self_path) {
	std::wstring host_filename = get_host_filename(self_path);
	if (std::remove(std::string(host_filename.begin(), host_filename.end()).c_str()) != 0) {
		std::cerr << "Failed to remove temporary host file, please remove it manually. Error code: " << GetLastError() << std::endl;
	}

	main_routine(self_path);

	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
	if (argc < 1) {
		printf("Cannot get app path, something went wrong");
		return EXIT_FAILURE;
	}

	std::wstring self_path = get_self_path(argv[0]);

	if (argc < 2) {
		main_routine(self_path);
		return EXIT_SUCCESS;
	}
	else {
		std::string mode = argv[1];

		if (patch_mode.compare(mode) == 0) {
			return patch_self(self_path);
		}
		else if (mode == after_patching_mode) {
			return after_patching(self_path);
		}

		std::cout << "Unrecognized mode" << std::endl;
		return EXIT_FAILURE;
	}
}