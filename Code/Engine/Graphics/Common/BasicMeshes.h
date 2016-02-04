#ifndef EAE_ENGINE_GRAPHICS_BASIC_MESHES
#define EAE_ENGINE_GRAPHICS_BASIC_MESHES
#include "Engine/Math/Vector.h"
#include "Engine/Math/Quaternion.h"
#include <vector>

namespace EAE_Engine 
{
	namespace Graphics
	{
		struct Rectangle
		{
			Rectangle();
			std::vector<Math::Vector3> _vertices;
		};

		struct Box
		{
			Box();
			std::vector<Math::Vector3> _vertices;
			std::vector<uint32_t> _indices;
		};

		struct Sphere 
		{
			Sphere(uint32_t sliceCount, uint32_t stackCount, float raduis = 1.0f);
			std::vector<Math::Vector3> _vertices;
			std::vector<uint32_t> _indices;
		};



	}
}

#endif//EAE_ENGINE_GRAPHICS_BASIC_MESHES