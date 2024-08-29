#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "cyCodeBase/cyMatrix.h"
#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyGL.h"
#include "cyCodeBase/cyTriMesh.h"

GLfloat bg[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
const char* window_name = "Utah Teapot";
const char* filename;
cy::Matrix4f scale_matrix = cy::Matrix4f(
	0.05f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.05f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.05f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);
cy::Matrix4f rotation_matrix = cy::Matrix4f::Identity();
cy::Matrix4f translation_matrix = cy::Matrix4f::Identity();
cy::Matrix4f projection_matrix = cy::Matrix4f::Identity();
float FOV = 90.0f;


int n_points = 0;

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
	glClearColor(bg[0], bg[1], bg[2], bg[3]);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_POINTS, 0, n_points);
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
	glutInitWindowSize(800, 600);
	glutCreateWindow(window_name);
	glewInit();
	CY_GL_REGISTER_DEBUG_CALLBACK;
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(render);
	glutIdleFunc(idle);
}

void init_points_from_mesh(cy::TriMesh& mesh)
{
	n_points = mesh.NV();
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
	cy::GLSLProgram program;
	bool shader_comp_success = program.BuildFiles("shader.vert", "shader.frag");
	program.Bind();

	// getting the projection matrix
	float aspect_ratio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	projection_matrix.SetPerspective(FOV, aspect_ratio, 0.0f, 3.0f);
	scale_matrix.Scale(0.05f);

	// Setting the uniform. The below code should be the same as the cy function
	// 
	// GLint mvp_location = glGetUniformLocation(program.GetID(), "mvp");
	// glUniformMatrix4fv(mvp_location, 1, GL_FALSE, scale_matrix.cell);
	program["mvp"] = projection_matrix * translation_matrix * scale_matrix;

	// Getting position into the vertex shader. The below code should be the same as the cy function
	// 
	//GLuint pos = glGetAttribLocation(program.GetID(), "position");
	//glEnableVertexAttribArray(pos);
	//glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	program.SetAttribBuffer("position", vbo, 3, GL_FLOAT, GL_FALSE, 0, 0);
	

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
