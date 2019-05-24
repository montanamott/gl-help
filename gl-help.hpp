
// ---------------- Montana Mott's OpenGL Helper File ---------------- // 

// A typical workflow using the file may look something like this!

//      GLFWwindow* window = setupWindow(600, 800, "Example_GL");
//      unsigned vertID = compileShader(&vert_text, GL_VERTEX_SHADER); 
//      unsigned fragID = compileShader(&frag_text, GL_FRAGMENT_SHADER);
//      unsigned programID = linkAndDelete(vertID, fragID);

// g++ main.cpp -o YOUR_EXECUTABLE.exe -lglfw -lglew -framework OpenGL -g
// Please contact me on GitHub or submit an issue if you have any 
// comments, questions, or suggestions!
// ------------------------------------------------------------------- //


#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <iostream> 
#include <fstream>
#include <string> 

using std::string; 
using std::fstream; 

inline void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool checkShaderError(unsigned shaderID);
bool checkShaderLinkError(unsigned int programID);

// Creates a window with the specified height, width, and name, and returns a pointer to that window 
// and sets the current OpenGL context to be the window, and initializes GLFW and GLEW
// (This functions should most likely get called before anything else)
GLFWwindow* setupWindow(const unsigned width, const unsigned height, string name)
{
    // Initialize GLFW and configure window properties 
    if(!glfwInit())
    {
        std::cerr << "Error: Could not initialize GLFW. " << std::endl; 
        exit(1);
    } 

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__ 
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Uncommenting this fixes compilation on OSX
    #endif

    // Create the window using GLFW 
    GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if(window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl; 
        glfwTerminate(); 
        return nullptr; 
    }

    glfwPollEvents(); // This is a hack to fix window resize issues on Mojave
    glfwSetWindowSize(window, width, height);
    glfwMakeContextCurrent(window); 

    // Initialize GLEW 
    glewExperimental = true; 
    glewInit(); 

    // OpenGL defaults I commonly use 
    // glEnable(GL_DEPTH_TEST); 
    // glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return window; 
}


// vertID and fragID should be ID's of compiled shaders. This function links them into a program
// and then sets the current program to be the one it links. Then it  deleets the compiled versions
// of the shaders to save memory and returns the ID of the program
GLuint linkAndDelete(unsigned vertID, unsigned fragID)
{
    GLuint programID = glCreateProgram(); 
    glAttachShader(programID, vertID);
    glAttachShader(programID, fragID);
    glLinkProgram(programID);

    glDeleteShader(vertID);
    glDeleteShader(fragID);

    if(checkShaderLinkError(programID)) return 0;
    
    glUseProgram(programID);
    return programID;
}

// shaderType example: GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
GLuint compileShader(const char* shader_src, unsigned shaderType)
{
    GLuint shaderID; 
    shaderID = glCreateShader(shaderType); 

    glShaderSource(shaderID, 1, &shader_src, NULL); 
    glCompileShader(shaderID);
    if(checkShaderError(shaderID)) return 0; 
    return shaderID;
}

string textFromFile(string filename)
{
    std::ifstream fin(filename);
    if(!fin.is_open())
    {
        std::cout << "A shader file, \"" << filename << "\" couldn't be found." << std::endl;
        return "";
    }

    return string( (std::istreambuf_iterator<char>(fin) ), (std::istreambuf_iterator<char>()) );
}

// Returns true of an error is present from compiling the given shaderID and prints
// the associated error message
bool checkShaderError(unsigned shaderID)
{
    int success; 
    char infoLog[512]; 
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        std::cerr << "Errror: Shader Compilation failed, the message is: " << infoLog << std::endl; 
        return true;
    }

    std::cout << "Shader " << shaderID << " compiled succesfully. \n";
    return false;
}

bool checkShaderLinkError(unsigned int programID)
{
    int success; 
    char infoLog[512]; 
    glGetProgramiv(programID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cerr << "Errror: Program linking failed, the message is: " << infoLog << std::endl; 
        return true;
    }

    std::cout << "Shader " << programID << " compiled succesfully. \n";
    return false;
}

inline void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

class VertexBuffer {
    private: 
        GLuint ID;
    public: 
        VertexBuffer(const void* data, unsigned size)
        {
            glGenBuffers(1, &ID);
            glBindBuffer(GL_ARRAY_BUFFER, ID);
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        }

        ~VertexBuffer()
        { glDeleteBuffers(1, &ID); }

        void Bind() 
        { glBindBuffer(GL_ARRAY_BUFFER, ID); }
       
        void Unbind() 
        { glBindBuffer(GL_ARRAY_BUFFER, 0); }
};
