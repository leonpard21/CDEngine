#include "Bitfield.h"
#include "MemoryNew.h"
#include "General/MemoryOp.h"
#include <cmath>
using namespace EAE_Engine::Memory;

Bitfield::Bitfield() :_pBitfieldSlot(nullptr), _numOfBitFieldSlot(0), _numOfBlocks(0), _align(EAE_Engine::Memory::NEW_ALIGN_8)
{
}

Bitfield::~Bitfield()
{
	//SAFE_DELETE_ARRAY(_pBitfieldSlot);
	switch (_align)
	{
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
		align_delete<Bitfield_Byte>(_pBitfieldSlot, _numOfBitFieldSlot, EAE_Engine::Memory::NEW_ALIGN_8);
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
		align_delete<Bitfield_Word>(_pBitfieldSlot, _numOfBitFieldSlot, EAE_Engine::Memory::NEW_ALIGN_16);
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
		align_delete<Bitfield_DWord>(_pBitfieldSlot, _numOfBitFieldSlot, EAE_Engine::Memory::NEW_ALIGN_32);
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
		align_delete<Bitfield_QWord>(_pBitfieldSlot, _numOfBitFieldSlot, EAE_Engine::Memory::NEW_ALIGN_64);
		break;
	default:
		break;
	}
	_align = EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8;
	_numOfBitFieldSlot = 0;
	_numOfBlocks = 0;
}

//create the bitfield by different BitAlign
Bitfield* Bitfield::Create(size_t numOfBlocks, EAE_Engine::Memory::NewAlignment align)
{
	//Bitfield* pBitfield = (Bitfield*)align_new<Bitfield>(1, EAE_Engine::Memory::NEW_ALIGN_64)
	NewAlignment memAlignment = EAE_Engine::Memory::NEW_ALIGN_8;
	Bitfield* pBitfield = (Bitfield*)EAE_Engine::Memory::align_malloc(sizeof(Bitfield) * 1, memAlignment);
	if (pBitfield == nullptr)
	{
		return nullptr;
	}
	pBitfield->Bitfield::Bitfield();
	pBitfield->GenerateBitfieldForBlocks(numOfBlocks, align);
	return pBitfield;
}


void Bitfield::Destory(Bitfield* pBitfield)
{
	//align_delete<Bitfield>( pBitfield, 1, EAE_Engine::Memory::NEW_ALIGN_64);// new Bitfield();
	pBitfield->Bitfield::~Bitfield();
	EAE_Engine::Memory::align_free(pBitfield);
}

void Bitfield::GenerateBitfieldForBlocks(size_t numOfBlocks, EAE_Engine::Memory::NewAlignment align)
{
	_numOfBitFieldSlot = (size_t)ceil((float)numOfBlocks / (float)align);//ceil means round up value 
	if (_numOfBitFieldSlot > 0)
	{
		_numOfBlocks = numOfBlocks;
		switch (align)
		{
		case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
			_pBitfieldSlot = (Bitfield_Byte*)align_new<Bitfield_Byte>(_numOfBitFieldSlot, EAE_Engine::Memory::NEW_ALIGN_8);
			break;
		case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
			_pBitfieldSlot = (Bitfield_Word*)align_new<Bitfield_Word>(_numOfBitFieldSlot, EAE_Engine::Memory::NEW_ALIGN_16);
			break;
		case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
			_pBitfieldSlot = (Bitfield_DWord*)align_new<Bitfield_DWord>(_numOfBitFieldSlot, EAE_Engine::Memory::NEW_ALIGN_32);
			break;
		case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
			_pBitfieldSlot = (Bitfield_QWord*)align_new<Bitfield_QWord>(_numOfBitFieldSlot, EAE_Engine::Memory::NEW_ALIGN_64);
			break;
		default:
			break;
		}
		if (_pBitfieldSlot)
		{
			_align = align;
			SetMem((uint8_t*)_pBitfieldSlot, _numOfBitFieldSlot*_align / EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8, 0);
		}
			
	}
}

