#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "type_define.h"
#include "cam_mgr.h"
#include "tile_mgr.h"
#include "config.h"


typedef struct
{
    // Handle to a program object
    GLuint programObject;
    GLuint programColor;

    // Attribute locations
    GLint  positionLoc;
    GLint  texCoordLoc;
    GLint  uViewMatLoc;
    GLint  uProjMatLoc;

    // Attribute locations
    GLint  pos4colorLoc;
    GLint  uColorLoc;
    GLint  uViewMat4colorLoc;
    GLint  uProjMat4colorLoc;

    // Sampler locations
    GLint baseMapLoc;

    sCtrlParam ctrl;

    camManager* camMgr;
    tileManager* tileMgr;

} UserData;


//static float rangeList[] = {2,5,10,25,50,100,250,400,600,750,1000};
#define KM2DEG (0.0030864197530864196)
static float rangeList[] = { 2 * KM2DEG,5 * KM2DEG,10 * KM2DEG,25 * KM2DEG,50 * KM2DEG,100 * KM2DEG,250 * KM2DEG,400 * KM2DEG,600 * KM2DEG,750 * KM2DEG,1000 * KM2DEG,2000 * KM2DEG,5000 * KM2DEG,7500 * KM2DEG,12000 * KM2DEG };
static short idx = 10;


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);


///
// Initialize the shader and program object
//
int Init(UserData* userData)
{
    std::cout << "MAP in =========>>" << std::endl;
    GLbyte vShaderStr[] =
        "attribute vec4 a_position;                   \n"
        "attribute vec2 a_texCoord;                   \n"
        "varying vec2 v_texCoord;                     \n"
        "uniform mat4 viewMat;                        \n"
        "uniform mat4 projMat;                        \n"
        "void main()                                  \n"
        "{                                            \n"
        "   gl_Position = projMat*viewMat*a_position; \n"
        "   v_texCoord = a_texCoord;                  \n"
        "}                                            \n";

    GLbyte fShaderStr[] =
        "precision mediump float;                            \n"
        "varying vec2 v_texCoord;                            \n"
        "uniform sampler2D texture;                          \n"
        "void main()                                         \n"
        "{                                                   \n"
        "  gl_FragColor = texture2D( texture, v_texCoord );  \n"
        "}                                                   \n";

    GLbyte vShaderStr_color[] =
        "attribute vec4 a_position;                   \n"
        "uniform mat4 viewMat;                        \n"
        "uniform mat4 projMat;                        \n"
        "void main()                                  \n"
        "{                                            \n"
        "   gl_Position = projMat*viewMat*a_position; \n"
        "}                                            \n";

    GLbyte fShaderStr_color[] =
        "precision mediump float;                            \n"
        "uniform vec4 ucolor;                                \n"
        "void main()                                         \n"
        "{                                                   \n"
        "  gl_FragColor = ucolor;                            \n"
        "}                                                   \n";

    // Load the shaders and get a linked program object
    userData->programObject = esLoadProgram((const char*)vShaderStr, (const char*)fShaderStr);
    userData->programColor = esLoadProgram((const char*)vShaderStr_color, (const char*)fShaderStr_color);

    // Get the attribute locations
    userData->positionLoc = glGetAttribLocation(userData->programObject, "a_position");
    userData->texCoordLoc = glGetAttribLocation(userData->programObject, "a_texCoord");
    userData->uViewMatLoc = glGetUniformLocation(userData->programObject, "viewMat");
    userData->uProjMatLoc = glGetUniformLocation(userData->programObject, "projMat");

    // Get the sampler location
    userData->baseMapLoc = glGetUniformLocation(userData->programObject, "texture");

    // for color shader
    userData->pos4colorLoc = glGetAttribLocation(userData->programColor, "a_position");
    userData->uColorLoc = glGetUniformLocation(userData->programColor, "ucolor");
    userData->uViewMat4colorLoc = glGetUniformLocation(userData->programColor, "viewMat");
    userData->uViewMat4colorLoc = glGetUniformLocation(userData->programColor, "projMat");

    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);

    userData->ctrl.lon = 120.0;
    userData->ctrl.lat = 30.0;
    userData->ctrl.range = rangeList[idx];

    int viewport[4] = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    userData->camMgr = new camManager(userData->ctrl, viewport);
    userData->tileMgr = new tileManager(userData->camMgr);

    return TRUE;
}


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "amap", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
