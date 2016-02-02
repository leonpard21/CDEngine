#ifndef MEMORY_NEW_H
#define MEMORY_NEW_H
#define _CRTDBG_MAP_ALLOC// get rid of the warning of C4005: '_malloca' : macro redefinition
#include <cstdio>
#include <cstdlib>
#include <new> // placement new
#include <cmath>
#include "../../UserOutput/Source/EngineDebuger.h"

namespace EAE_Engine
{
	namespace Memory
	{
	
		enum NewAlignment
		{
			NEW_ALIGN_DEFAULT = 4,
			NEW_ALIGN_4       = 4,
			NEW_ALIGN_8       = 8,
			NEW_ALIGN_16      = 16,
			NEW_ALIGN_32      = 32,
			NEW_ALIGN_64      = 64,
		};


		//realloc the block of align memory.
		extern void* align_realloc(void* i_ptr, size_t i_size, EAE_Engine::Memory::NewAlignment i_align);

		//malloc one block of align memory. 
		extern void* align_malloc(size_t i_size, EAE_Engine::Memory::NewAlignment i_align);

		//free the memory malloced 
		extern void align_free(void * i_ptr);
		
		//malloc several blocks, each block is malloced on align memory. 
		extern void* align_malloc_array(size_t i_size, size_t i_num, EAE_Engine::Memory::NewAlignment i_align);
		
		//malloc one block of align offset boundry memory. 
		extern void* align_malloc_offset(size_t i_size, EAE_Engine::Memory::NewAlignment i_align, size_t offset);

		//malloc several blocks, each block is malloced on align offset boundry memory. 
		extern void* align_malloc_offset_array(size_t i_size, size_t i_num, EAE_Engine::Memory::NewAlignment i_align, size_t offset);

		//get the address of the next object in the T array i_ptr.
		extern void* align_get_next(void * i_ptr, size_t i_sizeOfEachElement, EAE_Engine::Memory::NewAlignment i_align);

		//get the boundry offset between each two elements for the alignment
		inline size_t align_get_boundry_offset(size_t i_sizeOfEachElement, EAE_Engine::Memory::NewAlignment i_align)
		{
			size_t num = static_cast<size_t>(ceil( static_cast<float>(i_sizeOfEachElement) / static_cast<float>(i_align) ));//calculate how many memory actually used by alignment
			return num*i_align;
		}

		/* Align_new, malloc a group of memory and new the type T on this memory.
		 * T, the type we want to construct.
		 * Args, the parameters for the constructor of T.
		 * i_number, the amount of T we want to construct.
		 * i_align, the alignment of the memory.
		 * In fact, every T use this function should have the describtion of __declspec( align(i_align) )
		 * but if the input is not aligned it will also be OK because the offset is calculated by the i_align.
		 */
		template<typename T, typename ...Args>
		void* align_new(size_t i_number, EAE_Engine::Memory::NewAlignment i_align, Args&&... args)
		{
			size_t align_offset = align_get_boundry_offset(sizeof(T), i_align);
			size_t i_whole_size = align_offset*i_number;
			T* pResult = nullptr;
			switch (i_align)
			{
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_4:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
			case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
				pResult = static_cast<T*>(_aligned_malloc(i_whole_size, i_align));
				break;
			default:
				align_offset = align_get_boundry_offset(sizeof(T), EAE_Engine::Memory::NewAlignment::NEW_ALIGN_DEFAULT);
				i_whole_size = align_offset*i_number;
				pResult = static_cast<T*>(_aligned_malloc(i_whole_size, EAE_Engine::Memory::NewAlignment::NEW_ALIGN_DEFAULT));
				break;
			}
			if (!pResult)
			{
				return nullptr;
			}
			SetMem((uint8_t*)pResult, i_whole_size, 1);
			T* pTemp = pResult;
			for (size_t i = 0; i < i_number; ++i)
			{
				pTemp = new(pTemp)T(std::forward<Args>(args)...);//placement new
				//pTemp->T::T(std::forward<Args>(args)...);
				pTemp = reinterpret_cast<T*>(reinterpret_cast<char*>(pTemp)+align_offset);
				if ((reinterpret_cast<size_t>(pTemp) % i_align) == 0)
					TDEBUG_PRINT("This pointer, %x, is aligned on %d\n", Debugger::VerbosityDebugger::LEVEL2, const_cast<char*>(reinterpret_cast<char*>(pTemp)), i_align);
				else
					TDEBUG_PRINT("This pointer, %x, is not aligned on %d\n", Debugger::VerbosityDebugger::LEVEL2, const_cast<char*>(reinterpret_cast<char*>(pTemp)), i_align);
			}
			
			return pResult;
		}
		
