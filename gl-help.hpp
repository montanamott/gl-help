#ifndef GL_HELP_H
#define GL_HELP_H

// ---------------- Montana Mott's OpenGL Helper File ---------------- // 

// A typical workflow using the file may look something like this!

// g++ main.cpp -o YOUR_EXECUTABLE.exe -lglfw -lglew -framework OpenGL -g
// Please contact me on GitHub or submit an issue if you have any 
// comments, questions, or suggestions!
// ------------------------------------------------------------------- //

#include <GL/glew.h>
#include <GLFW/glfw3.h> 
#include <STBI/stb_image.h> 
#include <STBI/decoy_stb.cpp>
#include <iostream> 
#include <fstream>
#include <string> 
#include <vector>
#include <sstream>

using std::string; 
using std::fstream; 

inline void framebuffer_size_callback(GLFWwindow* window, int width, int height);


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
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    return window; 
}


inline void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

unsigned int GetSizeOfType(unsigned int type)
{
    switch(type)
    {
        case GL_FLOAT:          return 4;
        case GL_UNSIGNED_INT:   return 4;
        case GL_UNSIGNED_BYTE:  return 1;
    }
    std::cerr << "Error: Using unsupported GLType with unknown size" << std::endl;
    return 0; 
}

struct ShaderProgramSource 
{ 
    std::string vertexSource; 
    std::string fragmentSource;
};

enum class ShaderType 
{
    NONE = -1, 
    VERTEX = 0,
    FRAGMENT = 1
};

class Shader 
{
    private: 
        unsigned ID;
        string filePath;

    public: 
        Shader(const std::string& filePath)
            : filePath(filePath), ID(0)
        {
            ShaderProgramSource source = parseShader();
            ID = linkAndDelete(compileShader(source.vertexSource.c_str(), GL_VERTEX_SHADER), 
                                            compileShader(source.fragmentSource.c_str(), GL_FRAGMENT_SHADER));

        }

        ~Shader()
        {
            glDeleteProgram(ID);
        }

        void bind() const
        {
            glUseProgram(ID);
        }

        void unbind() const 
        {
            glUseProgram(0);
        }

        void setUniformMat4(const std::string& name, const GLfloat* value)
        {
            glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value);
        }

        void setUniform1i(const std::string& name, const int value)
        {
            glUniform1i(getUniformLocation(name), value);
        }

    private:

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
            
            return programID;
        }

        int getUniformLocation(const std::string& name)
        {
            int location = glGetUniformLocation(ID, name.c_str());
            if(location == -1)
            {
                std::cout << "A uniform in Program " << ID << " called " << name << " could not be found.\n";
            }

            return location;
        }

        ShaderProgramSource parseShader()
        {
            std::ifstream stream(filePath);
            std::string line; 
            std::stringstream sources[2]; 
            ShaderType type = ShaderType::NONE;
            while(std::getline(stream, line))
            {
                if(line.find("#shader") != std::string::npos)
                {
                    if(line.find("vertex") != std::string::npos)
                        type = ShaderType::VERTEX; 
                    else if (line.find("fragment") != std::string::npos)
                        type = ShaderType::FRAGMENT;
                }
                else
                {
                    sources[(int)type] << line << '\n';
                }
            }

            //  std::cout << "My vertex shader is: " << sources[0].str(); 
            // std::cout << "\n And my fragment shader is: " << sources[1].str();
            return { sources[0].str(), sources[1].str() };
        }
};

class VertexBuffer {
    private: 
        GLuint ID;
    public: 
        VertexBuffer(const void* data, unsigned sizeBytes)
        {
            glGenBuffers(1, &ID);
            glBindBuffer(GL_ARRAY_BUFFER, ID);
            glBufferData(GL_ARRAY_BUFFER, sizeBytes, data, GL_STATIC_DRAW);
        }

        ~VertexBuffer()
        { glDeleteBuffers(1, &ID); }

