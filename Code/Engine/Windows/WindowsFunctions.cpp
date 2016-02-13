// Header Files
//=============

#include "WindowsFunctions.h"

#include <iostream>
#include <ShlObj.h>
#include <sstream>
#include "Shlwapi.h"
#include <vector>
// Interface
//==========

bool EAE_Engine::CopyFile( const char* const i_path_source, const char* const i_path_target,
	const bool i_shouldFunctionFailIfTargetAlreadyExists, const bool i_shouldTargetFileTimeBeModified,
	std::string* o_errorMessage )
{
	if ( ::CopyFile( i_path_source, i_path_target, i_shouldFunctionFailIfTargetAlreadyExists ) != FALSE )
	{
		if ( i_shouldTargetFileTimeBeModified )
		{
			// Get the current system time
			FILETIME fileTime;
			{
				SYSTEMTIME systemTime;
				GetSystemTime( &systemTime );
				if ( SystemTimeToFileTime( &systemTime, &fileTime ) == FALSE )
				{
					if ( o_errorMessage )
					{
						*o_errorMessage = GetLastWindowsError();
					}
					return false;
				}
			}
			// Set the current system time as the file's last write time
			{
				bool wereThereErrors = false;

				HANDLE fileHandle = INVALID_HANDLE_VALUE;
				{
					const DWORD desiredAccess = FILE_WRITE_ATTRIBUTES;
					const DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
					SECURITY_ATTRIBUTES* useDefaultSecurity = NULL;
					const DWORD onlySucceedIfFileExists = OPEN_EXISTING;
					const DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
					const HANDLE dontUseTemplateFile = NULL;
					fileHandle = CreateFile( i_path_target, desiredAccess, otherProgramsCanStillReadTheFile,
						useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile );
					if ( fileHandle == INVALID_HANDLE_VALUE )
					{
						wereThereErrors = true;
						if ( o_errorMessage )
						{
							*o_errorMessage = GetLastWindowsError();
						}
						goto OnDoneChangingTime;
					}
				}
				FILETIME* const onlyChangeLastWriteTime = NULL;
				if ( SetFileTime( fileHandle, onlyChangeLastWriteTime, onlyChangeLastWriteTime, &fileTime ) == FALSE )
				{
					wereThereErrors = true;
					if ( o_errorMessage )
					{
						*o_errorMessage = GetLastWindowsError();
					}
					goto OnDoneChangingTime;
				}

			OnDoneChangingTime:

				if ( fileHandle != INVALID_HANDLE_VALUE )
				{
					if ( CloseHandle( fileHandle ) == FALSE )
					{
						wereThereErrors = true;
						if ( o_errorMessage )
						{
							*o_errorMessage += "\n";
							*o_errorMessage += GetLastWindowsError();
						}
					}
					fileHandle = INVALID_HANDLE_VALUE;
				}
				if ( wereThereErrors )
				{
					return false;
				}
			}
		}
		return true;
	}
	else
	{
		if ( o_errorMessage )
		{
			*o_errorMessage = GetLastWindowsError();
		}
		return false;
	}
}

