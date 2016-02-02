#include "AutoMemoryPool.h"

namespace EAE_Engine
{
	namespace Memory
	{
		AutoMemoryPool::AutoMemoryPool(unsigned int i_size, unsigned int i_count) :
			_size(i_size),
			_count(i_count),
			_pMemBlockAlloctor(nullptr)
		{}

		AutoMemoryPool::~AutoMemoryPool()
		{}

	}
}