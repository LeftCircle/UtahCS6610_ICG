#include "view.h"

void render(void)
{
    // First render the object to the texture
    renderTexture.Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    scene.set_mvp_and_update_uniforms();
    
    glBindVertexArray(test_global_vao);
    rc::rcTriMeshForGL& mesh = *scene.get_mesh();
    for (MaterialGroup& material_group : material_groups)
    {
        _render_material_group(material_group, mesh);
    }
    renderTexture.Unbind();

    // Then render the texture on the plane
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    planeProgram.Bind();
    renderTexture.BindTexture(0);  // Bind to texture unit 0
    planeProgram.SetUniform("renderTex", 0);
    
    glBindVertexArray(planeVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, planeIndices);

    glutSwapBuffers();
}

void init_render_texture_and_plane(GLuint VAO, GLuint VBO, 
								   const float *planeVertices,
								   const unsigned int n_vertices)
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
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
						 (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Setup plane shader
	planeProgram.BuildFiles("plane.vert", "plane.frag");
}