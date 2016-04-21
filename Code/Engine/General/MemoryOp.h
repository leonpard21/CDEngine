#ifndef MEMORY_OP_H
#define MEMORY_OP_H
#include <cstdint>

#define SAFE_RELEASE(p) {if ( (p) != NULL ) { (p)->Release(); (p) = 0; }}
#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;
#define SAFE_DELETE_ARRAY(a) if( (a) != NULL ) delete[] (a); (a) = NULL;

namespace EAE_Engine 
{
	// set mem by value
	inline void SetMem(uint8_t* dest, size_t numOfDest, uint8_t value)
	{
		for (size_t i = 0; i < numOfDest; ++i)
		{
			dest[i] = value;
		}
	}

	// copy mem from source to destination
	inline void CopyMem(const uint8_t* source, uint8_t* dest, size_t numOfDest)
	{
		for (size_t i = 0; i < numOfDest; ++i)
		{
			dest[i] = source[i];
		}
	}

  inline bool CompareMem(const uint8_t* pMem0, const uint8_t* pMem1, size_t numOfDest)
  {
    for (size_t i = 0; i < numOfDest; ++i)
    {
      if (pMem0[i] != pMem1[i])
      {
        return false;
      }
    }
    return false;
  }

	char* CopyStr(const char* pStr);
	void DeleteStr(char* pStr);
}

#endif//MEMORY_OP_H

