#ifndef OBJLOADER_H
#define OBJLOADER_H

// add pch.h if it exists
#ifdef PCH_H
#include "pch.h"
#endif

#include "objForGL.hpp"
#include "rcVector.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

bool is_obj_file(const std::string& path);

size_t split_string(const std::string& line,
 					std::vector<std::string>& tokens,
					char delimiter);

class ObjLoader
{
	// OBJ File format:
	// mtllib [filename].mtl
	// v [x] [y] [z]
	// vt [u] [v]
	// vn [x] [y] [z]
	// f [v1]/[vt1]/[vn1] [v2]/[vt2]/[vn2] [v3]/[vt3]/[vn3]
	// usemtl [material_name]
	// s [number]
	// o [object_name]
	// g [group_name]
	// [comment]
public:
	ObjLoader() {};
	~ObjLoader() {}; // Remove this line

	bool loadObjFile(const std::string& path);

	std::vector<rc::ObjMesh>& getObjMeshes() { return _meshes; }
	const std::vector<rc::ObjMesh>& getObjMeshes() const { return _meshes; }

	void add_face_indexes(rc::ObjMesh& mesh, const std::string& line);

private:
	std::vector<rc::ObjMesh> _meshes;

	void _write_three_token_face_to_mesh(rc::ObjMesh& mesh,
									const std::vector<std::string>& tokens,
									std::vector<std::string>& scratch);

	void _add_faces_to_mesh(rc::ObjMesh& mesh,
	 						const std::vector<std::string>& tokens);

	// Reads the file and adds obj meshes to the _meshes vector.
	void _build_obj_meshes(std::ifstream& file);

	void _add_value_to_v_or_t_or_n(rc::ObjFaceIndeces& v_face,
	 							   rc::ObjFaceIndeces& vt_face,
	 							   rc::ObjFaceIndeces& vn_face,
	 							   const int value,
	 							   const int vtn_case,
	 							   const int face_xyz);

	void _add_face_indexes_from_vtn(rc::ObjFaceIndeces& v_face,
	 								rc::ObjFaceIndeces& vt_face,
	 								rc::ObjFaceIndeces& vn_face,
	 								const std::string& line,
	 								const int face_xyz);

	void _add_faces_to_mesh_with_vtn_strings(rc::ObjMesh& mesh,
											 const std::string& s1,
											 const std::string& s2,
											 const std::string& s3);

};


#endif // OBJLOADER_H