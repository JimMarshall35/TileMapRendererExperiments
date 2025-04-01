#include "Engine.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <functional>
#include <thread>
#include "TileSetInfo.h"
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
#include "WaveFunctionCollapseTool.h"
#include "MetaspriteTool.h"
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
#include "flecs.h"
#include "PhysicsWorld.h"
#include "StaticCollisionDrawerTool.h"
#include "EntityInspectorTool.h"
#include "SaveLoadLevelTool.h"
#include <memory>

//#ifdef _WIN32
//#define __WIN32_WINNT 0x0a00
//#endif
//#define ASIO_STANDALONE
//#include <asio.hpp>
//#include <asio/ts/buffer.hpp>
//#include <asio/ts/internet.hpp>






float deltaTime = 0;
float lastFrame = 0;
EditorCamera* cam;
static bool wantMouseInput = false;
static bool wantKeyboardInput = false;

GLenum minimumLogSeverityIncluding = GL_DEBUG_SEVERITY_LOW;
NewRenderer* Engine::s_renderer;

u32 Engine::s_screenW;
u32 Engine::s_screenH;
NewRendererInitialisationInfo Engine::s_rendererInitialisationInfo;
TileMapConfigOptions Engine::s_tileMapConfigOptions;
ForthEngineSystemInitArgs Engine::s_forthSystemInitArgs;

static void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    if (severity >= minimumLogSeverityIncluding) {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
    }
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

Engine::Engine(const TileMapConfigOptions& config, const NewRendererInitialisationInfo& rendererInit, const ForthEngineSystemInitArgs& forthInitArgs)
{
    s_rendererInitialisationInfo = rendererInit;
    s_screenW = s_rendererInitialisationInfo.windowWidth;
    s_screenH = s_rendererInitialisationInfo.windowHeight;
    s_tileMapConfigOptions = config;
    s_forthSystemInitArgs = forthInitArgs;
}

std::unique_ptr<NewRenderer> Engine::RendererFactory()
{
    return std::make_unique<NewRenderer>(s_rendererInitialisationInfo);
}

std::unique_ptr<ECS> Engine::ECSFactory()
{
    return std::make_unique<ECS>();
}

std::unique_ptr<MetaAtlas> Engine::MetaAtlasFactory(ECS* ecs)
{
    return std::make_unique<MetaAtlas>(100, 100, ecs);
}

std::unique_ptr<AtlasLoader> Engine::AtlasLoaderFactory()
{
    return std::make_unique<AtlasLoader>(s_tileMapConfigOptions);
}

std::unique_ptr<DynamicQuadTreeContainer<flecs::entity>> Engine::EntityQuadTreeFactory()
{
    return std::make_unique<DynamicQuadTreeContainer<flecs::entity>>(Rect{ {-0.5,-0.5}, {2000,2000} });
}

std::unique_ptr<ForthEngineSystem> Engine::ForthEngineSystemFactory(ForthEngineSystemInitArgs* args, ECS* ecs)
{
    return std::make_unique<ForthEngineSystem>(args, ecs);
}

std::unique_ptr<ForthEngineSystemInitArgs> Engine::ForthEngineSystemInitArgsFactory()
{
    auto ptr = std::make_unique<ForthEngineSystemInitArgs>();
    *ptr = s_forthSystemInitArgs;
    return ptr;
}

