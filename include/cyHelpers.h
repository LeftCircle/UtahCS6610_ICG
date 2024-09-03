#include <GL/glew.h>
#include <GL/freeglut.h>
#include "cyCodeBase/cyMatrix.h"
#include "cyCodeBase/cyIVector.h"
#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyGL.h"
#include "cyCodeBase/cyTriMesh.h"
#include <iostream>


#define DEG2RAD(x) (x * 3.14159265359f / 180.0f)
#define RAD2DEG(x) (x * 180.0f / 3.14159265359f)
#define PI_OVER_2 1.57079632679f
#define PI 3.14159265359f

void print_vec3f(cy::Vec3f v)
{
	std::cout << " " << v.x << " " << v.y << " " << v.z << " " << std::endl;
}

void print_vec4f(cy::Vec4f v)
{
	std::cout << " " << v.x << " " << v.y << " " << v.z << " " << v.w << " " << std::endl;
}


// A camera class for the lookat function that contains the position and target of the camera
class Camera
{
private:
	cy::Vec4f _og_position;
	cy::Vec4f _og_up;
public:
	cy::Vec3f position;
	cy::Vec3f target;
	cy::Vec3f up;
	cy::Matrix4f view_matrix;
	float radius = 0.0f;
	float theta = 0.0f;
	float phi = 0.0f;

	Camera()
	{
		position = cy::Vec3f(0.0f, 0.0f, 60.0f);
		target = cy::Vec3f(0.0f, 0.0f, 0.0f);
		up = cy::Vec3f(0.0f, 1.0f, 0.0f);
		_og_position.Set(position.x, position.y, position.z, 1.0);
		_og_up.Set(up.x, up.y, up.z, 1.0);
		view_matrix = cy::Matrix4f::View(position, target, up);
		theta = 0.0f, phi = 0.0f;
	}

	cy::Matrix4f lookat(cy::Vec3f& pos, cy::Vec3f& targ_v, cy::Vec3f& up_v)
	{
		view_matrix.SetView(pos, targ_v, up_v);
		position = pos, target = targ_v;
		up.Set(view_matrix.cell[1], view_matrix.cell[5], view_matrix.cell[9]);
		
		_set_spherical_from_position();
		print_info();
		return view_matrix;
	}

	void print_info()
	{
		std::cout << "--------------------------------------------------------------------------------" << std::endl;
		std::cout << "Position: " << position.x << " " << position.y << " " << position.z << std::endl;
		std::cout << "Up:       " << up.x << " " << up.y << " " << up.z << std::endl;
		std::cout << "Theta:    " << theta << " Phi: " << phi << std::endl;
		std::cout << "--------------------------------------------------------------------------------" << std::endl;
	}

	void rotate(float t, float p)
	{
		// This seems almost correct, but we can't always just rotate around the x and y axis. Once we rotate 90 degrees about Y, we need
		// to rotate about the z axis instead of the x axis. 

		// Maybe rotate the new up about the old view matrix tangent vector and rotate the position around the old up vector?
		// Also need to feed in the dx then. 
		cy::Vec3f old_up = cy::Vec3f(view_matrix.cell[1], view_matrix.cell[5], view_matrix.cell[9]);
		cy::Vec3f old_tangent = cy::Vec3f(view_matrix.cell[0], view_matrix.cell[4], view_matrix.cell[8]);
		//old_tangent.Normalize();
		
		cy::Matrix4f rot_p_general = cy::Matrix4f::Rotation(old_tangent, DEG2RAD(p));
		cy::Matrix4f rot_t_general = cy::Matrix4f::Rotation(old_up, DEG2RAD(t));
		_og_up.Set(old_up.x, old_up.y, old_up.z, 1.0);
		_og_position.Set(position.x, position.y, position.z, 1.0);

		cy::Vec4f rotated_up_gen = rot_p_general * _og_up;
		cy::Vec4f rotated_position_gen = rot_t_general * rot_p_general * _og_position;
		position.Set(rotated_position_gen.x, rotated_position_gen.y, rotated_position_gen.z);
		up.Set(rotated_up_gen.x, rotated_up_gen.y, rotated_up_gen.z);
		view_matrix.SetView(position, target, up);

		//theta = t, phi = p;
		//// Rotate the OG position and up about the x axis by p and  then the position about y axis by t
		//cy::Matrix4f rot_p = cy::Matrix4f::RotationX(DEG2RAD(p));
		//cy::Matrix4f rot_t = cy::Matrix4f::RotationY(DEG2RAD(t));
		//cy::Vec4f rotated_up = rot_p * _og_up;
		//cy::Vec4f rotated_position = rot_t * rot_p * _og_position;
		//position.Set(rotated_position.x, rotated_position.y, rotated_position.z);
		//up.Set(rotated_up.x, rotated_up.y, rotated_up.z);
		//view_matrix.SetView(position, target, up);
	}
	
