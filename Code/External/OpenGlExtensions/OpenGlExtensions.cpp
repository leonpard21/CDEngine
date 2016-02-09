// Header Files
//=============

#include "OpenGlExtensions.h"

#include <sstream>
#include "../../Engine/Windows/WindowsFunctions.h"

// Helper Function Declarations
//=============================

namespace
{
	void* GetGlFunctionAddress( const char* i_functionName, std::string* o_errorMessage = NULL );
}

// Interface
//==========

// OpenGL Extension Definitions
//-----------------------------

PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
PFNGLMAPBUFFERPROC glMapBuffer = NULL;
PFNGLUNMAPBUFFERPROC glUnmapBuffer = NULL;
//PFNGLBUFFERSTORAGEPROC glBufferStorage = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArray = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArray = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLUNIFORM1FVPROC glUniform1fv = NULL;
PFNGLUNIFORM2FVPROC glUniform2fv = NULL;
PFNGLUNIFORM3FVPROC glUniform3fv = NULL;
PFNGLUNIFORM4FVPROC glUniform4fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = NULL;
PFNGLUNIFORM1IPROC glUniform1i = NULL;

PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex = NULL;
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding = NULL;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv = NULL;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName = NULL;

PFNGLBINDBUFFERBASEPROC glBindBufferBase = NULL;


// Initialization
//---------------

