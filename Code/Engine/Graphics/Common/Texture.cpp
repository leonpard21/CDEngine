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

		int TextureManager::GetIndex(TextureInfo* pTextureInfo)
		{
			int index = 0;
			for (std::map<const char*, TextureInfo>::const_iterator iter = _textures.begin(); iter != _textures.end(); ++iter, ++index)
			{
				if (&iter->second == pTextureInfo)
					return index;
			}
			return -1;
		}

	}
}

