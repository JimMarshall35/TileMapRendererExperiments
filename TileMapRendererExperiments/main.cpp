
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <functional>
#include "TileMapConfigOptions.h"
#include "TileSetInfo.h"
#include "AtlasLoader.h"
#include "OpenGlRenderer.h"
#include "TileChunk.h"
#include "EditorCamera.h"
#include "flecs.h"
#include "EditorUi.h"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "NewRenderer.h"
#include "TiledWorld.h"
#include "TileChunk.h"
#include "Undo.h"
#include "WindowsFilesystem.h"
#include "LutDrawTool.h"
#include "SingleTileDrawTool.h"

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

int main()
{
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Arkanoids 3D", NULL, NULL);
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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);



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
    auto config = TileMapConfigOptions();
    config.AtlasWidthPx = 800;
    //auto renderer = std::make_shared<OpenGlRenderer>(SCR_WIDTH,SCR_HEIGHT);
    auto rendererInit = NewRendererInitialisationInfo();
    rendererInit.chunkSizeX = 64;
    rendererInit.chunkSizeY = 64;
    rendererInit.tilemapSizeX = 1000;
    rendererInit.tilemapSizeY = 1000;
    rendererInit.windowHeight = SCR_HEIGHT;
    rendererInit.windowWidth = SCR_WIDTH;
    rendererInit.numLayers = 3;
    auto newRenderer = NewRenderer(rendererInit);
    gRenderer = &newRenderer;

    auto tiledWorld = TiledWorld(2000, 2000, 3);
    gTiledWorld = &tiledWorld;

    auto atlasLoader = AtlasLoader(config);

    atlasLoader.StartLoadingTilesets();
    TileSetInfo info;
    info.BottomMargin = 1;
    info.RightMargin = 1;
    info.PixelColStart = 0;
    info.PixelRowStart = 0;
    info.TileHeight = 16;
    info.TileWidth = 16;
    info.RowsOfTiles = 28;
    info.ColsOfTiles = 37;
    info.Path = "sprites\\roguelikeCity_magenta.png";//"C:\\Users\\james.marshall\\source\\repos\\Platformer\\Platformer\\batch1.png";
    atlasLoader.TryLoadTileset(info);

    //info.BottomMargin = 0;
    //info.RightMargin = 0;
    //info.PixelColStart = 0;
    //info.PixelRowStart = 0;
    //info.TileHeight = 24;
    //info.TileWidth = 24;
    //info.RowsOfTiles = 6;
    //info.ColsOfTiles = 4;
    //info.Path = "sprites\\24by24ModernRPGGuy_edit.png";//"C:\\Users\\james.marshall\\source\\repos\\Platformer\\Platformer\\batch1.png";
    //atlasLoader.TryLoadTileset(info);

    atlasLoader.StopLoadingTilesets(AtlasLoaderAtlasType::ArrayTexture | AtlasLoaderAtlasType::SingleTextureAtlas);


    EditorCameraInitializationSettings settings;
    settings.moveSpeed = 60;
    
    auto camera = EditorCamera(settings);
    camera.FocusPosition = { 0,0 };
    camera.Zoom = 3.0f;
    auto cameraStart = glm::vec2(camera.GetTLBR(WindowW, WindowH)[1], camera.GetTLBR(WindowW, WindowH)[0]);
    camera.FocusPosition += -cameraStart;
    cam = &camera;

    flecs::world ecs;
    ecs.system<>()
        .iter([](flecs::iter& it) {
        //std::cout << "delta_time: " << it.delta_time() << std::endl;
            });
    ecs.set_target_fps(30.0f);
    WindowsFilesystem fs;

    LutDrawTool lut((IFilesystem*)&fs, &tiledWorld, &atlasLoader);
    SingleTileDrawTool singleTileDraw(&tiledWorld);

    const u32 NUM_TOOLS = 2;
    EditorToolBase* toolBasePtrs[NUM_TOOLS] = {(EditorToolBase*)&lut, (EditorToolBase*)&singleTileDraw};

    EditorUi editorUi(&tiledWorld, &atlasLoader, &ecs, (IFileSystem*)&fs,
        (EditorToolBase**)toolBasePtrs, NUM_TOOLS);

    gEditorUi = &editorUi;


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
        wantMouseInput = io.WantCaptureMouse;
        wantKeyboardInput = io.WantCaptureKeyboard;


        // input
        // -----
        processInput(window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        editorUi.DoUiWindow();

        cam->OnUpdate(deltaTime);

        // render
        // ------
        ImGui::Render();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        TileChunk::DrawVisibleChunks(atlasLoader.TestGetFirstArrayTexture(), newRenderer, *cam, tiledWorld, rendererInit.chunkSizeX, rendererInit.chunkSizeY, WindowW, WindowH);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        

        glfwSwapBuffers(window);
        glfwPollEvents();

        ecs.progress();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
std::vector<bool> wasdKeys((u32)NUMDIRECTIONS);
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
        wasdKeys[(u32)UP] = false;
    }
    else if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) || wasdKeys[(u32)UP]) {
        cam->UpdatePosition(UP, deltaTime);
        wasdKeys[(u32)UP] = true;
    }
    

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
        wasdKeys[(u32)DOWN] = false;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || !wasdKeys[(u32)DOWN]) {
        cam->UpdatePosition(DOWN, deltaTime);
        wasdKeys[(u32)DOWN] = true;
    }
    

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
        wasdKeys[(u32)LEFT] = false;

    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || wasdKeys[(u32)LEFT]) {
        cam->UpdatePosition(LEFT, deltaTime);
        wasdKeys[(u32)LEFT] = true;

    }
    

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
        wasdKeys[(u32)RIGHT] = false;

    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || wasdKeys[(u32)RIGHT]) {
        cam->UpdatePosition(RIGHT, deltaTime);
        wasdKeys[(u32)RIGHT] = true;

    }
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
        Undo(*gTiledWorld, *gLutDrawTool);
        lastZPressed = true;
    }
    static bool lastYPressed = false;
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE) {
        lastYPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !lastYPressed) {
        Redo(*gTiledWorld, *gLutDrawTool);
        lastYPressed = true;
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
    cam->OnMouseMove(lastX, lastY, WindowW, WindowH, deltaTime);
    if (dragging) {
        gEditorUi->MouseButtonCallback(lastX, lastY, WindowW, WindowH, *cam);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (!wantMouseInput) {
        cam->Zoom *= yoffset > 0 ? (1.1 * yoffset) : 0.9 / abs(yoffset);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (!wantMouseInput) {
            //cam->StartDrag(lastX, lastY);
            gEditorUi->MouseButtonCallback(lastX, lastY, WindowW, WindowH, *cam);
            dragging = true;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        //cam->StopDrag();
        dragging = false;
    }
}