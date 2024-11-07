#include "rcMaterial.hpp"

using namespace rc;

std::unique_ptr<Material> load_material_file(const std::string& path){
	std::unique_ptr<Material> material = std::make_unique<Material>();
}


void MaterialHolder::set_material(std::unique_ptr<Material> material){
	_material = std::move(material);
}

std::unique_ptr<Material> MaterialHolder::release_material(){
	return std::move(_material);
}

const std::unique_ptr<Material>& MaterialHolder::get_material_ptr() const {
	return _material;
}

bool MaterialHolder::has_material() const {
	return _material != nullptr;
}