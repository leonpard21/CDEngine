#ifndef AUTO_MEMORY_POOL
#define AUTO_MEMORY_POOL

#include "MemoryAllocator.h"

namespace EAE_Engine
{
	namespace Memory
	{ 
		class AutoMemoryPool
		{
			AutoMemoryPool() {}
			// make non-copyable
			AutoMemoryPool(const AutoMemoryPool & i_other);
			void operator=(const AutoMemoryPool & i_other) = delete;
		public:
			AutoMemoryPool(unsigned int i_size, unsigned int i_count);
			~AutoMemoryPool();

		private:
			unsigned int _size;
			unsigned int _count;
			EAE_Engine::Memory::MemoryBlockAllocator* _pMemBlockAlloctor;
		};
	}
}

#endif//AUTO_MEMORY_POOL