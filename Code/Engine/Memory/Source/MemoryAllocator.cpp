#include "MemoryAllocator.h"
#include "MemoryNew.h"
#include <cmath>
#include "UserOutput/Source/EngineDebuger.h"
#include "UserOutput/Source/Assert.h"
#include "General/MemoryOp.h"

using namespace EAE_Engine::Memory;

MemoryBlockAllocator* MemoryBlockAllocator::Create(size_t blockSize, size_t i_blockCount, NewAlignment alignment)
{
	//create bitfield by alignment 64
	Bitfield* pBitfield = Bitfield::Create(i_blockCount, alignment);
	if (!pBitfield)
	{
		return nullptr;
	}
	//alloc the whole memory blocks 
	void* pMemoryAddress = align_malloc(blockSize*i_blockCount, alignment);//use 4bit offset to save the size of the memory
	if (pMemoryAddress == nullptr)
	{
		Bitfield::Destory(pBitfield);//delete bitfield
		return nullptr;
	}
	//create the memory block allocator
	MemoryBlockAllocator* pResult = static_cast<MemoryBlockAllocator*>(align_malloc(sizeof(MemoryBlockAllocator) * 1, alignment));
	if (pResult == nullptr)
	{
		Bitfield::Destory(pBitfield);//delete bitfield
		align_free(pMemoryAddress);//free the whole memory blocks
		return nullptr;
	}
	pResult->::MemoryBlockAllocator::MemoryBlockAllocator(blockSize, i_blockCount, pBitfield, pMemoryAddress, alignment);
	
	return pResult;
}



void MemoryBlockAllocator::Destroy(MemoryBlockAllocator* pAddress)
{
	if (pAddress)
	{
		pAddress->MemoryBlockAllocator::~MemoryBlockAllocator();
		align_free(pAddress);
		pAddress = nullptr;
	}

}

MemoryBlockAllocator::MemoryBlockAllocator():
_pBitfield(nullptr), _pBlocks(nullptr), _blockSize(0), _blockCounts(0), _pMemoryAddress(nullptr), 
_alignment(NewAlignment::NEW_ALIGN_64)
{
}


MemoryBlockAllocator::~MemoryBlockAllocator()
{
	Bitfield::Destory(_pBitfield);//delete bitfield
	//SAFE_DELETE_ARRAY(_pBlocks);//delete block pointers
	align_delete<MemoryBlock>(_pBlocks, _blockCounts, _alignment);//delete block pointers
	align_free(_pMemoryAddress);//free the whole memory blocks
}


MemoryBlockAllocator::MemoryBlockAllocator(size_t  size, size_t counts, Bitfield* pBitfield, void* pAddress, NewAlignment alignment) :
_pBitfield(pBitfield), _pBlocks(nullptr), _pMemoryAddress(pAddress), _blockSize(size), _blockCounts(counts),
_alignment(alignment)
{
	_pBlocks = (MemoryBlock*)align_new<MemoryBlock>(_blockCounts, _alignment);
	SetMem((uint8_t*)_pBlocks, _blockCounts*sizeof(MemoryBlock), 0);
	MemoryBlock* pTtemp = _pBlocks;
	for (unsigned int i = 0; i < _blockCounts; i++)
	{
		char* paddress = static_cast<char*>(_pMemoryAddress)+i*_blockSize;
		pTtemp->InitBlock(_blockSize, paddress);
		pTtemp = align_get_next<MemoryBlock>(pTtemp, _alignment);
	}
}


void* MemoryBlockAllocator::AllocMemory(size_t sizeOfBytes)
{
	if (sizeOfBytes == 0){
		return nullptr;
	}

	void* pResult = nullptr;
	//calculate how much of blocks we need.
	size_t numofRequiredblocks = (size_t)ceil((float)sizeOfBytes / (float)_blockSize);
	//check whether there are enough blocks for us, index of the memory block
	size_t startBit = this->_pBitfield->CheckEnoughSpaces(numofRequiredblocks);
	//if have enough blocks for the memory
	if (startBit != UINT_MAX)
	{
		//wirte the bits in bitfield
		bool success = _pBitfield->WriteBits(startBit, numofRequiredblocks);
		if (!success)
		{
			MessagedAssert(success == true, "Memory alloc failed!");
			TDEBUG_PRINT_FL("Alloc failed! %d bytes of memory is too much to alloc\n", Debugger::VerbosityDebugger::LEVEL1, sizeOfBytes);
			return nullptr;
		}
		//find the memory block which contains the address we want to return to user
		size_t offset = align_get_boundry_offset(sizeof(MemoryBlock), _alignment);
		MemoryBlock* pTtemp = reinterpret_cast<MemoryBlock*>(reinterpret_cast<char*>(_pBlocks)+startBit * offset);
		pResult = pTtemp->GetAddress();
		//record how much of blocks we alloced
		pTtemp->AllocBlock(numofRequiredblocks);
		TDEBUG_PRINT_FL("%d bytes = %d blocks of memory has been alloced\n", Debugger::VerbosityDebugger::LEVEL1, numofRequiredblocks*_blockSize, numofRequiredblocks);
	}
	else
	{
		MessagedAssert(startBit != UINT_MAX, "Memory alloc failed!");
		TDEBUG_PRINT_FL("Alloc failed! %d bytes of memory is too much to alloc\n", Debugger::VerbosityDebugger::LEVEL1, sizeOfBytes);
	}
	return pResult;
}

