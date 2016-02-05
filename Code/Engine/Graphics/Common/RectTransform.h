#ifndef EAE_ENGINE_GRAPHICS_RECT_TRANSFORM_H
#define EAE_ENGINE_GRAPHICS_RECT_TRANSFORM_H

#include "Engine/Common/Interfaces.h"
#include "Engine/Math/Vector.h"
#include "Engine/General/BasicShapes.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		class RectTransform : public Common::IRectTransform 
		{
		public:
			RectTransform();
			void SetPivot(Math::Vector2 i_pivot) { _pivot = i_pivot; }
			void SetAnchor(Rectangle anchorPoint) { _anchorPoint = anchorPoint; }
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

			For more: please read http://docs.unity3d.com/Manual/class-RectTransform.html
			*/
			void SetRect(Math::Vector4 values);
			Rectangle GetRect() { return _rect; }
		private:
			// @_pivot is the local ratio of the Image.
			// think it as the center of the rotation.
			Math::Vector2 _pivot;
			// anchor point uses the Viewport coordinate, 
			// it contains a pair of min and max points.
			Rectangle _anchorPoint;
			// The calculated rectangle in the local space of the Transform.
			Rectangle _rect;
		};
	}
}

#endif//EAE_ENGINE_GRAPHICS_RECT_TRANSFORM_H