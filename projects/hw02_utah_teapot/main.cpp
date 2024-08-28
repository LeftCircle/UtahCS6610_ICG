#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

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
	glutSwapBuffers();
}

void idle(void)
{

}

bool _are_command_arguments_valid(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "NO FILENAME GIVEN!! Usage: " << argv[0] << " <filename.obj>" << std::endl;
		return false;
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
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(render);
	glutIdleFunc(idle);
}

void init_points()
{
	
}

int main(int argc, char** argv)
{
	if (!_are_command_arguments_valid(argc, argv))
	{
		// If we are in debug mode, use the default filename.
		// Otherwise, exit the program.
		#ifdef _DEBUG
		filename = "teapot.obj";
		#else
		return 1;
		#endif
	}
	
	char *filename = argv[1];

	// Let's just render a simple triangle for now. 
	init_glut_and_glew(argc, argv);
	glutMainLoop();
	return 0;
}