bool EAE_Engine::OpenGlExtensions::Load( std::string* o_errorMessage )
{
	// A current OpenGL context must exist before extensions can be loaded
	{
		const HGLRC currentContext = wglGetCurrentContext();
		if ( currentContext == NULL )
		{
			if ( o_errorMessage )
			{
				*o_errorMessage = "OpenGL extensions can't be loaded without a current OpenGL context";
			}
			return false;
		}
	}

#define EAE_ENGINE_LOADGLFUNCTION( i_functionName, i_functionType )														\
		i_functionName = reinterpret_cast<i_functionType>( GetGlFunctionAddress( #i_functionName, o_errorMessage ) );	\
		if ( !i_functionName )																							\
			return false;

	EAE_ENGINE_LOADGLFUNCTION( glActiveTexture, PFNGLACTIVETEXTUREPROC );
	EAE_ENGINE_LOADGLFUNCTION( glAttachShader, PFNGLATTACHSHADERPROC );
	EAE_ENGINE_LOADGLFUNCTION( glBindBuffer, PFNGLBINDBUFFERPROC );
	EAE_ENGINE_LOADGLFUNCTION( glBindVertexArray, PFNGLBINDVERTEXARRAYPROC );
	EAE_ENGINE_LOADGLFUNCTION( glBufferData, PFNGLBUFFERDATAPROC );
	EAE_ENGINE_LOADGLFUNCTION( glBufferSubData, PFNGLBUFFERSUBDATAPROC);
	EAE_ENGINE_LOADGLFUNCTION( glMapBuffer, PFNGLMAPBUFFERPROC);
	EAE_ENGINE_LOADGLFUNCTION( glUnmapBuffer, PFNGLUNMAPBUFFERPROC);
	//EAE_ENGINE_LOADGLFUNCTION( glBufferStorage, PFNGLBUFFERSTORAGEPROC );
	EAE_ENGINE_LOADGLFUNCTION( glCompileShader, PFNGLCOMPILESHADERPROC );
	EAE_ENGINE_LOADGLFUNCTION( glCreateProgram, PFNGLCREATEPROGRAMPROC );
	EAE_ENGINE_LOADGLFUNCTION( glCreateShader, PFNGLCREATESHADERPROC );
	EAE_ENGINE_LOADGLFUNCTION( glDeleteBuffers, PFNGLDELETEBUFFERSPROC );
	EAE_ENGINE_LOADGLFUNCTION( glDeleteProgram, PFNGLDELETEPROGRAMPROC );
	EAE_ENGINE_LOADGLFUNCTION( glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC );
	EAE_ENGINE_LOADGLFUNCTION( glDeleteShader, PFNGLDELETESHADERPROC );
	EAE_ENGINE_LOADGLFUNCTION( glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYARBPROC );
	EAE_ENGINE_LOADGLFUNCTION( glDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYARBPROC);
	EAE_ENGINE_LOADGLFUNCTION( glGenBuffers, PFNGLGENBUFFERSPROC );
	EAE_ENGINE_LOADGLFUNCTION( glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC );
	EAE_ENGINE_LOADGLFUNCTION( glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC );
	EAE_ENGINE_LOADGLFUNCTION( glGetProgramiv, PFNGLGETPROGRAMIVPROC );
	EAE_ENGINE_LOADGLFUNCTION( glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC );
	EAE_ENGINE_LOADGLFUNCTION( glGetShaderiv, PFNGLGETSHADERIVPROC );
	EAE_ENGINE_LOADGLFUNCTION( glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC );
	EAE_ENGINE_LOADGLFUNCTION( glLinkProgram, PFNGLLINKPROGRAMPROC );
	EAE_ENGINE_LOADGLFUNCTION( glShaderSource, PFNGLSHADERSOURCEPROC );
	EAE_ENGINE_LOADGLFUNCTION( glUniform1fv, PFNGLUNIFORM1FVPROC );
	EAE_ENGINE_LOADGLFUNCTION( glUniform2fv, PFNGLUNIFORM2FVPROC );
	EAE_ENGINE_LOADGLFUNCTION( glUniform3fv, PFNGLUNIFORM3FVPROC );
	EAE_ENGINE_LOADGLFUNCTION( glUniform4fv, PFNGLUNIFORM4FVPROC );
	EAE_ENGINE_LOADGLFUNCTION( glUniformMatrix4fv, PFNGLUNIFORMMATRIX4FVPROC );
	EAE_ENGINE_LOADGLFUNCTION( glUseProgram, PFNGLUSEPROGRAMPROC );
	EAE_ENGINE_LOADGLFUNCTION( glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC );
	EAE_ENGINE_LOADGLFUNCTION( glGetActiveUniform, PFNGLGETACTIVEUNIFORMPROC );
	EAE_ENGINE_LOADGLFUNCTION( glCompressedTexImage2D, PFNGLCOMPRESSEDTEXIMAGE2DPROC);
	EAE_ENGINE_LOADGLFUNCTION( glUniform1i, PFNGLUNIFORM1IPROC);

	EAE_ENGINE_LOADGLFUNCTION(glGetUniformBlockIndex, PFNGLGETUNIFORMBLOCKINDEXPROC);
	EAE_ENGINE_LOADGLFUNCTION(glUniformBlockBinding, PFNGLUNIFORMBLOCKBINDINGPROC);
	EAE_ENGINE_LOADGLFUNCTION(glGetActiveUniformBlockiv, PFNGLGETACTIVEUNIFORMBLOCKIVPROC);
	EAE_ENGINE_LOADGLFUNCTION(glGetActiveUniformBlockName, PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC);

	EAE_ENGINE_LOADGLFUNCTION(glBindBufferBase, PFNGLBINDBUFFERBASEPROC);

#undef EAE_ENGINE_LOADGLFUNCTION

	return true;
}

// Helper Function Declarations
//=============================

namespace
{
	void* GetGlFunctionAddress( const char* i_functionName, std::string* o_errorMessage )
	{
		void* address = reinterpret_cast<void*>( wglGetProcAddress( i_functionName ) );
		// The documentation says that NULL will be returned if the function isn't found,
		// but according to https://www.opengl.org/wiki/Load_OpenGL_Functions
		// other values can be returned by some implementations
		if ( ( address != NULL )
			&& ( address != reinterpret_cast<void*>( 1 ) ) && ( address != reinterpret_cast<void*>( 2 ) )
			&& ( address != reinterpret_cast<void*>( 3 ) ) && ( address != reinterpret_cast<void*>( -1 ) ) )
		{
			return address;
		}
		std::string wglErrorMessage;
		if ( address == NULL )
		{
			wglErrorMessage = EAE_Engine::GetLastWindowsError();
		}
		// wglGetProcAddress() won't return the address of any 1.1 or earlier OpenGL functions
		// that are built into Windows' Opengl32.dll
		// but an attempt can be made to load those manually
		// in case the user of this function has made a mistake
		{
			// This library should already be loaded,
			// and so this function will just retrieve a handle to it
			HMODULE glLibrary = LoadLibrary( "Opengl32.dll" );
			if ( glLibrary != NULL )
			{
				// Look for an old OpenGL function
				void* address = reinterpret_cast<void*>( GetProcAddress( glLibrary, i_functionName ) );
				// Decrement the library's reference count
				FreeLibrary( glLibrary );
				// Return an address if it was found
				if ( address != NULL )
				{
					return address;
				}
			}
		}

		// If this code is reached the OpenGL function wasn't found
		if ( o_errorMessage )
		{
			std::stringstream errorMessage;
			errorMessage << "Windows failed to find the address of the OpenGL function \"" << i_functionName << "\"";
			if ( !wglErrorMessage.empty() )
			{
				errorMessage << ": " << wglErrorMessage;
			}
			*o_errorMessage = errorMessage.str();
		}

		return NULL;
	}
}
