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

	};
}; // namespace rc

#endif