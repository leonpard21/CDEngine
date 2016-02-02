#ifndef BITFIELD_H
#define BITFIELD_H
#include "General/BitOperate.h"
#include "MemoryNew.h"
#include <cstdio>

namespace EAE_Engine
{
	namespace Memory
	{
		//the idea is using one byte and represent its each bit to represent a bitfield
		typedef union Bitfield_Byte{
			struct BitByte{
				uint8_t zero : 1;
				uint8_t one : 1;
				uint8_t two : 1;
				uint8_t three : 1;
				uint8_t four : 1;
				uint8_t five : 1;
				uint8_t six : 1;
				uint8_t seven : 1;
			}BitByte;
			uint8_t _contain;
		}Bitfield_Byte;
		

		typedef union Bitfield_Word{
			struct BitWord{
				uint16_t zero : 1;
				uint16_t one : 1;
				uint16_t two : 1;
				uint16_t three : 1;
				uint16_t four : 1;
				uint16_t five : 1;
				uint16_t six : 1;
				uint16_t seven : 1;
				uint16_t eight : 1;
				uint16_t nine : 1;
				uint16_t ten : 1;
				uint16_t eleven : 1;
				uint16_t twelve : 1;
				uint16_t thirteen : 1;
				uint16_t fourteen : 1;
				uint16_t fifteen : 1;
			}BitWord;
			uint16_t _contain;
		}Bitfield_Word;

		//DoubleWord
		typedef union Bitfield_DWord{
			struct BitDWord{
				uint32_t zero : 1;
				uint32_t one : 1;
				uint32_t two : 1;
				uint32_t three : 1;
				uint32_t four : 1;
				uint32_t five : 1;
				uint32_t six : 1;
				uint32_t seven : 1;
				uint32_t eight : 1;
				uint32_t nine : 1;
				uint32_t ten : 1;
				uint32_t eleven : 1;
				uint32_t twelve : 1;
				uint32_t thirteen : 1;
				uint32_t fourteen : 1;
				uint32_t fifteen : 1;
				uint32_t sixteen : 1;
				uint32_t seventeen : 1;
				uint32_t eighteen : 1;
				uint32_t nineteen : 1;
				uint32_t twenty : 1;
				uint32_t twenty_one : 1;
				uint32_t twenty_two : 1;
				uint32_t twenty_three : 1;
				uint32_t twenty_four : 1;
				uint32_t twenty_five : 1;
				uint32_t twenty_six : 1;
				uint32_t twenty_seven : 1;
				uint32_t twenty_eight : 1;
				uint32_t twenty_nine : 1;
				uint32_t thirty : 1;
				uint32_t thirty_one : 1;
			}BitDWord;
			uint32_t _contain;
		}Bitfield_DWord;

		//QuadWord
		typedef union Bitfield_QWord{

			uint64_t _contain;
		}Bitfield_QWord;

		//using Bitfield_Byte array to represent bit field.
		//each bit will represent a block in class MemoryBlockAllocator
		class Bitfield
		{
		public:
			//create the bitfield by different BitAlign
			static Bitfield* Create(size_t numOfBlocks, EAE_Engine::Memory::NewAlignment align = EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8);
			static void Destory(Bitfield* pBitfield);
			~Bitfield();
		private:
			Bitfield();

			//Generate BitField based on number of blocks, deafult using BYTE
			//This function should be called only when creating the bitfield.
			void GenerateBitfieldForBlocks(size_t numOfBlocks, EAE_Engine::Memory::NewAlignment align = EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8);

		public:
			//check the state of one bit in the bitfield
			inline bool CheckBitInField(size_t indexOfBlock)
			{
				bool result = false;
				size_t slot = indexOfBlock / _align;
				size_t offset = indexOfBlock - slot * _align;
				void* pBitfieldSlot = nullptr;
				pBitfieldSlot = static_cast<uint8_t*>(_pBitfieldSlot)+slot*sizeof(_align / sizeof(uint8_t));
				switch (_align)
				{
				case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
					result = CheckBit<uint8_t>(static_cast<Bitfield_Byte*>(pBitfieldSlot)->_contain, offset);
					break;
				case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
					result = CheckBit<uint16_t>(static_cast<Bitfield_Word*>(pBitfieldSlot)->_contain, offset);
					break;
				case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
					result = CheckBit<uint32_t>(static_cast<Bitfield_DWord*>(pBitfieldSlot)->_contain, offset);
					break;
				case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
					result = CheckBit<uint64_t>(static_cast<Bitfield_QWord*>(pBitfieldSlot)->_contain, offset);
					break;
				default:
					break;
				}
				return result;
			}

