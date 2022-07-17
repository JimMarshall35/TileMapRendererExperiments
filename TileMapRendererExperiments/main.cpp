
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <functional>
#include "TileMapConfigOptions.h"
#include "TileSetInfo.h"
#include "AtlasLoader.h"
#include "OpenGlRenderer.h"
#include "TileChunk.h"
#include "Camera2D.h"
#include "flecs.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 1200

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

float deltaTime = 0;
float lastFrame = 0;
Camera2D cam;

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



int main()
{
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    auto config = TileMapConfigOptions();
    config.AtlasWidthPx = 1280;
    auto renderer = std::make_shared<OpenGlRenderer>(SCR_WIDTH,SCR_HEIGHT);
    auto atlasLoader = AtlasLoader(config, renderer);

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

    info.BottomMargin = 0;
    info.RightMargin = 0;
    info.PixelColStart = 0;
    info.PixelRowStart = 0;
    info.TileHeight = 24;
    info.TileWidth = 24;
    info.RowsOfTiles = 6;
    info.ColsOfTiles = 4;
    info.Path = "sprites\\24by24ModernRPGGuy_edit.png";//"C:\\Users\\james.marshall\\source\\repos\\Platformer\\Platformer\\batch1.png";
    atlasLoader.TryLoadTileset(info);

    atlasLoader.StopLoadingTilesets();

    auto map = std::unique_ptr<u32[]>(
        new u32[25]{
            2,2,2,2,2,
            2,2,2,2,2,
            0,1,35,293,0,
            28,29,31,420,837,
            2,2,2,2,2,

        });

    auto chunk = TileChunk(map, 5, 5, renderer.get());

    cam = Camera2D();
    cam.FocusPosition = { 0,0 };
    cam.Zoom = 3.0f;


    //tilemap.DebugDumpTiles("");
    flecs::world ecs;
    ecs.set_target_fps(30);

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

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer->DrawTileMap(
            chunk.GetVerticesHandle(),
            25,
            { 5,5 },
            {0,0}, //{ -(SCR_WIDTH/2),-(SCR_HEIGHT / 2) },
            { 16,16 },
            0.0f,
            cam
        );

        glfwSwapBuffers(window);
        glfwPollEvents();

        ecs.progress();
    }

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}