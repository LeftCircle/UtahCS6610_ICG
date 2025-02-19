#include "controller.h"

void mouse_motion(int x, int y)
{
    int modifiers = glutGetModifiers();
    useAltCamera = (modifiers & GLUT_ACTIVE_ALT);
    
    if (useAltCamera)
    {
        // Handle plane camera movement
        // ... implement plane camera controls
    }
    else
    {
        // Your existing object camera movement code
        bool can_rotate_obj = (button == GLUT_LEFT_BUTTON);
        if (can_rotate_obj)
        {
            int dx = x - scene.mouse_position.x;
            int dy = y - scene.mouse_position.y;
            scene.camera.rotate_about_og_up(-dx, -dy);
        }
    }
    
    scene.mouse_position.Set(x, y);
    glutPostRedisplay();
}