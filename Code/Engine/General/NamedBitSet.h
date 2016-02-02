/*
* The HashedString class comes from Joe.
*/
#ifndef __NAMED_BIT_SET_H
#define __NAMED_BIT_SET_H

#include "HashString/HashedString.h"

namespace EAE_Engine
{
	template<typename T>
	class NamedBitSet
	{
		static const unsigned int c_NumBits = sizeof(T) * 8; // num bytes * (8 bits per byte)
		HashedString _bitNames[c_NumBits];
	public:
		NamedBitSet(){}
		~NamedBitSet(){}
		unsigned int GetBitIndex(const HashedString& i_Name);
		bool FindBitIndex(const HashedString& i_Name, unsigned& o_BitIndex);

		T GetBitMask(const HashedString& i_Name);
		bool FindBitMask(const HashedString& i_Name, T& o_BitMask);

		static const unsigned int GetNumBits() { return c_NumBits; }
	};
} 

#include "NamedBitSet.inl"

#endif // __NAMED_BIT_SET_H