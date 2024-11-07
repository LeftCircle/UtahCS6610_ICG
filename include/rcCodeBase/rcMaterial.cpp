#include "rcMaterial.hpp"

using namespace rc;

std::unique_ptr<Material> load_material_file(const std::string& path){
	std::unique_ptr<Material> material = std::make_unique<Material>();
	return material;
}

bool MaterialHolder::has_material() const {
	return material != nullptr;
}