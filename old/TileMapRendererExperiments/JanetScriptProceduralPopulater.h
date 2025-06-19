#pragma once
#include "ITiledWorldPopulater.h"

extern "C" {
#include "janet.h"
}

class JanetVmService;

class JanetScriptProceduralPopulater 
:public ITiledWorldPopulater{
public:
	JanetScriptProceduralPopulater(JanetVmService* vm);
	// Inherited via ITiledWorldPopulater
	virtual void PopulateLayer(u32 layerNumber, u16* layer, u32 layerWidthTiles, u32 layerHeightTiles, std::string& outName) override;
	virtual u32 GetRequiredNumLayers() override;
	// Inherited via ITiledWorldPopulater
	virtual u32 GetWidthTiles() override;

	virtual u32 GetHeightTiles() override;
private:
	JanetVmService* m_vm;
private:

};