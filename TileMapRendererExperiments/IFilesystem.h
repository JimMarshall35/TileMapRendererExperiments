#pragma once
#include <vector>
#include <string>
#include "BasicTypedefs.h"
class IFilesystem {
public:
	virtual std::vector<std::string> ListFilesInDirectory(std::string dir) const = 0;
	virtual u32 GetMaxFilepathLength() const = 0;
};