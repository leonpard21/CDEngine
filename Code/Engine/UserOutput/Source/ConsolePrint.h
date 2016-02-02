#ifndef CONSOLE_PRINT_H
#define CONSOLE_PRINT_H

#include <mutex>
namespace EAE_Engine
{
	namespace Debugger
	{
		class ConsolePrintWrap
		{
		public:
			static void ConsolePrint(const char * i_fmt, ...);

		};
	}

} // Namespace Engine

// This is how we handle variable argument pre-processor macros
// This is only supported by compilers that support the C99 standard
#if defined(_DEBUG)  &&  !defined(DISABLE_DEBUG_PRINT)
#define DEBUG_PRINT(fmt,...) EAE_Engine::Debugger::ConsolePrintWrap::ConsolePrint((fmt),__VA_ARGS__)
#else
// This is one way we create a do nothing (NO OP) macro that doesn't
// generate a compiler warning or error
#define DEBUG_PRINT(fmt,...) void(0)
#endif


#endif // CONSOLE_PRINT_H