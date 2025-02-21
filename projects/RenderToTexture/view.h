#ifndef VIEW_H
#define VIEW_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <map>

#include "cyCodeBase/cyGL.h"

cyGLRenderTexture2D renderTexture;
cy::GLSLProgram planeProgram;
GLuint planeVAO;
GLuint planeVBO;

class View
{
private:
    std::map <GLuint, std::vector<int>> vao_to_mesh_indices;

    // Should the view contain links to the VAO and VBO and associated meshes?
    // I feel like each mesh should be responsible for its own VAO and VBO

public:
    cyGLSLProgram glsl_program;
private:

public:
    View();
    ~View();

}


#endif