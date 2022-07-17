#pragma once
#include <memory>
class IRenderer;
class TileMap
{
public:
	TileMap(const std::shared_ptr<IRenderer>& renderer);
private:
	std::shared_ptr<IRenderer> _renderer;
};

