// Header Files
//=============

#include "ColMatrix.h"
#include <cmath>
#include <cassert>
#include "Quaternion.h"
#include "General/MemoryOp.h"

// Interface
//==========
namespace EAE_Engine 
{
	namespace Math 
	{

		ColMatrix44 ColMatrix44::Identity = ColMatrix44();


		ColMatrix44::ColMatrix44():
			_m00(1.0f), _m10(0.0f), _m20(0.0f), _m30(0.0f),
			_m01(0.0f), _m11(1.0f), _m21(0.0f), _m31(0.0f),
			_m02(0.0f), _m12(0.0f), _m22(1.0f), _m32(0.0f),
			_m03(0.0f), _m13(0.0f), _m23(0.0f), _m33(1.0f)
		{}
		ColMatrix44::ColMatrix44(
			const float i_00, const float i_10, const float i_20, const float i_30,
			const float i_01, const float i_11, const float i_21, const float i_31,
			const float i_02, const float i_12, const float i_22, const float i_32,
			const float i_03, const float i_13, const float i_23, const float i_33) :
			_m00(i_00), _m10(i_10), _m20(i_20), _m30(i_30),
			_m01(i_01), _m11(i_11), _m21(i_21), _m31(i_31),
			_m02(i_02), _m12(i_12), _m22(i_22), _m32(i_32),
			_m03(i_03), _m13(i_13), _m23(i_23), _m33(i_33)
		{}

		ColMatrix44::ColMatrix44(const ColMatrix44& i_other)
		{
			CopyMem((uint8_t*)i_other._m, (uint8_t*)(this->_m), sizeof(float[16]));
		}

		ColMatrix44& ColMatrix44::operator=(const ColMatrix44& i_other)
		{
			CopyMem((uint8_t*)i_other._m, (uint8_t*)(this->_m), sizeof(float[16]));
			return *this;
		}

		ColMatrix44& ColMatrix44::operator*=(float i_other)
		{
			for (size_t index = 0; index < 16; ++index)
			{
				_m[index] *= i_other;
			}
			return *this;
		}

		bool ColMatrix44::operator==(const ColMatrix44& i_other)
		{
			for (size_t index = 0; index < 16; ++index)
			{
				if (fabsf(_m[index] - i_other._m[index]) > FLT_EPSILON)
				{
					return false;
				}
			}
			return true;
		}

		ColMatrix44 ColMatrix44::operator*(float i_other) const
		{
			ColMatrix44 result = *this;
			for (size_t index = 0; index < 16; ++index)
			{
				result._m[index] *= i_other;
			}
			return result;
		}

		Vector4 ColMatrix44::GetCol(size_t index) const
		{
			assert(index<4);
			size_t indexCol = index * 4;
			return Vector4(_m[indexCol], _m[indexCol + 1], _m[indexCol + 2], _m[indexCol + 3]);
		}

		Vector4 ColMatrix44::GetRow(size_t index) const
		{
			assert(index<4);
			Vector4 result = Vector4::Zero;
			size_t indexRow = index;
			return Vector4(_m[indexRow], _m[indexRow + 4], _m[indexRow + 4 * 2], _m[indexRow + 4 * 3]);
		}

		ColMatrix44& ColMatrix44::Transpose()
		{
			ColMatrix44 intern = *this;
			for (size_t index = 0; index < 16; ++index)
			{
				size_t col = index / 4;
				size_t row = index % 4;
				if (row == col)
				{
					continue;
				}
				else
				{
					float value = intern._m[index];
					SetRowCol(col, row, value);
				}
			}
			return *this;
		}

		ColMatrix44 ColMatrix44::GetTranspose() const
		{
			ColMatrix44 intern = *this;
			for (size_t index = 0; index < 16; ++index)
			{
				size_t col = index / 4;
				size_t row = index % 4;
				if (row == col)
				{
					continue;
				}
				else
				{
					float value = _m[index];
					intern.SetRowCol(col, row, value);
				}
			}
			return intern;
		}

		void ColMatrix44::SetRowCol(size_t row, size_t col, float value)
		{
			assert(row<4 && col<4);
			size_t index = col * 4 + row;
			_m[index] = value;
		}

		float ColMatrix44::GetRowCol(size_t row, size_t col) const 
		{
			assert(row<4 && col<4);
			size_t index = col * 4 + row;
			return _m[index];
		}

