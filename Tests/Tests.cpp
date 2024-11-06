#include "pch.h"
#include "CppUnitTest.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
// rc codebase includes
#include "rcCodeBase/rcObjModifier.h"
#include "rcCodeBase/rcOpenGLScene.hpp"
#include "rcCodeBase/rcLights.hpp"
#include "rcCodeBase/rcObjToGlFunc.h"
#include "rcCodeBase/objForGL.hpp"
#include "rcCodeBase/ObjLoader.hpp"
#include "rcCodeBase/rcMaterial.hpp"

// cy codebase includes
#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyMatrix.h"
#include <string>
#include <format>
#include <cwchar>
#include <iomanip>
#include <cmath>
#include <filesystem>

//const char* test_file_path = "E:\\Programming\\OpenGL\\Projects\\ICG_Utah\\Tests\\test_objs\\simple_obj.obj";
// macro to get the path that this file is in
std::string cur_folder = std::filesystem::path(__FILE__).parent_path().string() + "\\";

std::string test_file_path = cur_folder + "test_objs\\simple_obj.obj";
std::string yoda_file_path = cur_folder + "test_objs\\yoda\\yoda.obj";
//const char* teapot_file_path = "C:\\Programming\\OpenGL\\UtahCS6610_ICG\\projects\\hw02_utah_teapot\\teapot.obj";

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


bool AreEqualWithTolerance(const cy::Vec3<float>& expected, const cy::Vec3<float>& actual, float tolerance = 1e-5)
{
	return (std::fabs(expected.x - actual.x) < tolerance) &&
		(std::fabs(expected.y - actual.y) < tolerance) &&
		(std::fabs(expected.z - actual.z) < tolerance);
}

// Defining a ToString function for the cy::Vec3f class so that we can use it in the Assert::AreEqual function
namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<> std::wstring ToString<cy::Vec3<float>>(const cy::Vec3<float>& v)
			{
				wchar_t buffer[100];
				swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"%.2f, %.2f, %.2f", v.x, v.y, v.z);
				return std::wstring(buffer);
			}

			// Define a ToString function for the cy::Matrix3f class
            template<> std::wstring ToString<cy::Matrix3<float>>(const cy::Matrix3<float>& m)
            {
                wchar_t buffer[100];
                swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"%.2f, %.2f, %.2f\n%.2f, %.2f, %.2f\n%.2f, %.2f, %.2f",
                    m(0, 0), m(0, 1), m(0, 2), m(1, 0), m(1, 1), m(1, 2), m(2, 0), m(2, 1), m(2, 2));
                return std::wstring(buffer);
            }

			// Define a ToString function for the cy::Matrix4f class
			template<> std::wstring ToString<cy::Matrix4<float>>(const cy::Matrix4<float>& m)
			{
				wchar_t buffer[100];
				swprintf(buffer, sizeof(buffer) / sizeof(wchar_t), L"%.2f, %.2f, %.2f, %.2f\n%.2f, %.2f, %.2f, %.2f\n%.2f, %.2f, %.2f, %.2f\n%.2f, %.2f, %.2f, %.2f",
					m(0, 0), m(0, 1), m(0, 2), m(0, 3), m(1, 0), m(1, 1), m(1, 2), m(1, 3), m(2, 0), m(2, 1), m(2, 2), m(2, 3), m(3, 0), m(3, 1), m(3, 2), m(3, 3));
				return std::wstring(buffer);
			}
		}
	}
}

