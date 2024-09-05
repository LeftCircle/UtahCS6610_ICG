#ifndef RC_OBJ_MODIFIER_HPP
#define RC_OBJ_MODIFIER_HPP

#include <iostream>
#include "cyCodeBase/cyTriMesh.h"
#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyIVector.h"
#include <unordered_map>

namespace rc
{

// Contains the indeces of the vertices, normals, and textures that make up a face
struct n_and_t{
	unsigned int n;
	unsigned int t;

	bool operator==(const n_and_t& other) const
	{
		return (n == other.n && t == other.t);
	}
};

struct obj_vertex_data {
	unsigned int v; // vertex index
	n_and_t nt; // normal and texture index

	obj_vertex_data(unsigned int v, unsigned int n, unsigned int t)
	{
		this->v = v;
		nt.n = n;
		nt.t = t;
	}

	// override the == operator to check if the face matches another face
	bool operator==(const obj_vertex_data& other) const
	{
		return (v == other.v && nt == other.nt);
	}

	bool vertex_match(const obj_vertex_data& other) const
	{
		return (v == other.v);
	}

	bool n_t_match(const obj_vertex_data& other) const
	{
		return (nt == other.nt);
	}
};
	//unsigned int v[3]; // vertex indeces
	//unsigned int vn[3]; // normal indeces
	//unsigned int vt[3]; // texture indeces

	//// override the == operator to check if the face matches another face
	//bool operator==(const obj_face& other) const
	//{
	//	return (v[0] == other.v[0] && v[1] == other.v[1] && v[2] == other.v[2] &&
	//		vn[0] == other.vn[0] && vn[1] == other.vn[1] && vn[2] == other.vn[2] &&
	//		vt[0] == other.vt[0] && vt[1] == other.vt[1] && vt[2] == other.vt[2]);
	//}

	//bool vertex_match(const obj_face& other) const
	//{
	//	return (v[0] == other.v[0] && v[1] == other.v[1] && v[2] == other.v[2]);
	//}

	//bool normal_match(const obj_face& other) const
	//{
	//	return (vn[0] == other.vn[0] && vn[1] == other.vn[1] && vn[2] == other.vn[2]);
	//}

	//bool texture_match(const obj_face& other) const
	//{
	//	return (vt[0] == other.vt[0] && vt[1] == other.vt[1] && vt[2] == other.vt[2]);
	//}

	//template <typename T>
	//void set_v(T other_face)
	//{
	//	v[0] = other_face.v[0];
	//	v[1] = other_face.v[1];
	//	v[2] = other_face.v[2];
	//}
	//template <typename T>
	//void set_vn(T other_face)
	//{
	//	vn[0] = other_face.vn[0];
	//	vn[1] = other_face.vn[1];
	//	vn[2] = other_face.vn[2];
	//}

	//template <typename T>
	//void set_vt(T other_face)
	//{
	//	vt[0] = other_face.vt[0];
	//	vt[1] = other_face.vt[1];
	//	vt[2] = other_face.vt[2];
	//}
//};

class rcTriMeshForGL : public cyTriMesh
{
protected:
	cy::Vec3f* v_vbo; // vertexes arranged for a VBO to work with an element array buffer
	cy::Vec3f* vn_vbo; // normals arranged for a VBO to work with an element array buffer
	cy::Vec3f* vt_vbo; // texture coordinates arranged for a VBO to work with an element array buffer
	cy::Vec3f* elements; // elements arranged for a VBO to work with an element array buffer

	//NOTE -> do we have to memcopy the data?

public:
	rcTriMeshForGL(cy::TriMesh mesh)
	{
		// Order the vertices, normals, and textures so that each element is in the same index as the element array buffer
		// Start by creating temprorary std::vector to store the data then memcopy into the class member variables

		// assdert that v_vbo, vn_vbo, and vt_vbo all have the same length. 
		// assert that the number of elements is a multiple of 3

		// Add vertices, normals, and textures to the vbo as they are called in the faces?
		int n_faces = mesh.NF();
		//std::unordered_map<unsigned int, std::vector<n_and_t>> vertex_to_face;
		// Create a map of vertex to obj_face
		std::vector<cy::Vec3f> _v_vbo;
		std::vector<cy::Vec3f> _vn_vbo;
		std::vector<cy::Vec3f> _vt_vbo;
		std::vector<int> _elements;

		for (int i = 0; i < n_faces; i++)
		{
			// build the face
			unsigned int v[3] = { mesh.F(i).v[0], mesh.F(i).v[1], mesh.F(i).v[2] };
			unsigned int vn[3] = { mesh.FN(i).v[0], mesh.FN(i).v[1], mesh.FN(i).v[2] };
			unsigned int vt[3] = { mesh.FT(i).v[0], mesh.FT(i).v[1], mesh.FT(i).v[2] };
			for (int j = 0; j < 3; j++) {
				// Just do it the dumb and slow way
				bool duplicate = false;
				for (int k = 0; k < _v_vbo.size(); k++)
				{
					cy::Vec3f vertex = mesh.V(v[j]);
					cy::Vec3f normal = mesh.VN(vn[j]);
					cy::Vec3f texture = mesh.VT(vt[j]);
					if (_v_vbo[k] == vertex && _vn_vbo[k] == normal && _vt_vbo[k] == texture)
					{
						// We have found a duplicate vertex. Add the index to the elements array
						_elements.push_back(k);
						duplicate = true;
						break;
					}
				}
				if (!duplicate)
				{
					_v_vbo.push_back(mesh.V(v[j]));
					_vn_vbo.push_back(mesh.VN(vn[j]));
					_vt_vbo.push_back(mesh.VT(vt[j]));
					_elements.push_back(_v_vbo.size() - 1);
				}
			}
		}

		// Now memcopy the data into the class member variables
		memcpy(v_vbo, _v_vbo.data(), sizeof(cy::Vec3f) * _v_vbo.size());
		memcpy(vn_vbo, _vn_vbo.data(), sizeof(cy::Vec3f) * _vn_vbo.size());
		memcpy(vt_vbo, _vt_vbo.data(), sizeof(cy::Vec3f) * _vt_vbo.size());
	}

};

} // namespace rc
#endif // !RC_OBJ_MODIFIER_HPP