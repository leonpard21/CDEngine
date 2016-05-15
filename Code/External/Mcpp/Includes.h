/*
	This file can be included by a project that wants to use MCPP

	Using this intermediate file to store the version number
	means that no other source files will have to change
	if you ever change to a new version.
*/

#ifndef EAE_ENGINE__MCPPINCLUDES_H
#define EAE_ENGINE__MCPPINCLUDES_H

// Windows and mcpp both define a global "OUT".
// This is an example of why C++ namespaces are a good thing.
#if defined( _WINDOWS_ ) && defined( OUT )
	#define EAE_ENGINE__WINDOWS_OUT
	#undef OUT
#endif

extern "C"
{
	#include "2.7.2/src/mcpp_lib.h"
}

namespace EAE_ENGINE_
{
	namespace mcpp
	{
		namespace OUTDEST
		{
			enum eOUTDEST
			{
				Out = OUT,
				Err = ERR,
				Dbg = DBG,
			};
		}
	}
}

#ifdef EAE_ENGINE__WINDOWS_OUT
	#define OUT
	#undef EAE_ENGINE__WINDOWS_OUT
#endif

#endif	// EAE_ENGINE__MCPPINCLUDES_H