		ColMatrix44::ColMatrix44(const Quaternion& i_rotation, const Vector3& i_translation):
			_m03(i_translation._x), _m13(i_translation._y), _m23(i_translation._z), _m33(1.0f), 
			_m30(0.0f), _m31(0.0f), _m32(0.0f)
		{
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

			_m00 = 1.0f - _2yy - _2zz;
			_m10 = _2xy + _2zw;
			_m20 = _2xz - _2yw;

			_m01 = _2xy - _2zw;
			_m11 = 1.0f - _2xx - _2zz;
			_m21 = _2yz + _2xw;

			_m02 = _2xz + _2yw;
			_m12 = _2yz - _2xw;
			_m22 = 1.0f - _2xx - _2yy;
		}

////////////////////////Code about InverseMatrix/////////////////////// 
		bool ColMatrix44::GetInverse(ColMatrix44& o_out) const
		{
			o_out = this->GetAdjugateMatrix();
			float deter = this->GetDeter();
			if (deter == 0)
			{
				return false;
			}
			else
			{
				float oneOverDeter = 1.0f / deter;
				o_out *= oneOverDeter;
				return true;
			}
		}

		ColMatrix44 ColMatrix44::GetAdjugateMatrix() const
		{
			ColMatrix44 result = ColMatrix44::Identity;
			for (size_t index = 0; index < 16; ++index)
			{
				size_t currentCol = index / 4;
				size_t currentRow = index % 4;
				result._m[index] = GetSignOfCofactor(currentRow, currentCol) * GetCofactorMatrix(currentRow, currentCol).GetDeter();
			}
			result.Transpose();
			return result;
		}

		float ColMatrix44::GetDeter() const
		{
			ColMatrix33 m00 = GetCofactorMatrix(0, 0);
			ColMatrix33 m01 = GetCofactorMatrix(0, 1);
			ColMatrix33 m02 = GetCofactorMatrix(0, 2);
			ColMatrix33 m03 = GetCofactorMatrix(0, 3);
			float deter = GetSignOfCofactor(0, 0) * GetRowCol(0, 0) * m00.GetDeter() +
				GetSignOfCofactor(0, 1) * GetRowCol(0, 1) * m01.GetDeter() +
				GetSignOfCofactor(0, 2) * GetRowCol(0, 2) * m02.GetDeter() +
				GetSignOfCofactor(0, 3) * GetRowCol(0, 3) * m03.GetDeter();
			return deter;
		}

		ColMatrix33 ColMatrix44::GetCofactorMatrix(size_t row, size_t col) const
		{
			assert(row<4 && col<4);
			float m[9] = { 0.0f };
			size_t count = 0;
			for (size_t index = 0; index < 16; ++index)
			{
				size_t currentCol = index / 4;
				size_t currentRow = index % 4;
				if (currentRow == row)
				{
					continue;
				}
				if (currentCol == col)
				{
					continue;
				}
				m[count++] = _m[index];
			}
			assert(count == 9);
			ColMatrix33 result;
			result.SetValue(m);
			return result;
		} 

		int ColMatrix44::GetSignOfCofactor(size_t row, size_t col)
		{
			assert(row<4 && col<4);
			size_t time = row + col + 2;
			int result = 1;
			for (size_t index = 0; index < time; ++index)
			{
				result *= -1;
			}
			return result;
		}

////////////////////////End code of InverseMatrix/////////////////////// 



		////////////////////////////////static functions///////////////////////////////////////////

		ColMatrix44 ColMatrix44::CreateMovementMatrix(const Vector3& i_position)
		{
			ColMatrix44 result;
			result._m03 = i_position.x();
			result._m13 = i_position.y();
			result._m23 = i_position.z();
			return result;
		}

