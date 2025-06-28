#include "main.h"
#include "GameFramework.h"
#include <string.h>
#include "XMLUIGameLayer.h"
#include "DynArray.h"

void GameInit(InputContext* pIC, DrawContext* pDC)
{
    struct GameFrameworkLayer testLayer;
    memset(&testLayer, 0, sizeof(struct GameFrameworkLayer));
    struct XMLUIGameLayerOptions options;
    options.bLoadImmediately = true;
    options.xmlPath = "./Assets/test.xml";
    options.pDc = pDC;
    printf("making xml ui layer\n");
    XMLUIGameLayer_Get(&testLayer, &options);
    printf("done\n");
    printf("pushing framework layer\n");
    GF_PushGameFrameworkLayer(&testLayer);
    printf("done\n");
}

int main(int argc, char** argv)
{

    EngineStart(argc, argv, &GameInit);
}