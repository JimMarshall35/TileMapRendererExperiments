#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "DynArray.h"
#include "GameFramework.h"
#include "TestGameLayer.h"
#include <string.h>

#define SCR_WIDTH 800
#define SCR_HEIGHT 1200
#define TARGET_FPS 40

InputContext gInputContext;
DrawContext gDrawContext;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Dr_OnScreenDimsChange(width, height);
    In_FramebufferResize(&gInputContext, width, height);
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
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
    }
}

void Update(double deltaT)
{
    glfwPollEvents();
    In_Poll();
    InputGameFramework(&gInputContext);
    UpdateGameFramework((float)deltaT);
}

int main(int argc, char** argv)
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
        /*std::cout << "Failed to create GLFW window" << std::endl;*/
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

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
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
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



    double accumulator = 0;
    double lastUpdate = 0;
    double slice = 1.0 / TARGET_FPS;


    gDrawContext = Dr_InitDrawContext();
    In_InitInputContext();
    InitGameFramework();


    struct GameFrameworkLayer testLayer;
    memset(&testLayer, 0, sizeof(struct GameFrameworkLayer));
    TestLayer_Get(&testLayer);
    PushGameFrameworkLayer(&testLayer);


    while (!glfwWindowShouldClose(window))
    {
        double delta = glfwGetTime();// () - lastUpdate
        lastUpdate += delta;
        accumulator += delta;
        while (accumulator > slice)
        {
            Update(delta);
            accumulator -= slice;
        }
        
        DrawGameFramework(&gDrawContext);
        glfwSwapBuffers(window);
    }

    DestroyGameFramework();

    glfwTerminate();
}
