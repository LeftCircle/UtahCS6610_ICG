#ifndef RC_MATERIAL_HPP
#define RC_MATERIAL_HPP

#include <string>
#include <memory>

namespace rc{


class Material{
public:
	Material() { 
		name = "";
		ka[0] = ka[1] = ka[2] = 0.0f;
		kd[0] = ka[1] = ka[2] = 0.0f;
		ks[0] = ka[1] = ka[2] = 0.0f;
		ns = 0;
	};
	~Material() = default;

	std::string name;
	float ka[3]; // ambient
	float kd[3]; // diffuse
	float ks[3]; // specular
	float ns; // specular exponent

	std::string map_kd; // diffuse texture map
	std::string map_ks; // specular texture map
	std::string map_ka; // ambient texture map
};

std::unique_ptr<Material> load_material_file(const std::string& path);

class MaterialHolder{
protected:

public:
	std::unique_ptr<Material> material;
	
	MaterialHolder() = default;
	~MaterialHolder() = default;

	bool has_material() const;
};

}
#endif