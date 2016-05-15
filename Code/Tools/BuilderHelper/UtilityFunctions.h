/*
	This file contains useful utility functions for other build processes
*/

#ifndef EAE_ENGINE__TOOLS_UTILITYFUNCTIONS_H
#define EAE_ENGINE__TOOLS_UTILITYFUNCTIONS_H

#ifndef NULL
	#define NULL 0
#endif

namespace EAE_Engine
{
	namespace Tools
	{
		// Interface
		//==========

		// Errors can be formatted a specific way so that they show up
		// in Visual Studio's "Error List" tab
		void OutputErrorMessage(const char* const i_errorMessage, const char* const i_optionalFileName = NULL);
	}
}

#endif	// EAE_ENGINE__TOOLS_UTILITYFUNCTIONS_H
