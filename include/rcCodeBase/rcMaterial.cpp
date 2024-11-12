#include "rcMaterial.hpp"

using namespace rc;


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