#include "CppUnitTest.h"

#include <cyCodeBase/cyTriMesh.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework
		{

			template<> inline std::wstring ToString<std::wstring>(const std::wstring* t) {
				if (t == nullptr) return L"(NULL)";
				return *t;
			}


			namespace Tests
			{
				TEST_CLASS(cyTriMeshTests)
				{
				private:
					const char* test_file_path = "test_objs\\yoda\\yoda.obj";
				public:
					TEST_METHOD(TESTFileIsLoaded)
					{
						cy::TriMesh test_mesh;
						unsigned int expected_n_vertices = 0;
						//Assert::AreEqual(expected_n_vertices, test_mesh.NV(), L"Mesh should have 0 vertices on init");

						bool loaded = test_mesh.LoadFromFileObj(test_file_path);
						//Assert::IsTrue(loaded, L"File should be loaded");
					};
				};
			};
		}
	}
}