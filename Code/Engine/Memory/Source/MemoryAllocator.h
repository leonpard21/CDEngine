#ifndef MEMORY_BLOCK_ALLOCATOR_H
#define MEMORY_BLOCK_ALLOCATOR_H

#include "Bitfield.h"
#include "General/MemoryOp.h"

namespace EAE_Engine
{
	namespace Memory
	{
		/*
		 * This class represent the memory blocks that will be alloced for users.
		 * _pAddress point to the address of each memory block 
		 * _blockSize menas the bytes of this memory block
		 *
		 * _numOfAllocBlocks at first be set as 0, when this memory block has been assigned to user, 
		 * it will record how many consequent memory blocks have been alloced to the user,
		 * so that we can use this information when we free these memory blocks.
		 */
		class MemoryBlock
		{
		public:
			MemoryBlock() : _pAddress(nullptr), _blockSize(0), _numOfAllocBlocks(0){};//, _free_size(0)
			~MemoryBlock(){ _pAddress = nullptr; _blockSize = 0; _numOfAllocBlocks = 0; };//_free_size = 0; 
			
			inline void InitBlock(size_t blocksize, void* pAddress) 
			{ 
				_blockSize = blocksize;
				_pAddress = pAddress;
				SetMem((uint8_t*)_pAddress, _blockSize, 0);
				//_free_size = blocksize;
			}
			
			//alloc this memory block. If success, return true; else return false.
			inline void AllocBlock(size_t numOfSequence)
			{ 
				//_free_size = 0;
				_numOfAllocBlocks = numOfSequence;
			}

			inline void FreeBlock()
			{
				SetMem((uint8_t*)_pAddress, _blockSize, 0);
				_numOfAllocBlocks = 0;
				//_free_size = blocksize;
			}

			inline void* GetAddress(){ return _pAddress; }
			inline size_t GetNumOfAllocBlocks(){ return _numOfAllocBlocks; }

		private:
			void* _pAddress;
			size_t _blockSize;
			size_t _numOfAllocBlocks;// how many consequent memory blocks (start from this memory block) are used. 0 means free block.
			//size_t _free_size;  // how many free bytes in this memory block.
		};


		/* Memory pool class, mallco a brunches of memory blocks, 
		 * each block is represented by one bit in the bitfield.
         * _pBitfield, the Bitfield represent the blocks.
		 * _pMemoryAddress, the whole memory pool which is alloced when create the MemoryBlockAllocator
		 * _pBlocks, splits the _pMemoryAddress as different memory blocks.
		 * 
		 * Very Important: this class is suitable for alloc small blocks, but huge ones
		 */
		class MemoryBlockAllocator
		{
		public:
			static MemoryBlockAllocator* Create(size_t blockSize, size_t i_blockCount, NewAlignment alignment = NewAlignment::NEW_ALIGN_64);
			static void Destroy(MemoryBlockAllocator* pAddress);
		public:
			virtual ~MemoryBlockAllocator();

			//inline void* GetAddress(){ return _pMemoryAddress; }
			inline MemoryBlock* GetBlocks(){ return _pBlocks; }//get the address of the memory block
			bool Contains(void* i_ptr);//whether contains an address

			//return the memory (if there are enough)
			void* AllocMemory(size_t sizeOfBytes);
			//free the memory 
			void FreeMemory(void* pAddress);
			
			//for the simple alloc method, if the required memory is biger than block size, just using malloc
			void* SimpleAllocMemory(size_t sizeOfBytes);
			//for the simple Free method, if the required memory is biger than block size, just using free
			void SimpleFreeMemory(void* pAddress);

			//get the count of free blocks
			inline size_t GetCountOfFreeBlocks()
			{ 
				if (_pBitfield) 
					return _pBitfield->GetCountOfFreeBlocks(); 
				else 
					return UINT_MAX;
			}

		private:
			MemoryBlockAllocator();
			MemoryBlockAllocator(size_t  size, size_t counts, Bitfield* pBitfield, void* pAddress, NewAlignment align);

		private:
			size_t _blockSize;     // the size of each memory block
			size_t _blockCounts;   // counts of the memory blocks
			Bitfield* _pBitfield;  // bitfield address
			MemoryBlock* _pBlocks; // pointers of each block
			void* _pMemoryAddress; // the whole memory address

			NewAlignment _alignment;
		};

	}

}



#endif //MEMORY_BLOCK_ALLOCATOR_H
