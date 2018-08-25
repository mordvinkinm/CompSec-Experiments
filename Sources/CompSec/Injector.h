#ifndef INJECTOR
#define INJECTOR

#include <string>
#include <Windows.h>

#include "ByteArray.h"

typedef LONG (WINAPI * NtUnmapViewOfSection)(HANDLE ProcessHandle, PVOID BaseAddress);

/****************************************************************
 *
 * Runs an application from in-memory byte array
 *
 * @param executable binary data of executable we need to run
 * @param host_process path to a process that will be host application
 * @param optional_args optional args to run host application
 *
 * @returns bool false if run failed and true in case of success
 *
 ***************************************************************/
bool run_pe(byte_array executable, std::wstring host_process, std::wstring optional_args);

#endif INJECTOR