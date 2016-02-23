/*
	This class represents a full transformation
	(i.e. a 4x4 matrix composed of a 3x3 rotation matrix and a 3 element translation vector)

	Matrix Facts:
		* The float elements are stored as columns
			* In other words, the second float stored is the first element in the second row
			* This is so that all 16 floats can be copied directly to Direct3D or OpenGL
				(Both HLSL and GLSL interpret a matrix's registers to be columns)
		* The vectors are the rows
			* In other words, the first three rows are the rotation and the last row is the translation
			* This is an arbitrary choice, but it is arguably the most common one in computer graphics
				(every other field outside of computer graphics that I know of uses column vectors, though)
			* This matches standard Direct3D but is backwards from standard OpenGL
		* The vectors are right-handed
			* In other words, the first vector is right, the second vector is up, and the third vector is back
			* This is an arbitrary choice; it matches Maya and standard OpenGL, and is backwards from standard Direct3D
*/

#ifndef EAEENGINE_MATH_CMATRIX_TRANSFORMATION_H
#define EAEENGINE_MATH_CMATRIX_TRANSFORMATION_H
#include "Vector.h"
// Forward Declarations
//=====================

namespace EAE_Engine
{
	namespace Math
	{
		class Quaternion;
	}
}

// Class Declaration
//==================

namespace EAE_Engine
{
	namespace Math
	{

		class ColMatrix22
		{
		public:

			inline float GetDeter()//GetDeterminate
			{
				return _m00 * _m11 - _m01 * _m10;
			}

			union
			{
				float _m[4]; //col will be represented at first.
				struct
				{
					float _m00, _m10,// 0, 1,
						_m01, _m11;  // 2, 3,
				};
			};
		};

		class ColMatrix33
		{
		public:
			ColMatrix33() {}
			~ColMatrix33() {}
			ColMatrix33(const ColMatrix33& matrix33)
			{
				for (unsigned int i = 0; i < 9; ++i)
					_m[i] = matrix33._m[i];
			}
			inline float GetDeter()
			{
				float term1 = _m00 * _m11 * _m22;
				float term2 = _m01 * _m12 * _m20;
				float term3 = _m02 * _m10 * _m21;

				float term4 = _m00 * _m12 * _m21;
				float term5 = _m01 * _m10 * _m22;
				float term6 = _m02 * _m11 * _m20;

				return term1 + term2 + term3 - (term4 + term5 + term6);
			}
			inline void SetValue(float m[9])
			{
				for (size_t index = 0; index < 9; ++index)
				{
					_m[index] = m[index];
				}
			}

			union
			{
				float _m[9]; //col will be represented at first.
				struct
				{
					float _m00, _m10, _m20,// 0, 1, 2
						_m01, _m11, _m21,  // 3, 4, 5
						_m02, _m12, _m22;  // 6, 7, 8
				};
				Vector3 _col[3];//the axies 
			};

		};//end of Matrix33

		class ColMatrix44
		{
			// Interface
			//==========

		public:
			ColMatrix44();	// Identity
			ColMatrix44(
				const float i_00, const float i_10, const float i_20, const float i_30,
				const float i_01, const float i_11, const float i_21, const float i_31,
				const float i_02, const float i_12, const float i_22, const float i_32,
				const float i_03, const float i_13, const float i_23, const float i_33);
			ColMatrix44( const Quaternion& i_rotation, const Vector3& i_translation );
			ColMatrix44(const ColMatrix44& i_other);
			ColMatrix44& operator=(const ColMatrix44& i_other);
			ColMatrix44& operator*=(float i_other);
			bool operator==(const ColMatrix44& i_other);
			ColMatrix44 operator*(float i_other) const;
			ColMatrix44& Transpose();
			ColMatrix44 GetTranspose() const;
			ColMatrix44 GetAdjugateMatrix() const;
			bool GetInverse(ColMatrix44& o_outMatrix) const; // Get InverseMatrix
			float GetDeter() const; 			// Get Determinant of the matrix
			// Get the Element of the ColMatrix 
			Vector4 GetCol(size_t index) const;
			Vector4 GetRow(size_t index) const;
			float GetRowCol(size_t row, size_t col) const;
			void SetRowCol(size_t row, size_t col, float value);

			static ColMatrix44 CreateMovementMatrix(const Vector3& i_position);
			static ColMatrix44 CreateRotationMatrix(const Quaternion& i_rotation);
			static ColMatrix44 CreateScaleMatrix(const Vector3& i_scale);

		private:
			ColMatrix33 GetCofactorMatrix(size_t row, size_t col) const;
			static int GetSignOfCofactor(size_t row, size_t col);
		public:
			// Data
			//=====
			static ColMatrix44 Identity;
			union
			{
				struct { 
					// Storage is column-major; see notes at the top of the file
					float _m00, _m10, _m20, _m30, // 0, 1, 2, 3,
						_m01, _m11, _m21, _m31,   // 4, 5, 6, 7,
						_m02, _m12, _m22, _m32,   // 8, 9, 10, 11,
						_m03, _m13, _m23, _m33;   // 12, 13, 14, 15
				};
				Vector4 _u[4];
				float _m[16];
			};
		};

		ColMatrix44 operator*(const ColMatrix44& i_lhs, const ColMatrix44& i_rhs);
		// To assist with the D3D transform, we always use i_vec * i_matrix
		Vector4 operator*(const Vector4& i_vector, const ColMatrix44& i_matrix);
		Vector4 operator*(const ColMatrix44& i_matrix, const Vector4& i_vector);
	}
}

#endif	// EAEENGINE_MATH_CMATRIX_TRANSFORMATION_H
