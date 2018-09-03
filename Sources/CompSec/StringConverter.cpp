#include <locale>
#include <codecvt>

#include "StringConverter.h"

/****************************************************************
 *
 * Converts std::string to std::wstring
 *
 ***************************************************************/
std::wstring string_to_wstring(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;

	return converterX.from_bytes(str);
}

/****************************************************************
 *
 * Converts std::wstring to std::string
 *
 ***************************************************************/
std::string wstring_to_string(const std::wstring& wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}