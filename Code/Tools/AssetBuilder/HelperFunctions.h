/*
	This example program has the helper funtions in a separate file
	to make it easier for you to keep track of program flow
*/

#ifndef EAE_TOOLS_HELPERFUNCTIONS_H
#define EAE_TOOLS_HELPERFUNCTIONS_H

#ifndef NULL
	#define NULL 0
#endif

// Function Declarations
//======================

bool BuildAsset( const char* i_relativePath );

bool InitializeAssetBuild();
// Errors can be formatted a specific way so that they show up
// in Visual Studio's "Error List" tab
void OutputErrorMessage( const char* i_errorMessage, const char* i_optionalFileName = NULL );

#endif	// EAE_TOOLS_HELPERFUNCTIONS_H
