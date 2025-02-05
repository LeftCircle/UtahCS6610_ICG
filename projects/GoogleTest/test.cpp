#include "pch.h"
#include "gtest/gtest.h"

#include <cyCodeBase/cyTriMesh.h>

const char* yoda_path = "E:\\Programming\\OpenGL\\Projects\\ICG_Utah\\projects\\GoogleTest\\test_objs\\yoda.obj";

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(TestcyTriMesh, TestLoadObjFile) {
	cy::TriMesh test_mesh;
	unsigned int expected_n_vertices = 0;
	EXPECT_EQ(expected_n_vertices, test_mesh.NV());
	bool loaded = test_mesh.LoadFromFileObj(yoda_path);
	EXPECT_TRUE(loaded);
}