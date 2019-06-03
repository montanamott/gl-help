
// ----------------------- Important Note ---------------------------------//
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   This file is simply a template I keep in this repo for two reasons:   //
//        1. To test my gl-help.hpp                                        //
//        2. Keep as a starter template for OpenGL programs                //
//                                                                         //
//   It is not in any way a part of the helper functions in gl-help.hpp    //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "gl-help.hpp"


const unsigned WINDOW_WIDTH = 600;
const unsigned WINDOW_HEIGHT = 600;

int main(int argc, char* argv[])
{

    GLFWwindow* window = setupWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Testing Window");

    Shader shader("Basic.glsl");
    shader.bind();

    float positions[] = {
       -0.6f, -0.5f, 0.0f, 0.0f,     // Bottom left
        0.6f, -0.5f, 1.0f, 0.0f,       // Bottom right
        0.6f,  0.5f, 1.0f, 1.0f,       // Top right
       -0.6f,  0.5f,  0.0f, 1.0f     // Top Left
    }; 

    unsigned int indices[] = {
        0, 1, 2, 
        2, 3, 0
    };


    Texture texture("neptune.png");
    texture.bind(0);
    shader.setUniform1i("u_Texture", 0);
    



    VertexArray va;
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));
    BufferLayout layout; 
    layout.push<float>(2); 
    layout.push<float>(2); 
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6); 

    // uncomment this call to draw using wireframe 
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop

    while (!glfwWindowShouldClose(window))
    {
        // input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a triangle
        shader.bind();
        va.Bind(); 
        ib.Bind();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // glBindVertexArray(0); 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;


}