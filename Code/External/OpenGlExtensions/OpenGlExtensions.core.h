/*
	This file can be used by any project that needs to use OpenGL extensions
*/

#ifndef EAE_Engine_OPENGLEXTENSIONS_FUNCTIONS_H
#define EAE_Engine_OPENGLEXTENSIONS_FUNCTIONS_H

// Header Files
//=============
// <windows.h> must be #included before <gl/GL.h>
#include "../../Engine/Windows/WindowsIncludes.h"
// The core extension headers come from https://www.opengl.org/registry/
#include "32155/glcorearb.h"

#include <string>

// Interface
//==========

// OpenGL Extension Declarations
//------------------------------
/*
extern PFNGLGETERRORPROC glGetError;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLUNIFORM1FVPROC glUniform1fv;
extern PFNGLUNIFORM2FVPROC glUniform2fv;
extern PFNGLUNIFORM3FVPROC glUniform3fv;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
extern PFNGLUNIFORM1IPROC glUniform1i;
*/

// Initialization
//---------------

namespace EAE_Engine
{
	namespace OpenGlExtensions
	{
		bool LoadCore( std::string* o_errorMessage = NULL );
	}
}

#endif	// EAE_Engine_OPENGLEXTENSIONS_FUNCTIONS_H
