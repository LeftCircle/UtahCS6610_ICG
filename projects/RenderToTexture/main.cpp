/*
This script renders a .obj file to a texture, and displays that texture. 
The following requirements are as follows:


Your program must take the name of the .obj file as its first command-line
argument. The object should be rendered using the diffuse texture, if one is
specified in the mtl file. Instead of rendering the object directly to 
the OpenGL viewport, the object should be renderered to a texture via a 
render buffer. You can use the GLRenderTexture class for generating and 
using a render buffer. Display the rendered texture by mapping it on a 
square-shaped plane. As before, left mouse button (and drag) should adjust the
camera angles, and right mouse button (and drag) should adjust the camera 
distance, used for rendering the object to the texture. If the ALT/Option key
is pressed, the left and right mouse buttons (and drag) should control the
same view parameters for rendering the plane. Add some small constant to 
color of the plane to separate it from the background color.  
*/

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "cyCodeBase/cyMatrix.h"
#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyGL.h"

#include "rcCodeBase/rcCamera.hpp"
#include "rcCodeBase/rcOpenGLScene.hpp"
#include "rcCodeBase/rcCore.hpp"
#include "rcCodeBase/rcObjModifier.h"
#include "rcCodeBase/rcLights.hpp"
#include "rcCodeBase/rcTexture.h"

#include "model.h"
#include "view.h"
#include "controller.h"

#include <iostream>
#include <vector>
#include <map>
#include <filesystem>


// Add after your existing global variables

cyGLRenderTexture2D renderTexture;
cy::GLSLProgram planeProgram;
GLuint planeVAO;
GLuint planeVBO;
bool useAltCamera = false;


// Update main to init the render texture
int main(int argc, char** argv)
{
    // ... existing initialization code ...
    
    init_window(argc, argv);
    init_points_from_mesh(mesh);
    init_render_texture_and_plane();  // Add this line
    init_camera();
    bind_glut_functions();

    glutMainLoop();
    return 0;
}

/*
possible resources
https://github.com/kvmanohar22/opengl-examples/blob/e910c3263a30cdf1b2c53c6f82ddeebca9b11f41/04.advanced/05_framebuffers.cpp

*/