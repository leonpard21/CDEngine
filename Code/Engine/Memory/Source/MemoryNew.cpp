#include "General/MemoryOp.h"
#include "MemoryNew.h"

namespace EAE_Engine
{
	namespace Memory
	{
		inline void* align_realloc(void* i_ptr, size_t i_size, EAE_Engine::Memory::NewAlignment i_align)
		{
			void* pResult = nullptr;
			switch (i_align)
			{
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_4:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
				pResult = _aligned_realloc(i_ptr, i_size, i_align);
				break;
			default:
				pResult = _aligned_realloc(i_ptr, i_size, EAE_Engine::Memory::NewAlignment::NEW_ALIGN_DEFAULT);
				break;
			}
			if (!pResult)
			{
				return nullptr;
			}
			SetMem((uint8_t*)pResult, i_size, 0);
			return pResult;
		}



		//NewAlignment MemoryAlignment::_alignment = NEW_ALIGN_DEFAULT;
		//malloc one block of align boundry memory. 
		inline void* align_malloc(size_t i_size, EAE_Engine::Memory::NewAlignment i_align)
		{
			void* pResult = nullptr;
			switch (i_align)
			{
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_4:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
				pResult = _aligned_malloc(i_size, i_align);
				break;
			default:
				pResult = _aligned_malloc(i_size, EAE_Engine::Memory::NewAlignment::NEW_ALIGN_DEFAULT);
				break;
			}
			if (!pResult)
			{
				return nullptr;
			}
			SetMem((uint8_t*)pResult, i_size, 0);
			return pResult;
		}

		//malloc several blocks, each block is malloced on align boundry memory. 
		inline void* align_malloc_array(size_t i_size, size_t i_num, EAE_Engine::Memory::NewAlignment i_align)
		{
			size_t align_offset = align_get_boundry_offset(i_size, i_align);
			size_t i_whole_size = align_offset*i_num;
			void* pResult = nullptr;
			switch (i_align)
			{
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_4:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
				pResult = _aligned_malloc(i_whole_size, i_align);
				break;
			default:
				pResult = _aligned_malloc(i_whole_size, EAE_Engine::Memory::NewAlignment::NEW_ALIGN_DEFAULT);
				break;
			}
			if (!pResult)
			{
				return nullptr;
			}
			SetMem((uint8_t*)pResult, i_whole_size, 0);
			return pResult;
		}

		inline void align_free(void * i_ptr)
		{
			void* pPointer = i_ptr;
			// don't attempt to delete NULL pointers. i guess we could also assert
			if (pPointer != nullptr){
				_aligned_free(i_ptr);
			}
		}

		inline void* align_get_next(void * i_ptr, size_t i_sizeOfEachElement, EAE_Engine::Memory::NewAlignment i_align)
		{
			size_t align_offset = align_get_boundry_offset(i_sizeOfEachElement, i_align);
			char* pAddress = reinterpret_cast<char*>(i_ptr);
			if (pAddress)
			{
				pAddress += align_offset;
				return pAddress;
			}
			return nullptr;
		}


		//NewAlignment MemoryAlignment::_alignment = NEW_ALIGN_DEFAULT;
		//malloc one block of align boundry memory. 
		inline void* align_malloc_offset(size_t i_size, EAE_Engine::Memory::NewAlignment i_align, size_t offset)
		{
			void* pResult = nullptr;
			switch (i_align)
			{
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_4:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
				pResult = _aligned_offset_malloc(i_size, i_align, offset);//((int)pResult + offset) % alignment  == 0
				break;
			default:
				pResult = _aligned_offset_malloc(i_size, EAE_Engine::Memory::NewAlignment::NEW_ALIGN_DEFAULT, offset);
				break;
			}
			if (!pResult)
			{
				return nullptr;
			}
			SetMem((uint8_t*)pResult, i_size, 0);
			return pResult;
		}

		//malloc several blocks, each block is malloced on align offset boundry memory. 
		inline void* align_malloc_offset_array(size_t i_size, size_t i_num, EAE_Engine::Memory::NewAlignment i_align, size_t offset)
		{
			size_t align_offset = align_get_boundry_offset(i_size, i_align);
			size_t i_whole_size = align_offset*i_num;
			void* pResult = nullptr;
			switch (i_align)
			{
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_4:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
				pResult = _aligned_offset_malloc(i_whole_size, i_align, offset);//((int)pResult + offset) % alignment  == 0
				break;
			default:
				pResult = _aligned_offset_malloc(i_whole_size, EAE_Engine::Memory::NewAlignment::NEW_ALIGN_DEFAULT, offset);
				break;
			}
			if (!pResult)
			{
				return nullptr;
			}
			SetMem((uint8_t*)pResult, i_whole_size, 0);
			return pResult;
		}


	}
}
