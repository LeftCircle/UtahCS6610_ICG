#ifndef RC_MATERIAL_HPP
#define RC_MATERIAL_HPP

#include <string>
#include <memory>

namespace rc{


struct Material{
	Material() { 
		name = "";
		ka[0] = ka[1] = ka[2] = 0.0f;
		kd[0] = ka[1] = ka[2] = 0.0f;
		ks[0] = ka[1] = ka[2] = 0.0f;
		ns = 0;
	};

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
	std::unique_ptr<Material> _material;

public:
	MaterialHolder() { _material = nullptr; };
	~MaterialHolder() = default;

	void set_material(std::unique_ptr<Material> material);

	std::unique_ptr<Material> release_material();

	const Material& get_material() const { return *_material; };
	const std::unique_ptr<Material>& get_material_ptr() const;
	bool has_material() const;
};

}
#endif