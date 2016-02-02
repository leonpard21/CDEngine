#ifndef MEMORY_HEAP_MANAGER_H
#define MEMORY_HEAP_MANAGER_H

#include <mutex>

namespace EAE_Engine
{
	namespace Memory
	{
		//data struct of memory management
		//header of chunk of memory
		//Each MemoryChunkHeader represents a node in the MemoryHeapManager and it also saved on the chunk of memory.
		//The chunk records the size of the chunk (not including the size of the MemoryChunkHeader), the address that our user can use
		//and the chunk has been used or not. 
		//it also includes the info of next and prev chunk so that it is easier to merge consequent chunks.
		//
		//Because the memory address for the user should be align 4 and the MemoryChunkHeader will also be saved on the BigChunk of memory,
		//so just made the class MemoryChunkHeader be align(4) to be easier to calculate.
		__declspec(align(8)) class MemoryChunkHeader
		{
		public:
			MemoryChunkHeader() = default;
			inline void InitChunk(size_t i_size, void* i_ptr, MemoryChunkHeader* prev = nullptr, MemoryChunkHeader* pnext = nullptr);
			inline void ReSize(size_t i_size) { _size = i_size; }
			inline void* GetAddress(){ return _pMemAddress; }
			inline void SetNextPtr(MemoryChunkHeader* pNext){ _pNextChunk = pNext; }
			inline void SetPrevPtr(MemoryChunkHeader* pPrev){ _pPrevChunk = pPrev; }
			inline MemoryChunkHeader* GetNextPtr(){ return _pNextChunk; }
			inline MemoryChunkHeader* GetPrevPtr(){ return _pPrevChunk; }
			inline size_t GetSize(){ return _size; }
			inline bool Used(){ return _bUsed; }
			inline void SetUsed(bool state){ _bUsed = state; }

			//if return true, over boundry; else safe
			inline bool OverBoundry(void* i_Ptr);
		private:
			size_t _size;                     //size of memory can be used for user (not including the MemoryChunkHeader itself)
			void* _pMemAddress;               //address, where we can return for the user
			MemoryChunkHeader* _pNextChunk;   //next chunk
			MemoryChunkHeader* _pPrevChunk;   //next chunk
			bool _bUsed;                      //the chunk has been used or not.
			//char _paddle[3];
		};


		/*
		 * This HeapManager works in this way:
		 * It contains a big chunk of memory: _pMemoryAddress. So we need to Splite the big chunk we the user needs some memory
		 * At the beginning, there is one MemoryChunkHeader alloced on the big chunk and it records the information of the big chunk.\
		 * Such as the size, the address of memory where we can alloc for user, the chunk has been used or not.
		 * When the alloc function is called, 
		 * 1) Check if there is enough memory for the user. if has no spaces, end. else goto step 2)
		 * 2) Change the size of the chunk and mark it used.
		 * 3) We create a new MemoryChunkHeader structure, in the MemoryChunkHeader we set the reset size of the Big Chunk,
		 * and address(where the start of the reset of the big chunk), relationship with previous chunk. 
		 * 4) When Free the chunk, we should also check the chunk's prev and next chunk is used or not so that we can merge chunks.
		 * 
		 * BTW, the memory address for the user should be align 4.
		 */
		class MemoryHeapManager
		{
		public:
			static MemoryHeapManager* Create(size_t i_size);
			static void Destroy(MemoryHeapManager* pAddress);
		public:
			
			~MemoryHeapManager();

			
			void* Alloc(size_t i_size);
			void Free(void* i_ptr);
			bool CheckMemoryLeaks();
		private:
			MemoryHeapManager() = delete;
			MemoryHeapManager(void* pAddress, size_t size);
			//Find the First suitable Chunk of Memory for user
			MemoryChunkHeader* GetFirstSuitbaleChunk(size_t requiredSize);
			//create a new Chunk of Memory
			MemoryChunkHeader* CreateNewChunk(void* pAddress, size_t size);
			//get the ChunkHeader by address
			MemoryChunkHeader* GetTheChunkContainsAddress(void* i_ptr);
			MemoryChunkHeader* MergeTwoChunkHeaders(MemoryChunkHeader* pFirstChunk, MemoryChunkHeader* pSecondChunk);
			

		private:
			size_t _size;             // size of memory that this MemoryHeapManager is managing
			void* _pMemoryAddress;    // memory that this MemoryHeapManager is managing
			MemoryChunkHeader* _pHead;// point to the first memorychunkheader.
			std::mutex _heapMutex;

			
		};
	}
}


#endif//MEMORY_HEAP_MANAGER_H