int Engine::StartupBoilerPlate(ImGuiIO*& io, GLFWwindow*& window) {
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
    window = glfwCreateWindow(s_screenW, s_screenH, "Arkanoids 3D", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
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

std::unique_ptr<WindowsFilesystem> Engine::IFileSystemFactory()
{
    return std::make_unique<WindowsFilesystem>();
}

void Engine::RunUserStartupFunctions(MetaAtlas* metaAtlas, ForthEngineSystem* forthEngineSystem)
{
    std::vector<std::thread> threads;
    for (std::pair<StartupFunction, StartupFunctionType>& pair : m_startupFunctions) {
        switch (pair.second) {
        case StartupFunctionType::Normal:
            pair.first(metaAtlas,forthEngineSystem);
        case StartupFunctionType::ParallelWithPrevious:
            threads.push_back(std::thread([metaAtlas, forthEngineSystem, pair]() { pair.first(metaAtlas, forthEngineSystem); }));
        }
    }
    for (std::thread& thread : threads)
    {
        thread.join();
    }
    threads.clear();
}

injector Engine::BuildDependencyInjector()
{
    di_config di;
    di.add(&RendererFactory);
    di.add(&ECSFactory);
    di.add(&MetaAtlasFactory);
    di.add(&AtlasLoaderFactory);
    di.add(&EntityQuadTreeFactory);
    di.add(&IFileSystemFactory);
    di.add(&PhysicsWorld::PhysicsWorldFactory);
    di.add(&ForthEngineSystemInitArgsFactory);
    di.add(&ForthEngineSystemFactory);

    return di.build_injector();

}

int Engine::Run()
{
    if (StartupBoilerPlate(m_io, m_window) < 0) {
        return -1;
    }

    injector injector = BuildDependencyInjector();
    ///////////////////////////////////////////////////////////////////// initialise core systems
    s_renderer = injector.get_instance<NewRenderer>();
    auto tiledWorld = TiledWorld(&m_jsonTiledWorldPopulator);

    auto metaspritesQuadTree = injector.get_instance<DynamicQuadTreeContainer<flecs::entity>>();//DynamicQuadTreeContainer<flecs::entity>({ {-0.5,-0.5}, {2000,2000} });
    IFilesystem* fs = (IFilesystem*)injector.get_instance<WindowsFilesystem>();
    /*auto config = TileMapConfigOptions();
    config.AtlasWidthPx = 800;*/

    m_metaAtlas = injector.get_instance<MetaAtlas>();
    m_atlasLoader = injector.get_instance<AtlasLoader>();
    m_ecs = injector.get_instance<ECS>();
    
    PhysicsWorld* physicsWorld = injector.get_instance<PhysicsWorld>();

    ForthEngineSystem* forthSystem = injector.get_instance<ForthEngineSystem>();


    ///////////////////////////////////////////////////////////////////// load image files
    RunUserStartupFunctions(m_metaAtlas, forthSystem);

    ///////////////////////////////////////////////////////////////////// initialise cameras
    EditorCameraInitializationSettings settings;
    settings.moveSpeed = 60;
    settings.screenHeight = s_screenH;
    settings.screenWidth = s_screenW;
    auto camera = EditorCamera(settings);
    camera.FocusPosition = { 0,0 };
    camera.Zoom = 3.0f;
    auto cameraStart = glm::vec2(camera.GetTLBR()[1], camera.GetTLBR()[0]);
    camera.FocusPosition += -cameraStart;
    auto gameCam = GameCamera(s_screenW, s_screenH, 2000, 2000);
    Camera2D* cameras[] = { &gameCam, &camera };
    auto camManager = CameraManager(cameras, 2);


    ///////////////////////////////////////////////////////////////////// initialise editor tools
    LutDrawTool lut(fs, &tiledWorld, m_atlasLoader);
    SingleTileDrawTool singleTileDraw(&tiledWorld);
    TileInfoTool tileInfo(m_atlasLoader);
    WaveFunctionCollapseTool waveFunctionCollapse;
    MetaspriteTool metaspriteTool(m_metaAtlas, m_atlasLoader, metaspritesQuadTree, m_ecs, s_renderer);
    StaticCollisionDrawerTool staticCollisionTool(s_renderer, metaspritesQuadTree, physicsWorld, m_ecs);
    EntityInspectorTool entityInspectorTool(m_ecs, metaspritesQuadTree, s_renderer);
    SaveLoadLevelTool saveLoadLevelTool(m_ecs);
    const u32 NUM_TOOLS = 8;
    EditorToolBase* toolBasePtrs[NUM_TOOLS] = {
        &lut,
        &singleTileDraw,
        &tileInfo,
        &waveFunctionCollapse,
        &metaspriteTool,
        &staticCollisionTool,
        &entityInspectorTool,
        &saveLoadLevelTool
    };


    ///////////////////////////////////////////////////////////////////// game framework layers
    EditorUi editorUi(&tiledWorld, m_atlasLoader, m_ecs, fs,
        (EditorToolBase**)toolBasePtrs, NUM_TOOLS, m_window, &camManager);
    Game game(&camManager, s_renderer, metaspritesQuadTree, m_metaAtlas, m_atlasLoader, &tiledWorld, s_screenW, s_screenH);

    GameFramework::PushLayers("Game",
        GameLayerType::Draw |
        GameLayerType::Input |
        GameLayerType::Update);

    GameFramework::PushLayers("Editor",
        GameLayerType::Draw |
        GameLayerType::Input |
        GameLayerType::Update);



    
    

    bool show_demo_window = true;

    auto inputStage = m_ecs->GetWorld()->system("input")
        .kind(flecs::PreUpdate)
        .iter([this](flecs::iter& it) {
        //std::cout << "delta_time: " << it.delta_time() << std::endl;
        // input
        // -----
        glfwPollEvents();
        processInput(m_window);
            });

    auto updateStage = m_ecs->GetWorld()->system("update")
        .kind(flecs::OnUpdate)
        .iter([physicsWorld](flecs::iter& it) {
        GameFramework::Update(it.delta_time());
        physicsWorld->Step();
            });

    auto renderStage = m_ecs->GetWorld()->system("render")
        .kind(flecs::OnUpdate)
        .iter([&camManager, this](flecs::iter& it) {
        //// render
        //// ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        GameFramework::Draw(*camManager.GetActiveCamera());

        glfwSwapBuffers(m_window);
            });

    forthSystem->RunStartupScriptIfLoaded();

    auto rval = m_ecs->GetWorld()->app().enable_rest().run(); // https://flecs.dev/explorer
    // Cleanup

    glfwTerminate();
    return rval;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

Directions Engine::processDirectionKeys(GLFWwindow* window) {
    static std::vector<bool> wasdKeys(9);
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

void Engine::processInput(GLFWwindow* window)
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
    else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
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
void Engine::FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    s_screenW = width;
    s_screenH = height;
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    s_renderer->SetWindowWidthAndHeight(width, height);
    GameFramework::SendFrameworkMessage(WindowSizedChangedArgs{ (u32)width, (u32)height });
}



// glfw: whenever the mouse moves, this dcallback is called
// -------------------------------------------------------

void Engine::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    static float lastX = s_screenW / 2.0f;
    static float lastY = s_screenH / 2.0f;
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
void Engine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    GameInput gi;
    gi.type = GameInputType::MouseScrollWheel;
    gi.data.mouseScrollWheel.offset = yoffset;
    GameFramework::RecieveInput(gi);
}

void Engine::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
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

void Engine::LoadJsonTileMap(const std::string& jsonFolder, const std::string& jsonFile)
{
    m_jsonTiledWorldPopulator.LoadJSON(jsonFolder, jsonFile);
}

void Engine::StartLoadingTilesets()
{
    m_atlasLoader->StartLoadingTilesets();
}


void Engine::StopLoadingTilesets(AtlasLoaderAtlasType atlasTypeToMake)
{
    m_atlasLoader->StopLoadingTilesets(atlasTypeToMake);
}


bool Engine::TryLoadTileset(const TileSetInfo& tilesetInfo)
{
    return m_atlasLoader->TryLoadTileset(tilesetInfo);
}

bool Engine::LoadMetaSpritesFromFile(const std::string& folder, const std::string& file)
{
    std::string errorString;
    if (!m_metaAtlas->LoadFromJSON("data\\json", "city.json", errorString))
    {
        std::cerr << errorString << "\n";
        return false;
    }
    return true;
}

bool Engine::TryLoadTilesetFromJSONInfo(const std::string& folder, const std::string& file)
{
    return m_atlasLoader->TryLoadTilesetFromJSONInfo(folder, file);
}

void Engine::RegisterStartupFunction(StartupFunctionType type, StartupFunction function)
{
    m_startupFunctions.push_back(std::pair<StartupFunction, StartupFunctionType>(function, type));
}