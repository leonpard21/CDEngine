
namespace EAE_Engine
{
	namespace Math
	{
		
		inline Vector3 Rectangle::GetLeftTop()
		{
			return _leftTop;
		}

		inline Vector3 Rectangle::GetRightBottom()
		{
			return _rightBottom;
		}

		inline Vector3 Rectangle::GetLeftBottom()
		{
			return Vector3(_leftTopX, _leftTopY + _height, 0.f);
		}

		inline Vector3 Rectangle::GetRightTop()
		{
			return Vector3(_leftTopX + _width, _leftTopY, 0.f);
		}

		inline Vector3 Rectangle::GetCenter()
		{
			return Vector3(_leftTopX + _width / 2.0f, _leftTopY + _height / 2.0f, 0.f);
		}

		inline float Rectangle::GetWidth()
		{
			return _width;
		}

		inline float Rectangle::GetHeight()
		{
			return _height;
		}

		inline void Rectangle::SetWidth(float width)
		{
			_width = width;
		}

		inline void Rectangle::SetHeight(float height)
		{
			_height = height;
		}

		inline void Rectangle::Move(float x, float y)
		{
			_leftTop = Vector3(_leftTopX + x, _leftTopY+y, 0.0f);
			_rightBottom = Vector3(_leftTopX + x + _width, _leftTopY + y + _height, 0.f);
			_leftTopX = _leftTopX + x;
			_leftTopY = _leftTopY + y;
		}

	}
}