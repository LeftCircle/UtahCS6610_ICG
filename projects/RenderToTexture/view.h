#ifndef VIEW_H
#define VIEW_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "cyCodeBase/cyGL.h"

cyGLRenderTexture2D renderTexture;
cy::GLSLProgram planeProgram;
GLuint planeVAO;
GLuint planeVBO;

void render(void);

void init_render_texture_and_plane(GLuint VAO, GLuint VBO, 
			const float *planeVertices, const unsigned int n_vertices)
{
    // Initialize render texture
    renderTexture.Initialize(true, 3, 800, 600);
    renderTexture.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);
    
    // Create and setup plane VAO/VBO
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	const unsigned int size = n_vertices * sizeof(float);
	
	// NOTE -> The data access might be incorrect here
    glBufferData(GL_ARRAY_BUFFER, size, &planeVertices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Setup plane shader
    planeProgram.BuildFiles("plane.vert", "plane.frag");
}


#endif