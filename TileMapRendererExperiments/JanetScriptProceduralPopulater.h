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
	virtual void PopulateLayer(u32 layerNumber, u16* layer, u32 layerWidthTiles, u32 layerHeightTiles) override;
private:
	JanetVmService* m_vm;
private:


};