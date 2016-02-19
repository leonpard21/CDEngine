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

		/*
		 * This structure describe the uniform block.
		 * It contains 3 parts:
		 * @_offsetInUniformPackageNameBuffer will point to the name of the Uniform Block
		 * @_startUniformDescIndex is the start index of the UniformDesc contained in this block
		 * @_endUniformDescIndex is the last index of the UniformDesc contained in this block
		 * Remember that by default, there is a global Uniform Block.
		 */
		struct UniformBlockDesc 
		{
			UniformBlockDesc() = default;
			uint32_t _offsetInUniformPackageNameBuffer;
			uint32_t _startUniformDescIndex;
			uint32_t _endUniformDescIndex;
		};

		struct UniformDesc
		{
			tUniformHandle _handle;
			uint32_t _offsetInValueBuffer;
			uint32_t _valueBufferSize;
			uint32_t _offsetInNameBuffer;
			ShaderTypes _shaderType;
			UniformDesc() : _handle(0), _offsetInValueBuffer(0), _valueBufferSize(0), 
				_offsetInNameBuffer(0), _shaderType(ShaderTypes::Unknown) {}
			void SetHanlde(const char* pName, Effect* pEffect);
		};
	}
}

#endif//EAE_ENGINE_UNIFORM_DESC_H