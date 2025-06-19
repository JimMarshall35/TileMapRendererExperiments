#pragma once
#include <string>
#include <memory>
#include "BasicTypedefs.h"
#include "TypeOfTile.h"

class Tile {
public:
	Tile(std::unique_ptr<u8[]>& bytes, u32 PixelsRows, u32 PixelsCols, const std::string& filePath, const std::string& tilesetName);
	inline const u8* GetBytesPtr() const {
		return _bytes.get();
	}
	inline u32 GetPixelsRows() const {
		return _tile.GetHeight();
	}
	inline u32 GetPixelsCols() const {
		return _tile.GetWidth();
	}
	inline const TypeOfTile& GetTypeOfTile() const {
		return _tile;
	}
	float UTopLeft;
	float VTopLeft;
	float UBottomRight;
	float VBottomRight;
private:
	std::unique_ptr<u8[]> _bytes;
	TypeOfTile _tile;
	std::string _tilesetName;
};

