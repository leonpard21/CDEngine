#ifndef ENGINE_DEBUGER_H
#define ENGINE_DEBUGER_H

#include "ConsolePrint.h"

namespace EAE_Engine
{
	namespace Debugger
	{
		class VerbosityDebugger
		{
		public:
			enum Verbosity
			{
				LEVEL0, // highest level 
				LEVEL1, // 
				LEVEL2, // 
			};

			inline static void SetGlobalVerbosity(Verbosity verbosity){ s_global_verbosity = verbosity; }

			inline static Verbosity GetGlobalVerbosity(){ return s_global_verbosity; }

		private:
			static Verbosity s_global_verbosity;
		};

		/* Template debug print function with different Verbosity
		 * The FL in the tail of the function name means this will print file name and line number
		 * i_pDebugMessage means the debug message, it may takes parameters.
		 * Verbosity means the level of the debug message. When the input i_verbosity is higher than global verbosity, then show the message.
		 * i_pFile and i_LineNo will tell the FILE name and LINE number.
		 * args... will be the parameter of the i_pDebugMessage.
		 */
		template<typename ...Args>
		inline void TDebugPrintFL(const char * i_pDebugMessage, VerbosityDebugger::Verbosity i_verbosity, const char * i_pFile, unsigned int i_LineNo, Args&&... args)
		{
			if (!i_pDebugMessage)
				return;
			// if the verbosity is higher than the global verbosity, then ouput the info. 
			if (i_verbosity<=VerbosityDebugger::GetGlobalVerbosity())
			{
				const char* pEnginePlatform = "Windows";// EAE_Engine::EngineInfo::GetEnginePlatform();
				ConsolePrintWrap::ConsolePrint("File: %s Line: %d Platform: %s\n", i_pFile, i_LineNo, pEnginePlatform);
				ConsolePrintWrap::ConsolePrint(i_pDebugMessage, args...);
			}
		}

		template<typename ...Args>
		inline void TDebugPrint(const char * i_pDebugMessage, VerbosityDebugger::Verbosity i_verbosity, Args&&... args)
		{
			if (!i_pDebugMessage)
				return;
			// if the verbosity is higher than the global verbosity, then ouput the info. 
			if (i_verbosity <= VerbosityDebugger::GetGlobalVerbosity())
			{
				ConsolePrintWrap::ConsolePrint(i_pDebugMessage, args...);
			}
		}

	}
}

#if defined(_DEBUG)  &&  !defined(DISABLE_DEBUG_PRINT)
#define TDEBUG_PRINT_FL(fmt, verbosity, ...) EAE_Engine::Debugger::TDebugPrintFL<>((fmt), verbosity, __FILE__, __LINE__, __VA_ARGS__)
#define TDEBUG_PRINT(fmt, verbosity, ...) EAE_Engine::Debugger::TDebugPrint<>((fmt), verbosity, __VA_ARGS__)
#else
// This is one way we create a do nothing (NO OP) macro that doesn't
// generate a compiler warning or error
#define TDEBUG_PRINT_FL(fmt,verbosity,...) void(0)
#define TDEBUG_PRINT(fmt,verbosity,...) void(0)
#endif



#endif //ENGINE_DEBUGER_H