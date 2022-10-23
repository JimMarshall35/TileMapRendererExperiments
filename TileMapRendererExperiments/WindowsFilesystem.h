#pragma once
#include "IFilesystem.h"
#include <vector>
class WindowsFilesystem 
: public IFilesystem{
public:
	virtual std::vector<std::string> ListFilesInDirectory(std::string dir) const override;
};