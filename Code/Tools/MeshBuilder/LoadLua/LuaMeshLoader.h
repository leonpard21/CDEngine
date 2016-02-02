#ifndef EAE_ENGINE_LUAMESHLOADER_H
#define EAE_ENGINE_LUAMESHLOADER_H
#include <cstdint>

namespace EAE_Engine
{
	namespace Tools
	{
		bool GenerateBinaryMeshData(const char* i_luaMeshFile, char*& o_pBuffer, uint32_t& sizeOfBuffer);
	}
}

#endif//EAE_ENGINE_LUAMESHLOADER_H