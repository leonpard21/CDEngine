/*
	The main() function is where the program starts execution
*/

// Header Files
//=============

#include "AssetBuilder.h"
#include <cstdlib>

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	if (EAE_Engine::Tools::BuildAssets() )
	{
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}
}
