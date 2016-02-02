
namespace EAE_Engine
{
	namespace Math
	{
		inline RowMatrix44::RowMatrix44()
		{
			Identify();
		}

		inline RowMatrix44& RowMatrix44::Identify()
		{
			for (size_t index = 0; index < 16; ++index)
			{
				size_t row = index / 4;
				size_t col = index % 4;
				if (row == col)
				{
					_m[index] = 1.f;
				}
				else
				{
					_m[index] = 0.f;
				}
			}
			return *this;
		}

		RowMatrix44::RowMatrix44(const RowMatrix44& i_other)
		{
			CopyMem((const uint8_t*)i_other._m, (uint8_t*)(this->_m), sizeof(float[16]));
		}

		inline RowMatrix44& RowMatrix44::operator= (const RowMatrix44& i_other)
		{
			CopyMem((const uint8_t*)i_other._m, (uint8_t*)(this->_m), sizeof(float[16]));
			return *this;
		}

		bool RowMatrix44::operator== (const RowMatrix44& i_other) const
		{
			for (size_t index = 0; index < 16; ++index)
			{
				//if (!Implements::AlmostEqual2sComplement(_m[index], i_other._m[index], 12))
				if ( fabsf( _m[index] - i_other._m[index]) > 0.001f)
				{
					return false;
				}
			}
			return true;
		}

		inline RowMatrix44& RowMatrix44::operator*= (float i_f)
		{
			for (size_t index = 0; index < 16; ++index)
			{
				_m[index] *= i_f;
			}
			return *this;
		}

		inline RowMatrix44 RowMatrix44::operator*(float i_f) const
		{
			RowMatrix44 result = *this;
			for (size_t index = 0; index < 16; ++index)
			{
				result._m[index] *= i_f;
			}
			return result;
		}

		inline Vector4 RowMatrix44::GetRow(size_t index) const
		{
			assert(index<4);
			size_t indexRow = index * 4;
			return Vector4(_m[indexRow], _m[indexRow + 1], _m[indexRow + 2], _m[indexRow + 3]);
		}

		inline Vector4 RowMatrix44::GetCol(size_t index) const 
		{
			assert(index<4);
			Vector4 result = Vector4::Zero;
			size_t indexCol = index;
			return Vector4(_m[indexCol], _m[indexCol + 4], _m[indexCol + 4 * 2], _m[indexCol + 4 * 3]);
		}


		inline float RowMatrix44::GetRowCol(size_t row, size_t col) const
		{
			assert(row<4 && col<4);
			size_t index = row * 4 + col;
			return _m[index];
		}

		inline void RowMatrix44::SetRowCol(size_t row, size_t col, float value)
		{
			assert(row<4 && col<4);
			size_t index = row * 4 + col;
			_m[index] = value;
		}

