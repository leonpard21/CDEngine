/*
	The main() function is where the program starts execution
*/

// Header Files
//=============

#include "cShaderBuilder.h"

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	return  EAE_Engine::Tools::Build<EAE_Engine::Tools::cShaderBuilder>( i_arguments, i_argumentCount );
}
