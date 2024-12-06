#ifndef RC_OBJ_MODIFIER_HPP
#define RC_OBJ_MODIFIER_HPP

#include <iostream>
#include "cyCodeBase/cyTriMesh.h"
#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyIVector.h"
#include "rcCodeBase/rcObjToGlFunc.h"
#include <unordered_map>

namespace rc
{

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
	float constant_k[3] = { 1.0f, 1.0f, 1.0f }; // constant k values for the phong shading model

public:
	rcTriMeshForGL() { cyTriMesh(); };
	

	unsigned int get_n_elements() { return n_elements; }
	unsigned int get_vbo_size() { return vbo_size; }
	unsigned int NE() const { return n_elements; }	//!< returns the number of elements
	
	int const & E(int i) const { return elements[i]; }		//!< returns the i^th element
	int& E(int i) { return elements[i]; }		//!< returns the i^th element
	
	cy::Vec3f const& V_vbo(int i) const { return v_vbo[i]; }		//!< returns the i^th vertex
	cy::Vec3f const& VN_vbo(int i) const { return vn_vbo[i]; }	//!< returns the i^th vertex normal
	cy::Vec3f const& VT_vbo(int i) const { return vt_vbo[i]; }	//!< returns the i^th vertex texture

	// getters and setters for k
	float k(int i) { return constant_k[i]; }
	void set_k(cy::Vec3f const& k) { constant_k[0] = k.x; constant_k[1] = k.y; constant_k[2] = k.z; }
	void set_k(const float r, const float g, const float b) { constant_k[0] = r; constant_k[1] = g; constant_k[2] = b; }
	cy::Vec3f get_k_vec3f() { return cy::Vec3f(constant_k[0], constant_k[1], constant_k[2]); }

	// Creates a vbo for vertices, normals, and texture coordinates by just copying all of the data into the vbo
	// and duplicating the data if there are duplicate vertices
	void create_vbo_data_for_draw_arrays()
	{
		int n_faces = NF();
		std::vector<cy::Vec3f> _v_vbo;
		std::vector<cy::Vec3f> _vn_vbo;
		std::vector<cy::Vec3f> _vt_vbo;

		for (int i = 0; i < n_faces; i++)
		{
			// build the face
			unsigned int v[3] = { F(i).v[0], F(i).v[1], F(i).v[2] };
			unsigned int vn[3] = { FN(i).v[0], FN(i).v[1], FN(i).v[2] };
			unsigned int vt[3] = { FT(i).v[0], FT(i).v[1], FT(i).v[2] };
			for (int j = 0; j < 3; j++) {
				_v_vbo.push_back(V(v[j]));
				_vn_vbo.push_back(VN(vn[j]));
				_vt_vbo.push_back(VT(vt[j]));
			}
		}
		// allocate space for the data
		SetVBOSize((unsigned int)_v_vbo.size());
		memcpy(v_vbo, _v_vbo.data(), sizeof(cy::Vec3f) * _v_vbo.size());
		memcpy(vn_vbo, _vn_vbo.data(), sizeof(cy::Vec3f) * _vn_vbo.size());
		memcpy(vt_vbo, _vt_vbo.data(), sizeof(cy::Vec3f) * _vt_vbo.size());
	}

	void obj_to_gl_elements() {
		auto data = transformObjToGL(*this);
		
		SetNumElements((unsigned int)data._elements.size());
		SetVBOSize((unsigned int)data._v_vbo.size());

		memcpy(v_vbo, data._v_vbo.data(), sizeof(cy::Vec3f) * data._v_vbo.size());
		memcpy(vn_vbo, data._vn_vbo.data(), sizeof(cy::Vec3f) * data._vn_vbo.size());
		memcpy(vt_vbo, data._vt_vbo.data(), sizeof(cy::Vec3f) * data._vt_vbo.size());
		memcpy(elements, data._elements.data(), sizeof(int) * data._elements.size());
	}

	void SetNumElements(unsigned int n) { Allocate(n, elements, n_elements); };
	
	// Checks to see if we need to allocate data by checking vbo first. If the size is different then also allocate for vn and vt
	void SetVBOSize(unsigned int n) { if (Allocate(n, v_vbo, vbo_size) == false) return; Allocate(n, vn_vbo); Allocate(n, vt_vbo); };
};

} // namespace rc
#endif // !RC_OBJ_MODIFIER_HPP