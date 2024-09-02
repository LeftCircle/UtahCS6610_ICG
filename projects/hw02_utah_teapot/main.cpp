#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "cyCodeBase/cyMatrix.h"
#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyGL.h"
#include "cyCodeBase/cyTriMesh.h"
#include "cyHelpers.h"
#include <iostream>

#define DEG2RAD(x) (x * 3.14159265359f / 180.0f)

const char* filename;
GLScene scene;


// OpenGL function that adjusts theta and phi based on mouse movement when the left mouse button is pressed

void mouse_motion(int x, int y)
{
	int button = glutGetModifiers();
	if (button == GLUT_LEFT_BUTTON)
	{
		// change theta by the x movement and phi by the y movement
		int dx = x - scene.mouse_position.x;
		int dy = y - scene.mouse_position.y;
		int old_x = scene.mouse_position.x;

		// Up direction should rotate by phi
		
		float new_theta = scene.camera.theta + dy;
		float new_phi = scene.camera.phi + dx;
		scene.camera.view_matrix = cy::Matrix4f::RotationY(DEG2RAD(dx)) * cy::Matrix4f::RotationX(DEG2RAD(dy)) * scene.camera.view_matrix;
		//scene.camera.set_spherical_position(scene.camera.radius, scene.camera.theta, new_phi);
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
		scene.camera.radius -= 0.1f;
		scene.camera.set_spherical_position(scene.camera.radius, scene.camera.theta, scene.camera.phi);
	}
	if (button == 4)
	{
		scene.camera.radius += 0.1f;
		scene.camera.set_spherical_position(scene.camera.radius, scene.camera.theta, scene.camera.phi);
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

void render(void)
{
	glClearColor(scene.bg[0], scene.bg[1], scene.bg[2], scene.bg[3]);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_POINTS, 0, scene.n_points);
	scene.set_mvp();
	glutSwapBuffers();
}

void idle(void)
{
	// rotate the teapot around the y axis with respect to time
	float gl_time = glutGet(GLUT_ELAPSED_TIME);
	float dt = (gl_time - scene.previous_frame_time) / 100.0f;
	scene.previous_frame_time = gl_time;
	//scene.rotation_matrix = scene.rotation_matrix * cy::Matrix4f::Identity().RotationZ(DEG2RAD(3.0 * dt));
	//scene.set_mvp_matrix();
	//scene.view_matrix = cy::Matrix4f::RotationY(DEG2RAD(3.0 * dt)) * scene.view_matrix;
	//scene.program["mvp"] = scene.projection_matrix * scene.view_matrix;
	
	//scene.camera.rotateZ(3.0 * dt);
	//scene.program["mvp"] = scene.projection_matrix * scene.camera.view_matrix;
	//glutPostRedisplay();
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
	glutInitWindowSize(800, 600);
	glutCreateWindow("Utah Teapot");
	glewInit();
	CY_GL_REGISTER_DEBUG_CALLBACK;
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(render);
	glutIdleFunc(idle);
	glutMouseFunc(mouse_buttons);
	glutMotionFunc(mouse_motion);
}

void init_points_from_mesh(cy::TriMesh& mesh)
{
	scene.n_points = mesh.NV();

	/*std::vector<cy::Vec4f> vertices;
	for (int i = 0; i < mesh.NV(); i++)
	{
		vertices.push_back(cy::Vec4f(mesh.V(i), 1.0f));
	}*/



	// Create vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create vertex buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.NV(), &mesh.V(0), GL_STATIC_DRAW);
	// Trying with vec4s and divide by w in the shader
	//glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec4f) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// Load the shaders with cy calls
	bool shader_comp_success = scene.program.BuildFiles("shader.vert", "shader.frag");
	scene.program.Bind();

	// getting the projection matrix
	float aspect_ratio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	
	scene.projection_matrix.SetPerspective(DEG2RAD(scene.FOV), aspect_ratio, 0.0f, 10.0f);
	cy::Vec3f camera_pos = cy::Vec3f(0.0f, 0.0f, 30.0f);
	cy::Vec3f target = cy::Vec3f(0.0f, 0.0f, 0.0f);
	cy::Vec3f up = cy::Vec3f(1.0, 0.0, 0.0).Cross(target - camera_pos);
	//scene.view_matrix.SetView(camera_pos, target, up);
	scene.camera.lookat(camera_pos, target, up);

	// Just set the mvp with look at and then perspective projection
	scene.program["mvp"] = scene.projection_matrix * scene.camera.view_matrix;

	scene.program.SetAttribBuffer("position", vao, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//scene.program.SetAttribBuffer("position", vao, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

int main(int argc, char** argv)
{
	if (!_are_command_arguments_valid_or_debug(argc, argv))
	{
		return 1;
	}
	cy::TriMesh mesh;
	bool success = mesh.LoadFromFileObj(filename);
	if (!success)
	{
		std::cerr << "Error loading file: " << filename << std::endl;
		return 1;
	}

	init_glut_and_glew(argc, argv);
	init_points_from_mesh(mesh);
	glutMainLoop();
	return 0;
}
