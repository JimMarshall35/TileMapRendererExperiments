#include <iostream>
#include "AtlasLoader.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "TileSetInfo.h"
#include "TileMapConfigOptions.h"
#include "IRenderer.h"
//#define DEBUG_OUTPUT_ATLAS_PNG
//#define DEBUG_OUTPUT_TILES

#pragma region defines

#define NUM_CHANNELS 4

#pragma endregion

#pragma region static helpers

static bool validateTileset(const TileSetInfo& info, int img_w, int img_h, int n) {
	using namespace std;
	if (info.TileHeight > img_h) {
		cerr << "The specified tile height is greater than the image height... GOOD ONE!!!" << endl;
		return false;
	}
	if (info.TileWidth > img_w) {
		cerr << "The specified tile width is greater than the image width." << endl;
		return false;
	}
	if (info.PixelColStart >= img_w) {
		cerr << "the x coordinate you've asked to start reading tiles from is greater than the width of the image." << endl;
		return false;
	}
	if (info.PixelRowStart >= img_h) {
		cerr << "the y coordinate you've asked to start reading tiles from is greater than the height of the image." << endl;
		return false;
	}

	// the width of the "block" of pixels
	u32 tileBlockWidthPixels = ((info.TileWidth + info.RightMargin) * info.ColsOfTiles) - info.RightMargin;
	if ((info.PixelColStart + tileBlockWidthPixels) > img_w) {
		cerr << "given the width of tiles, number of columns of tiles and right margin you specified that number of tiles can't fit on the image." << endl;
		return false;
	}

	// the height of the "block" of pixels
	u32 tileBlockHeightPixels = ((info.TileHeight + info.BottomMargin) * info.RowsOfTiles) - info.BottomMargin;
	if ((info.PixelRowStart + tileBlockHeightPixels) > img_h) {
		cerr << "given the height of tiles, number of rows of tiles and bottom margin you specified that number of tiles can't fit on the image." << endl;
		return false;
	}

	return true;
}

#pragma endregion

AtlasLoader::AtlasLoader(const TileMapConfigOptions& configOptions, const std::shared_ptr<IRenderer>& renderer)
	:_atlasWidth(configOptions.AtlasWidthPx),
	_currentState(AtlasLoaderStates::Unloaded),
	_renderer(renderer)
{
}



static void PrintCurrentStateError(AtlasLoaderStates currentState) {

	static const std::string stateNamesForErrors[(u32)AtlasLoaderStates::NumStates] =
	{
		"Unknown",
		"Unloaded",
		"Loading",
		"FinishedLoading"
	};

	std::cerr << "StartLoadingTilesets Error. State was: " << stateNamesForErrors[(u32)currentState] << std::endl;
}

void AtlasLoader::StartLoadingTilesets()
{
	
	switch (_currentState)
	{
	case AtlasLoaderStates::Unloaded:
	case AtlasLoaderStates::FinishedLoading:
		_currentState = AtlasLoaderStates::Loading;
		break;
	case AtlasLoaderStates::Loading:
	case AtlasLoaderStates::Unknown:
		PrintCurrentStateError(_currentState);
		break;
	default:
		break;
	}
}

void AtlasLoader::StopLoadingTilesets()
{
	switch (_currentState)
	{
	case AtlasLoaderStates::Loading:
		MakeAtlas();
		_currentState = AtlasLoaderStates::FinishedLoading;
		break;
	case AtlasLoaderStates::Unloaded:
	case AtlasLoaderStates::Unknown:
	case AtlasLoaderStates::FinishedLoading:
		PrintCurrentStateError(_currentState);
		break;
	default:
		break;
	}
}

