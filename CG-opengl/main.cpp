#include <iostream>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "SOIL2/SOIL2.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"


// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void DoMovement( );

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera( glm::vec3( 0.0f, 0.0f, 3.0f ) );
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos( 1.2f, 1.0f, 2.0f );

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

// The MAIN function, from here we start the application and run the game loop
int main( )
{
    // Init GLFW
    glfwInit( );
    // Set all the required options for GLFW
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr );
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent( window );
    
    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    
    // Set the required callback functions
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseCallback );
    
    // GLFW Options
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    
    // OpenGL options
    glEnable( GL_DEPTH_TEST );
    
    
    // Build and compile our shader program
    Shader lightingShader( "res/shaders/lighting.vs", "res/shaders/lighting.frag" );
    Shader lampShader( "res/shaders/lamp.vs", "res/shaders/lamp.frag" );
    GLfloat cube_vertices[] ={
        // Positions            // Normals              // Texture Coords
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
        
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,      1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
        
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
        
        0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
        
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
        
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
    };
    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] =
    {
        
        // x, y, z,   tex0, tex1,
        // CHAO
        -100.0f, 0.0f, 100.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        100.0f, 0.0f, 100.0f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        100.0f, 0.0f, -100.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        100.0f, 0.0f, -100.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        -100.0f, 0.0f, -100.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -100.0f, 0.0f, 100.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        
        // WALLS
        0.0f,  0.0f,  -1.0f,    1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f,  0.0f, -30.0f,    1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f,  11.0f, -30.0f,   1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.0f,  11.0f, -30.0f,   1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.0f,  11.0f,  -1.0f,   1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f,  0.0f,  -1.0f,   1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        
        8.0f,  0.0f,  -1.0f,   -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
        8.0f,  0.0f, -30.0f,   -1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        8.0f,  11.0f, -30.0f,  -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        8.0f,  11.0f, -30.0f,  -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        8.0f,  11.0f,  -1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        8.0f,  0.0f,  -1.0f,  -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        // STAIRS
        
        //face frente
        0.0f, 0.0f, -1.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 0.0f, -1.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 0.3f, -1.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 0.3f, -1.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 0.3f, -1.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 0.0f, -1.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        
        // face cima
        0.0f, 0.3f, -1.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 0.3f, -1.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 0.3f, -2.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 0.3f, -2.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 0.3f, -2.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 0.3f, -1.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        
        0.0f, 0.3f, -2.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 0.3f, -2.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 0.6f, -2.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 0.6f, -2.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 0.6f, -2.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 0.3f, -2.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 0.6f, -2.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 0.6f, -2.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 0.6f, -3.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 0.6f, -3.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 0.6f, -3.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 0.6f, -2.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 0.6f, -3.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 0.6f, -3.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 0.9f, -3.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 0.9f, -3.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 0.9f, -3.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 0.6f, -3.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 0.9f, -3.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 0.9f, -3.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 0.9f, -4.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 0.9f, -4.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 0.9f, -4.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 0.9f, -3.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 0.9f, -4.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 0.9f, -4.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 1.2f, -4.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 1.2f, -4.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 1.2f, -4.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 0.9f, -4.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 1.2f, -4.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 1.2f, -4.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 1.2f, -5.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 1.2f, -5.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 1.2f, -5.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 1.2f, -4.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 1.2f, -5.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 1.2f, -5.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 1.5f, -5.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 1.5f, -5.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 1.5f, -5.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 1.2f, -5.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 1.5f, -5.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 1.5f, -5.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 1.5f, -6.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 1.5f, -6.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 1.5f, -6.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 1.5f, -5.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 1.5f, -6.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 1.5f, -6.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 1.8f, -6.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 1.8f, -6.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 1.8f, -6.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 1.5f, -6.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 1.8f, -6.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 1.8f, -6.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 1.8f, -7.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 1.8f, -7.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 1.8f, -7.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 1.8f, -6.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 1.8f, -7.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 1.8f, -7.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 2.1f, -7.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 2.1f, -7.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 2.1f, -7.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 1.8f, -7.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 2.1f, -7.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 2.1f, -7.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 2.1f, -8.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 2.1f, -8.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 2.1f, -8.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 2.1f, -7.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 2.1f, -8.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 2.1f, -8.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 2.4f, -8.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 2.4f, -8.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 2.4f, -8.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 2.1f, -8.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 2.4f, -8.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 2.4f, -8.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 2.4f, -9.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 2.4f, -9.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 2.4f, -9.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 2.4f, -8.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 2.4f, -9.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 2.4f, -9.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 2.7f, -9.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 2.7f, -9.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 2.7f, -9.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 2.4f, -9.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 2.7f, -9.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 2.7f, -9.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 2.7f, -10.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 2.7f, -10.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 2.7f, -10.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 2.7f, -9.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 2.7f, -10.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 2.7f, -10.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 3.0f, -10.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 3.0f, -10.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 3.0f, -10.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 2.7f, -10.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 3.0f, -10.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 3.0f, -10.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 3.0f, -11.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 3.0f, -11.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 3.0f, -11.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 3.0f, -10.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 3.0f, -11.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 3.0f, -11.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 3.3f, -11.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 3.3f, -11.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 3.3f, -11.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 3.0f, -11.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 3.3f, -11.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 3.3f, -11.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 3.3f, -12.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 3.3f, -12.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 3.3f, -12.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 3.3f, -11.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 3.3f, -12.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 3.3f, -12.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 3.6f, -12.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 3.6f, -12.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 3.6f, -12.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 3.3f, -12.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 3.6f, -12.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 3.6f, -12.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 3.6f, -13.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 3.6f, -13.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 3.6f, -13.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 3.6f, -12.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 3.6f, -13.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 3.6f, -13.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 3.9f, -13.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 3.9f, -13.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 3.9f, -13.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 3.6f, -13.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 3.9f, -13.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 3.9f, -13.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 3.9f, -14.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 3.9f, -14.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 3.9f, -14.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 3.9f, -13.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 3.9f, -14.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 3.9f, -14.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 4.2f, -14.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 4.2f, -14.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 4.2f, -14.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 3.9f, -14.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 4.2f, -14.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 4.2f, -14.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 4.2f, -15.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 4.2f, -15.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 4.2f, -15.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 4.2f, -14.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 4.2f, -15.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 4.2f, -15.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 4.5f, -15.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 4.5f, -15.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 4.5f, -15.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 4.2f, -15.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 4.5f, -15.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 4.5f, -15.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 4.5f, -16.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 4.5f, -16.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 4.5f, -16.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 4.5f, -15.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 4.5f, -16.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 4.5f, -16.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 4.8f, -16.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 4.8f, -16.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 4.8f, -16.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 4.5f, -16.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 4.8f, -16.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 4.8f, -16.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 4.8f, -17.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 4.8f, -17.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 4.8f, -17.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 4.8f, -16.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 4.8f, -17.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 4.8f, -17.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 5.1f, -17.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 5.1f, -17.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 5.1f, -17.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 4.8f, -17.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 5.1f, -17.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 5.1f, -17.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 5.1f, -18.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 5.1f, -18.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 5.1f, -18.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 5.1f, -17.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 5.1f, -18.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 5.1f, -18.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 5.4f, -18.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 5.4f, -18.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 5.4f, -18.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 5.1f, -18.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 5.4f, -18.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 5.4f, -18.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 5.4f, -19.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 5.4f, -19.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 5.4f, -19.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 5.4f, -18.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 5.4f, -19.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 5.4f, -19.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 5.7f, -19.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 5.7f, -19.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 5.7f, -19.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 5.4f, -19.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 5.7f, -19.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 5.7f, -19.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 5.7f, -20.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 5.7f, -20.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 5.7f, -20.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 5.7f, -19.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 5.7f, -20.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 5.7f, -20.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 6.0f, -20.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 6.0f, -20.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 6.0f, -20.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 5.7f, -20.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 6.0f, -20.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 6.0f, -20.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 6.0f, -21.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 6.0f, -21.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 6.0f, -21.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 6.0f, -20.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 6.0f, -21.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 6.0f, -21.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 6.3f, -21.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 6.3f, -21.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 6.3f, -21.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 6.0f, -21.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 6.3f, -21.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 6.3f, -21.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 6.3f, -22.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 6.3f, -22.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 6.3f, -22.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 6.3f, -21.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 6.3f, -22.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 6.3f, -22.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 6.6f, -22.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 6.6f, -22.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 6.6f, -22.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 6.3f, -22.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 6.6f, -22.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 6.6f, -22.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 6.6f, -23.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 6.6f, -23.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 6.6f, -23.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 6.6f, -22.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 6.6f, -23.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 6.6f, -23.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 6.9f, -23.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 6.9f, -23.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 6.9f, -23.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 6.6f, -23.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 6.9f, -23.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 6.9f, -23.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 6.9f, -24.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 6.9f, -24.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 6.9f, -24.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 6.9f, -23.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 6.9f, -24.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 6.9f, -24.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 7.2f, -24.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 7.2f, -24.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 7.2f, -24.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 6.9f, -24.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 7.2f, -24.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 7.2f, -24.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 7.2f, -25.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 7.2f, -25.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 7.2f, -25.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 7.2f, -24.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 7.2f, -25.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 7.2f, -25.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 7.5f, -25.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 7.5f, -25.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 7.5f, -25.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 7.2f, -25.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 7.5f, -25.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 7.5f, -25.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 7.5f, -26.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 7.5f, -26.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 7.5f, -26.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 7.5f, -25.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 7.5f, -26.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 7.5f, -26.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 7.8f, -26.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 7.8f, -26.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 7.8f, -26.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 7.5f, -26.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 7.8f, -26.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 7.8f, -26.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 7.8f, -27.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 7.8f, -27.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 7.8f, -27.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 7.8f, -26.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 7.8f, -27.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 7.8f, -27.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 8.1f, -27.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 8.1f, -27.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 8.1f, -27.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 7.8f, -27.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 8.1f, -27.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 8.1f, -27.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 8.1f, -28.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 8.1f, -28.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 8.1f, -28.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 8.1f, -27.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 8.1f, -28.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 8.1f, -28.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 8.4f, -28.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 8.4f, -28.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 8.4f, -28.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 8.1f, -28.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 8.4f, -28.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 8.4f, -28.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 8.4f, -29.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 8.4f, -29.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 8.4f, -29.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 8.4f, -28.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 8.4f, -29.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 8.4f, -29.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 8.7f, -29.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 8.7f, -29.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 8.7f, -29.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 8.4f, -29.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        0.0f, 8.7f, -29.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        8.0f, 8.7f, -29.0f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
        8.0f, 8.7f, -30.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        8.0f, 8.7f, -30.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
        0.0f, 8.7f, -30.0f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
        0.0f, 8.7f, -29.0f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
        0.0f, 8.7f, -30.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
        8.0f, 8.7f, -30.0f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
        8.0f, 9.0f, -30.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        8.0f, 9.0f, -30.0f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
        0.0f, 9.0f, -30.0f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 8.7f, -30.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
    };
    
    // Positions all containers
    glm::vec3 cubePositions[] = {
        glm::vec3(  -2.0f,   -2.5f,   0.0f    ),
    };
    
    // Positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(  -1.9f,  6.2f,  -26.25f      ),
        glm::vec3(  -1.9f,  5.2f,  -22.5f      ),
        glm::vec3(  -1.9f,  4.2f,  -18.75f      ),
        glm::vec3(  -1.9f,  3.2f,  -15.0f      ),
        glm::vec3(  -1.9f,  2.2f,  -11.25f      ),
        glm::vec3(  -1.9f,  1.2f,  -7.5f    ),
        glm::vec3(  -1.9f,  0.2f,  -3.75f      ),
        
        glm::vec3(  5.9f,  6.2f,  -26.25f      ),
        glm::vec3(  5.9f,  5.2f,  -22.5f      ),
        glm::vec3(  5.9f,  4.2f,  -18.75f      ),
        glm::vec3(  5.9f,  3.2f,  -15.0f      ),
        glm::vec3(  5.9f,  2.2f,  -11.25f      ),
        glm::vec3(  5.9f,  1.2f,  -7.5f    ),
        glm::vec3(  5.9f,  0.2f,  -3.75f      ),
        
    };
    
    // First, set the container's VAO (and VBO)
    GLuint VBO, boxVAO, LIGHT;
    glGenVertexArrays( 1, &boxVAO );
    glGenBuffers( 1, &VBO );
    glGenBuffers( 1, &LIGHT );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );
    
    glBindVertexArray( boxVAO );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), ( GLvoid * )0 );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), ( GLvoid * )( 3 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), ( GLvoid * )( 6 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray( 2 );
    glBindVertexArray( 0 );
    
    glBindBuffer( GL_ARRAY_BUFFER, LIGHT );
    glBufferData( GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW );
    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    GLuint lightVAO;
    glGenVertexArrays( 1, &lightVAO );
    glBindVertexArray( lightVAO );
    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
    glBindBuffer( GL_ARRAY_BUFFER, LIGHT);
    // Set the vertex attributes (only position data for the lamp))
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ), ( GLvoid * )0 ); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
    glEnableVertexAttribArray( 0 );
    glBindVertexArray( 0 );
    
    // Load textures
    GLuint diffuseMap, specularMap, emissionMap;
    glGenTextures( 1, &diffuseMap );
    glGenTextures( 1, &specularMap );
    glGenTextures( 1, &emissionMap );
    
    int imageWidth, imageHeight;
    unsigned char *image;
    
    // Diffuse map
    image = SOIL_load_image( "res/images/wall.jpg", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB );
    glBindTexture( GL_TEXTURE_2D, diffuseMap );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap( GL_TEXTURE_2D );
    SOIL_free_image_data( image );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST );
    
    // Set texture units
    lightingShader.Use( );
    glUniform1i( glGetUniformLocation( lightingShader.Program, "material.diffuse" ), 0 );
    glUniform1i( glGetUniformLocation( lightingShader.Program, "material.specular" ), 1 );
    
    glm::mat4 projection = glm::perspective( camera.GetZoom( ), ( GLfloat )SCREEN_WIDTH / ( GLfloat )SCREEN_HEIGHT, 0.1f, 100.0f );
    
    // Game loop
    while ( !glfwWindowShouldClose( window ) )
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime( );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents( );
        DoMovement( );
        
        // Clear the colorbuffer
        glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        
        // Use cooresponding shader when setting uniforms/drawing objects
        lightingShader.Use( );
        GLint viewPosLoc = glGetUniformLocation( lightingShader.Program, "viewPos" );
        glUniform3f( viewPosLoc, camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition( ).z);
        // Set material properties
        glUniform1f( glGetUniformLocation( lightingShader.Program, "material.shininess" ), 10.0f );
        // == ==========================
        // Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
        // the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
        // by defining light types as classes and set their values in there, or by using a more efficient uniform approach
        // by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
        // == ==========================
        
        // Directional light
        glUniform3f( glGetUniformLocation( lightingShader.Program, "dirLight.direction" ), -0.2f, -1.0f, -0.3f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "dirLight.ambient" ), 0.05f, 0.05f, 0.05f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "dirLight.diffuse" ), 0.04f, 0.04f, 0.4f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "dirLight.specular" ), 0.05f, 0.05f, 0.05f );

        // Point light 1
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[0].position" ), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[0].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[0].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[0].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[0].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[0].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[0].quadratic" ), 0.032f );
        
        // Point light 2
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[1].position" ), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[1].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[1].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[1].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[1].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[1].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[1].quadratic" ), 0.032f );
        
        // Point light 3
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[2].position" ), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[2].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[2].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[2].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[2].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[2].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[2].quadratic" ), 0.032f );
        
        // Point light 4
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[3].position" ), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[3].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[3].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[3].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[3].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[3].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[3].quadratic" ), 0.032f );
   
        // Point light 5
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[4].position" ), pointLightPositions[4].x, pointLightPositions[4].y, pointLightPositions[4].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[4].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[4].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[4].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[4].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[4].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[4].quadratic" ), 0.032f );
        
        // Point light 6
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[5].position" ), pointLightPositions[5].x, pointLightPositions[5].y, pointLightPositions[5].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[5].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[5].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[5].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[5].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[5].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[5].quadratic" ), 0.032f );
        
        // Point light 7
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[6].position" ), pointLightPositions[6].x, pointLightPositions[6].y, pointLightPositions[6].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[6].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[6].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[6].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[6].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[6].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[6].quadratic" ), 0.032f );
        
        // Point light 1
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[7].position" ), pointLightPositions[7].x, pointLightPositions[7].y, pointLightPositions[7].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[7].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[7].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[7].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[7].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[7].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[7].quadratic" ), 0.032f );
        
        // Point light 2
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[8].position" ), pointLightPositions[8].x, pointLightPositions[8].y, pointLightPositions[8].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[8].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[8].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[8].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[8].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[8].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[8].quadratic" ), 0.032f );
        
        // Point light 3
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[9].position" ), pointLightPositions[9].x, pointLightPositions[9].y, pointLightPositions[9].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[9].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[9].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[9].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[9].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[9].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[9].quadratic" ), 0.032f );
        
        // Point light 4
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[10].position" ), pointLightPositions[10].x, pointLightPositions[10].y, pointLightPositions[10].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[10].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[10].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[10].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[10].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[10].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[10].quadratic" ), 0.032f );
        
        // Point light 5
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[11].position" ), pointLightPositions[11].x, pointLightPositions[11].y, pointLightPositions[11].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[11].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[11].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[11].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[11].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[11].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[11].quadratic" ), 0.032f );
        
        // Point light 6
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[12].position" ), pointLightPositions[12].x, pointLightPositions[12].y, pointLightPositions[12].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[12].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[12].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[12].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[12].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[12].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[12].quadratic" ), 0.032f );
        
        // Point light 7
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[13].position" ), pointLightPositions[13].x, pointLightPositions[13].y, pointLightPositions[13].z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[13].ambient" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[13].diffuse" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "pointLights[13].specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[13].constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[13].linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "pointLights[13].quadratic" ), 0.032f );
        
        // SpotLight
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.position" ), camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition( ).z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.direction" ), camera.GetFront( ).x, camera.GetFront( ).y, camera.GetFront( ).z );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.ambient" ), 0.0f, 0.0f, 0.0f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.diffuse" ), 1.0f, 1.0f, 1.0f );
        glUniform3f( glGetUniformLocation( lightingShader.Program, "spotLight.specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.linear" ), 0.09f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.quadratic" ), 0.032f );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.cutOff" ), glm::cos( glm::radians( 12.5f ) ) );
        glUniform1f( glGetUniformLocation( lightingShader.Program, "spotLight.outerCutOff" ), glm::cos( glm::radians( 15.0f ) ) );
        
        // Create camera transformations
        glm::mat4 view;
        view = camera.GetViewMatrix( );
        
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation( lightingShader.Program, "model" );
        GLint viewLoc = glGetUniformLocation( lightingShader.Program, "view" );
        GLint projLoc = glGetUniformLocation( lightingShader.Program, "projection" );
        // Pass the matrices to the shader
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        // Bind diffuse map
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, diffuseMap );
        // Bind specular map
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, specularMap );
        
        // Draw 10 containers with the same VAO and VBO information; only their world space coordinates differ
        glm::mat4 model;
        glBindVertexArray( boxVAO );
        for ( GLuint i = 0; i < 1; i++ )
        {
            model = glm::mat4( );
            model = glm::translate( model, cubePositions[i] );
            GLfloat angle = 20.0f * i;
            model = glm::rotate( model, angle, glm::vec3( 1.0f, 0.3f, 0.5f ) );
            glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
            
            glDrawArrays( GL_TRIANGLES, 0, 360 );
        }
        glBindVertexArray( 0 );
        
        
        // Also draw the lamp object, again binding the appropriate shader
        lampShader.Use( );
        // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
        modelLoc = glGetUniformLocation( lampShader.Program, "model" );
        viewLoc = glGetUniformLocation( lampShader.Program, "view" );
        projLoc = glGetUniformLocation( lampShader.Program, "projection" );
        // Set matrices
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        model = glm::mat4( );
        model = glm::translate( model, lightPos );
        model = glm::scale( model, glm::vec3( 0.2f ) ); // Make it a smaller cube
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );

        // We now draw as many light bulbs as we have point lights.
        glBindVertexArray( lightVAO );
        for ( GLuint i = 0; i < 14; i++ )
        {
            model = glm::mat4( );
            model = glm::translate( model, pointLightPositions[i] );
            model = glm::scale( model, glm::vec3( 0.2f ) ); // Make it a smaller cube
            glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
            glDrawArrays( GL_TRIANGLES, 0, 36 );
        }
        glBindVertexArray( 0 );
        
        // Swap the screen buffers
        glfwSwapBuffers( window );
    }
    
    glDeleteVertexArrays( 1, &boxVAO );
    glDeleteVertexArrays( 1, &lightVAO );
    glDeleteBuffers( 1, &VBO );
    
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate( );
    
    return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement( )
{
    // Camera controls
    if ( keys[GLFW_KEY_W] || keys[GLFW_KEY_UP] )
    {
        camera.ProcessKeyboard( FORWARD, deltaTime );
    }
    
    if ( keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN] )
    {
        camera.ProcessKeyboard( BACKWARD, deltaTime );
    }
    
    if ( keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT] )
    {
        camera.ProcessKeyboard( LEFT, deltaTime );
    }
    
    if ( keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT] )
    {
        camera.ProcessKeyboard( RIGHT, deltaTime );
    }
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if ( GLFW_KEY_ESCAPE == key && GLFW_PRESS == action )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    
    if ( key >= 0 && key < 1024 )
    {
        if ( action == GLFW_PRESS )
        {
            keys[key] = true;
        }
        else if ( action == GLFW_RELEASE )
        {
            keys[key] = false;
        }
    }
}

void MouseCallback( GLFWwindow *window, double xPos, double yPos )
{
    if ( firstMouse )
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xPos;
    lastY = yPos;
    
    camera.ProcessMouseMovement( xOffset, yOffset );
}