bool EAE_Engine::CreateDirectoryIfNecessary( const std::string& i_path, std::string* o_errorMessage )
{
	// If the path is to a file (likely), remove it so that only the directory remains
	std::string directory;
	{
		size_t pos_slash = i_path.find_last_of( "\\/" );
		if ( pos_slash != std::string::npos )
		{
			directory = i_path.substr( 0, pos_slash );
		}
		else
		{
			directory = i_path;
		}
	}
	// Get the path in a form Windows likes (without any ".."s).
	// Windows requires a character buffer
	// to copy the path variable into.
	// An arbitrary value is chosen that "should" be "big enough":
	const DWORD maxCharacterCount = MAX_PATH;
	char buffer[maxCharacterCount];
	{
		char** pathIsDirectory = NULL;
		DWORD characterCount = GetFullPathName( directory.c_str(), maxCharacterCount, buffer, pathIsDirectory );
		if ( characterCount > 0 )
		{
			if ( characterCount <= maxCharacterCount )
			{
				// Create the directory
				int result;
				{
					HWND noWindowIsDisplayed = NULL;
					const SECURITY_ATTRIBUTES* useDefaultSecurityAttributes = NULL;
					result = SHCreateDirectoryEx( noWindowIsDisplayed, buffer, useDefaultSecurityAttributes );
				}
				if ( result == ERROR_SUCCESS )
				{
					std::cout << "Created directory " << buffer << "\n";
					return true;
				}
				else if ( ( result == ERROR_FILE_EXISTS ) || ( result== ERROR_ALREADY_EXISTS ) )
				{
					return true;
				}
				else
				{
					if ( o_errorMessage )
					{
						*o_errorMessage = EAE_Engine::GetFormattedWindowsError( result );
					}
					return false;
				}
			}
			else
			{
				// If you're seeing this error you will need to increase maxCharacterCount
				if ( o_errorMessage )
				{
					std::stringstream errorMessage;
					errorMessage << "The full path of \"" << directory << "\" requires " << characterCount <<
						" characters and the provided buffer only has room for " << maxCharacterCount;
					*o_errorMessage = errorMessage.str();
				}
				return false;
			}
		}
		else
		{
			if ( o_errorMessage )
			{
				*o_errorMessage = EAE_Engine::GetLastWindowsError();
			}
			return false;
		}
	}
}

bool EAE_Engine::DoesFileExist( const char* const i_path, std::string* o_errorMessage, DWORD* o_errorCode )
{
	// Try to get information about the file
	WIN32_FIND_DATA fileData;
	HANDLE file = FindFirstFile( i_path, &fileData );
	if ( file != INVALID_HANDLE_VALUE )
	{
		if ( FindClose( file ) == FALSE )
		{
			std::stringstream errorMessage;
			errorMessage << "Windows failed to close the file handle to \"" << i_path << "\": " << GetLastWindowsError();
			MessageBox( NULL, errorMessage.str().c_str(), "Error Closing File Handle", MB_OK | MB_ICONERROR );
		}
		return true;
	}
	else
	{
		if ( o_errorMessage )
		{
			*o_errorMessage = EAE_Engine::GetLastWindowsError( o_errorCode );
		}
		return false;
	}
}

bool EAE_Engine::ExecuteCommand( const char* const i_command, DWORD* o_exitCode, std::string* o_errorMessage )
{
	return ExecuteCommand( NULL, i_command, o_exitCode, o_errorMessage );
}

