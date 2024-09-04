#ifndef RC_CAMERA_HPP
#define RC_CAMERA_HPP
#include "cyCodeBase/cyMatrix.h"
#include "rcCodeBase/rcCore.hpp"
#include <iostream>


namespace rc{

class Camera
{
public:
	cy::Vec3f position = cy::Vec3f(0, 0, 10);
	cy::Vec3f target = cy::Vec3f(0, 0, 0);
	cy::Matrix4f view_matrix;
	cy::Matrix4f projection_matrix;
	float fov;

	void set_perspective_projection(float new_fov, float aspect, float near, float far);
	void lookat(cy::Vec3f& pos, cy::Vec3f& targ_v, cy::Vec3f& up_v);

	//! Rotates the position of the camera by theta about the up vector and phi about the tangent vector
	//! while continuing to look at the target. 
	void rotate(float t, float p);
	void zoom(float z);
	
	cy::Vec3f get_up_vector();
	cy::Vec3f get_tangent_vector();
};

} // namespace rc
#endif