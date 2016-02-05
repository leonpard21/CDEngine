#include "RectTransform.h"
#include "Device.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		RectTransform::RectTransform() :
			_pivot({ 0.5f, 0.5f }), _anchorPoint({0.5f, 0.5f, 0.5f, 0.5f}),
			_rect({0.0f, 0.0f, 0.0f, 0.0f})
		{
		}

		/*
		Now the GUI image has 2 modes:
		1. position-mode (the anchor min and max are the same)
		The 4 parameters of the screenRect are:
		x, y, the screen position of the anchor point).
		width, height, the width and the height of the rect we want to render.
		In this case, the screenRect size will be fixed when we change the window size.

		2. stretch-mode (the anchor min and max are not the same)
		The 4 parameters of the screenRect are:
		_left, _right, how far the left edge and right edge from the anchorMinX and anchorMaxX,
		_bottom, _top, how far the bottom edge and top edge from the anchorMinY and anchorMaxY
		*/
		void RectTransform::SetRect(Math::Vector4 values)
		{
			ScreenRect screenInfo = GetScreenRect();
			float screenWidth = screenInfo._width;
			float screenHeight = screenInfo._height;
			Math::Vector2 _anchorMinScreen(_anchorPoint._left * screenWidth, _anchorPoint._bottom * screenHeight);
			Math::Vector2 _anchorMaxScreen(_anchorPoint._right * screenWidth, _anchorPoint._top * screenHeight);
			Rectangle imageScreenRectangle;
			// if the anchorPositionX is the same, then based on pivot, 
			// we call it position-mode
			if (_anchorPoint._right - _anchorPoint._left == 0.0f)
			{
				ScreenRect imageScreenRect = { values._u[0], values._u[1], values._u[2], values._u[3] };
				float screenRectX = imageScreenRect._x + (_anchorMinScreen._x + _anchorMaxScreen._x) * 0.5f;
				imageScreenRectangle._left = screenRectX - _pivot._x * imageScreenRect._width;
				imageScreenRectangle._right = screenRectX + (1.0f - _pivot._x) * imageScreenRect._width;
			}
			else // or use stretch-mode
			{
				Rectangle imageStretchRect = { values._u[0], values._u[1], values._u[2], values._u[3] };
				imageScreenRectangle._left = _anchorMinScreen._x + imageStretchRect._left;
				imageScreenRectangle._right = _anchorMaxScreen._x - imageStretchRect._right;
			}
			// if the anchorPositionY is the same, then based on pivot,
			// we call it position-mode
			if (_anchorPoint._top - _anchorPoint._bottom == 0.0f)
			{
				ScreenRect imageScreenRect = { values._u[0], values._u[1], values._u[2], values._u[3] };
				float screenRectY = imageScreenRect._y + (_anchorMinScreen._y + _anchorMaxScreen._y) * 0.5f;
				imageScreenRectangle._bottom = screenRectY - _pivot._y * imageScreenRect._height;
				imageScreenRectangle._top = screenRectY + (1.0f - _pivot._y) * imageScreenRect._height;
			}
			else  // or use stretch-mode
			{
				Rectangle imageStretchRect = { values._u[0], values._u[1], values._u[2], values._u[3] };
				imageScreenRectangle._bottom = _anchorMinScreen._y + imageStretchRect._bottom;
				imageScreenRectangle._top = _anchorMaxScreen._y - imageStretchRect._top;
			}
			// Since we have got the 4 positions of the 4 vertices in screen pos, 
			// we can convert the screen pos to clip pos. 
			Rectangle clipPos;
			{
				clipPos._left = imageScreenRectangle._left / screenWidth * 2.0f - 1.0f; // the screen vertex coordinate is from (-1.0f, 1.0f)
				clipPos._right = imageScreenRectangle._right / screenWidth * 2.0f - 1.0f;
				clipPos._bottom = imageScreenRectangle._bottom / screenHeight * 2.0f - 1.0f;
				clipPos._top = imageScreenRectangle._top / screenHeight* 2.0f - 1.0f;
			}
			_rect = clipPos;
		}
	}
}
