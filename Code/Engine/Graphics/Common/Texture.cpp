#include "Texture.h"
#include "General/MemoryOp.h"
#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3d9.h>
#elif defined( EAEENGINE_PLATFORM_GL )
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif

namespace EAE_Engine
{
	namespace Graphics
	{
		///////////////////////////////TextureManager static members//////////////////////////////
		TextureManager* TextureManager::s_pTextureManager = nullptr;

		TextureManager* TextureManager::GetInstance()
		{
			if (!s_pTextureManager)
				s_pTextureManager = new TextureManager();
			return s_pTextureManager;
		}

		void TextureManager::CleanInstance()
		{
			if (!s_pTextureManager)
				return;
			s_pTextureManager->Clean();
			SAFE_DELETE(s_pTextureManager);
		}

		////////////////////////////////////member function//////////////////////////////////////

	}
}

