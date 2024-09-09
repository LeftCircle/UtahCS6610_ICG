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

class rcTriMeshForGL : public cyTriMesh
{
private:
	// The allocate functions from cyTriMesh that are private
	template <class T> void Allocate(unsigned int n, T*& t) { if (t) delete[] t; if (n > 0) t = new T[n]; else t = nullptr; }
	template <class T> bool Allocate(unsigned int n, T*& t, unsigned int& nt) { if (n == nt) return false; nt = n; Allocate(n, t); return true; }

protected:
	cy::Vec3f* v_vbo; // vertexes arranged for a VBO to work with an element array buffer
	cy::Vec3f* vn_vbo; // normals arranged for a VBO to work with an element array buffer
	cy::Vec3f* vt_vbo; // texture coordinates arranged for a VBO to work with an element array buffer
	int* elements; // elements arranged for a VBO to work with an element array buffer
	unsigned int n_elements;
	unsigned int vbo_size;

public:
	rcTriMeshForGL() { cyTriMesh(); };

	void create_vbo_data_and_elements()
	{
		// Order the vertices, normals, and textures so that each element is in the same index as the element array buffer
		// Start by creating temprorary std::vector to store the data then memcopy into the class member variables
		int n_faces = NF();
		std::vector<cy::Vec3f> _v_vbo;
		std::vector<cy::Vec3f> _vn_vbo;
		std::vector<cy::Vec3f> _vt_vbo;
		std::vector<int> _elements;

		for (int i = 0; i < n_faces; i++)
		{
			// build the face
			unsigned int v[3] = { F(i).v[0], F(i).v[1], F(i).v[2] };
			unsigned int vn[3] = { FN(i).v[0], FN(i).v[1], FN(i).v[2] };
			unsigned int vt[3] = { FT(i).v[0], FT(i).v[1], FT(i).v[2] };
			for (int j = 0; j < 3; j++) {
				// Just do it the dumb and slow way
				bool duplicate = false;
				for (int k = 0; k < _v_vbo.size(); k++)
				{
					cy::Vec3f vertex = V(v[j]);
					cy::Vec3f normal = VN(vn[j]);
					cy::Vec3f texture = VT(vt[j]);
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
					_v_vbo.push_back(V(v[j]));
					_vn_vbo.push_back(VN(vn[j]));
					_vt_vbo.push_back(VT(vt[j]));
					_elements.push_back(_v_vbo.size() - 1);
				}
			}
		}

		// Allocate data and set sizes
		SetNumElements((unsigned int)_elements.size());
		SetVBOSize((unsigned int)_v_vbo.size());
		// Assert n_elements is a multiple of 3
		if (n_elements % 3 != 0)
		{
			std::cerr << "Number of elements is not a multiple of 3" << std::endl;
		}
		// Now memcopy the data into the class member variables
		memcpy(v_vbo, _v_vbo.data(), sizeof(cy::Vec3f) * _v_vbo.size());
		memcpy(vn_vbo, _vn_vbo.data(), sizeof(cy::Vec3f) * _vn_vbo.size());
		memcpy(vt_vbo, _vt_vbo.data(), sizeof(cy::Vec3f) * _vt_vbo.size());
		memcpy(elements, _elements.data(), sizeof(int) * _elements.size());
	}

	void SetNumElements(unsigned int n) { Allocate(n, elements, n_elements); };
	// Checks to see if we need to allocate data by checking vbo first. If the size is different then also allocate for vn and vt
	void SetVBOSize(unsigned int n) { if (Allocate(n, v_vbo, vbo_size) == false) return; Allocate(n, vn_vbo); Allocate(n, vt_vbo); };

	int get_n_elements() { return n_elements; }
	int get_vbo_size() { return vbo_size; }
	cy::Vec3f const &  V_vbo(int i) const { return v_vbo[i]; }		//!< returns the i^th vertex
	cy::Vec3f const &  VN_vbo(int i) const { return vn_vbo[i]; }	//!< returns the i^th vertex normal
	cy::Vec3f const &  VT_vbo(int i) const { return vt_vbo[i]; }	//!< returns the i^th vertex texture
	int const &  E(int i) const { return elements[i]; }		//!< returns the i^th elemen
};

} // namespace rc
#endif // !RC_OBJ_MODIFIER_HPP