bool EAE_Engine::ExecuteCommand( const char* const i_command, const char* const i_optionalArguments, DWORD* o_exitCode, std::string* o_errorMessage )
{
	bool wereThereErrors = false;

	// Copy the const arguments into a non-const buffer
	const DWORD bufferSize = 512;
	char arguments[bufferSize] = { 0 };
	if ( i_optionalArguments )
	{
		size_t argumentLength = strlen( i_optionalArguments );
		if ( bufferSize > argumentLength )
		{
			strcpy_s( arguments, bufferSize, i_optionalArguments );
		}
		else
		{
			if ( o_errorMessage )
			{
				std::ostringstream errorMessage;
				errorMessage << "Couldn't copy the command (of length " << argumentLength
					<< ") into a non-const buffer of size " << bufferSize;
				*o_errorMessage = errorMessage.str();
			}
			return false;
		}
	}
	// Start a new process
	SECURITY_ATTRIBUTES* useDefaultAttributes = NULL;
	const BOOL dontInheritHandles = FALSE;
	const DWORD createDefaultProcess = 0;
	void* useCallingProcessEnvironment = NULL;
	const char* useCallingProcessCurrentDirectory = NULL;
	STARTUPINFO startupInfo = { 0 };
	{
		startupInfo.cb = sizeof( startupInfo );
	}
	PROCESS_INFORMATION processInformation = { 0 };
	if ( CreateProcess( i_command, arguments, useDefaultAttributes, useDefaultAttributes,
		dontInheritHandles, createDefaultProcess, useCallingProcessEnvironment, useCallingProcessCurrentDirectory,
		&startupInfo, &processInformation ) != FALSE )
	{
		// Wait for the process to finish
		if ( WaitForSingleObject( processInformation.hProcess, INFINITE ) != WAIT_FAILED )
		{
			// Get the exit code
			if ( o_exitCode )
			{
				if ( GetExitCodeProcess( processInformation.hProcess, o_exitCode ) == FALSE )
				{
					wereThereErrors = true;
					if ( o_errorMessage )
					{
						std::stringstream errorMessage;
						errorMessage << "Windows failed to get the exit code of the process \"" << i_command <<
							"\": " << GetLastWindowsError();
						*o_errorMessage = errorMessage.str();
					}
				}
			}
		}
		else
		{
			wereThereErrors = true;
			if ( o_errorMessage )
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to wait for the process \"" << i_command <<
					"\" to finish: " << GetLastWindowsError();
				*o_errorMessage = errorMessage.str();
			}
		}
		// Close the process handles
		if ( CloseHandle( processInformation.hProcess ) == FALSE )
		{
			std::stringstream errorMessage;
			errorMessage << "Windows failed to close the handle to the process \"" << i_command <<
				"\": " << GetLastWindowsError();
			MessageBox( NULL, errorMessage.str().c_str(), "Error Closing Process Handle", MB_OK | MB_ICONERROR );
		}
		if ( CloseHandle( processInformation.hThread ) == FALSE )
		{
			std::stringstream errorMessage;
			errorMessage << "Windows failed to close the handle to the process \"" << i_command <<
				"\" thread: " << GetLastWindowsError();
			MessageBox( NULL, errorMessage.str().c_str(), "Error Closing Process Thread Handle", MB_OK | MB_ICONERROR );
		}

		return !wereThereErrors;
	}
	else
	{
		if ( o_errorMessage )
		{
			std::stringstream errorMessage;
			errorMessage << "Windows failed to start the process \"" << i_command << "\": " << GetLastWindowsError();
			*o_errorMessage = errorMessage.str();
		}
		return false;
	}
}

bool EAE_Engine::GetEnvironmentVariable( const char* const i_key, std::string& o_value, std::string* o_errorMessage )
{
	// Windows requires a character buffer
	// to copy the environment variable into.
	// An arbitrary value is chosen that "should" be "big enough":
	const DWORD maxCharacterCount = MAX_PATH;
	char buffer[maxCharacterCount];
	// Ask Windows for the environment variable
	const DWORD characterCount = ::GetEnvironmentVariable( i_key, buffer, maxCharacterCount );
	if ( characterCount > 0 )
	{
		if ( characterCount <= maxCharacterCount )
		{
			o_value = buffer;
			return true;
		}
		else
		{
			// If you're seeing this error you will need to increase maxCharacterCount
			if ( o_errorMessage )
			{
				std::stringstream errorMessage;
				errorMessage << "The environment variable \"" << i_key << "\" requires " << characterCount <<
					" characters and the provided buffer only has room for " << maxCharacterCount;
				*o_errorMessage = errorMessage.str();
			}
			return false;
		}
	}
	else
	{
		if ( o_errorMessage )
		{
			DWORD errorCode;
			std::string errorString = GetLastWindowsError( &errorCode );
			if ( errorCode == ERROR_ENVVAR_NOT_FOUND )
			{
				// If you're seeing this error and the environment variable is spelled correctly
				// it _probably_ means that you are debugging and haven't set up the environment.
				//	* Right click the project name and choose "Properties"
				//	* In the tree view to the left select "Configuration Properties->Debugging"
				//	* In the "Environment" field make sure that you have this environment variable set like:
				//		* someKey=$(someKey)
				std::stringstream errorMessage;
				errorMessage << "The environment variable \"" << i_key << "\" doesn't exist";
				*o_errorMessage = errorMessage.str();
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to get the environment variable \"" << i_key << "\": " << errorString;
				*o_errorMessage = errorMessage.str();
			}
		}
		return false;
	}
}

