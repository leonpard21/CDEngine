#ifndef MEMORY_OP_H
#define MEMORY_OP_H
#include <cstdint>

#define SAFE_RELEASE(p) {if ( (p) != NULL ) { (p)->Release(); (p) = 0; }}
#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;
#define SAFE_DELETE_ARRAY(a) if( (a) != NULL ) delete[] (a); (a) = NULL;

namespace EAE_Engine 
{
	// set mem by value
	inline void SetMem(uint8_t* dest, size_t numOfDest, int value)
	{
		for (unsigned int i = 0; i < numOfDest; i++)
		{
			dest[i] = value;
		}
	}

	// copy mem from source to destination
	inline void CopyMem(const uint8_t* source, uint8_t* dest, size_t numOfDest)
	{
		for (unsigned int i = 0; i < numOfDest; i++)
		{
			dest[i] = source[i];
		}
	}

	char* CopyStr(const char* pStr);
	void DeleteStr(char* pStr);
}

#endif//MEMORY_OP_H

