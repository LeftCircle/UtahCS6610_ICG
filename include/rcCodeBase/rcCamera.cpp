#ifndef RC_CAMERA_CPP
#define RC_CAMERA_CPP

#include "rcCodeBase/rcCamera.hpp"

using namespace rc;

void Camera::lookat(cy::Vec3f& pos, cy::Vec3f& targ_v, cy::Vec3f& up_v)
{
	position = pos, target = targ_v;
	view_matrix.SetView(pos, targ_v, up_v);
}

void Camera::set_perspective_projection(float new_fov, float aspect, float near, float far)
{
	fov = new_fov;
	projection_matrix.SetPerspective(DEG2RAD(new_fov), aspect, near, far);
}

void Camera::rotate(float t, float p)
{
	cy::Vec3f old_up = get_up_vector();
	cy::Vec3f old_tangent = get_tangent_vector();
	
	cy::Matrix4f rot_p_general = cy::Matrix4f::Rotation(old_tangent, DEG2RAD(p));
	cy::Matrix4f rot_t_general = cy::Matrix4f::Rotation(old_up, DEG2RAD(t));

	cy::Vec3f up = cy::Vec3f(rot_p_general * old_up);
	position = cy::Vec3f(rot_t_general * rot_p_general * position);
	view_matrix.SetView(position, target, up);
}

//! Moves the position of the camera either closer to or further from the target
void Camera::zoom(float z)
{
	cy::Vec3f direction = position - target;
	direction.Normalize();
	position += direction * z;
	view_matrix.SetView(position, target, get_up_vector());
}

cy::Vec3f Camera::get_tangent_vector()
{
	return cy::Vec3f(view_matrix.cell[0], view_matrix.cell[4], view_matrix.cell[8]);
}

cy::Vec3f Camera::get_up_vector()
{
	return cy::Vec3f(view_matrix.cell[1], view_matrix.cell[5], view_matrix.cell[9]);
}

#endif // !RC_CAMERA_CPP