// get the count of free bits in the bitfield
size_t Bitfield::GetCountOfFreeBlocks()
{
	size_t result = 0;
	for (size_t i = 0; i < this->_numOfBlocks; i++)
	{
		//if one bit is free
		if (CheckBitInField(i) == 0)
		{
			result++;
		}
	}
	return result;
}

//check there are consequence blocks that can hold the num of blocks or not.
//if there are enough spaces, return the start bit in the bitfield
//else return UINT_MAX, as the illegal value.
size_t Bitfield::CheckEnoughSpaces(size_t numOfBlocksRequired)
{
	size_t numoffreeblocks = 0;
	size_t start = _numOfBlocks;
	//loop until finished all the bits or get enough bits
	for (size_t i = 0; (i < this->_numOfBlocks) && (numoffreeblocks != numOfBlocksRequired); i++)
	{
		//if one bit is free
		if (CheckBitInField(i) == 0)
		{
			numoffreeblocks == 0 ? start = i : start;//update the start bit when just find a free bit.
			numoffreeblocks++;                       //update the size of the bit.
		}
		else//else need to restart count.
		{
			numoffreeblocks = 0;
			start = UINT_MAX;
		}
	}
	//if have enough blocks of free memory, return the start address of bit that have enough blocks
	if (numoffreeblocks == numOfBlocksRequired && start != _numOfBlocks)
	{
		return start;
	}
	//else means don't have enough blocks of free memory, return UINT_MAX as illeage value
	return UINT_MAX;
}

//write several bits, if write sucess, return true; else return false.
bool Bitfield::WriteBits(size_t start, size_t numOfbits)
{
	if (start + numOfbits>_numOfBlocks)
	{
		return false;
	}
	if (!CheckWriteBitsSafty(start, numOfbits))
	{
		return false;
	}
	//set all the bit as used.
	for (size_t i = start; i < start + numOfbits && i<_numOfBlocks; i++)
	{
		SetBitInField(i);
	}
	return true;
}

//erase several bits, fromt the start bit, erase the amount of numofbits bits.
void Bitfield::EraseBits(size_t start, size_t numOfbits)
{
	//set all the bit as used.
	for (size_t i = start; i < start + numOfbits && i<_numOfBlocks; i++)
	{
		ClearBitInField(i);
	}
}


//Chec write the bits is Safty or not. return true for safe and false or not.
bool Bitfield::CheckWriteBitsSafty(size_t start, size_t numOfbits)
{
	//set all the bit as used.
	for (size_t i = start; i < start + numOfbits && i<_numOfBlocks; i++)
	{
		if (this->CheckBitInField(i))
		{
			return false;
		}
	}
	return true;
}


bool Bitfield::GetFristFreeBit(size_t& out_indexOfBit)
{	
	void* pBitfieldSlot = nullptr;
	size_t indexOfSlot = UINT_MAX; // index of the free slot
	out_indexOfBit = UINT_MAX;
	switch (_align)
	{
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
		pBitfieldSlot = GetFirstFreeSlot<Bitfield_Byte, uint8_t>(static_cast<Bitfield_Byte*>(_pBitfieldSlot), indexOfSlot);
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
		pBitfieldSlot = GetFirstFreeSlot<Bitfield_Word, uint16_t>(static_cast<Bitfield_Word*>(_pBitfieldSlot), indexOfSlot);
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
		pBitfieldSlot = GetFirstFreeSlot<Bitfield_DWord, uint32_t>(static_cast<Bitfield_DWord*>(_pBitfieldSlot), indexOfSlot);
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
		pBitfieldSlot = GetFirstFreeSlot<Bitfield_QWord, uint64_t>(static_cast<Bitfield_QWord*>(_pBitfieldSlot), indexOfSlot);
		break;
	default:
		break;
	}
	if (!pBitfieldSlot)
	{
		return false;
	}
	size_t indexInSlot = UINT_MAX; // index of the free bit in the slot
	switch (_align)
	{
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
		indexInSlot = GetFirstFreeBitInSlot<uint8_t>(static_cast<Bitfield_Byte*>(pBitfieldSlot)->_contain);
		out_indexOfBit = indexOfSlot * sizeof(uint8_t) + indexInSlot;
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
		indexInSlot = GetFirstFreeBitInSlot<uint16_t>(static_cast<Bitfield_Word*>(pBitfieldSlot)->_contain);
		out_indexOfBit = indexOfSlot * sizeof(uint16_t) + indexInSlot;
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
		indexInSlot = GetFirstFreeBitInSlot<uint32_t>(static_cast<Bitfield_DWord*>(pBitfieldSlot)->_contain);
		out_indexOfBit = indexOfSlot * sizeof(uint32_t) + indexInSlot;
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
		indexInSlot = GetFirstFreeBitInSlot<uint64_t>(static_cast<Bitfield_QWord*>(pBitfieldSlot)->_contain);
		out_indexOfBit = indexOfSlot * sizeof(uint64_t) + indexInSlot;
		break;
	default:
		break;
	}

	if (out_indexOfBit != UINT_MAX)
	{
		return true;
	}
	return false;

}

