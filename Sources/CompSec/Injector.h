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
 *
 * @returns bool false if run failed and true in case of success
 *
 ***************************************************************/
bool run_pe(byte_array exe, std::wstring host_process);

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
bool run_pe(byte_array exe, std::wstring host_process, std::wstring optional_arguments);

/****************************************************************
 *
 * Runs an application from in-memory byte array
 *
 * @param executable binary data of executable we need to run
 * @param host_process path to a process that will be host application
 * @param optional_args optional args to run host application
 * @param startup_flags application run flags
 *
 * @returns bool false if run failed and true in case of success
 *
 ***************************************************************/
bool run_pe(byte_array executable, std::wstring host_process, std::wstring optional_args, DWORD startup_flags);

/*****************************************************************
 *
 * Reads memory of a particular process as a binary array.
 *
 * @param handle process' handle
 * @param ptr address in memory to read
 * @param size number of bytes to read
 *
 * @returns binary data of a process memory or byte_array(NULL, 0)
 *	in case of failure
 *
 *****************************************************************/
byte_array read_proc_memory(HANDLE handle, void* ptr, size_t size);

/****************************************************************
 *
 * Reads memory of a current process as a binary array.
 *
 * @param ptr address in memory to read
 * @param size number of bytes to read
 *
 * @returns binary data of a current process memory 
 *	or byte_array(NULL, 0) in case of failure
 *
 ****************************************************************/
byte_array read_current_proc_memory(void* ptr, size_t size);

/****************************************************************
 *
 * Re-writes memory of a particular process using binary array provider.
 *
 * @param handle process' handler
 * @param ptr address in memory to write
 * @param data structure, that contains pointer to data and size of data
 *
 * @returns none
 *
 ****************************************************************/
void write_proc_memory(HANDLE handle, void* ptr, byte_array data);

/****************************************************************
 *
 * Re-writes memory of a current process using binary array provider.
 *
 * @param ptr address in memory to write
 * @param data structure, that contains pointer to data and size of data
 *
 * @returns none
 *
 ****************************************************************/
void write_current_proc_memory(void* ptr, byte_array data);

#endif INJECTOR