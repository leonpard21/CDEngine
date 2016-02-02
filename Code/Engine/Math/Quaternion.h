/*
	This class represents a rotation
*/
#ifndef EAEENGINE_MATH_CQUATERNION_H
#define EAEENGINE_MATH_CQUATERNION_H

#include "Vector.h"

// Class Declaration
//==================

namespace EAE_Engine
{
	namespace Math
	{
		class ColMatrix44;
		class Quaternion
		{
		public:
			// Concatenation
			Quaternion operator *( const Quaternion& i_rhs ) const;
			bool operator ==(const Quaternion& i_rhs) const;
			// Inversion
			void Invert();
			Quaternion CreateInverse() const;
			// Normalization
			void Normalize();
			Quaternion CreateNormalized() const;

			// Initialization / Shut Down
			//---------------------------
			Quaternion();	// Identity
			Quaternion( const float i_angleInRadians, const Vector3& i_axisOfRotation_normalized );
			Quaternion(const Quaternion& i_other);
			
			// Static member functions
			// Create Matrix for a quaternion
			static ColMatrix44 CreateColMatrix(const Quaternion& i_rotation);

			// Data
			//=====
			static Quaternion Identity;
		private:
			union
			{
				struct { float _w; float _x; float _y; float _z; };
				float _u[4];
			};

		private:
			Quaternion( const float i_w, const float i_x, const float i_y, const float i_z );
			// Friend Classes
			//===============
			friend class ColMatrix44;
			// Products
			friend float Dot(const Quaternion& i_lhs, const Quaternion& i_rhs);
		};

		// Products
		float Dot(const Quaternion& i_lhs, const Quaternion& i_rhs);
	}
}

#endif	// EAEENGINE_MATH_CQUATERNION_H