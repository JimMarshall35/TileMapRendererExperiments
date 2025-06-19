add_executable(StardewEngine
Atlas.c
Geometry.c
SliderWidget.c
BackgroundBoxWidget.c
glad.c
StackPanelWidget.c
CanvasWidget.c
ImageFileRegstry.c
StaticWidget.c
cJSON.c
InputContext.c
TextButtonWidget.c
main.c
TextWidget.c
CtorDtor.c
ObjectPool.c
Widget.c
DrawContext.c
RadioButtonWidget.c
WidgetVertexOutputHelpers.c
DynArray.c
RadioGroupWidget.c
xml.c
FileHelpers.c
RootWidget.c
XMLHelpers.c
FloatingPointLib.c
Scripting.c
XMLUIGameLayer.c
GameFramework.c
SharedPtr.c)

target_include_directories(StardewEngine  PRIVATE ../include)
target_include_directories(StardewEngine  PRIVATE ../lib/freetype-2.13.3/include)
target_include_directories(StardewEngine  PRIVATE ../lib/glad/include)

