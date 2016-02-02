#ifndef ASSERT_H
#define ASSERT_H

#include "Engine/General/Target.h"
#include "EngineDebuger.h"


namespace EAE_Engine
{
	namespace Debugger
	{
		bool _MessagedAssert(const char * i_pExp, const char * i_pMessage, const char * i_pFile, unsigned int i_LineNo);
		// , EAE_Engine::Debugger::VerbosityDebugger::Verbosity verbosity = EAE_Engine::Debugger::VerbosityDebugger::Verbosity::LEVEL0);
		bool _MessagedBox(const char * i_pMessage, const char * i_pFile, unsigned int i_LineNo);
	}

}

#define ErrorMessageBox(_Message)      if(!EAE_Engine::Debugger::_MessagedBox(_Message, __FILE__, __LINE__)) { DEBUGGER_BREAK; }

#ifdef NDEBUG

#define MessagedAssert(_Expression, _Message)     ((void)0)

#else

#define MessagedAssert(_Expression, _Message)      if( (!(_Expression)) && !EAE_Engine::Debugger::_MessagedAssert(#_Expression, _Message, __FILE__, __LINE__) ) { DEBUGGER_BREAK; }


#endif  // NDEBUG

#endif // ASSERT_H