namespace Tests
{
TEST_CLASS(rcObjectModifierTests)
{
public:
	TEST_METHOD(TESTFileIsLoaded)
	{
		rc::rcTriMeshForGL test_class;
		Assert::IsTrue(test_class.LoadFromFileObj(yoda_file_path.c_str()));
	}

	TEST_METHOD(TESTcreate_vbos_and_element_buffer)
	{
		// Based on the simple_obj.obj file
		const int expected_vbo_size = 8;
		const int expected_element_buffer_size = 9;
		cy::Vec3f expected_vbo_vertices[expected_vbo_size] = {
			cy::Vec3f(0, 0, 0),
			cy::Vec3f(1, 0, 0),
			cy::Vec3f(0, 1, 0),
			cy::Vec3f(10, 0, 0),
			cy::Vec3f(12, 0, 0),
			cy::Vec3f(10, 1, 0),
			cy::Vec3f(0, 0, 0),
			cy::Vec3f(0, -1, 0)
		};

		cy::Vec3f expected_vbo_normals[expected_vbo_size] = {
			cy::Vec3f(0.0, 0.0, 0.0),
			cy::Vec3f(0.1, 0.1, 0.1),
			cy::Vec3f(0.2, 0.2, 0.2),
			cy::Vec3f(0.0, 0.0, 0.0),
			cy::Vec3f(0.0, 0.0, 0.0),
			cy::Vec3f(0.0, 0.0, 0.0),
			cy::Vec3f(0.3, 0.3, 0.3),
			cy::Vec3f(0.2, 0.2, 0.2)
		};

		cy::Vec3f expected_vbo_texcoords[expected_vbo_size] = {
			cy::Vec3f(0.0, 0.0, 0.0),
			cy::Vec3f(0.1, 0.1, 0.1),
			cy::Vec3f(0.2, 0.2, 0.2),
			cy::Vec3f(0.0, 0.0, 0.0),
			cy::Vec3f(0.0, 0.0, 0.0),
			cy::Vec3f(0.0, 0.0, 0.0),
			cy::Vec3f(0.3, 0.3, 0.3),
			cy::Vec3f(0.2, 0.2, 0.2)
		};

		int expected_element_buffer[expected_element_buffer_size] = {
			0, 1, 2,
			3, 4, 5,
			6, 1, 7
		};

		cy::TriMesh test_tri_mesh;
		test_tri_mesh.LoadFromFileObj(test_file_path.c_str());
		//test_tri_mesh.create_vbo_data_and_elements();
		GlElementArrayData element_data = transformObjToGL(test_tri_mesh);
		
		Assert::AreEqual(expected_element_buffer_size, int(element_data._elements.size()));
		Assert::AreEqual(expected_vbo_size, int(element_data._v_vbo.size()));

		//// check that the vbo's and elements match by looping through each element and ensuring they are the same
		cy::Vec3f v_vbo0 = element_data._v_vbo[0];
		cy::Vec3f expected_vbo0 = expected_vbo_vertices[0];
		wchar_t error_msg[100];
		for (int i = 0; i < expected_vbo_size; i++)
		{
			// Vertex Coordinates
			cy::Vec3f a_v = element_data._v_vbo[i];
			cy::Vec3f e_v = expected_vbo_vertices[i];
			swprintf(error_msg, sizeof(error_msg) / sizeof(wchar_t), L"Vertex mismatch at index %d", i);
			Assert::AreEqual(e_v, a_v, error_msg);

			// Normals
			cy::Vec3f a_n = element_data._vn_vbo[i];
			cy::Vec3f e_n = expected_vbo_normals[i];
			swprintf(error_msg, sizeof(error_msg) / sizeof(wchar_t), L"Normal mismatch at index %d", i);
			Assert::AreEqual(e_n, a_n, error_msg);

			// Texture Coordinates
			cy::Vec3f a_t = element_data._vt_vbo[i];
			cy::Vec3f e_t = expected_vbo_texcoords[i];
			swprintf(error_msg, sizeof(error_msg) / sizeof(wchar_t), L"Texture Coordinate mismatch at index %d", i);
			Assert::AreEqual(e_t, a_t, error_msg);
		}
		for (int i = 0; i < expected_element_buffer_size; i++)
		{
			swprintf(error_msg, sizeof(error_msg) / sizeof(wchar_t), L"Element mismatch at index %d. Expected: %d Actual: %d", i, expected_element_buffer[i], element_data._elements[i]);
			Assert::AreEqual(expected_element_buffer[i], element_data._elements[i], error_msg);
		}
	}
}; // class Tests

TEST_CLASS(TESTrcOpenGLScene)
{
public:
	TEST_METHOD(TESTNormalMatrixIsInverseScaleOfPointMV)
	{
		// Check that there is a model-view, model-view-projection, and model-view-normal matrix
		rc::GLScene test_scene;
		// Initialize the camera
		test_scene.camera.set_perspective_projection(DEG2RAD(90), 1.0f, 0.0f, 30.0f);
		cy::Vec3f camera_pos = cy::Vec3f(0.0f, 0.0f, 1200.0f);
		cy::Vec3f up = cy::Vec3f(0.0, 1.0, 0.0);

		cy::Vec3f target = cy::Vec3f(0.0f, 0.0f, 0.0f);
		test_scene.camera.lookat(camera_pos, target, up);
		test_scene.point_transform.SetScale(cy::Vec3f(1.0f, 2.0f, 3.0f));
		test_scene.point_transform.SetRotationX(-PI_OVER_2);

		test_scene.set_mvp();
		cy::Matrix4f& mvp = test_scene.MVP();
		cy::Matrix4f& mv_points = test_scene.MV_points();
		cy::Matrix3f& mv_normals = test_scene.MV_normals();
		bool is_mvp_identity = mvp == cy::Matrix4f::Identity();
		Assert::IsFalse(is_mvp_identity, L"MVP matrix should not be identity");
		Assert::IsFalse(mv_points == cy::Matrix4f::Identity(), L"MV_points matrix should not be identity");
		Assert::IsFalse(mv_normals == cy::Matrix3f::Identity(), L"MV_normals matrix should not be identity");

		cy::Matrix3f expected_normals(mv_points);
		expected_normals.Invert();
		expected_normals.Transpose();

		Assert::AreEqual(expected_normals, mv_normals, L"Normal matrix is not the inverse of the point matrix");
		Assert::AreNotEqual(mv_points, mvp, L"Point matrix is the same as the MVP matrix");
	}

	// Test the normal matrix is the inverse of the point matrix

};

TEST_CLASS(TestLightingClasses)
{
public:
	TEST_METHOD(TESTDirectionalLighting)
	{
		// Test that the directional light has a position, intensity, and direction.
		rc::DirectionalLight light;
		Assert::AreEqual(cy::Vec3f(0.0f, 0.0f, 0.0f), light.position(), L"Directional light position should be (0, 0, 0)");
		Assert::AreEqual(1.0f, light.intensity(), L"Directional light intensity should be 1.0");
		Assert::AreEqual(cy::Vec3f(0.0f, -1.0f, 0.0f), light.direction(), L"Directional light direction should be (0, -1, 0)");
	}

	TEST_METHOD(TestSphericalLighting)
	{
		// Test that the spherical light points in the correct direction after specific rotations. 
		rc::SphericalDirectionalLight light(0, 0);
		Assert::AreEqual(cy::Vec3f(0.0f, 0.0f, -1.0f), light.direction(), L"Directional light direction should be (0, 0, -1)");
		light.add_rotation_and_get_direction(90, 0);
		bool matches = AreEqualWithTolerance(cy::Vec3f(0.0f, 0.0f, -1.0f), light.direction());
		Assert::IsTrue(matches, L"Directional light direction should be (0, 0, -1)");
		light.add_rotation_and_get_direction(-90, 90);
		matches = AreEqualWithTolerance(cy::Vec3f(-1.0f, 0.0f, 0.0f), light.direction());
		Assert::IsTrue(matches, L"Directional light direction should be (-1, 0, 0)");
		light.add_rotation_and_get_direction(0, 90);
		matches = AreEqualWithTolerance(cy::Vec3f(0.0f, 0.0f, 1.0f), light.direction());
		Assert::IsTrue(matches, L"Directional light direction should be (0, 0, 1)");

		light.add_rotation_and_get_direction(0, 90);
		matches = AreEqualWithTolerance(cy::Vec3f(1.0f, 0.0f, 0.0f), light.direction());
		Assert::IsTrue(matches, L"Directional light direction should be (1, 0, 0)");

		// Add a rotation to the light and check that the direction is correct
		light.SetRotation(cy::Matrix4f::RotationX(-PI_OVER_2));
		matches = AreEqualWithTolerance(cy::Vec3f(0.0f, 0.0f, -1.0f), light.direction());
	}
};

TEST_CLASS(TestMultipleMaterialsAndObjectGroups)
{
private:
	const char* yoda_obj_path = "..\\projects\\hw02_utah_teapot\\yoda\\yoda.obj";
public:
	TEST_METHOD(TESTMaterialForDifferentGroups)
	{
		rc::rcTriMeshForGL yoda_mesh;
		bool error = yoda_mesh.LoadFromFileObj(yoda_obj_path);
		unsigned int expected_n_materials = 7;
		Assert::AreEqual(expected_n_materials, yoda_mesh.NM(), L"Yoda mesh should have 7 materials");

	}
};

TEST_CLASS(TESTCustomMaterialReader)
{
private:
	const char* teapot_path = "..\\projects\\hw02_utah_teapot\\teapot.obj";
public:
	TEST_METHOD(TESTMaterialHolderSubclasses)
	{
		rc::ObjMesh mesh = rc::ObjMesh();
		rc::ObjMesh* mesh_b = new rc::ObjMesh();
		// assert that the material is a nullptr
		Assert::IsNull(mesh.get_material_ptr(), L"Material should be nullptr");

		// Create a new material and set it to the mesh
		rc::Material* new_material = new rc::Material();
		mesh.set_material(new_material);
		Assert::IsNotNull(mesh.get_material_ptr(), L"Material should not be nullptr");
		
		// Pass the material to mesh b
		mesh_b->set_material(mesh.release_material());
		Assert::IsNull(mesh.get_material_ptr(), L"Material should be nullptr");
		Assert::IsNotNull(mesh_b->get_material_ptr(), L"Material should not be nullptr");
	}


	TEST_METHOD(TESTMaterialReader)
	{
		ObjLoader obj_loader;
		Assert::IsTrue(obj_loader.loadObjFile(teapot_path), L"Teapot obj file should load");
		std::vector<rc::ObjMesh> meshes = obj_loader.getObjMeshes();
		Assert::AreEqual(1, int(meshes.size()), L"Teapot obj file should have 1 mesh");
		rc::ObjMesh& teapot_mesh = meshes[0];
		/*unsigned int n_materials = teapot_mesh.NM();
		Assert::AreEqual(1, int(n_materials), L"Teapot obj file should have 1 material");
		rc::Material& teapot_material = teapot_mesh.get_material(0);
		std::string expected_material_name = "default";
		Assert::AreEqual(expected_material_name, teapot_material.name, L"Material name should be 'default'");*/


	}
};

} // namespace Tests

