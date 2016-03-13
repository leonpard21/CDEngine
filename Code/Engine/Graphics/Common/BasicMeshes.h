#ifndef EAE_ENGINE_GRAPHICS_BASIC_MESHES
#define EAE_ENGINE_GRAPHICS_BASIC_MESHES
#include "Engine/Math/Vector.h"
#include "Engine/Math/Quaternion.h"
#include <vector>

namespace EAE_Engine 
{
	namespace Graphics
	{
		struct RectangleSOAMesh
		{
			RectangleSOAMesh();
			RectangleSOAMesh(float i_left, float i_right, float i_bottom, float i_top);
			std::vector<Math::Vector3> _vertices;
		};

		struct BoxSOAMesh
		{
			BoxSOAMesh();
			std::vector<Math::Vector3> _vertices;
			std::vector<Math::Vector3> _normals;
			std::vector<Math::Vector3> _tangents;
			std::vector<Math::Vector2> _uvs;
			std::vector<Math::TVector4<uint8_t>> _colors;
			std::vector<uint32_t> _indices;
		};

		struct SphereSOAMesh 
		{
			SphereSOAMesh(uint32_t sliceCount, uint32_t stackCount, float raduis = 1.0f);
			std::vector<Math::Vector3> _vertices;
			std::vector<Math::Vector3> _normals;
			std::vector<Math::Vector3> _tangents;
			std::vector<Math::Vector2> _uvs;
			std::vector<Math::TVector4<uint8_t>> _colors;
			std::vector<uint32_t> _indices;
		};

		struct CylinderSOAMesh 
		{
			CylinderSOAMesh(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount);
			std::vector<Math::Vector3> _vertices;
			std::vector<Math::Vector3> _normals;
			std::vector<Math::Vector3> _tangents;
			std::vector<Math::Vector2> _uvs;
			std::vector<Math::TVector4<uint8_t>> _colors;
			std::vector<uint32_t> _indices;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_BASIC_MESHES