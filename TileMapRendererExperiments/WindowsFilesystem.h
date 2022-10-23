#pragma once
#include "IFilesystem.h"
#include "BasicTypedefs.h"
#include <vector>
class WindowsFilesystem 
: public IFilesystem{
public:
	virtual std::vector<std::string> ListFilesInDirectory(std::string dir) const override;
	virtual u32 GetMaxFilepathLength() const override;
};