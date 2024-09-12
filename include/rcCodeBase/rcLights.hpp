#ifndef RC_LIGHTS_HPP
#define RC_LIGHTS_HPP

#include "cyCodeBase/cyVector.h"
#include "cyCodeBase/cyMatrix.h"
#include "rcCodeBase/rcCore.hpp"


namespace rc
{

	class Light
	{
	protected:
		cy::Vec3f _position = cy::Vec3f(0, 0, 0);
		cy::Vec3f _color = cy::Vec3f(1, 1, 1);
		float _intensity = 1.0f;

	public:
		Light() {};
		Light(cy::Vec3f position, cy::Vec3f color, float intensity)
		{
			_position = position;
			_color = color;
			_intensity = intensity;
		}
		~Light() {};

		cy::Vec3f const& position() const { return _position; }
		cy::Vec3f& position() { return _position; }
		cy::Vec3f const& color() const { return _color; }
		cy::Vec3f& color() { return _color; }
		float const& intensity() const { return _intensity; }
		float& intensity() { return _intensity; }
	};

	class DirectionalLight : public Light
	{
	protected:
		cy::Vec3f _direction = cy::Vec3f(0, -1, 0);
		cy::Vec3f _ambient_intensity= cy::Vec3f(0.1f, 0.1f, 0.1f);
		cy::Vec3f _diffuse_intensity= cy::Vec3f(0.5f, 0.5f, 0.5f);
		cy::Vec3f _specular_intensity= cy::Vec3f(0.5f, 0.5f, 0.5f);

	public:
		DirectionalLight() {};
		DirectionalLight(cy::Vec3f position, cy::Vec3f color, float intensity, cy::Vec3f direction) : Light(position, color, intensity)
		{
			_direction = direction;
		}
		~DirectionalLight() {};

		void set_direction(cy::Vec3f direction) { _direction = direction; }
		cy::Vec3f const& direction() const { return _direction; }
		cy::Vec3f& direction() { return _direction; }
		
		// Intensities
		void set_ambient_intensity(cy::Vec3f const& intensity) { _ambient_intensity = intensity; }
		void set_diffuse_intensity(cy::Vec3f const& intensity) { _diffuse_intensity = intensity; }
		void set_specular_intensity(cy::Vec3f const& intensity) { _specular_intensity = intensity; }
		cy::Vec3f const& ambient_intensity() const { return _ambient_intensity; }
		cy::Vec3f const& diffuse_intensity() const { return _diffuse_intensity; }
		cy::Vec3f const& specular_intensity() const { return _specular_intensity; }
		cy::Vec3f& ambient_intensity() { return _ambient_intensity; }
		cy::Vec3f& diffuse_intensity() { return _diffuse_intensity; }
		cy::Vec3f& specular_intensity() { return _specular_intensity; }
		
		void rotate_direction(float theta, float phi)
		{
			cy::Matrix4f rot_p_general = cy::Matrix4f::Rotation(cy::Vec3f(1, 0, 0), DEG2RAD(phi));
			cy::Matrix4f rot_t_general = cy::Matrix4f::Rotation(cy::Vec3f(0, 1, 0), DEG2RAD(theta));
			_direction = cy::Vec3f(rot_t_general * rot_p_general * _direction);
		}
	};
}; // namespace rc

#endif