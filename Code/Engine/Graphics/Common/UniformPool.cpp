#include "UniformPool.h"
#include "Engine/General/MemoryOp.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		UniformValue::~UniformValue()
		{
			SAFE_DELETE(_pName);
			SAFE_DELETE(_pBuffer);
		}

		//////////////////////////////////UniformPool//////////////////////////////
		UniformPool::UniformPool():
			_pBuffer(nullptr), _sizeOfBuffer(0), _currentOffset(0)
		{
		}

		UniformPool::~UniformPool()
		{
			SAFE_DELETE_ARRAY(_pBuffer);
		}

		void UniformPool::Init() 
		{
			_sizeOfBuffer = 1024 * 1024 * 4;//4MB
			_pBuffer = new uint8_t[_sizeOfBuffer];
			_currentOffset = 0;
			ResetBuffer();
		}

		void UniformPool::ResetBuffer()
		{
			SetMem(_pBuffer, _sizeOfBuffer, 0);
		}

	}
}