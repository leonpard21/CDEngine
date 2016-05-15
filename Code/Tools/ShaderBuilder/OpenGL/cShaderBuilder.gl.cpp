// Header Files
//=============

#include "../cShaderBuilder.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include "Engine/Graphics/GraphicsInclude.h"
#include "Engine/Windows/WindowsFunctions.h"
#include "External/Mcpp/Includes.h"


// Helper Function Declarations
//=============================

namespace
{
	bool PreProcessShaderSource( const char* const i_path_source, std::string& o_shaderSource_preProcessed );
	bool SaveShaderSource( const char* const i_path, const std::string& i_source );
}

// Interface
//==========

// Build
//------

namespace EAE_Engine
{
	namespace Tools
	{
		bool cShaderBuilder::Build(const std::vector<std::string>& i_arguments)
		{
			std::string shaderSource_preProcessed;
			if (!PreProcessShaderSource(_path_source, shaderSource_preProcessed))
			{
				return false;
			}
			if (!SaveShaderSource(_path_target, shaderSource_preProcessed))
			{
				return false;
			}

			return true;
		}
	}
}

// Helper Function Definitions
//============================

namespace
{
	bool PreProcessShaderSource(const char* const i_path_source, std::string& o_shaderSource_preProcessed)
	{
		bool wereThereErrors = false;

		// mcpp consumes non-const char*s
		// and so an array of temporary strings must be allocated
		const char* arguments_const[] =
		{
			// The command (necessary even though it is being run as a function)
			"mcpp",
			// The platform #define
			"-DEAEENGINE_PLATFORM_GL",
#ifdef EAEENGINE_GRAPHICS_AREDEBUGSHADERSENABLED
			// Keep comments
			"-C",
#endif
			// Don't output #line number information
			"-P",
			// Treat unknown directives (like #version and #extension) as warnings instead of errors
			"-a",
			// The input file to pre-process
			i_path_source
		};
		const size_t argumentCount = sizeof(arguments_const) / sizeof(char*);
		char* arguments[argumentCount] = { NULL };
		for (size_t i = 0; i < argumentCount; ++i)
		{
			const size_t stringSize = strlen(arguments_const[i]) + 1;	// NULL terminator
			char* temporaryString = reinterpret_cast<char*>(malloc(stringSize));
			memcpy(temporaryString, arguments_const[i], stringSize);
			arguments[i] = temporaryString;
		}

		// Configure mcpp to output to buffers instead of files
		mcpp_use_mem_buffers(1);
		// Preprocess the file
		const int result = mcpp_lib_main(static_cast<int>(argumentCount), arguments);
		if (result == 0)
		{
			o_shaderSource_preProcessed = mcpp_get_mem_buffer(static_cast<OUTDEST>(EAE_ENGINE_::mcpp::OUTDEST::Out));
		}
		else
		{
			wereThereErrors = true;
			std::cerr << mcpp_get_mem_buffer(static_cast<OUTDEST>(EAE_ENGINE_::mcpp::OUTDEST::Err));
			goto OnExit;
		}

	OnExit:

		for (size_t i = 0; i < argumentCount; ++i)
		{
			if (arguments[i])
			{
				free(arguments[i]);
				arguments[i] = NULL;
			}
		}

		// Release mcpp's internal buffers
		mcpp_use_mem_buffers(0);

		return !wereThereErrors;
	}

	bool SaveShaderSource(const char* const i_path, const std::string& i_shader)
	{
		bool wereThereErrors = false;
		HANDLE fileHandle = INVALID_HANDLE_VALUE;

		// Open the file
		{
			const DWORD desiredAccess = FILE_GENERIC_WRITE;
			const DWORD noOtherProgramsCanShareAccess = 0;
			SECURITY_ATTRIBUTES* useDefaultSecurity = NULL;
			const DWORD alwaysCreateANewFile = CREATE_ALWAYS;
			const DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
			const HANDLE dontUseTemplateFile = NULL;
			fileHandle = CreateFile(i_path, desiredAccess, noOtherProgramsCanShareAccess,
				useDefaultSecurity, alwaysCreateANewFile, useDefaultAttributes, dontUseTemplateFile);
			if (fileHandle == INVALID_HANDLE_VALUE)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "Windows failed to open the target shader file for writing: " << EAE_Engine::GetLastWindowsError();
				EAE_Engine::Tools::OutputErrorMessage(errorMessage.str().c_str(), i_path);
				goto OnExit;
			}
		}
		// Write the modified shader source
		{
			OVERLAPPED* writeSynchronously = NULL;
			const size_t stringSize = i_shader.length();	// The NULL terminator isn't necessary
			assert(stringSize < (uint64_t(1) << 32));	// A DWORD is 32 bits
			const DWORD bytesToWriteCount = static_cast<DWORD>(stringSize);
			DWORD bytesWrittenCount;
			if (WriteFile(fileHandle, i_shader.c_str(), bytesToWriteCount, &bytesWrittenCount, writeSynchronously) != FALSE)
			{
				if (bytesWrittenCount != bytesToWriteCount)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "Windows was supposed to write out " << bytesToWriteCount << " bytes for the target shader, "
						"but only actually wrote " << bytesWrittenCount;
					EAE_Engine::Tools::OutputErrorMessage(errorMessage.str().c_str(), i_path);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "Windows failed to write the target shader: " << EAE_Engine::GetLastWindowsError();
				EAE_Engine::Tools::OutputErrorMessage(errorMessage.str().c_str(), i_path);
				goto OnExit;
			}
		}

	OnExit:

		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			if (CloseHandle(fileHandle) == FALSE)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "Windows failed to close the target shader file's handle: " << EAE_Engine::GetLastWindowsError();
				EAE_Engine::Tools::OutputErrorMessage(errorMessage.str().c_str(), i_path);
				goto OnExit;
			}
			fileHandle = INVALID_HANDLE_VALUE;
		}

		return !wereThereErrors;
	}
}