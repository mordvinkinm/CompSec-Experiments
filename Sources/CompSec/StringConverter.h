#ifndef STRING_CONVERTER_H
#define STRING_CONVERTER_H

#include <string>

/****************************************************************
 *
 * Converts std::string to std::wstring
 *
 ***************************************************************/
std::wstring string_to_wstring(const std::string& str);

/****************************************************************
 *
 * Converts std::wstring to std::string
 *
 ***************************************************************/
std::string wstring_to_string(const std::wstring& wstr);

#endif /* STRING_CONVERTER_H */
