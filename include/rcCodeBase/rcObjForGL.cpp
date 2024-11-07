#include "objForGL.hpp"

using namespace rc;

void GLMesh::_add_new_vtn_and_element(const Vector3f& v,
									  const Vector3f& n,
									  const Vector3f& t,
									  const int element){
	_vertices.push_back(v);
	_normals.push_back(n);
	_tex_coords.push_back(t);
	_elements.push_back(element);
};

void GLMesh::_add_vtn_values_or_index(const ObjMesh& mesh,
							std::unordered_map<pointIndeces, int>& locations,
							const pointIndeces& point){
	// check if the hash exists and if not add it
	size_t new_index = _vertices.size();
	std::pair<std::unordered_map<pointIndeces, int>::iterator, bool> result =
		locations.emplace(point, new_index);

	if (result.second) {
		const Vector3f& v = mesh.V(point.vert_index - 1);
		const Vector3f& n = mesh.VN(point.normal_index - 1);
		const Vector3f& t = mesh.VT(point.texture_index - 1);
		_add_new_vtn_and_element(v, n, t, new_index);
	}
	else {
		int existingElementIndex = result.first->second;
		_elements.push_back(existingElementIndex);
	}
};

void GLMesh::_create_faces_and_add_values(const ObjMesh& mesh,
							std::unordered_map<pointIndeces, int>& locations,
							const int i){
	ObjFaceIndeces face = mesh.F(i);
	ObjFaceIndeces normal_face = mesh.FN(i);
	ObjFaceIndeces texture_face = mesh.FT(i);
	for (int j = 0; j < 3; j++) {
		pointIndeces point = { 
			face.index[j],
			normal_face.index[j],
			texture_face.index[j]
		};
		_add_vtn_values_or_index(mesh, locations, point);
	}
};

void GLMesh::transformObjToGL(const ObjMesh& mesh) {
	std::unordered_map<pointIndeces, int> locations;
	locations.reserve(3 * mesh.NV());

	_n_faces = mesh.NF();

	for (int i = 0; i < _n_faces; ++i) {
		_create_faces_and_add_values(mesh, locations, i);
	}
};
