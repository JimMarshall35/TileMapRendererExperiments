#pragma once
#include <vector>
#include <string>
class IFilesystem {
	virtual std::vector<std::string> ListFilesInDirectory(std::string dir) const = 0;
};