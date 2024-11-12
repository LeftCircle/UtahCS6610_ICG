#ifndef OBJ_FOR_GL_HPP
#define OBJ_FOR_GL_HPP

#include <vector>
#include <unordered_map>
#include <string>

#include "rcVector.hpp"
#include "rcMaterial.hpp"

namespace rc
{
	// This holds the index into the array for either vertices, normals, or
	// textures for each face.
	struct ObjFaceIndeces {
		int index[3];

		int& operator[](int i) { return index[i];}
	};

	struct pointIndeces {
		const unsigned int vert_index;
		const unsigned int normal_index;
		const unsigned int texture_index;

		bool operator==(const pointIndeces& other) const {
			return (vert_index == other.vert_index && normal_index ==
			other.normal_index && texture_index == other.texture_index);
		}
	};

} // namespace rc

// Specialize std::hash for pointIndeces
namespace std {
	template <> struct hash<rc::pointIndeces> {
		size_t operator()(const rc::pointIndeces& point) const {
			return (std::hash<unsigned int>()(point.vert_index) ^
					std::hash<unsigned int>()(point.normal_index) ^
					std::hash<unsigned int>()(point.texture_index));
		}
	};
}

namespace rc {

	// This holds the data in the obj file to be converted into a glMesh. 
	class ObjMesh : public MaterialHolder
	{
	public:
		ObjMesh() : MaterialHolder() { group_name = ""; };
		~ObjMesh() {};
		
		std::string group_name;
		std::vector<Vector3f> vertices;
		std::vector<Vector3f> normals;
		std::vector<Vector3f> tex_coords;
		std::vector<ObjFaceIndeces> vertex_faces;
		std::vector<ObjFaceIndeces> normal_faces;
		std::vector<ObjFaceIndeces> tex_coord_faces;

		const int NV() const { return vertices.size(); };
		const int NF() const { return vertex_faces.size(); };

		const ObjFaceIndeces& F(const int i) const { return vertex_faces[i]; };
		const ObjFaceIndeces& FN(const int i) const { return normal_faces[i]; };
		const ObjFaceIndeces& FT(const int i) const { return tex_coord_faces[i]; };

		const Vector3f& V(const int i) const { return vertices[i]; };
		const Vector3f& VN(const int i) const { return normals[i]; };
		const Vector3f& VT(const int i) const { return tex_coords[i]; };

	};


	// Class for a mesh in OpenGL. Each vertex, normal, and position index
	// corresponds to the same vertice in the mesh.

	// Might be better to create an obj to glMesh class that takes an objMesh
	// and converts it to a glMesh. This way we can keep the GlMesh class 
	// simple and not have to worry about conversion functions
	class GLMesh : public MaterialHolder
	{
	private:
		std::vector<Vector3f> _vertices;
		std::vector<Vector3f> _normals;
		std::vector<Vector3f> _tex_coords;
		std::vector<int> _elements;
		Material* _material;
		unsigned int _n_faces;

		void _add_new_vtn_and_element(const Vector3f& v,
									  const Vector3f& n,
									  const Vector3f& t,
									  const int element);

		void _add_vtn_values_or_index(const ObjMesh& mesh,
							std::unordered_map<pointIndeces, int>& locations,
							const pointIndeces& point);

		void _create_faces_and_add_values(const ObjMesh& mesh,
							std::unordered_map<pointIndeces, int>& locations,
							const int i);

	public:
		GLMesh() : MaterialHolder() {};
		~GLMesh() = default;

		void transformObjToGL(const ObjMesh& mesh);

		const std::vector<Vector3f>& getVertices() const { return _vertices; };
		const std::vector<Vector3f>& getNormals() const { return _normals; };
		const std::vector<Vector3f>& getTexCoords() const { return _tex_coords; };
		const std::vector<int>& getElements() const { return _elements; };

		const int NV() const { return _vertices.size(); };
		const int NE() const { return _elements.size(); };
		const int NN() const { return _normals.size(); };
		const int NT() const { return _tex_coords.size(); };
		const unsigned int NF() { return _n_faces; };
		const Vector3f& V(const int i) const { return _vertices[i]; };
		const Vector3f& N(const int i) const { return _normals[i]; };
		const Vector3f& T(const int i) const { return _tex_coords[i]; };
		const int& E(const int i) const { return _elements[i]; };
		
	};
}
#endif // OBJ_FOR_GL_HPP