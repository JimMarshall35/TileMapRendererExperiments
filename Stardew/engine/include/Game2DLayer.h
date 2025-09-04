#ifndef H_GAME2DLAYER
#define XMLUI_GAME_LAYER_H

struct GameLayer2DData
{
	int i;
};

struct Game2DLayerOptions
{
	const char* dataFilePath;
};

void Game2DLayer_Get(struct GameFrameworkLayer* pLayer, struct Game2DLayerOptions* pOptions);

#endif // !H_GAME2DLAYER
