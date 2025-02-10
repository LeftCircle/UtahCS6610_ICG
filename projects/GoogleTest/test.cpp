#include "pch.h"
#include "gtest/gtest.h"

#include <cyCodeBase/cyTriMesh.h>
#include <rcCodeBase/rcObjModifier.h>

const char* yoda_path = "E:\\Programming\\OpenGL\\Projects\\ICG_Utah\\projects\\GoogleTest\\test_objs\\yoda\\yoda.obj";

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(TestcyTriMesh, TestLoadObjFile) {
	rc::rcTriMeshForGL test_mesh;
	unsigned int expected_n_vertices = 0;
	EXPECT_EQ(expected_n_vertices, test_mesh.NV());
	bool loaded = test_mesh.LoadFromFileObj(yoda_path);
	EXPECT_TRUE(loaded);
}

TEST(TestcyTriMesh, TestMultipleMaterials) {
	cy::TriMesh test_mesh;
	bool loaded = test_mesh.LoadFromFileObj(yoda_path);
	EXPECT_TRUE(loaded);
	unsigned int expected_n_materials = 7;
	EXPECT_EQ(expected_n_materials, test_mesh.NM());

	int NO_MATERIAL = -1;
	int face_0_material = test_mesh.GetMaterialIndex(0);
	EXPECT_EQ(0, face_0_material);

	int m0_face_count = test_mesh.GetMaterialFaceCount(0);
	int m0_first_face = test_mesh.GetMaterialFirstFace(0);
	EXPECT_EQ(0, m0_first_face);
	EXPECT_EQ(27720, m0_face_count);
}

TEST(TestcyTriMesh, TestGetobjMaterials) {
	// Tests getting the materials and images from cyTriMesh. 
	cy::TriMesh test_mesh;
	bool loaded = test_mesh.LoadFromFileObj(yoda_path);
	EXPECT_TRUE(loaded);
	if (loaded) {
		bool found_head = false;
		// Check that we can grab the path to the yoda-head.png 
		unsigned int nm = test_mesh.NM();
		for (int i = 0; i < nm; i++) {
			cy::TriMesh::Mtl mtl = test_mesh.M(i);
			char* mtl_name = mtl.name.data;
			// convert the char*data to a string
			std::string mtl_name_str(mtl_name);
			if (mtl_name_str == "Head") {
				found_head = true;
				// Now see if we can find the image path
				char* image_path = mtl.map_Kd.data;
				std::string image_path_str(image_path);
				std::string expected = "yoda-head.png";
				EXPECT_EQ(expected, image_path_str);
			}
			if (mtl_name_str == "Limbs") {
				// There is no mapo_Ks for this material, so we should know that somehow.
				char* ks_path = mtl.map_Ks.data;
				// Check to see that ks_path is null
				EXPECT_EQ(nullptr, ks_path);
			}
		}
		EXPECT_TRUE(found_head);
	}
}

TEST(TestcyTriMesh, TestBreakUpBuffersByMaterial) {
	rc::rcTriMeshForGL test_mesh;
	bool loaded = test_mesh.LoadFromFileObj(yoda_path);
	EXPECT_TRUE(loaded);
	test_mesh.obj_to_gl_elements();

	// Now we want to try to create an element array with just the first material
	int m0_face_count = test_mesh.GetMaterialFaceCount(0);
	int m0_first_face = test_mesh.GetMaterialFirstFace(0);

	int n_m0_elements = m0_face_count * 3;
	int m0_element_start = m0_first_face * 3;

	EXPECT_EQ(83160, n_m0_elements);
	EXPECT_EQ(0, m0_element_start);

	//int m1_first_face = test_mesh.GetMaterialFirstFace(1);
	//EXPECT_EQ(m1_first_face, test_mesh.GetMaterialIndex(m1_first_face));
	//EXPECT_EQ(m1_first_face, m0_face_count);
	
}