#include "TypeOfTile.h"
#include <functional>

TypeOfTile::TypeOfTile(u32 width, u32 height, std::string filePath)
    :_width(width), _height(height), _filePath(filePath)
{
}

bool TypeOfTile::operator!=(const TypeOfTile& rhs) const
{
    return !(*this == rhs);
}

bool TypeOfTile::operator==(const TypeOfTile& rhs) const
{
    return 
        rhs._height == _height &&
        rhs._width == _width &&
        rhs._filePath == _filePath;
}

bool TypeOfTile::operator<(const TypeOfTile& o) const
{
    // Is this right? i have no idea but seems to work
    return (_width + _height) + std::hash<std::string>()(_filePath) < (o._width + o._height) + std::hash<std::string>()(o._filePath);
}