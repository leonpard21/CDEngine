#ifndef EAE_ENGINE_GRAPHICS_BASIC_SHAPES
#define EAE_ENGINE_GRAPHICS_BASIC_SHAPES
#include "Engine/Math/Vector.h"
#include "Engine/Math/Quaternion.h"
#include <vector>

namespace EAE_Engine 
{
	namespace Graphics
	{
		struct Rectangle
		{
			Rectangle() = default;
			float _left, _right, _bottom, _top;
		};

		struct Sphere
		{
			Sphere() = default;
			Math::Vector3 _pos;
			float _radius;
		};
	}
}

#endif//EAE_ENGINE_GRAPHICS_BASIC_SHAPES