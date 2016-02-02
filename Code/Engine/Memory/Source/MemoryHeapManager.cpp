#include "MemoryHeapManager.h"
#include <cmath>
#include "UserOutput/Source/Assert.h"
#include "UserOutput/Source/EngineDebuger.h"
#include "MemoryNew.h"
#include "General/MemoryOp.h"


namespace EAE_Engine
{
	namespace Memory
	{

		void MemoryChunkHeader::InitChunk(size_t i_size, void* i_ptr, MemoryChunkHeader* prev, MemoryChunkHeader* pnext)
		{
			_size = i_size;
			_pMemAddress = i_ptr;
			_pPrevChunk = prev;
			_pNextChunk = pnext;
		}

		//if return true, over boundry; else safe
		bool MemoryChunkHeader::OverBoundry(void* i_Ptr)
		{
			size_t minAddress = reinterpret_cast<size_t>(_pMemAddress);
			size_t maxAddress = minAddress + _size;
			size_t inputAddress = reinterpret_cast<size_t>(i_Ptr);
			if (inputAddress>maxAddress || inputAddress<minAddress)
			{
				return true;
			}
			return false;
		}



/////////////////////////////////////////////////////HeapManager//////////////////////////////////////////////////////////////////
		MemoryHeapManager* MemoryHeapManager::Create(size_t i_size)
		{
			//alloca align memory for the user 
			void* paddress = _aligned_malloc(i_size, NewAlignment::NEW_ALIGN_4);
			MessagedAssert(paddress != nullptr, "Memory chunk alloc failed!");
			if (!paddress)
			{
				return nullptr;
			}
			//create heap manager
			MemoryHeapManager* pHeapManager = static_cast<MemoryHeapManager*>(EAE_Engine::Memory::align_malloc(sizeof(MemoryHeapManager), NewAlignment::NEW_ALIGN_4));
			if (!pHeapManager)
			{
				_aligned_free(paddress);
				return nullptr;
			}
			pHeapManager->MemoryHeapManager::MemoryHeapManager(paddress, i_size);
			return pHeapManager;
		}

		void MemoryHeapManager::Destroy(MemoryHeapManager* pAddress)
		{
			if (pAddress)
			{
				MessagedAssert(!pAddress->CheckMemoryLeaks(), "Memory Leaks Happened in the Heap Manager!");
				//assert(!pAddress->CheckMemoryLeaks());
				if (!pAddress->CheckMemoryLeaks())
				{
					TDEBUG_PRINT("Cool, No Memory Leaks In Heap Memory Manager!\n", EAE_Engine::Debugger::VerbosityDebugger::LEVEL1);
				}
				//clear
				pAddress->MemoryHeapManager::~MemoryHeapManager();
				align_free(pAddress);
			}
		}




		MemoryHeapManager::MemoryHeapManager(void* pAddress, size_t i_size) : _pMemoryAddress(pAddress), _size(i_size)
		{
			//if there is no memory for the Chunk structure, just return.
			if (!_pMemoryAddress || i_size<sizeof(MemoryChunkHeader))
			{
				return;
			}
			//create a chunk of header structure by alignment new;
			_pHead = CreateNewChunk(_pMemoryAddress, i_size);
		}

		MemoryHeapManager::~MemoryHeapManager()
		{
			//free the memory alloced for user
			if (_pMemoryAddress)
			{
				_aligned_free(_pMemoryAddress);
				_pMemoryAddress = nullptr;
			}
		}

		bool MemoryHeapManager::CheckMemoryLeaks()
		{
			//if there are only one block, and its size = the whole size of the memory heap, 
			//then there is no leaks.
			if (_pHead && _pHead->GetSize() + sizeof(MemoryChunkHeader) == _size)
			{
				return false;
			}
			else if (_pHead && _pHead->GetSize() + sizeof(MemoryChunkHeader) != _size)
			{
				return true;
			}
			return false;
		}

		MemoryChunkHeader* MemoryHeapManager::CreateNewChunk(void* pAddress, size_t i_size)
		{
			if (((size_t)pAddress % NewAlignment::NEW_ALIGN_4) != 0)
			{
				//TDEBUG_PRINT_FL("Should always align on 4.\n", Debugger::VerbosityDebugger::LEVEL0, EngineInfo::GetEnginePlatform());
				MessagedAssert(((size_t)pAddress % NewAlignment::NEW_ALIGN_4) == 0, "Should always align on 4.");
				return nullptr;
			}
			MemoryChunkHeader* pChunk = nullptr;
			//create a chunk of header structure by alignment new;
			pChunk = static_cast<MemoryChunkHeader*>(placement_new<MemoryChunkHeader>(pAddress, 1));
			pChunk->SetUsed(false);
			//Init the size of it.
			size_t offset = sizeof(MemoryChunkHeader);
			size_t left_size = i_size - offset;
			pChunk->InitChunk(left_size, reinterpret_cast<char*>(pAddress)+offset, nullptr, nullptr);
			return pChunk;
		}


