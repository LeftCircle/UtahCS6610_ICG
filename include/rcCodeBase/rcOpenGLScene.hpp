#ifndef RC_OPENGL_SCENE_HPP
#define RC_OPENGL_SCENE_HPP

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "cyCodeBase/cyIVector.h"
#include "rcCodeBase/rcCamera.hpp"
#include "cyCodeBase/cyGL.h"


class GLScene
{
public:
	GLfloat bg[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const char* window_name = "Window Name";
	cy::GLSLProgram program;
	float FOV = 90.0f;
	float previous_frame_time = 0.0f;
	rc::Camera camera;
	cy::IVec2i mouse_position = cy::IVec2i(0, 0);
	cy::Matrix4f point_transform = cy::Matrix4f::Identity();

	int n_points = 0;

	void set_mvp()
	{
		program["mvp"] = camera.projection_matrix * camera.view_matrix * point_transform;
	}
};
#endif // !RC_OPENGL_SCENE_HPP