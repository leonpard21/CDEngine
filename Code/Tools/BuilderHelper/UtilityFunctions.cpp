// Header Files
//=============

#include "UtilityFunctions.h"
#include "Engine/UserOutput/Source/ConsolePrint.h"
#include "Engine/UserOutput/Source/Assert.h"
#include <iostream>

// Interface
//==========

void EAE_Engine::Tools::OutputErrorMessage( const char* const i_errorMessage, const char* const i_optionalFileName )
{
	// This formatting causes the errors to show up in Visual Studio's "Error List" tab
	std::cerr << ( i_optionalFileName ? i_optionalFileName : "Asset Build" ) << ": error: " << i_errorMessage << "\n";
	ErrorMessageBox(i_errorMessage);
//	DEBUG_PRINT("For file %s, Asset Build: error: %s\n", i_optionalFileName, i_errorMessage);
}
