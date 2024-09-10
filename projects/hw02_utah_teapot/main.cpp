#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "cyCodeBase/cyMatrix.h"
#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyGL.h"
#include "cyCodeBase/cyTriMesh.h"
#include "rcCodeBase/rcCamera.hpp"
#include "rcCodeBase/rcOpenGLScene.hpp"
#include "rcCodeBase/rcCore.hpp"
#include "rcCodeBase/rcObjModifier.h"
#include <iostream>


using namespace rc;

const char* filename;
GLScene scene;


// OpenGL function that adjusts theta and phi based on mouse movement when the left mouse button is pressed

void mouse_motion(int x, int y)
{
	int button = glutGetModifiers();
	if (button == GLUT_LEFT_BUTTON)
	{
		int dx = x - scene.mouse_position.x;
		int dy = y - scene.mouse_position.y;
	
		scene.camera.rotate_about_og_up(-dx, -dy);
		scene.mouse_position.Set(x, y);
	}
	glutPostRedisplay();
}

void mouse_buttons(int button, int state, int x, int y)
{
	// store the mouse position when the left button is pressed
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		scene.mouse_position.Set(x, y);
	}
	// adjust the radius of the camera based on the mouse wheel
	if (button == 3)
	{
		scene.camera.zoom(-5.0f);
	}
	if (button == 4)
	{
		scene.camera.zoom(5.0f);
	}
	glutPostRedisplay();
}

// function to close the window when escape is pressed
void keyboard(unsigned char key, int x, int y)
{
	if (key == 27)
	{
		glutLeaveMainLoop();
	}
}

void fn_keyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_F6)
	{
		std::cout << "Recompiling shaders" << std::endl;
		scene.program.BuildFiles("shader.vert", "shader.frag");
		glutPostRedisplay();
	}
}

void render(void)
{
	glClearColor(scene.bg[0], scene.bg[1], scene.bg[2], scene.bg[3]);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, scene.n_elements, GL_UNSIGNED_INT, 0);
	scene.set_mvp();
	scene.program["mvp"] = scene.MVP();
	glutSwapBuffers();
}

void idle(void)
{

}

bool _are_command_arguments_valid_or_debug(int argc, char** argv)
{
	if (argc != 2)
	{
		#ifdef _DEBUG
		filename = "teapot.obj";
		return true;
		#else
		std::cerr << "NO FILENAME GIVEN!! Usage: " << argv[0] << " <filename.obj>" << std::endl;
		return false;
		#endif
	}
	return true;
}

void init_glut_and_glew(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glEnable(GL_DEPTH_TEST);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Utah Teapot");
	glewInit();
	CY_GL_REGISTER_DEBUG_CALLBACK;
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(render);
	glutIdleFunc(idle);
	glutMouseFunc(mouse_buttons);
	glutMotionFunc(mouse_motion);
	glutSpecialFunc(fn_keyboard);
}

void init_points_from_mesh(rc::rcTriMeshForGL& mesh)
{
	scene.n_points = mesh.NV();
	scene.n_elements = mesh.NE();

	// Create vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	GLuint v_vbo;
	GLuint vn_vbo;
	GLuint vt_vbo;
	GLuint ebuffer;

	glGenBuffers(1, &v_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, v_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.get_vbo_size(), &mesh.V_vbo(0), GL_STATIC_DRAW);

	/*glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.get_vbo_size(), &mesh.VN_vbo(0), GL_STATIC_DRAW);

	glGenBuffers(1, &vt_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vt_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.get_vbo_size(), &mesh.VT_vbo(0), GL_STATIC_DRAW);*/

	glGenBuffers(1, &ebuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * mesh.get_n_elements(), &mesh.E(0), GL_STATIC_DRAW);

	// Load the shaders with cy calls
	bool shader_comp_success = scene.program.BuildFiles("shader.vert", "shader.frag");
	scene.program.Bind();	

	scene.program.SetAttribBuffer("position", v_vbo, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//scene.program.SetAttribBuffer("normal", vn_vbo, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//scene.program.SetAttribBuffer("texcoord", vt_vbo, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);
	
	
	// Some final point transformations 
	// Rotate the points to sit on the +y axis
	scene.point_transform = cy::Matrix4f::RotationX(-PI_OVER_2);

	// Center the object using the bounding box
	mesh.ComputeBoundingBox();
	cy::Vec3f bounding_box_center = (mesh.GetBoundMin() + mesh.GetBoundMax()) / 2.0f;
	bounding_box_center = cy::Vec3f(cy::Matrix4f::RotationX(-PI_OVER_2) * bounding_box_center);
	scene.point_transform.AddTranslation(-bounding_box_center);
}

void init_camera()
{
	// getting the projection matrix
	float aspect_ratio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	scene.camera.set_perspective_projection(DEG2RAD(90), aspect_ratio, 0.0f, 30.0f);

	cy::Vec3f camera_pos = cy::Vec3f(0.0f, 0.0f, 1200.0f);
	cy::Vec3f up = cy::Vec3f(0.0, 1.0, 0.0);

	cy::Vec3f target = cy::Vec3f(0.0f, 0.0f, 0.0f);
	scene.camera.lookat(camera_pos, target, up);

	scene.set_mvp();
	scene.program["mvp"] = scene.MVP();
}

int main(int argc, char** argv)
{
	if (!_are_command_arguments_valid_or_debug(argc, argv))
	{
		return 1;
	}
	rc::rcTriMeshForGL mesh;
	bool success = mesh.LoadFromFileObj(filename);
	if (!success)
	{
		std::cerr << "Error loading file: " << filename << std::endl;
		return 1;
	}
	mesh.create_vbo_data_and_elements();

	init_glut_and_glew(argc, argv);
	init_points_from_mesh(mesh);
	init_camera();
	glutMainLoop();
	return 0;
}