	void set_spherical_position(float r, float t, float p)
	{
		// phi is the angle from the z axis to the x axis?
		// theta is the angle from the x axis
		radius = r;
		theta = t, phi = p;
		float t_rad = DEG2RAD(t);
		float p_rad = DEG2RAD(p);
		//p_rad = PI_OVER_2;
		cy::Vec3f old_position = position;
		position.Set(r * sin(p_rad) * cos(t_rad), r * sin(p_rad) * sin(t_rad), r * cos(p_rad));
		//position.y = r * sin(t_rad);
		//std::cout << "Position = " << std::endl;
		
		//print_vec3f(up);
		//std::cout << "Phi = " << phi << " Position: " << position.x << " " << position.y << " " << position.z << std::endl;
		
		cy::Vec4f up_v = cy::Vec4f(0.0, 1.0, 0.0, 0.0);
		//up_v.Set(1.0, p_rad, sin(p_rad) * t_rad);
		//up_v = cy::Matrix4f::RotationX(p_rad) * cy::Matrix4f::RotationY(t_rad) * up_v;
		

		// This one was the most stable yet
		//up_matrix = cy::Matrix4f::RotationZ(p_rad) * cy::Matrix4f::RotationX(PI_OVER_2 - t_rad) * up_matrix;

		//up_matrix = cy::Matrix4f::RotationX(p_rad) * cy::Matrix4f::RotationZ(PI_OVER_2 - t_rad) * up_matrix;
		//up_v = cy::Matrix4f::RotationZ(p_rad) * cy::Matrix4f::RotationX(t_rad) * up_v;
		//print_vec4f(up_v);

		//up.Set(up_matrix.cell[1], up_matrix.cell[5], up_matrix.cell[9]);
		up.Set(up_v.x, up_v.y, up_v.z);
		//print_info();

		view_matrix.SetView(position, target, up);
		//up.Set(view_matrix.cell[0], view_matrix.cell[4], view_matrix.cell[8]);
	}

private:
	void _set_spherical_from_position() {
		radius = position.Length();
		theta = RAD2DEG(atan2(position.y, position.x));
		std::cout << "Theta: " << theta << std::endl;
		phi = RAD2DEG(acos(position.z / radius));
		std::cout << "Phi: " << phi << std::endl;
	}
};


class GLScene
{
public:

	// Parameters
	GLfloat bg[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const char* window_name = "Window Name";
	cy::Matrix4f projection_matrix = cy::Matrix4f::Identity();
	cy::GLSLProgram program;
	float FOV = 90.0f;
	float previous_frame_time = 0.0f;
	Camera camera;
	cy::IVec2i mouse_position = cy::IVec2i(0, 0);

	int n_points = 0;

	void set_mvp()
	{
		program["mvp"] = projection_matrix * camera.view_matrix;
	}

};


class cyMeshExtension : public cy::TriMesh
{
public:
	static cy::Vec3f mesh_com_from_vertices(cy::TriMesh& mesh)
	{
		cy::Vec3f com(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < mesh.NV(); i++)
		{
			com += mesh.V(i);
		}
		com /= mesh.NV();
		return com;
	}

	static void center_mesh(cy::TriMesh& mesh)
	{
		cy::Vec3f com = mesh_com_from_vertices(mesh);
		for (int i = 0; i < mesh.NV(); i++)
		{
			mesh.V(i) -= com;
		}
	}
};

