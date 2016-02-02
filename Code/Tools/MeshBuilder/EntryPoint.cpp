/*
	The main() function is where the program starts execution
*/

// Header Files
//=============

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif // _DEBUG


//#ifdef DEBUG
//	#include <vld.h>
//#endif


#include "cMeshBuilder.h"

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(0);
#endif
	return EAE_Engine::Tools::Build<EAE_Engine::Tools::cMeshBuilder>( i_arguments, i_argumentCount );
}
