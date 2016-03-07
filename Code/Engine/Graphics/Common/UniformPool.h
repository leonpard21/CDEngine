#ifndef EAE_ENGINE_GRAPHICS_UNIFORMPOOL_H
#define EAE_ENGINE_GRAPHICS_UNIFORMPOOL_H

#include "Engine/General/Singleton.hpp"
#include <cstdint>

namespace EAE_Engine 
{
	namespace Graphics 
	{
		struct UniformValue
		{
			UniformValue() = default;
			~UniformValue();
			uint32_t _offsetInPool;
			const char* _pName;
			void* _pBuffer;
			uint32_t _sizeOfBuffer;
		};

		class UniformPool : public Singleton<UniformPool> 
		{
		public:
			UniformPool();
			~UniformPool();
			void Init();
			void ResetBuffer();
			UniformValue AddUniformValue();

		private:
			uint8_t* _pBuffer;
			uint32_t _sizeOfBuffer;
			uint32_t _currentOffset;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_UNIFORMPOOL_H