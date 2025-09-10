#include <gtest/gtest.h>
#include <filesystem>
#include "GameFramework.h"
#include "XMLUIGameLayer.h"
#include "Scripting.h"
#include "DrawContext.h"
#include "GameFrameworkEvent.h"
#include "DataNode.h"

struct GameFrameworkEventListener* gOnUIPushListener = NULL;

HUIVertexBuffer MockNewUIVertexBufferFn(int size)
{
    return NULL_HANDLE;
}

void MockSetCurrentAtlasFn(hTexture atlas)
{
    
}

void OnGameUIPush(void* pUserData, void* pEventData)
{
    struct DataNode* pNode = (struct DataNode*)pEventData;
    enum DNPropValType type = pNode->fnGetPropType(pNode, "prop");
    ASSERT_EQ(type, DN_String);
    ASSERT_TRUE(pNode->fnStrCmp(pNode, "prop", "val"));
    
    char* c = (char*)malloc(pNode->fnGetStrlen(pNode, "prop") + 1);
    pNode->fnGetStrcpy(pNode, "prop", c);

    ASSERT_EQ(strcmp(c, "val"), 0);

    free(c);

    type = pNode->fnGetPropType(pNode, "intProp");
    ASSERT_EQ(type, DN_Int);
    int intv = pNode->fnGetInt(pNode, "intProp");
    ASSERT_EQ(intv, 42);

    type = pNode->fnGetPropType(pNode, "floatProp");
    ASSERT_EQ(type, DN_Float);
    float fV = pNode->fnGetFloat(pNode, "floatProp");
    ASSERT_FLOAT_EQ(fV, 32.5);
    
    type = pNode->fnGetPropType(pNode, "boolProp");
    ASSERT_EQ(type, DN_Bool);
    bool bV = pNode->fnGetBool(pNode, "boolProp");
    ASSERT_EQ(bV, false);
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


    gOnUIPushListener = Ev_SubscribeEvent("OnGameHUDPush", &OnGameUIPush, NULL);
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