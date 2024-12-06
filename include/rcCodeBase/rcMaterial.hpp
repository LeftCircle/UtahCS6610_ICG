#ifndef RC_MATERIAL_HPP
#define RC_MATERIAL_HPP

#include <string>

namespace rc {
	class Material {
	public:
		Material() {
			name = "";
			ka[0] = ka[1] = ka[2] = 0.0f;
			kd[0] = ka[1] = ka[2] = 0.0f;
			ks[0] = ka[1] = ka[2] = 0.0f;
			ns = 0;
		};
		~Material() {};

		std::string name;
		float ka[3]; // ambient
		float kd[3]; // diffuse
		float ks[3]; // specular
		float ns; // specular exponent

		std::string map_kd; // diffuse texture map
		std::string map_ks; // specular texture map
		std::string map_ka; // ambient texture map
	};

	class MaterialHolder {
	protected:
		// Because we are doing manual memory management, we need to
		// Implement the big 5: 
		// Destructor, Copy Constructor, Copy Assignment Operator,
		// Move Constructor, and Move Assignment Operator
		Material* material;

	public:

		MaterialHolder() { material = nullptr; };
		// Copy Constructor
		MaterialHolder(const MaterialHolder& other);
		~MaterialHolder();

		//// Copy Assignment Operator
		//MaterialHolder& operator=(const MaterialHolder& other);

		//// Move Constructor
		//MaterialHolder(MaterialHolder&& other) noexcept;

		//// Move Assignment Operator
		//MaterialHolder& operator=(MaterialHolder&& other) noexcept;

		void set_material(Material* mat);
		Material* release_material();

		bool has_material() const;
	};
};
#endif