bool Bitfield::SetFirstFreeBit(size_t indexOfBit)
{
	void* pBitfieldSlot = nullptr;
	size_t indexOfSlot = UINT_MAX;
	switch (_align)
	{
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
		pBitfieldSlot = GetFirstFreeSlot<Bitfield_Byte, uint8_t>(static_cast<Bitfield_Byte*>(_pBitfieldSlot), indexOfSlot);
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
		pBitfieldSlot = GetFirstFreeSlot<Bitfield_Word, uint16_t>(static_cast<Bitfield_Word*>(_pBitfieldSlot), indexOfSlot);
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
		pBitfieldSlot = GetFirstFreeSlot<Bitfield_DWord, uint32_t>(static_cast<Bitfield_DWord*>(_pBitfieldSlot), indexOfSlot);
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
		pBitfieldSlot = GetFirstFreeSlot<Bitfield_QWord, uint64_t>(static_cast<Bitfield_QWord*>(_pBitfieldSlot), indexOfSlot);
		break;
	default:
		break;
	}
	bool result = false;
	if (!pBitfieldSlot)
	{
		return result;
	}
	switch (_align)
	{
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
		result = SetFirstFreeBitInSlot<uint8_t>(static_cast<Bitfield_Byte*>(pBitfieldSlot)->_contain);
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
		result = SetFirstFreeBitInSlot<uint16_t>(static_cast<Bitfield_Word*>(pBitfieldSlot)->_contain);
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
		result = SetFirstFreeBitInSlot<uint32_t>(static_cast<Bitfield_DWord*>(pBitfieldSlot)->_contain);
		break;
	case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
		result = SetFirstFreeBitInSlot<uint64_t>(static_cast<Bitfield_QWord*>(pBitfieldSlot)->_contain);
		break;
	default:
		break;
	}
	return result;
}



template<typename T, typename U>
void* Bitfield::GetFirstFreeSlot(T* pBitFieldSlot, size_t& out_indexOfSlot)
{
	T* pTempBitFieldSlot = pBitFieldSlot;
	if (pTempBitFieldSlot)
	{
		out_indexOfSlot = 0;
		//max of the type is static_cast<U>(-1), if the _contain == max, then the slot can be jump over
		U max = static_cast<U>(-1);
		while (pTempBitFieldSlot->_contain == max)
		{
			pTempBitFieldSlot++;
			out_indexOfSlot++;
		}
	}
	return pTempBitFieldSlot;
}

//UINT_MAX means didn't find the result.
//retun the index of the free bit in the slot contain.
template<typename U>
size_t Bitfield::GetFirstFreeBitInSlot(U contain)
{
	for (size_t i = 0; i < sizeof(contain); i++)
	{
		if (CheckBit<U>(contain, i) == 0)
		{
			return i;
		}
	}
	return UINT_MAX;
}


//Set the first free slot in the field
//return true for successful. false for field
template<typename U>
bool Bitfield::SetFirstFreeBitInSlot(U& contain)
{
	for (size_t i = 0; i < sizeof(contain); i++)
	{
		if (CheckBit<U>(contain, i) == 0)
		{
			SetBit<U>(contain, i);
			return true;
		}
	}
	return false;
}

