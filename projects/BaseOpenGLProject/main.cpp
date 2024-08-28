#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>

GLfloat bg[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

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
	bg[0] += 0.001f;
	if (bg[0] > 1.0f)
	{
		bg[0] = 0.0f;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Hello World");
	glewInit();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(render);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
