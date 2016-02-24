#ifndef EAE_ENGINE_BINARYFILELOADER_H
#define EAE_ENGINE_BINARYFILELOADER_H

#include "Engine/Math/Vector.h"
#include "General/Singleton.hpp"

namespace EAE_Engine
{
	namespace General
	{
		struct SubMesh 
		{
			uint32_t _firstIndex;
			uint32_t _endIndex;
		};

		struct CollisionMesh
		{
			Math::Vector3* _pVertices;
			uint8_t* _pIndices;
		};



	}
}

#endif//EAE_ENGINE_BINARYFILELOADER_H