			//set the state of one bit in the bitfield
			inline void SetBitInField(size_t indexOfBlock)
			{
				size_t slot = indexOfBlock / _align;
				size_t offset = indexOfBlock - slot * _align;
				void* pBitfieldSlot = nullptr;
				switch (_align)
				{
				case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
					pBitfieldSlot = static_cast<Bitfield_Byte*>(_pBitfieldSlot)+slot;
					SetBit<uint8_t>(static_cast<Bitfield_Byte*>(pBitfieldSlot)->_contain, offset);
					break;
				case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
					pBitfieldSlot = static_cast<Bitfield_Word*>(_pBitfieldSlot)+slot;
					SetBit<uint16_t>(static_cast<Bitfield_Word*>(pBitfieldSlot)->_contain, offset);
					break;
				case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
					pBitfieldSlot = static_cast<Bitfield_DWord*>(_pBitfieldSlot)+slot;
					SetBit<uint32_t>(static_cast<Bitfield_DWord*>(pBitfieldSlot)->_contain, offset);
					break;
				case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
					pBitfieldSlot = static_cast<Bitfield_QWord*>(_pBitfieldSlot)+slot;
					SetBit<uint64_t>(static_cast<Bitfield_QWord*>(pBitfieldSlot)->_contain, offset);
					break;
				default:
					break;
				}
			}

			//clear the state of one bit in the bitfield
			inline void ClearBitInField(size_t indexOfBlock)
			{
				size_t slot = indexOfBlock / _align;
				size_t offset = indexOfBlock - slot * _align;
				void* pBitfieldSlot = nullptr;
				switch (_align)
				{
				case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_8:
					pBitfieldSlot = (Bitfield_Byte*)_pBitfieldSlot + slot;
					ClearBit<uint8_t>(static_cast<Bitfield_Byte*>(pBitfieldSlot)->_contain, offset);
					break;
				case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_16:
					pBitfieldSlot = (Bitfield_Word*)_pBitfieldSlot + slot;
					ClearBit<uint16_t>(static_cast<Bitfield_Word*>(pBitfieldSlot)->_contain, offset);
					break;
				case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_32:
					pBitfieldSlot = (Bitfield_DWord*)_pBitfieldSlot + slot;
					ClearBit<uint32_t>(static_cast<Bitfield_DWord*>(pBitfieldSlot)->_contain, offset);
					break;
				case EAE_Engine::Memory::NewAlignment::NEW_ALIGN_64:
					pBitfieldSlot = (Bitfield_QWord*)_pBitfieldSlot + slot;
					ClearBit<uint64_t>(static_cast<Bitfield_QWord*>(pBitfieldSlot)->_contain, offset);
					break;
				default:
					break;
				}
			}


			inline bool operator[](size_t index)
			{
				return CheckBitInField(index);
			}

			size_t GetCountOfFreeBlocks();
			
			//check there are consequence blocks that can hold the num of blocks or not.
			//if there are enough spaces, return the start bit in the bitfield
			//else return -1, as the illegal value.
			size_t CheckEnoughSpaces(size_t numOfBlocksRequired);

			//write several bits, if write sucess, return true; else return false.
			bool WriteBits(size_t start, size_t numOfbits);
			//erase several bits, fromt the start bit, erase the amount of numofbits bits.
			void EraseBits(size_t start, size_t numOfbits);
			
			//get the first free bit in the bitfield by out_indexOfBit. UINT_MAX means no memory.
			//return true for find successful, false for failed
			bool GetFristFreeBit(size_t& out_indexOfBit);
			//Set the first free bit in the bitfield, return true for set successful, false for failed.
			bool SetFirstFreeBit(size_t indexOfBit);

		private:
			//Chec write the bits is Safty or not. return true for safe and false or not.
			bool CheckWriteBitsSafty(size_t start, size_t numOfbits);

			//get the first slot which has free bit.
			template<typename T, typename U = uint32_t> void* GetFirstFreeSlot(T* pBitFieldSlot, size_t& out_indexOfSlot);
			//get the first free bit in the offered slot, UINT_MAX means didn't find it 
			template<typename U = uint32_t> size_t GetFirstFreeBitInSlot(U contain);
			//Set the first free slot in the field, return true for successful. false for field
			template<typename U = uint32_t> bool SetFirstFreeBitInSlot(U& contain);

		private:
			void* _pBitfieldSlot;     //the slots of the blocks
			size_t _numOfBitFieldSlot;//the number of slots to represent the blocks
			size_t _numOfBlocks;      //the number of blocks actually used.
			EAE_Engine::Memory::NewAlignment _align;          //the alignment of the bitfield, can be uint8,16,32,64, and so on.
		};
	}



}


#endif //BITFIELD_H
