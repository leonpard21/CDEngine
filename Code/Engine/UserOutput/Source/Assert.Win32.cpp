#include "Assert.h"
#include <stdio.h>
#include <Windows.h>

#include "EngineDebuger.h"

namespace EAE_Engine
{
	namespace Debugger
	{
		//Thread::Mutex g_AssertMutex("MessageAssertMutex");

		bool _MessagedAssert(const char * i_pExp, const char * i_pMessage, const char * i_pFile, unsigned int i_LineNo)
		{

			const size_t sizeMessage = 1024;
			char Message[sizeMessage] = { 0 };

			const char* pEnginePlatform = "Windows";// EngineInfo::GetEnginePlatform();

			sprintf_s(Message, sizeMessage, "ASSERT: %s\n\nFile: %s Line: %d\n System: %s\n\nMessage: %s\n\nShould we try to continue ?",
				i_pExp, i_pFile, i_LineNo, pEnginePlatform, i_pMessage);

			int result = MessageBoxA(NULL, Message, "Oh crap!", MB_YESNO);

			switch (result)
			{
			case IDYES:
				TDEBUG_PRINT("File: %s Line: %d Condition: %s TargetSystem: %s\n", EAE_Engine::Debugger::VerbosityDebugger::LEVEL1, i_pFile, i_LineNo, i_pExp, pEnginePlatform);
				TDEBUG_PRINT("Message: %s\n", EAE_Engine::Debugger::VerbosityDebugger::LEVEL1, i_pMessage);
				return true;
			default:
				return false;
			}
		}

		bool _MessagedBox(const char * i_pMessage, const char * i_pFile, unsigned int i_LineNo)
		{
			const size_t sizeMessage = 1024;
			char Message[sizeMessage] = { 0 };

			const char* pEnginePlatform = "Windows";// EngineInfo::GetEnginePlatform();

			sprintf_s(Message, sizeMessage, "File: %s Line: %d\n System: %s\n\nMessage: %s\n\nShould we try to continue ?",
				i_pFile, i_LineNo, pEnginePlatform, i_pMessage);

			int result = MessageBoxA(NULL, Message, "Oh crap!", MB_YESNO);

			switch (result)
			{
			case IDYES:
				TDEBUG_PRINT("File: %s Line: %d TargetSystem: %s\n", EAE_Engine::Debugger::VerbosityDebugger::LEVEL1, i_pFile, i_LineNo, pEnginePlatform);
				TDEBUG_PRINT("Message: %s\n", EAE_Engine::Debugger::VerbosityDebugger::LEVEL1, i_pMessage);
				return true;
			default:
				return false;
			}
		}
	}
	
} // namespace Engine