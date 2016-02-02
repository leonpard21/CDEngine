#ifndef EAE_USEROUTPUT_H
#define EAE_USEROUTPUT_H
#include "Source/ConsolePrint.h"

namespace EAE_Engine
{
	namespace UserOutput
	{
		template<typename ...Args>
		void Print(const char* pStr, Args&&... args)
		{
			DEBUG_PRINT(pStr, std::forward<Args>(args)...);
		}
		void Print(const char* pStr);
	}
}

#endif//EAE_USEROUTPUT_H