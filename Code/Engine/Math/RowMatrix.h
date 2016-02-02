#ifndef MATRIX_H
#define MATRIX_H
#include "Vector.h"
#include <assert.h>
#include <cmath>
#include "General/MemoryOp.h"

namespace EAE_Engine 
{
	namespace Math
	{	
		class RowMatrix22
		{
		public:
			
			inline float GetDeter()//GetDeterminate
			{
				return _m00 * _m11 - _m01 * _m10;
			}

			union
			{
				float _m[4]; //row will be represented at first.
				struct
				{
					float _m00, _m01,// 0, 1,
						_m10, _m11;  // 2, 3,
				};
			};
		};
		
		class RowMatrix33
		{
		public:
			RowMatrix33() {}
			~RowMatrix33() {}
			RowMatrix33(const RowMatrix33& matrix33);

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
				float _m[9]; //row will be represented at first.
				struct
				{
					float _m00, _m01, _m02,// 0, 1, 2
						_m10, _m11, _m12,  // 3, 4, 5
						_m20, _m21, _m22;  // 6, 7, 8
				};
				Vector3 _row[3];//the axies 
			};

		};//end of Matrix33

		//This is the row Matrix.
		class RowMatrix44
		{
		public:
			inline RowMatrix44();
			inline RowMatrix44(const RowMatrix44& i_other);
			inline RowMatrix44& operator=(const RowMatrix44& i_other);
			inline RowMatrix44& operator*=(float i_other);
			inline RowMatrix44 operator*(float i_other) const;
			inline bool operator==(const RowMatrix44& i_other) const;

			inline RowMatrix44& Identify();
			inline Vector4 GetRow(size_t index) const;
			inline Vector4 GetCol(size_t index) const;
			inline float GetRowCol(size_t row, size_t col) const;
			inline void SetRowCol(size_t row, size_t col, float value);
			inline RowMatrix44& Transpose();
			inline RowMatrix44 GetTranspose() const;
			inline RowMatrix44 GetAdjMatrix() const;


			//These are the row rotate Matrixs. So they will be suit for directX.
			inline static RowMatrix44 GetRotateAroundXLH(float degree);
			inline static RowMatrix44 GetRotateAroundYLH(float degree);
			inline static RowMatrix44 GetRotateAroundZLH(float degree);
			inline static RowMatrix44 GetRotateAroundAxisLH(Vector3 axis, float degree);
			inline static RowMatrix44 GetTranformLH(float x, float y, float z);
			inline static RowMatrix44 GetTranformLH(const Vector3& position);
			inline static RowMatrix44 GetScaleLH(float x, float y, float z);
			inline static RowMatrix44 GetScaleLH(const Vector3& scale);

			inline float GetDeter() const;
			inline bool GetInvert(RowMatrix44& out) const;

		private:
			inline int GetSignOfCofactor(size_t row, size_t col) const;
			inline RowMatrix33 GetCofactorMatrix(size_t row, size_t col) const;

		public:
			union
			{
				Vector4 _u[4];//we use row major matrix by default
				float _m[16]; //row will be represented at first.
				//Row Major Matrix
				struct
				{
					float _m00, _m01, _m02, _m03, // 0, 1, 2, 3,
						_m10, _m11, _m12, _m13,   // 4, 5, 6, 7,
						_m20, _m21, _m22, _m23,   // 8, 9, 10, 11,
						_m30, _m31, _m32, _m33;   // 12, 13, 14, 15,
				};
				/*
				Vector4 _col[4];
				//Coloum Major Matrix
				struct
				{
					float _m00, _m10, _m20, _m30, // 0, 1, 2, 3,
						_m01, _m11, _m21, _m31,   // 4, 5, 6, 7,
						_m02, _m12, _m22, _m32,   // 8, 9, 10, 11,
						_m03, _m13, _m23, _m33;   // 12, 13, 14, 15,
				};
				*/
			};
			
			const static RowMatrix44 Identity;
		};



		inline RowMatrix44 operator*(const RowMatrix44& i_lhs, const RowMatrix44& i_rhs)
		{
			RowMatrix44 result = RowMatrix44::Identity;
			for (size_t indexRow = 0; indexRow<4; ++indexRow)
			{
				Vector4 leftRow = i_lhs.GetRow(indexRow);
				for (size_t indexCol = 0; indexCol < 4; ++indexCol)
				{
					Vector4 righCol = i_rhs.GetCol(indexCol);
					float value = leftRow.Dot(righCol);
					result.SetRowCol(indexRow, indexCol, value);
				}
			}
			return result;
		}

		inline Vector4 operator*(const Vector4& i_vector, const RowMatrix44& i_matrix)
		{
			Vector4 result = Vector4::Zero;
			Vector4 righCol = i_matrix.GetCol(0);
			result.x(i_vector.Dot(righCol));
			righCol = i_matrix.GetCol(1);
			result.y(i_vector.Dot(righCol));
			righCol = i_matrix.GetCol(2);
			result.z(i_vector.Dot(righCol));
			righCol = i_matrix.GetCol(3);
			result.w(i_vector.Dot(righCol));
			return result;
		}

		inline RowMatrix44 operator*(float i_f, const RowMatrix44& i_matrix)
		{
			return i_matrix * i_f;
		}

		
	}
}

#include "RowMatrix.inl"

#endif //MATRIX_H

