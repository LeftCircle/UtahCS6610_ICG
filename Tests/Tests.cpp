#include "pch.h"
#include "CppUnitTest.h"
#include "rcCodeBase/rcObjModifier.h"
#include <string>
#include <format>

const char* test_file_path = "E:\\Programming\\OpenGL\\Projects\\ICG_Utah\\Tests\\test_objs\\simple_obj.obj";

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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
			cy::Vec3f(-1, 0, 0)
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
		for (int i = 0; i < expected_vbo_size; i++)
		{
			
			bool vertex_match = test_tri_mesh.V_vbo(i) == expected_vbo_vertices[i];
			bool normal_match = test_tri_mesh.VN_vbo(i) == expected_vbo_normals[i];
			bool texcoord_match = test_tri_mesh.VT_vbo(i) == expected_vbo_texcoords[i];
			if (!vertex_match)
			{
				std::string vertex_msg = std::format("Vertex mismatch at index {}", i);
				std::string actual_vertex = std::format("Actual: {} {} {}", test_tri_mesh.V_vbo(i).x, test_tri_mesh.V_vbo(i).y, test_tri_mesh.V_vbo(i).z);
				std::string expected_vertex = std::format("Expected: {} {} {}", expected_vbo_vertices[i].x, expected_vbo_vertices[i].y, expected_vbo_vertices[i].z);
				Logger::WriteMessage(vertex_msg.c_str());
				Logger::WriteMessage(actual_vertex.c_str());
				Logger::WriteMessage(expected_vertex.c_str());
			}
			Assert::IsTrue(vertex_match);
			Assert::IsTrue(normal_match);
			Assert::IsTrue(texcoord_match);
		}
		for (int i = 0; i < expected_element_buffer_size; i++)
		{
			Assert::IsTrue(test_tri_mesh.E(i) == expected_element_buffer[i]);
		}
	}
}; // class Tests
} // namespace Tests
