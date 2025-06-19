#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Tile.h"

struct TileSetInfo;
struct TileMapConfigOptions;

typedef u32 TileSetFrameHandle;

class IRenderer;

enum class AtlasLoaderStates {
	Unknown,
	Unloaded,
	Loading,
	FinishedLoading,
	// leave this one at the end
	NumStates
};

enum class AtlasLoaderAtlasType : u32 {
	SingleTextureAtlas = 1,
	ArrayTexture = 2
};
inline AtlasLoaderAtlasType operator|(AtlasLoaderAtlasType a, AtlasLoaderAtlasType b)
{
	return static_cast<AtlasLoaderAtlasType>(static_cast<u32>(a) | static_cast<u32>(b));
}
inline u32 operator&(AtlasLoaderAtlasType a, AtlasLoaderAtlasType b)
{
	return static_cast<u32>(a) & static_cast<u32>(b);
}


class AtlasLoader
{
public:
	AtlasLoader(const TileMapConfigOptions& configOptions);
	void StartLoadingTilesets();
	void StopLoadingTilesets(AtlasLoaderAtlasType atlasTypeToMake);
	bool TryLoadTileset(const TileSetInfo& info);
	bool TryLoadTilesetFromJSONInfo(const std::string& folder, const std::string& file);
	void DebugDumpTiles(std::string path);
	// get a handle that can be used to free tiles that were loaded afterwards
	TileSetFrameHandle GetTilesetFrameHandle();
	// delete all tiles that were loaded after the handle was called
	void ReleaseTilesetFrameHandle(TileSetFrameHandle handle);
	inline const std::vector<Tile>& GetIndividualTiles() {
		return _individualTiles;
	}
	inline u32 GetAtlasTextureHandle() {
		return _atlasTextureHandle;
	}
	const inline std::map<TypeOfTile, u32>& GetrrayTexureMap() {
		return _arrayTextureForType;
	}; 
	u32 GetArrayTextureByName(const std::string& name);
	inline u32 GetNumTiles() const { return _individualTiles.size(); }
private:
	u32 GetRequiredAtlasSizeInBytes(u32& rows);
	void MakeAtlas();
	void MakeAtlasAsArrayTexture();
	
private:
	std::vector<Tile> _individualTiles;
	std::map<TypeOfTile, u32> _numberOfEachTileType;
	std::map<TypeOfTile, u32> _arrayTextureForType;

	u32 _atlasWidth;
	u32 _atlasTextureHandle = 0;
	AtlasLoaderStates _currentState;
};