bool AtlasLoader::TryLoadTileset(const TileSetInfo& info)
{
	// Early return if the tilemap is in the wrong state
	if (_currentState != AtlasLoaderStates::Loading) {
		std::cerr << "TryLoadTileset The tile map state machine was not in the Loading state" << std::endl;
		return false;
	}
	int img_w, img_h;
	int n;
	const u8* data = stbi_load(info.Path.c_str(), &img_w, &img_h, &n, NUM_CHANNELS);
	// Early return if the file can't be opened
	if (data == NULL) {
		std::cerr << "can't open file " << info.Path << std::endl;
		return false;
	}

	// Early return if the tileset is invalid - the TileSetInfo
	// defines a tileset that doesn't fit on the image
	if (!validateTileset(info, img_w, img_h, n)) {
		return false;
	}

	/* split tileset into tiles and store in _individualTiles vector */
	const u8* readPtr;
	for (u32 tileRow = 0; tileRow < info.RowsOfTiles; tileRow++) {
		readPtr = &data[(info.PixelRowStart * (img_w * NUM_CHANNELS)) + (info.PixelColStart * NUM_CHANNELS)];
		readPtr += (NUM_CHANNELS * img_w * (info.TileHeight + info.BottomMargin)) * tileRow;
		for (u32 tileCol = 0; tileCol < info.ColsOfTiles; tileCol++) {
			u32 tileSizeBytes = ((info.TileWidth * NUM_CHANNELS) * info.TileHeight);
			auto Bytes = std::make_unique<u8[]>(tileSizeBytes);//new u8[tileSizeBytes];
			u32 PixelsCols = info.TileWidth;
			u32 PixelsRows = info.TileHeight;
			u8* writePtr = Bytes.get();
			for (u32 i = 0; i < info.TileHeight; i++) {
				memcpy(writePtr, readPtr, info.TileWidth * NUM_CHANNELS);
				// move readPtr to next line of tile
				readPtr += NUM_CHANNELS * img_w;
				writePtr += NUM_CHANNELS * info.TileWidth;
			}
			readPtr -= info.TileHeight * (NUM_CHANNELS * img_w);
			readPtr += (info.TileWidth * NUM_CHANNELS) + (info.RightMargin * NUM_CHANNELS);
			
			_individualTiles.push_back(Tile(Bytes,PixelsRows,PixelsCols, info.Path));
		}
	}
	auto type = TypeOfTile(info.TileWidth, info.TileHeight, info.Path);

	_numberOfEachTileType[type] += (info.RowsOfTiles * info.ColsOfTiles);
	return true;
}

void AtlasLoader::DebugDumpTiles(std::string path)
{
	int num = 0;
	for (const auto& tile : _individualTiles) {
		u32 pixelsCols = tile.GetPixelsCols();
		stbi_write_png(
			(path + std::to_string(num++) + ".png").c_str(),
			pixelsCols,
			tile.GetPixelsRows(),
			NUM_CHANNELS,
			tile.GetBytesPtr(),
			pixelsCols *NUM_CHANNELS);
	}
}



TileSetFrameHandle AtlasLoader::GetTilesetFrameHandle()
{
	return 0;
}

void AtlasLoader::ReleaseTilesetFrameHandle(TileSetFrameHandle handle)
{
}

u32 AtlasLoader::GetRequiredAtlasSizeInBytes(u32& rows)
{
	auto atlasHeightPx = 0;
	for (const auto& kv : _numberOfEachTileType) {
		const auto& tileType = kv.first;
		const auto numberOfThisTileType = kv.second;

		auto numberOfTilesPerAtlasRow = _atlasWidth / tileType.GetWidth();
		auto atlasHeightIncrement = numberOfThisTileType / numberOfTilesPerAtlasRow;
		if (numberOfThisTileType % numberOfTilesPerAtlasRow > 0) atlasHeightIncrement++;
		atlasHeightPx += atlasHeightIncrement * tileType.GetHeight();
		//auto 
	}
	rows = atlasHeightPx;
	return atlasHeightPx * _atlasWidth * NUM_CHANNELS;
}

