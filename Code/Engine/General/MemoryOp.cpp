#include "MemoryOp.h"
#include <cstring>

namespace EAE_Engine 
{
	char* CopyStr(const char* pStr) 
	{
		size_t strLen = strlen(pStr) + 1;
		char* pnewStr = new char[strLen];
		SetMem((uint8_t*)pnewStr, strLen, 0);
		CopyMem((uint8_t*)pStr, (uint8_t*)pnewStr, strLen - 1);
		return pnewStr;
	}
	void DeleteStr(char* pStr) 
	{
		SAFE_DELETE_ARRAY(pStr);
	}
}



