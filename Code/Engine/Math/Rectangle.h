#ifndef RECTANGLE_H
#define RECTANGLE_H
#include "Vector.h"

namespace EAE_Engine
{
	namespace Math
	{
		class Rectangle
		{
		public:
			Rectangle();
			~Rectangle();
			Rectangle(float x, float y, float width, float height);
			Rectangle(const Rectangle& rect);
			inline Vector3 GetLeftTop();
			inline Vector3 GetLeftBottom();
			inline Vector3 GetRightTop();
			inline Vector3 GetRightBottom();
			inline Vector3 GetCenter();
			inline float GetWidth();
			inline float GetHeight();
			inline void SetWidth(float width);
			inline void SetHeight(float height);
			inline void Move(float x, float y);


		private:
			Vector3 _leftTop;
			Vector3 _rightBottom;
			float _leftTopX;
			float _leftTopY;
			float _width;
			float _height;
		};
	}
}

#include "Rectangle.inl"

#endif//RECTANGLE_H