#include "Rectangle.h"

namespace EAE_Engine
{
	namespace Math
	{
		Rectangle::Rectangle() :
			_leftTop(Vector3(0.f, 0.f, 0.f)), _rightBottom(0.f, 0.f, 0.f),
			_leftTopX(0.f), _leftTopY(0.f),
			_width(0.f), _height(0.f)
		{
		}


		Rectangle::~Rectangle()
		{
		}

		Rectangle::Rectangle(float x, float y, float width, float height) :
			_leftTop(Vector3(x, y, 0.0f)), _rightBottom(x + width, y + height, 0.f),
			_leftTopX(x), _leftTopY(y),
			_width(width), _height(height)
		{
		}


		Rectangle::Rectangle(const Rectangle& rect):
			_leftTop(rect._leftTop), _rightBottom(rect._rightBottom),
			_leftTopX(rect._leftTopX), _leftTopY(rect._leftTopY),
			_width(rect._width), _height(rect._height)
		{
		}

	}
}