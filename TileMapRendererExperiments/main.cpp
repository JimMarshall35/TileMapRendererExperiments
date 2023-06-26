

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <functional>
#include "TileMapConfigOptions.h"
#include "TileSetInfo.h"
#include "AtlasLoader.h"
//#include "OpenGlRenderer.h"
#include "ProceduralCppTiledWorldPopulater.h"

#include "EditorCamera.h"
#include "TileChunk.h"
#include "EditorUi.h"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "NewRenderer.h"
#include "TiledWorld.h"
#include "Undo.h"
#include "WindowsFilesystem.h"
#include "LutDrawTool.h"
#include "SingleTileDrawTool.h"
#include "TileInfoTool.h"
#include "JanetVmService.h"
#include "JanetScriptProceduralPopulater.h"
#include "WaveFunctionCollapseTool.h"
#include "MetaspriteTool.h"
#include "MetaAtlas.h"
#include "QuadTree.h"
#include "MetaSpriteComponent.h"
#include "Position.h"
#include "TestMoveComponent.h"
#include "Scale.h"
#include "GameFramework.h"
#include "GameInput.h"
#include "GameCamera.h"
#include "CameraManager.h"
#include "Game.h"
#include "JSONPopulator.h"
#include "ECS.h"
#include "flecs.h"


#define SCR_WIDTH 800
#define SCR_HEIGHT 1200

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

float deltaTime = 0;
float lastFrame = 0;
EditorCamera* cam;
IRenderer* gRenderer;
TiledWorld* gTiledWorld;
EditorUi* gEditorUi;
LutDrawTool* gLutDrawTool;
static bool wantMouseInput = false;
static bool wantKeyboardInput = false;

u32 WindowW = SCR_WIDTH;
u32 WindowH = SCR_HEIGHT;

static void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}

inline u32 GetRandomIntBetween(u32 min, u32 max) {
    return (u32)rand() % (max - min + 1) + min;
}

static std::vector<u32> GetRandomTileMap(int rows, int cols, int minTileValue, int maxTileValue) {
    auto tileMap = std::vector<u32>(rows * cols);
    for (int i = 0; i < rows * cols; i++) {
        tileMap[i] = GetRandomIntBetween(minTileValue, maxTileValue);
    }
    return tileMap;
}


int StartupBoilerPlate(ImGuiIO*& io, GLFWwindow*& window) {
    srand(time(0));
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Arkanoids 3D", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);




    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    return 0;
}


