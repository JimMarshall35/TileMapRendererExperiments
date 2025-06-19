#pragma once
#include <string>
#include "BasicTypedefs.h"

class TypeOfTile
{

public:
	TypeOfTile(u32 width, u32 height, std::string filePath, std::string name);
	bool operator!=(const TypeOfTile& rhs) const;
	bool operator==(const TypeOfTile& rhs) const;
	bool operator<(const TypeOfTile& o)  const;
	inline u32 GetWidth() const {
		return _width;
	};
	inline u32 GetHeight() const {
		return _height;
	}
	inline std::string GetFilePath() const {
		return _filePath;
	}
	inline std::string GetName() const {
		return _name;
	}
	u32 TextureArrayHandle;
private:
	u32 _width;
	u32 _height;
	std::string _filePath;
	std::string _name;
};

