#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyGL.h"

GLfloat bg[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
const char* window_name = "Utah Teapot";
const char* filename;

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
	glDrawArrays(GL_TRIANGLES, 0, 3);
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

void init_points()
{
	cy::Vec3f points[3] = {
		cy::Vec3f(-0.5f, -0.5f, 0.0f),
		cy::Vec3f(0.5f, -0.5f, 0.0f),
		cy::Vec3f(0.0f, 0.5f, 0.0f)
	};

	// Create vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create vertex buffer object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	// Load the shaders with cy calls
	cy::GLSLProgram program;
	bool shader_comp_success = program.BuildFiles("shader.vert", "shader.frag");
	program.Bind();

	// Getting position into the vertex shader. The below code should be the same as the cy function
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
	
	char *filename = argv[1];

	// Let's just render a simple triangle for now. 
	init_glut_and_glew(argc, argv);
	init_points();
	glutMainLoop();
	return 0;
}