std::string EAE_Engine::GetFormattedWindowsError( const DWORD i_errorCode )
{
	std::string errorMessage;
	{
		const DWORD formattingOptions =
			// Get the error message from Windows
			FORMAT_MESSAGE_FROM_SYSTEM
			// The space for the error message should be allocated by Windows
			| FORMAT_MESSAGE_ALLOCATE_BUFFER
			// Any potentially deferred inserts should be ignored
			// (i.e. the error message should be in its final form)
			| FORMAT_MESSAGE_IGNORE_INSERTS;
		const void* messageIsFromWindows = NULL;
		const DWORD useTheDefaultLanguage = 0;
		char* messageBuffer = NULL;
		const DWORD minimumCharacterCountToAllocate = 1;
		va_list* insertsAreIgnored = NULL;
		const DWORD storedCharacterCount = FormatMessage( formattingOptions, messageIsFromWindows, i_errorCode,
			useTheDefaultLanguage, reinterpret_cast<LPSTR>( &messageBuffer ), minimumCharacterCountToAllocate, insertsAreIgnored );
		if ( storedCharacterCount != 0 )
		{
			errorMessage = messageBuffer;
		}
		else
		{
			// If there's an error GetLastError() can be called again,
			// but that is too complicated for this program :)
			errorMessage = "Unknown Windows Error";
		}
		// Try to free the memory regardless of whether formatting worked or not,
		// and ignore any error messages
		LocalFree( messageBuffer );
	}
	return errorMessage;
}

std::string EAE_Engine::GetLastWindowsError( DWORD* o_optionalErrorCode )
{
	// Windows stores the error as a code
	const DWORD errorCode = GetLastError();
	if ( o_optionalErrorCode )
	{
		*o_optionalErrorCode = errorCode;
	}
	return GetFormattedWindowsError( errorCode );
}

bool EAE_Engine::GetLastWriteTime( const char* const i_path, uint64_t& o_lastWriteTime, std::string* o_errorMessage )
{
	// Get the last time that the file was written to
	ULARGE_INTEGER lastWriteTime;
	{
		WIN32_FIND_DATA fileData;
		{
			HANDLE file = FindFirstFile( i_path, &fileData );
			if ( file != INVALID_HANDLE_VALUE )
			{
				if ( FindClose( file ) == FALSE )
				{
					if ( o_errorMessage )
					{
						*o_errorMessage = GetLastWindowsError();
					}
					return false;
				}
			}
			else
			{
				if ( o_errorMessage )
				{
					*o_errorMessage = GetLastWindowsError();
				}
				return false;
			}
		}
		FILETIME fileTime = fileData.ftLastWriteTime;
		lastWriteTime.HighPart = fileTime.dwHighDateTime;
		lastWriteTime.LowPart = fileTime.dwLowDateTime;
	}
	o_lastWriteTime = static_cast<uint64_t>( lastWriteTime.QuadPart );
	return true;
}

std::string EAE_Engine::GetFileNameWithoutExtension(const char* path) 
{
	char* tempPath = _strdup(path);
	PathStripPath(tempPath);
	PathRemoveExtension(tempPath);
	std::string result = tempPath;
	if (tempPath)
		delete[] tempPath;
	return result;
}

bool EAE_Engine::GetCursorPos(POINT& o_point)
{
	return (bool)GetCursorPos(&o_point);
}

bool EAE_Engine::GetCursorPosInWindow(HWND hWnd, POINT& o_point)
{
	GetCursorPos(&o_point);
	return (bool)ScreenToClient(hWnd, &o_point);
}

HWND EAE_Engine::GeActiceWindowHandle()
{
	HWND hWnd = ::GetActiveWindow();
	return hWnd;
}

bool EAE_Engine::GetCursorPosInActiveWindow(POINT& o_point)
{
	return GetCursorPosInWindow(GeActiceWindowHandle(), o_point);
}


