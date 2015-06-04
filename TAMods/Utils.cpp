#include "Utils.h"

namespace Utils
{
	ATrPlayerController *tr_pc = NULL;
	UTrGameViewportClient *tr_gvc = NULL;
}

// Converts UE3's FString to std::string
std::string Utils::f2std(FString &fstr)
{
	wchar_t *wch = fstr.Data;
	std::wstring wstr(wch);
	return (std::string(wstr.begin(), wstr.end()));
}

// Removes whitespace and lowercases the string
std::string Utils::cleanString(const std::string &str)
{
	std::string out = str;
	out.erase(std::remove(out.begin(), out.end(), ' '), out.end());
	out.erase(std::remove(out.begin(), out.end(), '\t'), out.end());
	std::transform(out.begin(), out.end(), out.begin(), ::tolower);
	return (out);
}

// Removes whitespace from both ends of a string
std::string Utils::trim(const std::string &str)
{
	size_t begin = str.find_first_not_of(" \t");
	size_t end = str.find_last_not_of(" \t");

	if (begin == std::string::npos || end == std::string::npos)
		return ("");
	return (str.substr(begin, end - begin + 1));
};

// Regex search in a map of type <string, int> where the string are regexes
// location and print_on_fail are only used for error messages
int Utils::searchMapId(const std::map<std::string, int> map, const std::string &str, const std::string &location, bool print_on_fail)
{
	std::string clean = Utils::cleanString(str);
	for (auto const &it : map)
	{
		if (std::regex_match(clean, std::regex(it.first)))
			return (it.second);
	}
	if (!print_on_fail)
		return (0);
	if (!location.size())
		console("WARNING: searched item '%s' could not be identified", str.c_str());
	else
		console("WARNING: searched item '%s' could not be identified as %s", str.c_str(), location.c_str());
	return (0);
}

// Notify the user (like friend online/offline)
// TODO: use char instead of wchar_t for the title
void Utils::notify(wchar_t *title, const char *format, ...)
{
	if (!tr_pc)
		return;
	va_list args;
	va_start(args, format);
	char buff[256];
	vsprintf(buff, format, args);
	va_end(args);
	std::string str(buff);
	std::wstring wstr = std::wstring(str.begin(), str.end());
	wchar_t* wch = (wchar_t *)wstr.c_str();
	tr_pc->ShowNotification(wch, title);
}

// Print a message in the user's console
// TODO: specify error level (message, log, warning, error, etc)
// TODO: customizable color
void Utils::console(const char *format, ...)
{
	if (!tr_gvc)
		return;
	va_list args;
	va_start(args, format);
	char buff[256];
	vsprintf(buff, format, args);
	va_end(args);
	std::string str(buff);
	std::wstring wstr = std::wstring(str.begin(), str.end());
	wchar_t* wch = (wchar_t *)wstr.c_str();
	FColor col;
	col.A = 255;
	col.R = 0;
	col.G = 255;
	col.B = 0;
	tr_gvc->ChatConsole->OutputTextLine(wch, col);
	Utils::notify(L"Ensis' mod", "Error: see console for details");
}
