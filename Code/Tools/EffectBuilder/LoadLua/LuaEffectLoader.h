#ifndef EAE_ENGINE_LUAEFFECTLOADER_H
#define EAE_ENGINE_LUAEFFECTLOADER_H
#include <cstdint>

namespace EAE_Engine
{
	namespace Tools
	{
		bool GenerateBinaryEffectData(const char* i_luaEffectFile, char*& o_pBuffer, size_t& sizeOfBuffer);
	}
}

#endif//EAE_ENGINE_LUAEFFECTLOADER_H