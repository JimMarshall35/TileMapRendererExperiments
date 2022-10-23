#include "WindowsFilesystem.h"
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include <atlstr.h>
#include <iostream>

#pragma comment(lib, "User32.lib")

std::vector<std::string> list_directory(const std::string& directory)
{
	WIN32_FIND_DATAA findData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	std::string full_path = directory + "\\*";
	std::vector<std::string> dir_list;

	hFind = FindFirstFileA(full_path.c_str(), &findData);

	if (hFind == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Invalid handle value! Please check your path...");

	while (FindNextFileA(hFind, &findData) != 0)
	{
		dir_list.push_back(std::string(findData.cFileName));
	}

	FindClose(hFind);

	return dir_list;
}

std::vector<std::string> WindowsFilesystem::ListFilesInDirectory(std::string dir) const
{
	auto v = list_directory(dir);
	for (const auto& s : v) {
		std::cout << s << "\n";
	}
	return v;
}

u32 WindowsFilesystem::GetMaxFilepathLength() const
{
	return MAX_PATH;
}
