#ifndef EAE_ENGINE_LUAEFFECTLOADER_H
#define EAE_ENGINE_LUAEFFECTLOADER_H
#include <cstdint>

namespace EAE_Engine
{
	namespace Tools
	{
		bool GenerateBinaryMaterialData(const char* i_luaMaterialFile, char*& o_pBuffer, uint32_t& sizeOfBuffer);
	}
}

#endif//EAE_ENGINE_LUAEFFECTLOADER_H