#include "../Common/Sprite.h"
#include "../Common/Device.h"
#include "../Common/Texture.h"

#include "General/MemoryOp.h"
#include <string>

namespace
{
	struct SpriteVertex
	{
		SpriteVertex() = default;
		float x, y;
#if defined( EAEENGINE_PLATFORM_D3D9 )
		uint8_t b, g, r, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
#elif defined( EAEENGINE_PLATFORM_GL )
		uint8_t r, g, b, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
#endif	
	};
}

namespace EAE_Engine
{
	namespace Graphics
	{
		SpriteRender::SpriteRender() :
			_pName(nullptr)
		{
			
		}
		
		SpriteRender::~SpriteRender()
		{
			SAFE_DELETE(_pName);
		}

		bool SpriteRender::Init(const char* pName)
		{
			IDirect3DDevice9* pDevice = GetD3DDevice();
			if (pDevice == NULL)
				return false;
			TextureManager::GetInstance()->LoadTexture(pName);
		}


	}
}