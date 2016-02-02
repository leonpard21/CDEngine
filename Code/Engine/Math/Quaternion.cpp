// Header Files
//=============
#include "Quaternion.h"
#include <cassert>
#include <cmath>
#include "ColMatrix.h"

// Static Data Initialization
//===========================

namespace
{
	const float s_epsilon = 1.0e-9f;
}

// Interface
//==========

namespace EAE_Engine 
{
	namespace Math 
	{
		// Concatenation
		Quaternion Quaternion::operator *(const Quaternion& i_rhs) const
		{
			return Quaternion(
				(_w * i_rhs._w) - ((_x * i_rhs._x) + (_y * i_rhs._y) + (_z * i_rhs._z)),
				(_w * i_rhs._x) + (_x * i_rhs._w) + ((_y * i_rhs._z) - (_z * i_rhs._y)),
				(_w * i_rhs._y) + (_y * i_rhs._w) + ((_z * i_rhs._x) - (_x * i_rhs._z)),
				(_w * i_rhs._z) + (_z * i_rhs._w) + ((_x * i_rhs._y) - (_y * i_rhs._x)));
		}

		bool Quaternion::operator ==(const Quaternion& i_rhs) const
		{
			bool b0 = std::fabsf(_w - i_rhs._w) < 0.00001f;
			bool b1 = std::fabsf(_x - i_rhs._x) < 0.00001f;
			bool b2 = std::fabsf(_y - i_rhs._y) < 0.00001f;
			bool b3 = std::fabsf(_z - i_rhs._z) < 0.00001f;
			return b0 && b1 && b2 && b3;
		}

		// Inversion
		void Quaternion::Invert()
		{
			_x = -_x;
			_y = -_y;
			_z = -_z;
		}
		Quaternion Quaternion::CreateInverse() const
		{
			return Quaternion(_w, -_x, -_y, -_z);
		}

		// Normalization
		void Quaternion::Normalize()
		{
			const float length = std::sqrt((_w * _w) + (_x * _x) + (_y * _y) + (_z * _z));
			assert(length > s_epsilon);
			const float length_reciprocal = 1.0f / length;
			_w *= length_reciprocal;
			_x *= length_reciprocal;
			_y *= length_reciprocal;
			_z *= length_reciprocal;
		}

		Quaternion Quaternion::CreateNormalized() const
		{
			const float length = std::sqrt((_w * _w) + (_x * _x) + (_y * _y) + (_z * _z));
			assert(length > s_epsilon);
			const float length_reciprocal = 1.0f / length;
			return Quaternion(_w * length_reciprocal, _x * length_reciprocal, _y * length_reciprocal, _z * length_reciprocal);
		}

		Quaternion Quaternion::Identity = Quaternion(1.f, 0.f, 0.f, 0.f);

		// Initialization / Shut Down
		//---------------------------

		Quaternion::Quaternion() :
			_w(1.0f), _x(0.0f), _y(0.0f), _z(0.0f)
		{
		}

		Quaternion::Quaternion(const float i_angleInRadians, const Vector3& i_axisOfRotation_normalized)
		{
			const float theta_half = i_angleInRadians * 0.5f;
			_w = std::cos(theta_half);
			const float sin_theta_half = std::sin(theta_half);
			_x = i_axisOfRotation_normalized._x * sin_theta_half;
			_y = i_axisOfRotation_normalized._y * sin_theta_half;
			_z = i_axisOfRotation_normalized._z * sin_theta_half;
		}

		Quaternion::Quaternion(const Quaternion& i_other) :
			_w(i_other._w), _x(i_other._x), _y(i_other._y), _z(i_other._z)
		{
		
		}

		// Implementation
		//===============

		// Initialization / Shut Down
		//---------------------------
		Quaternion::Quaternion(const float i_w, const float i_x, const float i_y, const float i_z) :
			_w(i_w), _x(i_x), _y(i_y), _z(i_z)
		{

		}

		ColMatrix44 Quaternion::CreateColMatrix(const Quaternion& i_rotation)
		{
			ColMatrix44 result = ColMatrix44::Identity;
			const float _2x = i_rotation._x + i_rotation._x;
			const float _2y = i_rotation._y + i_rotation._y;
			const float _2z = i_rotation._z + i_rotation._z;
			const float _2xx = i_rotation._x * _2x;
			const float _2xy = _2x * i_rotation._y;
			const float _2xz = _2x * i_rotation._z;
			const float _2xw = _2x * i_rotation._w;
			const float _2yy = _2y * i_rotation._y;
			const float _2yz = _2y * i_rotation._z;
			const float _2yw = _2y * i_rotation._w;
			const float _2zz = _2z * i_rotation._z;
			const float _2zw = _2z * i_rotation._w;
			result._m00 = 1.0f - _2yy - _2zz;
			result._m10 = _2xy + _2zw;
			result._m20 = _2xz - _2yw;
			result._m01 = _2xy - _2zw;
			result._m11 = 1.0f - _2xx - _2zz;
			result._m21 = _2yz + _2xw;
			result._m02 = _2xz + _2yw;
			result._m12 = _2yz - _2xw;
			result._m22 = 1.0f - _2xx - _2yy;
			return result;
		}

		// Products
		float Dot(const Quaternion& i_lhs, const Quaternion& i_rhs)
		{
			return (i_lhs._w * i_rhs._w) + (i_lhs._x * i_rhs._x) + (i_lhs._y * i_rhs._y) + (i_lhs._z * i_rhs._z);
		}

	}
}