void MemoryBlockAllocator::FreeMemory(void* pAddress)
{
	if (!pAddress)
	{
		MessagedAssert(pAddress != NULL, "You cannot free empty memory.");
		TDEBUG_PRINT_FL("Hey, don't free the empty address.\n", Debugger::VerbosityDebugger::LEVEL1);
		return;
	}
	if (!this->Contains(pAddress))
	{
		MessagedAssert(this->Contains(pAddress), "The address is not contained by this allocator.");
		TDEBUG_PRINT_FL("Hey, this memory is not owned by me.\n", Debugger::VerbosityDebugger::LEVEL1);
		return;
	}
	//get the memory block which contains the pAddress.
	size_t memoryOffset = (size_t)pAddress - (size_t)this->_pMemoryAddress;
	size_t startBit = memoryOffset / this->_blockSize;// index of the memory block
	size_t align_offset = align_get_boundry_offset(sizeof(MemoryBlock), _alignment);
	MemoryBlock* pBlock = reinterpret_cast<MemoryBlock*>(reinterpret_cast<char*>(_pBlocks) + startBit * align_offset);
	//read how much of blocks we alloced
	size_t numOfSequence = pBlock->GetNumOfAllocBlocks();
	if (numOfSequence != UINT_MAX)
	{
		//clear the bitfield
		this->_pBitfield->EraseBits(startBit, numOfSequence);
		//free the memory blocks, just set memory 0
		for (unsigned int i = 0; i < numOfSequence; i++)
		{
			pBlock->FreeBlock();
			pBlock = align_get_next<MemoryBlock>(pBlock, _alignment);
		}
		TDEBUG_PRINT_FL("%d bytes = %d blocks of memory has been freed\n", Debugger::VerbosityDebugger::LEVEL1, numOfSequence*_blockSize, numOfSequence);
	}
	else
	{
		MessagedAssert(numOfSequence != UINT_MAX, "Memory free failed!");
		TDEBUG_PRINT_FL("Free failed!\n", Debugger::VerbosityDebugger::LEVEL1);
	}
}

//whether contains an address
bool MemoryBlockAllocator::Contains(void* i_ptr)
{
	size_t start = reinterpret_cast<size_t>(_pMemoryAddress);
	size_t end = reinterpret_cast<size_t>(_pMemoryAddress) + _blockCounts * _blockSize;
	size_t input = reinterpret_cast<size_t>(i_ptr);
	if (input<start || input>end)
	{
		MessagedAssert((input>=start && input<=end), "Access Memory Out of Boundry!");
		return false;
	}
	return  true;
}

//for the simple alloc method, if the required memory is biger than block size, just using malloc
void* MemoryBlockAllocator::SimpleAllocMemory(size_t sizeOfBytes)
{
	if (sizeOfBytes == 0){
		return nullptr;
	}
	if (sizeOfBytes>this->_blockSize)
	{
		return malloc(sizeOfBytes);
	}
	void* pResult = nullptr;
	pResult = AllocMemory(sizeOfBytes);
	return pResult;
}

//for the simple Free method, if the required memory is biger than block size, just using free
void MemoryBlockAllocator::SimpleFreeMemory(void* pAddress)
{
	if (!pAddress)
	{
		MessagedAssert(pAddress != NULL, "You cannot free empty memory.");
		return;
	}
	
	if (this->Contains(pAddress))
	{
		FreeMemory(pAddress);
	}
	else//if the memory is not contained by this allocator, just free it.
	{
		free(pAddress);
	}
}


