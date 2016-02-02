// Header Files
//=============

#include "AssetBuilder.h"

#include <iostream>
#include <string>
#include "../BuilderHelper/UtilityFunctions.h"
#include "Engine/Windows/WindowsFunctions.h"
#include "../../External/Lua/Includes.h"

// Static Data Initialization
//===========================
namespace
{
	lua_State* s_luaState = NULL;
}

// Helper Function Declarations
//=============================

namespace
{
	// Initialization / Shutdown
	//--------------------------

	bool Initialize();
	bool ShutDown();

	// Lua Wrapper Functions
	//----------------------

	int luaCopyFile( lua_State* io_luaState );
	int luaCreateDirectoryIfNecessary( lua_State* io_luaState );
	int luaDoesFileExist( lua_State* io_luaState );
	int luaGetEnvironmentVariable( lua_State* io_luaState );
	int luaGetLastWriteTime( lua_State* io_luaState );
	int luaOutputErrorMessage( lua_State* io_luaState );
}

// Interface
//==========

bool EAE_Engine::Tools::BuildAssets()
{
	bool wereThereErrors = false;
	std::string scriptDir;

	if ( !Initialize() )
	{
		wereThereErrors = true;
		goto OnExit;
	}

	// Get $(ScriptDir)
	{
		std::string errorMessage;
		if ( !EAE_Engine::GetEnvironmentVariable( "ScriptDir", scriptDir, &errorMessage ) )
		{
			wereThereErrors = true;
			OutputErrorMessage( errorMessage.c_str(), __FILE__ );
			goto OnExit;
		}
	}
	// Load and execute the build script
	{
		// Load the script
		const std::string path_buildScript = scriptDir + "BuildAssets.lua";
		const int result = luaL_loadfile( s_luaState, path_buildScript.c_str() );
		if ( result == LUA_OK )
		{
			// Execute it with the asset list path as an argument
			const int argumentCount = 1;
			{
				const std::string path_assetsToBuild = scriptDir + "AssetsToBuild.lua";
				lua_pushstring( s_luaState, path_assetsToBuild.c_str() );
			}
			// The return value should be true (on success) or false (on failure)
			const int returnValueCount = 1;
			const int noMessageHandler = 0;
			if ( lua_pcall( s_luaState, argumentCount, returnValueCount, noMessageHandler ) == LUA_OK )
			{
				// Note that lua_toboolean() follows the same rules as if something then statements in Lua:
				// false or nil will evaluate to false, and anything else will evaluate to true
				// (this means that if the script doesn't return anything it will result in a build failure)
				wereThereErrors = !lua_toboolean( s_luaState, -1 );
				lua_pop( s_luaState, returnValueCount );
			}
			else
			{
				wereThereErrors = true;
				const char* errorMessage = lua_tostring( s_luaState, -1 );
				EAE_Engine::Tools::OutputErrorMessage(errorMessage, __FILE__);
				lua_pop( s_luaState, 1 );
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			const char* errorMessage = lua_tostring( s_luaState, -1 );
			EAE_Engine::Tools::OutputErrorMessage(errorMessage, __FILE__);
			lua_pop( s_luaState, 1 );

			goto OnExit;
		}
	}

OnExit:

	if ( !ShutDown() )
	{
		wereThereErrors = true;
	}

	return !wereThereErrors;
}

// Helper Function Definitions
//============================

namespace
{
	// Initialization / Shutdown
	//--------------------------

	bool Initialize()
	{
		// Create a new Lua state
		{
			s_luaState = luaL_newstate();
			if ( !s_luaState )
			{
				EAE_Engine::Tools::OutputErrorMessage( "Memory allocation error creating Lua state", __FILE__ );
				return false;
			}
		}
		// Open the standard libraries
		luaL_openlibs( s_luaState );
		// Register custom functions
		{
			lua_register( s_luaState, "CopyFile", luaCopyFile );
			lua_register( s_luaState, "CreateDirectoryIfNecessary", luaCreateDirectoryIfNecessary );
			lua_register( s_luaState, "DoesFileExist", luaDoesFileExist );
			lua_register( s_luaState, "GetEnvironmentVariable", luaGetEnvironmentVariable );
			lua_register( s_luaState, "GetLastWriteTime", luaGetLastWriteTime );
			lua_register( s_luaState, "OutputErrorMessage", luaOutputErrorMessage );
		}

		return true;
	}

	bool ShutDown()
	{
		bool wereThereErrors = false;

		if ( s_luaState )
		{
			lua_close( s_luaState );
			s_luaState = NULL;
		}

		return !wereThereErrors;
	}

	// Lua Wrapper Functions
	//----------------------

	int luaCopyFile( lua_State* io_luaState )
	{
		// Argument #1: The source path
		const char* i_path_source;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_path_source = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}
		// Argument #2: The target path
		const char* i_path_target;
		if ( lua_isstring( io_luaState, 2 ) )
		{
			i_path_target = lua_tostring( io_luaState, 2 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #2 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 2 ) );
		}

		// Copy the file
		{
			std::string errorMessage;
			// There are many reasons that a source should be rebuilt,
			// and so even if the target already exists it should just be written over
			const bool noErrorIfTargetAlreadyExists = false;
			// Since we rely on timestamps to determine when a target was built
			// its file time should be updated when the source gets copied
			const bool updateTheTargetFileTime = true;
			if ( EAE_Engine::CopyFile( i_path_source, i_path_target, noErrorIfTargetAlreadyExists, updateTheTargetFileTime, &errorMessage ) )
			{
				lua_pushboolean( io_luaState, true );
				const int returnValueCount = 1;
				return returnValueCount;
			}
			else
			{
				lua_pushboolean( io_luaState, false );
				lua_pushstring( io_luaState, errorMessage.c_str() );
				const int returnValueCount = 2;
				return returnValueCount;
			}
		}
	}

	int luaCreateDirectoryIfNecessary( lua_State* io_luaState )
	{
		// Argument #1: The path
		const char* i_path;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_path = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}

		std::string errorMessage;
		if (EAE_Engine::CreateDirectoryIfNecessary( i_path, &errorMessage ) )
		{
			const int returnValueCount = 0;
			return returnValueCount;
		}
		else
		{
			return luaL_error( io_luaState, errorMessage.c_str() );
		}
	}

	int luaDoesFileExist( lua_State* io_luaState )
	{
		// Argument #1: The path
		const char* i_path;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_path = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}

		DWORD errorCode;
		std::string errorMessage;
		if (EAE_Engine::DoesFileExist( i_path, &errorMessage, &errorCode ) )
		{
			lua_pushboolean( io_luaState, true );
			const int returnValueCount = 1;
			return returnValueCount;
		}
		else
		{
			// If DoesFileExist() returns false because the file doesn't exist
			// (i.e. the common case)
			// then the Lua function should also return false
			// (the "error message" will also be returned, although it's implied by the return value)
			if ( ( errorCode == ERROR_FILE_NOT_FOUND ) || ( errorCode == ERROR_PATH_NOT_FOUND ) )
			{
				lua_pushboolean( io_luaState, false );
				lua_pushstring( io_luaState, errorMessage.c_str() );
				const int returnValueCount = 2;
				return returnValueCount;
			}
			else
			{
				// If DoesFileExist() fails for a non-standard error, though,
				// then the Lua function will throw an error
				return luaL_error( io_luaState, errorMessage.c_str() );
			}
		}
	}

	int luaGetEnvironmentVariable( lua_State* io_luaState )
	{
		// Argument #1: The key
		const char* i_key;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_key = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}

		std::string value;
		std::string errorMessage;
		if (EAE_Engine::GetEnvironmentVariable( i_key, value, &errorMessage ) )
		{
			lua_pushstring( io_luaState, value.c_str() );
			const int returnValueCount = 1;
			return returnValueCount;
		}
		else
		{
			lua_pushboolean( io_luaState, false );
			lua_pushstring( io_luaState, errorMessage.c_str() );
			const int returnValueCount = 2;
			return returnValueCount;
		}
	}

	int luaGetLastWriteTime( lua_State* io_luaState )
	{
		// Argument #1: The path
		const char* i_path;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_path = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}

		// Get the last time that the file was written to
		uint64_t lastWriteTime;
		std::string errorMessage;
		if (EAE_Engine::GetLastWriteTime( i_path, lastWriteTime, &errorMessage ) )
		{
			lua_pushnumber( io_luaState, static_cast<lua_Number>( lastWriteTime ) );
			const int returnValueCount = 1;
			return returnValueCount;
		}
		else
		{
			return luaL_error( io_luaState, errorMessage.c_str() );
		}
	}

	int luaOutputErrorMessage( lua_State* io_luaState )
	{
		// Argument #1: The error message
		const char* i_errorMessage;
		if ( lua_isstring( io_luaState, 1 ) )
		{
			i_errorMessage = lua_tostring( io_luaState, 1 );
		}
		else
		{
			return luaL_error( io_luaState,
				"Argument #1 must be a string (instead of a %s)",
				luaL_typename( io_luaState, 1 ) );
		}
		// Argument #2: An optional file name
		const char* i_optionalFileName = NULL;
		if ( !lua_isnoneornil( io_luaState, 2 ) )
		{
			if ( lua_isstring( io_luaState, 2 ) )
			{
				i_optionalFileName = lua_tostring( io_luaState, 2 );
			}
			else
			{
				return luaL_error( io_luaState,
					"Argument #2 must be a string (instead of a %s)",
					luaL_typename( io_luaState, 2 ) );
			}
		}

		// Output the error message
		EAE_Engine::Tools::OutputErrorMessage( i_errorMessage, i_optionalFileName );

		const int returnValueCount = 0;
		return returnValueCount;
	}
}