int main()
{
    ImGuiIO* io;
    GLFWwindow* window;
    if (StartupBoilerPlate(io,window) < 0) {
        return -1;
    }
    
    ///////////////////////////////////////////////////////////////////// initialise core systems
    auto rendererInit = NewRendererInitialisationInfo();
    rendererInit.windowHeight = SCR_HEIGHT;
    rendererInit.windowWidth = SCR_WIDTH;
    auto newRenderer = NewRenderer(rendererInit);
    gRenderer = &newRenderer;
    auto vm = JanetVmService();
    auto janetPopulator = JanetScriptProceduralPopulater(&vm);
    auto populater = ProceduralCppTiledWorldPopulater();
    auto jsonPopulator = JSONTiledWorldPopulator("data\\json", "lvl1_test5.json");
    auto tiledWorld = TiledWorld(&jsonPopulator);
    gTiledWorld = &tiledWorld;
    auto metaspritesQuadTree = DynamicQuadTreeContainer<flecs::entity>({ {-0.5,-0.5}, {2000,2000} });
    auto config = TileMapConfigOptions();
    config.AtlasWidthPx = 800;
    auto atlasLoader = AtlasLoader(config);
    auto metaAtlas = MetaAtlas(100, 100);
    
    ECS ecs;
    WindowsFilesystem fs;

    ///////////////////////////////////////////////////////////////////// load image files
    atlasLoader.StartLoadingTilesets();
    TileSetInfo info;


    atlasLoader.TryLoadTilesetFromJSONInfo("data\\json", "city.json");


    info.BottomMargin = 0;
    info.RightMargin = 0;
    info.PixelColStart = 0;
    info.PixelRowStart = 0;
    info.TileHeight = 24;
    info.TileWidth = 24;
    info.RowsOfTiles = 6;
    info.ColsOfTiles = 4;
    info.Path = "sprites\\24by24ModernRPGGuy_edit.png";
    info.Name = "RPG guy";
    atlasLoader.TryLoadTileset(info);

    atlasLoader.StopLoadingTilesets(AtlasLoaderAtlasType::ArrayTexture | AtlasLoaderAtlasType::SingleTextureAtlas);

    /*std::string errorString;
    if (!metaAtlas.LoadFromJSON("data\\json", "city.json", errorString))
    {
        std::cerr << errorString << "\n";
    }*/
    ///////////////////////////////////////////////////////////////////// initialise cameras
    EditorCameraInitializationSettings settings;
    settings.moveSpeed = 60;
    settings.screenHeight = SCR_HEIGHT;
    settings.screenWidth = SCR_WIDTH;
    auto camera = EditorCamera(settings);
    camera.FocusPosition = { 0,0 };
    camera.Zoom = 3.0f;
    auto cameraStart = glm::vec2(camera.GetTLBR()[1], camera.GetTLBR()[0]);
    camera.FocusPosition += -cameraStart;
    auto gameCam = GameCamera(SCR_WIDTH, SCR_WIDTH, 2000, 2000);
    Camera2D* cameras[] = {&gameCam, &camera};
    auto camManager = CameraManager(cameras, 2);

    
    ///////////////////////////////////////////////////////////////////// initialise editor tools
    LutDrawTool lut((IFilesystem*)&fs, &tiledWorld, &atlasLoader);
    SingleTileDrawTool singleTileDraw(&tiledWorld);
    TileInfoTool tileInfo(&atlasLoader);
    WaveFunctionCollapseTool waveFunctionCollapse;
    MetaspriteTool metaspriteTool(&metaAtlas, &atlasLoader, &metaspritesQuadTree, &ecs, &newRenderer);

    const u32 NUM_TOOLS = 5;
    EditorToolBase* toolBasePtrs[NUM_TOOLS] = { &lut, &singleTileDraw, &tileInfo, &waveFunctionCollapse, &metaspriteTool };


    ///////////////////////////////////////////////////////////////////// game framework layers
    EditorUi editorUi(&tiledWorld, &atlasLoader, &ecs, (IFileSystem*)&fs,
        (EditorToolBase**)toolBasePtrs, NUM_TOOLS, window, &camManager);
    Game game(&camManager,&newRenderer,&metaspritesQuadTree,&metaAtlas, &atlasLoader, &tiledWorld, WindowW, WindowH);

    GameFramework::PushLayers("Game",
        GameLayerType::Draw |
        GameLayerType::Input |
        GameLayerType::Update);

    GameFramework::PushLayers("Editor",
        GameLayerType::Draw |
        GameLayerType::Input |
        GameLayerType::Update);

    gEditorUi = &editorUi;

    //ecs.system<>()
    //    .iter([](flecs::iter& it) {
    //    //std::cout << "delta_time: " << it.delta_time() << std::endl;
    //        });

    /////////////////////////////////////////////////////////////////////// initialise ecs world
    //ecs.system<Position, TestMoveComponent, MetaSpriteComponent, Scale, Rect>()
    //    .each([&metaspritesQuadTree](Position& p, TestMoveComponent& t, MetaSpriteComponent& m, Scale& s, Rect& r) {
    //    t.t += deltaTime / t.cycleTime;
    //    p.val = glm::mix(t.startPos, t.endPos, t.t);
    //    r.pos = p.val;
    //    auto handle = m.metaSprite->item.handle;
    //    auto& pos = m.metaSprite->item.pos;
    //    pos = r.pos;
    //    metaspritesQuadTree.remove(m.metaSprite);

    //    metaspritesQuadTree.insert(MetaSprite{ handle,r.pos }, r);
    //    m.metaSprite = std::prev(metaspritesQuadTree.end());
    //        });


    bool show_demo_window = true;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        GameFramework::Update(deltaTime);

        //// render
        //// ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        GameFramework::Draw(*camManager.GetActiveCamera());
        

        glfwSwapBuffers(window);
        glfwPollEvents();

        ecs.Progress();
    }

    // Cleanup

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
std::vector<bool> wasdKeys(9);
Directions processDirectionKeys(GLFWwindow* window) {
    Directions dir = Directions::NONE;
    if (wantKeyboardInput) {
        return dir;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
        wasdKeys[(u32)Directions::UP] = false;
    }
    else if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) || wasdKeys[(u32)Directions::UP]) {
        wasdKeys[(u32)Directions::UP] = true;
        dir |= Directions::UP;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
        wasdKeys[(u32)Directions::DOWN] = false;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || !wasdKeys[(u32)Directions::DOWN]) {
        wasdKeys[(u32)Directions::DOWN] = true;
        dir |= Directions::DOWN;

    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
        wasdKeys[(u32)Directions::LEFT] = false;

    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || wasdKeys[(u32)Directions::LEFT]) {
        wasdKeys[(u32)Directions::LEFT] = true;
        dir |= Directions::LEFT;

    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
        wasdKeys[(u32)Directions::RIGHT] = false;

    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || wasdKeys[(u32)Directions::RIGHT]) {
        wasdKeys[(u32)Directions::RIGHT] = true;
        dir |= Directions::RIGHT;

    }

    return dir;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    auto dir = processDirectionKeys(window);
    GameInput directionInput;
    directionInput.type = GameInputType::DirectionInput;
    directionInput.data.direction.directions = dir;
    GameFramework::RecieveInput(directionInput);

    static bool controlPressed = false;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {
        controlPressed = false;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ) {
        controlPressed = true;
    }
    static bool lastZPressed = false;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) {
        lastZPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !lastZPressed) {
        GameInput ctrlComboInput;
        ctrlComboInput.type = GameInputType::CtrlHeldKeyPress;
        ctrlComboInput.data.ctrlKeyCombo.keyPressed = 'z';
        GameFramework::RecieveInput(ctrlComboInput);
        //Undo(*gTiledWorld, *gLutDrawTool);
        lastZPressed = true;
    }
    static bool lastYPressed = false;
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE) {
        lastYPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !lastYPressed) {
        GameInput ctrlComboInput;
        ctrlComboInput.type = GameInputType::CtrlHeldKeyPress;
        ctrlComboInput.data.ctrlKeyCombo.keyPressed = 'y';
        GameFramework::RecieveInput(ctrlComboInput);
        //Redo(*gTiledWorld, *gLutDrawTool);
        lastYPressed = true;
    }

    static bool last1Pressed = false;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
        last1Pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !last1Pressed) {
        auto l = GameFramework::PeekDrawableLayer();
        if (l->GetDrawableLayerName() == "Editor") {
            GameFramework::PopLayers(
                GameLayerType::Draw |
                GameLayerType::Input |
                GameLayerType::Update);
        }
        else if (l->GetDrawableLayerName() == "Game") {
            GameFramework::PushLayers("Editor",
                GameLayerType::Draw |
                GameLayerType::Input |
                GameLayerType::Update);
        }
        last1Pressed = true;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WindowW = width;
    WindowH = height;
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    gRenderer->SetWindowWidthAndHeight(width, height);
    GameFramework::SendFrameworkMessage(WindowSizedChangedArgs{ (u32)width, (u32)height });
}

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool dragging = false;
float dragStartX, dragStartY;
float maxScaler = 0;

// glfw: whenever the mouse moves, this dcallback is called
// -------------------------------------------------------

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    lastX = xposIn;
    lastY = yposIn;
    GameInput gi;
    gi.type = GameInputType::MouseMove;
    gi.data.mouseMove.xposIn = lastX;
    gi.data.mouseMove.yposIn = lastY;

    GameFramework::RecieveInput(gi);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    GameInput gi;
    gi.type = GameInputType::MouseScrollWheel;
    gi.data.mouseScrollWheel.offset = yoffset;
    GameFramework::RecieveInput(gi);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    GameInput gi;
    gi.type = GameInputType::MouseButton;
    gi.data.mouseButton.action = MouseButtonAction::None;
    gi.data.mouseButton.button = MouseButton::None;
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        gi.data.mouseButton.button = MouseButton::Left;
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        gi.data.mouseButton.button = MouseButton::Right;
        break;
    default:
        break;
    }

    switch (action)
    {
    case GLFW_PRESS:
        gi.data.mouseButton.action = MouseButtonAction::Press;
        break;
    case GLFW_RELEASE:
        gi.data.mouseButton.action = MouseButtonAction::Release;
        break;
    default:
        break;
    }
    GameFramework::RecieveInput(gi);
}