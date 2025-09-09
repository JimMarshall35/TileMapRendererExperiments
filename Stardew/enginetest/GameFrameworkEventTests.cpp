#include <gtest/gtest.h>
#include <filesystem>
#include "GameFramework.h"
#include "XMLUIGameLayer.h"
#include "Scripting.h"
#include "DrawContext.h"

HUIVertexBuffer MockNewUIVertexBufferFn(int size)
{
    return NULL_HANDLE;
}

void MockSetCurrentAtlasFn(hTexture atlas)
{
    
}


TEST(Events, FilePresent)
{
	ASSERT_TRUE(std::filesystem::exists("data/GameFrameworkEventTestUILayer.lua"));
	ASSERT_TRUE(std::filesystem::exists("data/GameFrameworkEventTestUILayer.xml"));
}

TEST(Events, GameToUI)
{
    DrawContext dc;
    memset(&dc, 0, sizeof(DrawContext));
    dc.NewUIVertexBuffer = &MockNewUIVertexBufferFn;
    dc.SetCurrentAtlas = &MockSetCurrentAtlasFn;
    Sc_InitScripting();
    UI_Init();
    GF_InitGameFramework();
    struct GameFrameworkLayer testLayer;
    memset(&testLayer, 0, sizeof(struct GameFrameworkLayer));
    struct XMLUIGameLayerOptions options;
    options.bLoadImmediately = false;
    options.xmlPath = "./data/GameFrameworkEventTestUILayer.xml";
    options.pDc = NULL;
    testLayer.flags |= (EnableOnPush | EnableOnPop);
    XMLUIGameLayer_Get(&testLayer, &options);
    GF_PushGameFrameworkLayer(&testLayer);
    GF_EndFrame(&dc, NULL);
    Sc_DeInitScripting();
}