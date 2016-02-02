// Header Files
//=============

#include "HelperFunctions.h"

#include <iostream>
#include <string>
#include "Engine/Windows/WindowsFunctions.h"

// Static Data Initialization
//===========================

namespace
{
	std::string s_AuthoredAssetDir;
	std::string s_BuiltAssetDir;
}

// Function Definitions
//=====================

bool BuildAsset( const char* i_relativePath )
{
	// Get the absolute paths to the source and target
	// (The "source" is the authored asset,
	// and the "target" is the built asset that is ready to be used in-game.
	// In this example program we will just copy the source to the target
	// and so the two will be the same,
	// but in a real asset build pipeline the two will usually be different:
	// The source will be in a format that is optimal for authoring purposes
	// and the target will be in a format that is optimal for real-time purposes.)
	const std::string path_source = s_AuthoredAssetDir + i_relativePath;
	const std::string path_target = s_BuiltAssetDir + i_relativePath;

	// If the source file doesn't exist then it can't be built
	{
		std::string errorMessage;
		if ( !EAE_Engine::DoesFileExist( path_source.c_str(), &errorMessage ) )
		{
			OutputErrorMessage( errorMessage.c_str(), path_source.c_str() );
			return false;
		}
	}

	// Decide if the target needs to be built
	bool shouldTargetBeBuilt;
	{
		// The simplest reason a target should be built is if it doesn't exist
		if ( EAE_Engine::DoesFileExist( path_target.c_str() ) )
		{
			// Even if the target exists it may be out-of-date.
			// If the source has been modified more recently than the target
			// then the target should be re-built.
			uint64_t lastWriteTime_source, lastWriteTime_target;
			{
				std::string errorMessage;
				if ( !EAE_Engine::GetLastWriteTime( path_source.c_str(), lastWriteTime_source, &errorMessage ) ||
					!EAE_Engine::GetLastWriteTime( path_target.c_str(), lastWriteTime_target, &errorMessage ) )
				{
					OutputErrorMessage( errorMessage.c_str() );
					return false;
				}
			}
			shouldTargetBeBuilt = lastWriteTime_source > lastWriteTime_target;
		}
		else
		{
			shouldTargetBeBuilt = true;
		}
	}

	// Build the target if necessary
	if ( shouldTargetBeBuilt )
	{
		std::string errorMessage;

		// Display a message to the user for each asset
		std::cout << "Building " << path_source << "\n";

		// Create the target directory if necessary
		if ( !EAE_Engine::CreateDirectoryIfNecessary( path_target, &errorMessage ) )
		{
			OutputErrorMessage( errorMessage.c_str(), path_target.c_str() );
			return false;
		}

		// Copy the source to the target
		{
			// There are many reasons that a source should be rebuilt,
			// and so even if the target already exists it should just be written over
			const bool shouldFunctionFailIfTargetAlreadyExists = false;
			// Since we rely on timestamps to determine when a target was built
			// its file time should be updated when the source gets copied
			const bool shouldTargetFileTimeBeModified = true;
			if ( !EAE_Engine::CopyFile( path_source.c_str(), path_target.c_str(),
				shouldFunctionFailIfTargetAlreadyExists,shouldTargetFileTimeBeModified,
				&errorMessage ) )
			{
				OutputErrorMessage( errorMessage.c_str(), path_target.c_str() );
				return false;
			}
		}
	}

	return true;
}

bool InitializeAssetBuild()
{
	// These environment variables are set in SolutionMacros.props
	if ( !EAE_Engine::GetEnvironmentVariable( "AuthoredAssetDir", s_AuthoredAssetDir ) )
	{
		return false;
	}
	if ( !EAE_Engine::GetEnvironmentVariable( "BuiltAssetDir", s_BuiltAssetDir ) )
	{
		return false;
	}

	return true;
}

void OutputErrorMessage( const char* i_errorMessage, const char* i_optionalFileName )
{
	// This formatting causes the errors to show up in Visual Studio's "Error List" tab
	std::cerr << ( i_optionalFileName ? i_optionalFileName : "Asset Build" ) << ": error: " <<
		i_errorMessage << "\n";
}
