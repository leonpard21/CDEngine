#ifndef EAE_ENGINE_GRAPHICS_RECT_TRANSFORM_H
#define EAE_ENGINE_GRAPHICS_RECT_TRANSFORM_H

#include "Engine/Common/Interfaces.h"
#include "Engine/Math/Vector.h"
#include "Engine/General/BasicShapes.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		class RectTransform// : public Common::IRectTransform 
		{
		public:
			RectTransform();
			void SetPivot(Math::Vector2 i_pivot) { _pivot = i_pivot; }
			Math::Vector2 GetPivot() { return _pivot; }
			void SetAnchor(Rectangle anchorPoint) { _anchorPoint = anchorPoint; }
			Rectangle GetAnchor() { return _anchorPoint; }
			void SetRect(Rectangle rect) { _rect = rect; }
	//		Rectangle GetRect() { return _rect; }
			Rectangle GetScreenSpaceRect();
			Rectangle GetClipRect();

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