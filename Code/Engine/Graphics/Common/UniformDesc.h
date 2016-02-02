#ifndef EAE_ENGINE_UNIFORM_DESC_H
#define EAE_ENGINE_UNIFORM_DESC_H

#include "../GraphicsInclude.h"
#include "Engine/Windows/WindowsIncludes.h"
#if defined( EAEENGINE_PLATFORM_GL )
#include <gl/GL.h>
#endif

// typedef for the Material Class
//==========
typedef
#if defined( EAEENGINE_PLATFORM_D3D9 )
// This is conceptually a D3DXHANDLE but is defined this way
// so that external files aren't required to specify the #include path to the DirectX SDK
const char*
#elif defined( EAEENGINE_PLATFORM_GL )
GLint
#endif
tUniformHandle;

namespace EAE_Engine
{
	namespace Graphics
	{

		class Effect;
		struct UniformDesc
		{
			tUniformHandle _handle;
			uint32_t _offsetInValueBuffer;
			uint32_t _valueBufferSize;
			ShaderTypes _shaderType;
			UniformDesc() : _handle(0), _offsetInValueBuffer(0), _valueBufferSize(0), _shaderType(ShaderTypes::Unknown) {}
			void SetHanlde(const char* pName, Effect* pEffect);
		};
	}
}

#endif//EAE_ENGINE_UNIFORM_DESC_H