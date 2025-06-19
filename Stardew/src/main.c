#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "DynArray.h"
#include "GameFramework.h"
#include "XMLUIGameLayer.h"
#include "ImageFileRegstry.h"
#include "Atlas.h"
#include "Widget.h"
#include "Scripting.h"
#include <string.h>
#include "PlatformDefs.h"

#define SCR_WIDTH 640
#define SCR_HEIGHT 480
#define TARGET_FPS 60

InputContext gInputContext;
DrawContext gDrawContext;

int Mn_GetScreenWidth()
{
    return gDrawContext.screenWidth;
}

int Mn_GetScreenHeight()
{
    return gDrawContext.screenHeight;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    Dr_OnScreenDimsChange(&gDrawContext, width, height);
    In_FramebufferResize(&gInputContext, width, height);
    GF_OnWindowDimsChanged(width, height);
}

void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    In_RecieveMouseMove(&gInputContext, xposIn, yposIn);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    In_RecieveScroll(&gInputContext, xoffset, yoffset);
}

void MouseBtnCallback(GLFWwindow* window, int button, int action, int mods)
{
    In_RecieveMouseButton(&gInputContext, button, action, mods);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    In_RecieveKeyboardKey(&gInputContext, key, scancode, action, mods);
}

void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        In_SetControllerPresent(jid);
    }
    else if (event == GLFW_DISCONNECTED)
    {
        In_SetControllerPresent(-1);
    }
}


static void GLAPIENTRY MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    //if (severity >= minimumLogSeverityIncluding) 
    {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
    }
}

#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

void sleep_ms(int milliseconds){ // cross-platform sleep function
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    if (milliseconds >= 1000)
      sleep(milliseconds / 1000);
    usleep((milliseconds % 1000) * 1000);
#endif
}

int main(int argc, char** argv)
{
    printf("hello world\n");
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    printf("glfwInit\n");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Arkanoids 3D", NULL, NULL);
    if (window == NULL)
    {
        /*std::cout << "Failed to create GLFW window" << std::endl;*/
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

/*

    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    return 0;

*/
    
    glfwJoystickPresent(GLFW_JOYSTICK_1);

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetMouseButtonCallback(window, MouseBtnCallback);
    glfwSetKeyCallback(window, key_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
#if GAME_GL_API_TYPE == GAME_GL_API_TYPE_CORE
    printf("loading Opengl procs\n");
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }
#elif GAME_GL_API_TYPE == GAME_GL_API_TYPE_ES
    printf("loading Opengl ES procs\n");
    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }
#endif


    // configure global opengl state
    // -----------------------------
    printf("configuring global opengl state\n");
    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);

#if GAME_GL_API_TYPE == GAME_GL_API_TYPE_CORE
    glDebugMessageCallback(MessageCallback, 0);
#endif
    printf("done\n");

    double accumulator = 0;
    double lastUpdate = 0;
    double slice = 1.0 / TARGET_FPS;

    printf("initialising draw context\n");
    gDrawContext = Dr_InitDrawContext();
    printf("done\n");
    printf("initial screen dims change\n");
    Dr_OnScreenDimsChange(&gDrawContext, SCR_WIDTH, SCR_HEIGHT);
    printf("done\n");
    printf("initialising input context\n");
    gInputContext = In_InitInputContext();
    printf("done\n");
    printf("Initialising game framework\n");
    GF_InitGameFramework();
    printf("done\n");
    printf("initialising image registry\n");
    IR_InitImageRegistry();
    printf("done\n");
    printf("initialising atlas\n");
    At_Init();
    printf("done\n");
    printf("initialising UI\n");
    UI_Init();
    printf("done\n");
    printf("initialising scripting\n");
    Sc_InitScripting();
    printf("done\n");

    struct GameFrameworkLayer testLayer;
    memset(&testLayer, 0, sizeof(struct GameFrameworkLayer));
    struct XMLUIGameLayerOptions options;
    options.bLoadImmediately = true;
    options.xmlPath = "./Assets/test.xml";
    options.pDc = &gDrawContext;
    printf("making xml ui layer\n");
    XMLUIGameLayer_Get(&testLayer, &options);
    printf("done\n");
    printf("pushing framework layer\n");
    GF_PushGameFrameworkLayer(&testLayer);
    printf("done\n");
    
    while (!glfwWindowShouldClose(window))
    {
        double delta = glfwGetTime() - lastUpdate;
        lastUpdate += delta;
        accumulator += delta;
        while (accumulator > slice)
        {
            glfwPollEvents();
            In_Poll();
            GF_InputGameFramework(&gInputContext);
            GF_UpdateGameFramework((float)delta);
            accumulator -= slice;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        GF_DrawGameFramework(&gDrawContext);
        glfwSwapBuffers(window);
        GF_EndFrame(&gDrawContext, &gInputContext);
    }

    Sc_DeInitScripting();
    IR_DestroyImageRegistry();
    GF_DestroyGameFramework();

    glfwTerminate();
}