		ColMatrix44 ColMatrix44::CreateRotationMatrix(const Quaternion& i_rotation)
		{
			ColMatrix44 result;
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

		ColMatrix44 ColMatrix44::CreateScaleMatrix(const Vector3& i_scale)
		{
			ColMatrix44 result;
			result._m00 = i_scale.x();
			result._m11 = i_scale.y();
			result._m22 = i_scale.z();
			return result;
		}

    // Reference: 
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
    Quaternion ColMatrix44::CreateQuaternion(const ColMatrix44& i_rotation)
    {
      Quaternion result = Quaternion::Identity;
      /*
      float trace = i_rotation._m00 + i_rotation._m11 + i_rotation._m22;
      if (trace > 0) 
      {
        float s = sqrtf(trace + 1.0f) * 2.0f; // S=4*qw 
        result._w = 0.25f * s;
        result._x = (i_rotation._m21 - i_rotation._m12) / s;
        result._y = (i_rotation._m02 - i_rotation._m20) / s;
        result._z = (i_rotation._m10 - i_rotation._m01) / s;
      }
      else if ((i_rotation._m00 > i_rotation._m11)&&(i_rotation._m00 > i_rotation._m22)) 
      {
        float s = sqrtf(1.0f + i_rotation._m00 - i_rotation._m11 - i_rotation._m22) * 2.0f; // S=4*qx 
        result._w = (i_rotation._m21 - i_rotation._m12) / s;
        result._x = 0.25f * s;
        result._y = (i_rotation._m01 + i_rotation._m10) / s;
        result._z = (i_rotation._m02 + i_rotation._m20) / s;
      }
      else if (i_rotation._m11 > i_rotation._m22) 
      {
        float s = sqrtf(1.0f + i_rotation._m11 - i_rotation._m00 - i_rotation._m22) * 2.0f; // S=4*qy
        result._w = (i_rotation._m02 - i_rotation._m20) / s;
        result._x = (i_rotation._m01 + i_rotation._m10) / s;
        result._y = 0.25 * s;
        result._z = (i_rotation._m12 + i_rotation._m21) / s;
      }
      else {
        float s = sqrtf(1.0f + i_rotation._m22 - i_rotation._m00 - i_rotation._m11) * 2.0f; // S=4*qz
        result._w = (i_rotation._m10 - i_rotation._m01) / s;
        result._x = (i_rotation._m02 + i_rotation._m20) / s;
        result._y = (i_rotation._m12 + i_rotation._m21) / s;
        result._z = 0.25f * s;
      }
      */
      float w = std::sqrtf(i_rotation._m00 + i_rotation._m11 + i_rotation._m22 + 1.0f);
      // Determine which of w, x, y, or z has the largest absolute value
      float fourWSquaredMinus1 = i_rotation._m00 + i_rotation._m11 + i_rotation._m22;
      float fourXSquaredMinus1 = i_rotation._m00 - i_rotation._m11 - i_rotation._m22;
      float fourYSquaredMinus1 = i_rotation._m11 - i_rotation._m00 - i_rotation._m22;
      float fourZSquaredMinus1 = i_rotation._m22 - i_rotation._m00 - i_rotation._m11;
      int biggestIndex = 0;
      float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
      if(fourXSquaredMinus1 > fourBiggestSquaredMinus1)
      {
        fourBiggestSquaredMinus1 = fourXSquaredMinus1;
        biggestIndex = 1;
      }
      if(fourYSquaredMinus1 > fourBiggestSquaredMinus1)
      {
        fourBiggestSquaredMinus1 = fourYSquaredMinus1;
        biggestIndex = 2;
      }
      if(fourZSquaredMinus1 > fourBiggestSquaredMinus1)
      {
        fourBiggestSquaredMinus1 = fourZSquaredMinus1;
        biggestIndex = 3;
      }
      // Perform square root and division
      float biggestVal = sqrtf(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
      float mult = 0.25f / biggestVal;
      // Apply table to compute quaternion values
      switch (biggestIndex) 
      {
      case 0:
        result._w = biggestVal;
        result._x = (i_rotation._m21 - i_rotation._m12) * mult;
        result._y = (i_rotation._m02 - i_rotation._m20) * mult;
        result._z = (i_rotation._m10 - i_rotation._m01) * mult;
        break;
      case 1:
        result._x = biggestVal;
        result._w = (i_rotation._m21 - i_rotation._m12) * mult;
        result._y = (i_rotation._m01 + i_rotation._m10) * mult;
        result._z = (i_rotation._m02 + i_rotation._m20) * mult;
        break;
      case 2:
        result._y = biggestVal;
        result._w = (i_rotation._m02 - i_rotation._m20) * mult;
        result._x = (i_rotation._m01 + i_rotation._m10) * mult;
        result._z = (i_rotation._m21 + i_rotation._m12) * mult;
        break;
      case 3:
        result._z = biggestVal;
        result._w = (i_rotation._m10 - i_rotation._m01) * mult;
        result._x = (i_rotation._m02 + i_rotation._m20) * mult;
        result._y = (i_rotation._m21 + i_rotation._m12) * mult;
        break;
      }
      return result;
    }




		////////////////////////////////////Global Functions////////////////////////////////////////////////
		ColMatrix44 operator*(const ColMatrix44& i_lhs, const ColMatrix44& i_rhs)
		{
			ColMatrix44 result = ColMatrix44::Identity;
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

		Vector4 operator*(const Vector4& i_vector, const ColMatrix44& i_matrix)
		{
			Vector4 result = Vector4::Zero;
			Vector4 col = i_matrix.GetCol(0);
			result.x(i_vector.Dot(col));
			col = i_matrix.GetCol(1);
			result.y(i_vector.Dot(col));
			col = i_matrix.GetCol(2);
			result.z(i_vector.Dot(col));
			col = i_matrix.GetCol(3);
			result.w(i_vector.Dot(col));
			return result;
		}

		Vector4 operator*(const ColMatrix44& i_matrix, const Vector4& i_vector)
		{
			Vector4 result = Vector4::Zero;
			Vector4 row = i_matrix.GetRow(0);
			result.x(row.Dot(i_vector));
			row = i_matrix.GetRow(1);
			result.y(row.Dot(i_vector));
			row = i_matrix.GetRow(2);
			result.z(row.Dot(i_vector));
			row = i_matrix.GetRow(3);
			result.w(row.Dot(i_vector));
			return result;
		}

	}
}