static void CalculateUvFromWritePointerStart(
	const u8* writePtr,
	const u8* atlasPtr,
	u32 atlasRows,
	u32 atlasCols,
	float& u,
	float& v){

	u32 bytesOffsetFromStart = writePtr - atlasPtr;
	u32 xBytes = bytesOffsetFromStart % (atlasCols * NUM_CHANNELS);
	u32 yRows = bytesOffsetFromStart / (atlasCols * NUM_CHANNELS);
	

	u = (float)xBytes / (float)(atlasCols * NUM_CHANNELS);
	v = ((float)yRows / (float)(atlasRows));
}

void CopyASingleTile(const Tile& tile, u8*& writePtr, u32 atlasWidthBytes) {
	for (u32 i = 0; i < tile.GetPixelsRows(); i++) {
		memcpy(
			writePtr,
			tile.GetBytesPtr() + (i * (tile.GetPixelsCols() * NUM_CHANNELS)),
			tile.GetPixelsCols() * NUM_CHANNELS);
		writePtr += atlasWidthBytes;
	}
}

void AtlasLoader::MakeAtlas()
{
	u32 rows;
	u32 bytesRequired = GetRequiredAtlasSizeInBytes(rows);
	const u32 atlasWidthBytes = _atlasWidth * NUM_CHANNELS;
	auto bytes = std::make_unique<u8[]>(bytesRequired);
	u8* writePtr = bytes.get();
	for (const auto& kv : _numberOfEachTileType) {
		const auto& tileType = kv.first;
		const auto numberOfThisTileType = kv.second;
		auto numberOfTilesPerAtlasRow = _atlasWidth / tileType.GetWidth();

		auto numTilesThisRow = 0;
		// for each type of tile copy all tiles of that type to the atlas
		for (Tile& tile : _individualTiles) {
			if (tile.GetTypeOfTile() != tileType) {
				continue;
			}
			float u, v;
			CalculateUvFromWritePointerStart(writePtr, bytes.get(), rows, _atlasWidth,
				u, v);
			tile.UTopLeft = u;
			tile.VTopLeft = v;
			CalculateUvFromWritePointerStart(
				writePtr + (atlasWidthBytes* tileType.GetHeight()) + (tileType.GetWidth() * NUM_CHANNELS) - 1,
				bytes.get(), rows, _atlasWidth,
				u, v);
			tile.UBottomRight = u;
			tile.VBottomRight = v;

			CopyASingleTile(tile, writePtr, atlasWidthBytes);
			// set the write ptr to write the next tile in the row
			writePtr -= (tile.GetPixelsRows() * atlasWidthBytes);
			writePtr += tile.GetPixelsCols() * NUM_CHANNELS;

			// if the number of rows has overflowed the allowed number per row
			// the set the write ptr to the next row down, at the left hand edge
			numTilesThisRow++;
			if (numTilesThisRow >= numberOfTilesPerAtlasRow) {
				writePtr -= numberOfTilesPerAtlasRow * tileType.GetWidth() * NUM_CHANNELS;
				writePtr += atlasWidthBytes * tile.GetPixelsRows();
				numTilesThisRow = 0;
			}
		}
		// if the drawing of a type of tile finished part way through a row
		// we need to set the write pointer back to the start and down a row 
		// to start drawing the next type of tile
		if (numTilesThisRow != 0) {
			writePtr -= numTilesThisRow * tileType.GetWidth() * NUM_CHANNELS;
			writePtr += atlasWidthBytes * tileType.GetHeight();
		}
	}

#ifdef DEBUG_OUTPUT_ATLAS_PNG
	stbi_write_png(
		"atlas.png",
		_atlasWidth,
		rows,
		NUM_CHANNELS,
		bytes.get(),
		_atlasWidth * NUM_CHANNELS);
#endif // DEBUG_OUTPUT_ATLAS_PNG
#ifdef DEBUG_OUTPUT_TILES
	DebugDumpTiles("tiledump\\");
#endif

	_renderer->GPULoadAtlasTexture(
		bytes.get(),
		_atlasWidth,
		rows,
		&_atlasTextureHandle);
	_renderer->GPULoadTileData(_individualTiles);
}
