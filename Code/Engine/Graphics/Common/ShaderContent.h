#ifndef EAE_ENGINE_GRAPHICS_SHADERCONTENT
#define EAE_ENGINE_GRAPHICS_SHADERCONTENT
#include <map>
#include "General/EngineObj.h"
#include "Graphics/GraphicsInclude.h"
#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3d9.h>
#include <d3dx9shader.h>
#elif defined( EAEENGINE_PLATFORM_GL )
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#endif



namespace EAE_Engine
{
	namespace Graphics
	{
		class ShaderContent 
		{
			ShaderContent() = delete;
		public:
			ShaderContent(uint8_t* pBuffer, size_t sizeOfBuffer);
			~ShaderContent();
			inline uint8_t* GetBufferPtr() { return _pShaderBuffer; }

		private:
			size_t _sizeOfBuffer;
			uint8_t* _pShaderBuffer;
		};

	}
}


#endif//EAE_ENGINE_GRAPHICS_SHADERCONTENT

