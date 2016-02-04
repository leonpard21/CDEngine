#ifndef EAE_ENGINE_GRAPHICS_TEXTURE_TYPES_H
#define EAE_ENGINE_GRAPHICS_TEXTURE_TYPES_H

#include "Engine/Windows/WindowsIncludes.h"

#if defined( EAEENGINE_PLATFORM_GL )
#include <gl/GL.h>
#endif
#include "../GraphicsInclude.h"

typedef
#if defined( EAEENGINE_PLATFORM_D3D9 )
const char*
#elif defined( EAEENGINE_PLATFORM_GL )
GLuint
#endif
tTexture;


typedef
#if defined( EAEENGINE_PLATFORM_D3D9 )
const char*
#elif defined( EAEENGINE_PLATFORM_GL )
GLuint
#endif
tSprite;

typedef
#if defined( EAEENGINE_PLATFORM_D3D9 )
DWORD
#elif defined( EAEENGINE_PLATFORM_GL )
GLint
#endif
tSamplerID;

#endif//EAE_ENGINE_GRAPHICS_TEXTURE_TYPES_H