        void bind() const
        { glBindBuffer(GL_ARRAY_BUFFER, ID); }
       
        void Unbind() const
        { glBindBuffer(GL_ARRAY_BUFFER, 0); }
};

class IndexBuffer {
    private: 
        GLuint ID, count;
    public: 
        IndexBuffer(const void* data, unsigned count)
        :   count(count)
        {
            glGenBuffers(1, &ID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(GLuint), data, GL_STATIC_DRAW);
        }

        ~IndexBuffer()
        { glDeleteBuffers(1, &ID); }

        void bind() const
        { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
       
        void unbind() const
        { glBindBuffer(GL_ARRAY_BUFFER, 0); }
};

struct VertexBufferElement 
{
    GLuint type; 
    GLuint count; 
    unsigned char normalized;
};

class BufferLayout
{
    private: 
        std::vector<VertexBufferElement> elements;
        unsigned stride;
    public: 

        BufferLayout()
            : stride(0)
        {}

        BufferLayout(unsigned reserve_amount)
            : stride(0)
        {
            elements.reserve(reserve_amount);
        }

        template<typename T> 
        void push(unsigned count)
        {
            std::cerr << "Error: Unmatched Type, Push is doing nothing " << std::endl; 
        }

        template<>
        void push<float>(unsigned count)
        {
            elements.push_back({GL_FLOAT, count, GL_FALSE});
            stride += count * GetSizeOfType(GL_FLOAT); 
        }

        template<>
        void push<unsigned int>(unsigned count)
        {
            elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
            stride += count * GetSizeOfType(GL_UNSIGNED_INT);
        }

        template<>
        void push<unsigned char>(unsigned count)
        {
            elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
            stride += count * GetSizeOfType(GL_UNSIGNED_BYTE); 
        }

        inline const std::vector<VertexBufferElement>& GetElements() const { return elements; }
        inline unsigned GetStride() const { return stride; }
};

class VertexArray {
    private: 
        unsigned ID;
    public: 
        VertexArray() 
        {
            glGenVertexArrays(1, &ID);
        }

        ~VertexArray()
        {
            glDeleteVertexArrays(1, &ID);
        }

        void bind() const
        { glBindVertexArray( ID); }
       
        void unbind() const
        { glBindVertexArray(0); }

        void AddBuffer(const VertexBuffer& vb, const BufferLayout& layout)
        {
            bind();
            vb.bind();
            const auto& elements = layout.GetElements();
            unsigned offset = 0; 
            for(unsigned i = 0; i < elements.size(); ++i)
            {
                const auto& element = elements[i];
                glEnableVertexAttribArray(i); 
                glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
                offset += element.count * GetSizeOfType(element.type);
            }
        }

}; 

class Texture {
    private: 
        unsigned ID; 
        std::string filePath; 
        unsigned char* localBuffer; 
        int width, height, BPP; // Bits Per Pixel
    
    public: 
        Texture(const std::string& path)
            : ID(0), filePath(path), localBuffer(nullptr), width(0), height(0), BPP(0)
        {
            glGenTextures(1, &ID); 
            glBindTexture(GL_TEXTURE_2D, ID);

            stbi_set_flip_vertically_on_load(1);
            localBuffer = stbi_load(path.c_str(), &width, &height, &BPP, 4);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
            glBindTexture(GL_TEXTURE_2D, 0);

            if(localBuffer)
                stbi_image_free(localBuffer);
        }

        //stbi_set_flip_vertically_on_load(1);
        //stbi_load(path.c_str(), &width, &height, &BPP, 4);
        //stbi_image_free(localBuffer);

        ~Texture()
        {
            glDeleteTextures(1, &ID);
        }

        void bind(unsigned slot)
        {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, ID);
        }

        void unbind()
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        inline unsigned getWidth() const { return width; }
        inline unsigned getHeight() const { return height; }

};

#endif