add_library( StardewEngine SHARED
core/Atlas.c
core/CtorDtor.c
core/DynArray.c
core/Geometry.c
core/SharedPtr.c
core/FloatingPointLib.c
core/ObjectPool.c
core/FileHelpers.c
core/ImageFileRegstry.c
gameframework/GameFramework.c
gameframework/layers/UI/XMLUIGameLayer.c
gameframework/layers/UI/WidgetVertexOutputHelpers.c
gameframework/layers/UI/widgets/SliderWidget.c
gameframework/layers/UI/widgets/BackgroundBoxWidget.c
gameframework/layers/UI/widgets/StackPanelWidget.c
gameframework/layers/UI/widgets/CanvasWidget.c
gameframework/layers/UI/widgets/StaticWidget.c
gameframework/layers/UI/widgets/TextButtonWidget.c
gameframework/layers/UI/widgets/TextWidget.c
gameframework/layers/UI/widgets/Widget.c
gameframework/layers/UI/widgets/RadioButtonWidget.c
gameframework/layers/UI/widgets/RadioGroupWidget.c
gameframework/layers/UI/widgets/RootWidget.c
gameframework/layers/UI/widgets/TextEntryWidget.c
vendor/glad.c
vendor/cJSON.c
rendering/DrawContext.c
scripting/Scripting.c
input/InputContext.c
main.c
)


