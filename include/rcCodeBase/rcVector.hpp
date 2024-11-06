#ifndef RC_VECTOR_HPP
#define RC_VECTOR_HPP

#include <cmath>

namespace rc
{

template <typename T>
class Vector3
{
public:
	T x, y, z;

	Vector3() : x(0), y(0), z(0) {};
	Vector3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {};

};


#define Vector3f Vector3<float>
#define Vector3d Vector3<double>
} // namespace rc


#endif // RC_VECTOR_HPP