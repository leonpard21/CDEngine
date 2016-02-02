// Header Files
//=============

#include "cGenericBuilder.h"

#include <sstream>
#include "Engine/Windows/WindowsFunctions.h"

// Interface
//==========

// Build
//------

bool EAE_Engine::Tools::cGenericBuilder::Build( const std::vector<std::string>& )
{
	bool wereThereErrors = false;

	// Copy the source to the target
	{
		const bool dontFailIfTargetAlreadyExists = false;
		const bool updateTheTargetFileTime = true;
		std::string errorMessage;
		if ( !EAE_Engine::CopyFile( _path_source, _path_target, dontFailIfTargetAlreadyExists, updateTheTargetFileTime, &errorMessage ) )
		{
			wereThereErrors = true;
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "Windows failed to copy \"" << _path_source << "\" to \"" << _path_target << "\": " << errorMessage;
			OutputErrorMessage( decoratedErrorMessage.str().c_str(), __FILE__ );
		}
	}
	
	return !wereThereErrors;
}