		void* MemoryHeapManager::Alloc(size_t i_size)
		{
			MessagedAssert(_pMemoryAddress != nullptr, "_pMemoryAddress cannot be nullptr!");
			void* pResult = nullptr;
			//calculate the realsize of memory alloc for the user.
			//becuase every time the memory address should be align 4, so we should calculate spaces beased on this size.
			size_t sizeOfMemory = (size_t)ceil((float)i_size / (float)NewAlignment::NEW_ALIGN_4) * NewAlignment::NEW_ALIGN_4;//ceil means round up value
			//std::unique_lock<std::mutex> lk(_heapMutex);
			MemoryChunkHeader* pChunk = GetFirstSuitbaleChunk(sizeOfMemory);
			if (pChunk == nullptr)
			{
				MessagedAssert(pChunk != nullptr, "No enough Memory!");
				return nullptr;
			}
			pResult = pChunk->GetAddress();
			//calculate how many memory left in the chunk
			size_t leftMemSize = pChunk->GetSize() - sizeOfMemory;
			pChunk->SetUsed(true);
			//if there is no enough space left for generating the next chunk, just return the result chunk for user
			if (leftMemSize <= sizeof(MemoryChunkHeader))
			{
				return pResult;
			}
			//Before create a new Chunk, we should resize the size of the current Chunk
			pChunk->ReSize(sizeOfMemory);
			//else we need to create a new Chunk and link it with others
			void* pNewStartAddress = static_cast<char*>(pResult) + sizeOfMemory;
			MemoryChunkHeader* pNextChunk = CreateNewChunk(pNewStartAddress, leftMemSize);
			SetMem((uint8_t*)pNextChunk->GetAddress(), (size_t)pNextChunk->GetSize(), 0);
			pNextChunk->SetUsed(false);
			//link the new chunk with others
			pNextChunk->SetPrevPtr(pChunk);
			MemoryChunkHeader* pPreviousNext = pChunk->GetNextPtr();
			pChunk->SetNextPtr(pNextChunk);
			pNextChunk->SetNextPtr(pPreviousNext);
			//return the address for user
			return pResult;
		}

		void MemoryHeapManager::Free(void* i_ptr)
		{
			MessagedAssert(i_ptr != nullptr, "Cannot free nullptr!");
			std::unique_lock<std::mutex> lk(_heapMutex);
			MemoryChunkHeader* pChunk = GetTheChunkContainsAddress(i_ptr);
			MessagedAssert(pChunk != nullptr, "You are trying to free some memory not on the boundry!");
			if (pChunk == nullptr)
			{
				return;
			}
			//set the chunk unUsed.
			pChunk->SetUsed(false); 
			MemoryChunkHeader* pPrevChunk = pChunk->GetPrevPtr();
			MemoryChunkHeader* pNextChunk = pChunk->GetNextPtr();
			
			// Be careful, we merge with the next chunk, then the previous chunk
			//if the next chunk is free chunk
			if (pNextChunk != nullptr && !pNextChunk->Used())
			{
				//Because the last one will always be free (only except when there is no memeory):
				//when we point to the third last, but the second last is free
				//then, we will not be able to merge the last one because it is always free...
				//So, when checking the next chunk is free or not, we should also check the next chunk's next chunk.
				//By this, we will not forget the last chunk, which is always free(if there are memory left).
				if (pNextChunk->GetNextPtr() && pNextChunk->GetNextPtr() != nullptr && !pNextChunk->GetNextPtr()->Used())
				{
					//Merge the chunk
					MergeTwoChunkHeaders(pNextChunk, pNextChunk->GetNextPtr());
				}
				//Merge the chunk 
				MergeTwoChunkHeaders(pChunk, pNextChunk);
				if (pChunk->GetAddress())
				{
					SetMem((uint8_t*)(pChunk->GetAddress()), (size_t)pChunk->GetSize(), 0);
				}
			}
			if (pPrevChunk != nullptr && !pPrevChunk->Used())//if the prev chunk have been freed.
			{
				//Merge the chunk
				MergeTwoChunkHeaders(pPrevChunk, pChunk);
				if (pPrevChunk->GetAddress())
				{
				//	Implements::SetMem((char*)(pPrevChunk->GetAddress()), pPrevChunk->GetSize(), 0);
				}
			}
		}


		MemoryChunkHeader* MemoryHeapManager::GetFirstSuitbaleChunk(size_t requiredSize)
		{
			MemoryChunkHeader* pBlock = _pHead;
			if (pBlock == nullptr)
			{
				return nullptr;
			}
			
			while (pBlock)
			{
				//if the chunk havn't been used, and it has engouh size to satisfied the requirement
				// then we find the suitable chunk for user.
				if (!pBlock->Used() && requiredSize < pBlock->GetSize())
				{
					return pBlock;
				}
				pBlock = pBlock->GetNextPtr();
			}
			return nullptr;
		}

		//get the ChunkHeader by address
		MemoryChunkHeader* MemoryHeapManager::GetTheChunkContainsAddress(void* i_ptr)
		{
			MemoryChunkHeader* pBlock = _pHead;
			if (pBlock == nullptr)
			{
				return nullptr;
			}

			while (pBlock)
			{
				if (pBlock->GetAddress() == i_ptr)
				{
					return pBlock;
				}
				pBlock = pBlock->GetNextPtr();
			}
			return nullptr;
		}

		//merge two chunk headers
		MemoryChunkHeader* MemoryHeapManager::MergeTwoChunkHeaders(MemoryChunkHeader* pFirstChunk, MemoryChunkHeader* pSecondChunk)
		{
			if (pFirstChunk == nullptr || pSecondChunk == nullptr)
			{
				return pFirstChunk;
			}
			size_t theSecondSize = sizeof(MemoryChunkHeader) + pSecondChunk->GetSize();
			MemoryChunkHeader* pSecondNext = pSecondChunk->GetNextPtr();
			pFirstChunk->ReSize(pFirstChunk->GetSize() + theSecondSize);
			pFirstChunk->SetNextPtr(pSecondNext);
			if (pSecondNext)
			{
				pSecondNext->SetPrevPtr(pFirstChunk);
			}
			placement_delete<MemoryChunkHeader>(pSecondChunk, 1);
			if (pFirstChunk->GetAddress())
			{
			//	Implements::SetMem((char*)(pFirstChunk->GetAddress()), pFirstChunk->GetSize(), 0);
			}
			return pFirstChunk;
		}

	}
}