		/* Align_delete, delete the type T on the memory address i_ptr, then free the memory
		 * T, the type we want to delete
		 * i_ptr, the memory we want to free
		 * i_number, the number of T we want to delete
		 * i_align, the alignment of the memory
		 */
		// needed to keep compiler happy if above operator throws which
		// is important if exception handling is enabled
		template<typename T>
		inline void align_delete(void * i_ptr, size_t i_number, EAE_Engine::Memory::NewAlignment i_align)
		{
			size_t align_offset = align_get_boundry_offset(sizeof(T), i_align);
			T* pPointer = static_cast<T*>(i_ptr);
			// don't attempt to delete NULL pointers. i guess we could also assert
			if (pPointer != nullptr){
				for (size_t i = 0; i < i_number; ++i)
				{
					pPointer->~T();
					pPointer = reinterpret_cast<T*>(reinterpret_cast<char*>(pPointer)+align_offset);
				}
				//delete[] pPointer;
				_aligned_free(i_ptr);
			}	
		}

		/* Align_get_next, get the address of the next T object in the T array i_ptr. 
		* T, the type we want to operator
		* i_ptr, the current address
		* i_align, the alignment of the memory
		*/
		template<typename T>
		inline T* align_get_next(void * i_ptr, EAE_Engine::Memory::NewAlignment i_align)
		{
			size_t align_offset = align_get_boundry_offset(sizeof(T), i_align);
			T* pPointer = static_cast<T*>(i_ptr);
			if (pPointer != nullptr){
				pPointer = reinterpret_cast<T*>(reinterpret_cast<char*>(pPointer)+align_offset);
			}
			return pPointer;
		}
		

		template<typename T, typename ...Args>
		inline T* align_placement_new(void * i_ptr, size_t num, EAE_Engine::Memory::NewAlignment i_align, Args&&... args)
		{
			if (!i_ptr)
			{
				return nullptr;
			}
			size_t size = _aligned_msize(i_ptr, i_align, 0);//Returns the size of a memory block allocated in the heap.
			char* pTemp = static_cast<char*>(i_ptr);
			size_t align_offset = align_get_boundry_offset(sizeof(T), i_align);
			for (unsigned int i = 0; i < num; ++i)
			{
				(T*)new(pTemp)T(std::forward<Args>(args)...);
				pTemp += align_offset;
			}
			return static_cast<T*>(i_ptr);
		}
		
		template<typename T>
		inline void align_placement_delete(void * i_ptr, size_t num, EAE_Engine::Memory::NewAlignment i_align)
		{
			if (!i_ptr)
			{
				return;
			}
			size_t size = _aligned_msize(i_ptr, i_align, 0);//Returns the size of a memory block allocated in the heap.
			char* pTemp = static_cast<char*>(i_ptr);
			size_t align_offset = align_get_boundry_offset(sizeof(T), i_align);
			for (unsigned int i = 0; i < num; ++i)
			{
				(reinterpret_cast<T*>(pTemp))->T::~T();
				pTemp = pTemp + align_offset;
			}
		}


		template<typename T, typename ...Args>
		inline T* placement_new(void * i_ptr, size_t num, Args&&... args)
		{
			if (!i_ptr)
			{
				return nullptr;
			}
			T* pTemp = static_cast<T*>(i_ptr);
			for (unsigned int i = 0; i < num; ++i)
			{
				new(pTemp)T(std::forward<Args>(args)...);
				pTemp ++;
			}
			return static_cast<T*>(i_ptr);
		}

		template<typename T>
		inline void placement_delete(void * i_ptr, size_t num)
		{
			if (!i_ptr)
			{
				return;
			}
			T* pTemp = static_cast<T*>(i_ptr);
			for (unsigned int i = 0; i < num; ++i)
			{
				pTemp->T::~T();
				++pTemp;
			}
		}
	}
}



#endif //MEMORY_NEW_H
