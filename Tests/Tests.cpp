#include "pch.h"
#include "CppUnitTest.h"
#include "rcCodeBase/rcObjModifier.h"
#include <string>
#include <format>
#include <cwchar>

const char* test_file_path = "E:\\Programming\\OpenGL\\Projects\\ICG_Utah\\Tests\\test_objs\\simple_obj.obj";

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


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
		Assert::IsTrue(test_class.LoadFromFileObj(test_file_path));
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

		rc::rcTriMeshForGL test_tri_mesh;
		test_tri_mesh.LoadFromFileObj(test_file_path);
		test_tri_mesh.create_vbo_data_and_elements();
		Assert::AreEqual(expected_element_buffer_size, test_tri_mesh.get_n_elements());
		Assert::AreEqual(test_tri_mesh.get_vbo_size(), expected_vbo_size);

		// check that the vbo's and elements match by looping through each element and ensuring they are the same
		cy::Vec3f v_vbo0 = test_tri_mesh.V_vbo(0);
		cy::Vec3f expected_vbo0 = expected_vbo_vertices[0];
		wchar_t error_msg[100];
		for (int i = 0; i < expected_vbo_size; i++)
		{	
			// Vertex Coordinates
			cy::Vec3f a_v = test_tri_mesh.V_vbo(i);
			cy::Vec3f e_v = expected_vbo_vertices[i];
			swprintf(error_msg, sizeof(error_msg) / sizeof(wchar_t), L"Vertex mismatch at index %d", i);
			Assert::AreEqual(e_v, a_v, error_msg);

			// Normals
			cy::Vec3f a_n = test_tri_mesh.VN_vbo(i);
			cy::Vec3f e_n = expected_vbo_normals[i];
			swprintf(error_msg, sizeof(error_msg) / sizeof(wchar_t), L"Normal mismatch at index %d", i);
			Assert::AreEqual(e_n, a_n, error_msg);
			
			// Texture Coordinates
			cy::Vec3f a_t = test_tri_mesh.VT_vbo(i);
			cy::Vec3f e_t = expected_vbo_texcoords[i];
			swprintf(error_msg, sizeof(error_msg) / sizeof(wchar_t), L"Texture Coordinate mismatch at index %d", i);
			Assert::AreEqual(e_t, a_t, error_msg);
		}
		for (int i = 0; i < expected_element_buffer_size; i++)
		{
			swprintf(error_msg, sizeof(error_msg) / sizeof(wchar_t), L"Element mismatch at index %d. Expected: %d Actual: %d", i, expected_element_buffer[i], test_tri_mesh.E(i));
			Assert::AreEqual(expected_element_buffer[i], test_tri_mesh.E(i), error_msg);
		}
	}
}; // class Tests
} // namespace Tests
