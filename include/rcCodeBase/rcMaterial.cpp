#include "rcMaterial.hpp"

using namespace rc;

// Copy Constructor
MaterialHolder::MaterialHolder(const MaterialHolder& other){
	if (other.has_material()) {
		material = new Material(*other.material);
	}
}

bool MaterialHolder::has_material() const {
	return material != nullptr;
}

void MaterialHolder::set_material(Material* mat){
	if (material != nullptr) delete material;
	material = mat;
}

Material* MaterialHolder::release_material(){
	Material* mat = material;
	material = nullptr;
	return mat;
}

MaterialHolder::~MaterialHolder(){
	if (material != nullptr) delete material;
	material = nullptr;
}

