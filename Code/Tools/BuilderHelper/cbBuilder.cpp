// Header Files
//=============

#include "cbBuilder.h"
#include <sstream>

// Interface
//==========

// Build
//------

bool EAE_Engine::Tools::cbBuilder::ParseCommandArgumentsAndBuild( char** i_arguments, const unsigned int i_argumentCount )
{
	const unsigned int commandCount = 1;
	const unsigned int actualArgumentCount = i_argumentCount - commandCount;
	const unsigned int requiredArgumentCount = 2;
	if ( actualArgumentCount >= requiredArgumentCount )
	{
		_path_source = i_arguments[commandCount + 0];
		_path_target = i_arguments[commandCount + 1];

		std::vector<std::string> optionalArguments;
		for ( unsigned int i = ( commandCount + requiredArgumentCount ); i < i_argumentCount; ++i )
		{
			optionalArguments.push_back( i_arguments[i] );
		}
		return Build( optionalArguments );// Here we call the Build function we implemented in the child classes.
	}
	else
	{
		std::stringstream errorMessage;
		errorMessage << "An asset builder must be called with at least " << requiredArgumentCount << " command line arguments "
			"(the source path and the target path), but ";
		if ( actualArgumentCount == 0 )
		{
			errorMessage << "none were provided\n";
		}
		else
		{
			errorMessage << "only " << actualArgumentCount << " were provided\n";
		}
		EAE_Engine::Tools::OutputErrorMessage( errorMessage.str().c_str() );
		return false;
	}
}

// Initialization / Shut Down
//---------------------------

EAE_Engine::Tools::cbBuilder::cbBuilder():
	_path_source( NULL ), _path_target( NULL )
{

}
