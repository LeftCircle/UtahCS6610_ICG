#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "cyCodeBase/cyMatrix.h"
#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyGL.h"
#include "cyCodeBase/cyTriMesh.h"
#include "cyHelpers.h"

#define DEG2RAD(x) (x * 3.14159265359f / 180.0f)

const char* filename;
GLScene scene;


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
	glutSwapBuffers();
}

void idle(void)
{
	// rotate the teapot around the y axis with respect to time
	float gl_time = glutGet(GLUT_ELAPSED_TIME);
	float dt = (gl_time - scene.previous_frame_time) / 100.0f;
	scene.previous_frame_time = gl_time;
	scene.rotation_matrix = scene.rotation_matrix * cy::Matrix4f::Identity().RotationZ(DEG2RAD(3.0 * dt));
	scene.set_mvp_matrix();
	glutPostRedisplay();
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
}

void init_points_from_mesh(cy::TriMesh& mesh)
{
	scene.n_points = mesh.NV();
	//cyMeshExtension::center_mesh(mesh);
	// Create vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create vertex buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.NV(), &mesh.V(0), GL_STATIC_DRAW);

	// Load the shaders with cy calls
	bool shader_comp_success = scene.program.BuildFiles("shader.vert", "shader.frag");
	scene.program.Bind();

	// getting the projection matrix
	float aspect_ratio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	scene.projection_matrix.SetPerspective(DEG2RAD(scene.FOV), aspect_ratio, 0.0f, 20.0f);
	cy::Vec3f camera_pos = cy::Vec3f(0.0f, -3.0f, -2.0f);
	cy::Vec3f target = cy::Vec3f(0.0f, 0.0f, 0.0f);
	cy::Vec3f up = cy::Vec3f(1.0, 0.0, 0.0).Cross(target - camera_pos);
	scene.view_matrix.SetView(camera_pos, target, up);
	//scene.set_perspective_matrix(scene.FOV, aspect_ratio, 0.0f, 20.0f);
	//scene.rotation_matrix.SetRotationX(DEG2RAD(-90.0f));
	scene.scale_matrix.SetScale(0.05f);
	scene.translation_matrix.SetTranslation(cy::Vec3f(0.0f, 0.0f, 0.0f));
	
	// Setting the uniform. The below code should be the same as the cy function
	// 
	// GLint mvp_location = glGetUniformLocation(program.GetID(), "mvp");
	// glUniformMatrix4fv(mvp_location, 1, GL_FALSE, scale_matrix.cell);
	scene.set_mvp_matrix();

	// Getting position into the vertex shader. The below code should be the same as the cy function
	// 
	//GLuint pos = glGetAttribLocation(program.GetID(), "position");
	//glEnableVertexAttribArray(pos);
	//glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	scene.program.SetAttribBuffer("position", vbo, 3, GL_FLOAT, GL_FALSE, 0, 0);
	

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
