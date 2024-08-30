#include <GL/glew.h>
#include <GL/freeglut.h>
#include "cyCodeBase/cyMatrix.h"
#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyGL.h"
#include "cyCodeBase/cyTriMesh.h"


#define DEG2RAD(x) (x * 3.14159265359f / 180.0f)

class Camera
{
private:
	cy::Vec3f position = cy::Vec3f(0.0f, 0.0f, 0.0f);
	cy::Vec3f target = cy::Vec3f(0.0f, 0.0f, 0.0f);
	cy::Vec3f U = cy::Vec3f(1.0f, 0.0f, 0.0f);
	cy::Vec3f V = cy::Vec3f(0.0f, 1.0f, 0.0f);
	cy::Vec3f N = cy::Vec3f(0.0f, 0.0f, 1.0f);
	cy::Matrix4f camera_matrix;
public:
	Camera(cy::Vec3f position, cy::Vec3f target, cy::Vec3f up)
	{
		position = position;
		target = target;
		N = (position - target).GetNormalized();
		U = up.Cross(N).GetNormalized();
		V = N.Cross(U);
		camera_matrix = cy::Matrix4f(
			U.x, U.y, U.z, -position.x,
			V.x, V.y, V.z, -position.y,
			N.x, N.y, N.z, -position.z,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
	void set_position(cy::Vec3f new_pos)
	{
		// Update the target to be shifted by the same amount
		target += new_pos - position;
		position = new_pos;
		// Just have to update the last column of the camera matrix
		camera_matrix.Column(3).Set(-position.x, -position.y, -position.z, 1.0f);
	}

	cy::Matrix4f get_camera_matrix(){return camera_matrix;}
	
	
};

class GLScene
{
public:

	// Parameters
	GLfloat bg[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const char* window_name = "Window Name";
	cy::Matrix4f scale_matrix = cy::Matrix4f::Identity();
	cy::Matrix4f rotation_matrix = cy::Matrix4f::Identity();
	cy::Matrix4f translation_matrix = cy::Matrix4f::Identity();
	cy::Matrix4f projection_matrix = cy::Matrix4f::Identity();
	cy::Matrix4f view_matrix = cy::Matrix4f::Identity();
	cy::GLSLProgram program;
	float FOV = 90.0f;
	float previous_frame_time = 0.0f;
	Camera camera = Camera(cy::Vec3f(0.0f, 0.0f, 0.0f), cy::Vec3f(0.0f, 0.0f, 1.0f), cy::Vec3f(0.0f, 1.0f, 0.0f));

	int n_points = 0;

	// functions:
	cy::Matrix4f set_perspective_matrix(float fov, float aspect_ratio, float nearz, float farz)
	{
		float f = 1.0f / tan(fov * 0.5f * 3.14159265359f / 180.0f);
		float zdiff = nearz - farz;
		projection_matrix = cy::Matrix4f(
			f / aspect_ratio, 0.0f, 0.0f, 0.0f,
			0.0f, f, 0.0f, 0.0f,
			0.0f, 0.0f, (-nearz - farz) / zdiff, 2.0f * farz * nearz / zdiff,
			0.0f, 0.0f, 1.0f, 0.0f
		);
		return projection_matrix;
	}

	void set_mvp_matrix()
	{
		program["mvp"] =  projection_matrix * view_matrix * translation_matrix * rotation_matrix * scale_matrix;
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