		inline RowMatrix44& RowMatrix44::Transpose()
		{
			RowMatrix44 intern = *this;
			for (size_t index = 0; index < 16; ++index)
			{
				size_t row = index / 4;
				size_t col = index % 4;
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

		inline RowMatrix44 RowMatrix44::GetTranspose() const
		{
			RowMatrix44 intern = *this;
			for (size_t index = 0; index < 16; ++index)
			{
				size_t row = index / 4;
				size_t col = index % 4;
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

		inline RowMatrix44 RowMatrix44::GetRotateAroundXLH(float degree)
		{
			float sinTheta = sinf(degree);
			float cosTheta = cosf(degree);
			RowMatrix44 result = RowMatrix44::Identity;
			//1, 0, 0
			//0,cos,sin
			//0,-sin,cos
			result._m00 = 1.f; result._m01 = 0;             result._m02 = 0;        result._m03 = 0;
			result._m10 = 0; result._m11 = cosTheta;      result._m12 = sinTheta; result._m13 = 0;
			result._m20 = 0; result._m21 = -1.f * sinTheta; result._m22 = cosTheta; result._m23 = 0;
			result._m30 = 0; result._m31 = 0;             result._m32 = 0;        result._m33 = 1;
			return result;
		}

		inline RowMatrix44 RowMatrix44::GetRotateAroundYLH(float degree)
		{
			float sinTheta = sinf(degree);
			float cosTheta = cosf(degree);
			RowMatrix44 result = RowMatrix44::Identity;
			//cos, 0, -sin
			//0, 1, 0
			//sin,0,cos
			result._m00 = cosTheta; result._m01 = 0; result._m02 = -1.f * sinTheta; result._m03 = 0;
			result._m10 = 0;        result._m11 = 1.f; result._m12 = 0;             result._m13 = 0;
			result._m20 = sinTheta; result._m21 = 0; result._m22 = cosTheta;      result._m23 = 0;
			result._m30 = 0;        result._m31 = 0; result._m32 = 0;             result._m33 = 1;
			return result;
		}

		inline RowMatrix44 RowMatrix44::GetRotateAroundZLH(float degree)
		{
			float sinTheta = sinf(degree);
			float cosTheta = cosf(degree);
			RowMatrix44 result = RowMatrix44::Identity;
			//cos, sin, 0
			//-sin,cos, 0
			//0, 0, 1
			result._m00 = cosTheta;      result._m01 = sinTheta; result._m02 = 0; result._m03 = 0;
			result._m10 = -1.f * sinTheta; result._m11 = cosTheta; result._m12 = 0; result._m13 = 0;
			result._m20 = 0;             result._m21 = 0;        result._m22 = 1.f; result._m23 = 0;
			result._m30 = 0;             result._m31 = 0;        result._m32 = 0; result._m33 = 1.f;
			return result;
		}

		inline RowMatrix44 RowMatrix44::GetRotateAroundAxisLH(Vector3 axis, float degree)
		{
			float sinTheta = sinf(degree);
			float cosTheta = cosf(degree);
			float oneSubCosTheta = 1.0f - cosTheta;
			RowMatrix44 result = RowMatrix44::Identity;
			axis.Normalize();
			//row 0, rotation of p[1, 0, 0, 0]
			result._m00 = axis.x() * axis.x() * oneSubCosTheta + cosTheta;           //nx*nx*(1-cos)+cos
			result._m01 = axis.x() * axis.y() * oneSubCosTheta + axis.z() * sinTheta;//nx*ny*(1-cos)+nz*sin
			result._m02 = axis.x() * axis.z() * oneSubCosTheta - axis.y() * sinTheta;//nx*nz*(1-cos)-ny*sin
			result._m03 = 0.0f;
			//row 1, rotation of q[0, 1, 0, 0]
			result._m10 = axis.x() * axis.y() * oneSubCosTheta - axis.z() * sinTheta;//nx*ny*(1-cos)-nz*sin
			result._m11 = axis.y() * axis.y() * oneSubCosTheta + cosTheta;           //ny*ny*(1-cos)+cos
			result._m12 = axis.y() * axis.z() * oneSubCosTheta + axis.x() * sinTheta;//ny*nz*(1-cos)+nx*sin
			result._m13 = 0.0f;
			//row 2, rotation of r[0, 0, 1, 0]
			result._m20 = axis.x() * axis.z() * oneSubCosTheta + axis.y() * sinTheta;//nx*nz*(1-cos)+ny*sin
			result._m21 = axis.y() * axis.z() * oneSubCosTheta - axis.x() * sinTheta;//ny*nz*(1-cos)-nx*sin
			result._m22 = axis.z() * axis.z() * oneSubCosTheta + cosTheta;           //nz*nz*(1-cos)+cos
			result._m23 = 0.0f;
			//row 3
			result._m30 = 0.0f; result._m31 = 0.0f; result._m32 = 0.0f; result._m33 = 1.0f;
			return result;
		}


		inline RowMatrix44 RowMatrix44::GetTranformLH(float x, float y, float z)
		{
			RowMatrix44 result = Identity;
			result._m30 = x;
			result._m31 = y;
			result._m32 = z;
			return result;
		}

		inline RowMatrix44 RowMatrix44::GetTranformLH(const Vector3& position)
		{
			return RowMatrix44::GetTranformLH(position.x(), position.y(), position.z());
		}

		inline RowMatrix44 RowMatrix44::GetScaleLH(float x, float y, float z)
		{
			RowMatrix44 result = Identity;
			result._m00 = x;
			result._m11 = y;
			result._m22 = z;
			return result;
		}

		inline RowMatrix44 RowMatrix44::GetScaleLH(const Vector3& scale)
		{
			return RowMatrix44::GetScaleLH(scale.x(), scale.y(), scale.z());
		}

		//Get the sign for the Minor
		inline int RowMatrix44::GetSignOfCofactor(size_t row, size_t col) const
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
		
		inline RowMatrix33 RowMatrix44::GetCofactorMatrix(size_t row, size_t col) const
		{
			assert(row<4 && col<4);
			float m[9] = {0.0f};
			size_t count = 0;
			for (size_t index = 0; index < 16; ++index)
			{
				size_t currentRow = index / 4;
				size_t currentCol = index % 4;
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
			RowMatrix33 result;
			result.SetValue(m);
			return result;
		}

		inline float RowMatrix44::GetDeter() const
		{
			RowMatrix33 m00 = GetCofactorMatrix(0, 0);
			RowMatrix33 m01 = GetCofactorMatrix(0, 1);
			RowMatrix33 m02 = GetCofactorMatrix(0, 2);
			RowMatrix33 m03 = GetCofactorMatrix(0, 3);

			float deter = GetSignOfCofactor(0, 0) * GetRowCol(0,0) * m00.GetDeter() +
				GetSignOfCofactor(0, 1) * GetRowCol(0, 1) * m01.GetDeter() +
				GetSignOfCofactor(0, 2) * GetRowCol(0, 2) * m02.GetDeter() +
				GetSignOfCofactor(0, 3) * GetRowCol(0, 3) * m03.GetDeter();
			return deter;
		}


		inline RowMatrix44 RowMatrix44::GetAdjMatrix() const
		{
			RowMatrix44 result = RowMatrix44::Identity;
			for (size_t index = 0; index < 16; ++index)
			{
				size_t currentRow = index / 4;
				size_t currentCol = index % 4;
				result._m[index] = GetSignOfCofactor(currentRow, currentCol) * GetCofactorMatrix(currentRow, currentCol).GetDeter();
			}
			result.Transpose();
			return result;
		}

		inline bool RowMatrix44::GetInvert(RowMatrix44& out) const
		{
			out = this->GetAdjMatrix();// GetTranspose();
			float deter = this->GetDeter();
			if (deter == 0)
			{
				return false;
			}
			else
			{
				float oneOverDeter = 1.0f / deter;
				out *= oneOverDeter;
				return true;
			}
		}
		
	}
}

