#ifndef EAE_ENGINE_GRAPHICS_SPRITERENDER_H
#define EAE_ENGINE_GRAPHICS_SPRITERENDER_H

#include <vector>
#include "Engine/General/Singleton.hpp"
#include "Engine/Math/Vector.h"
#include "Engine/Common/Interfaces.h"
#include "BasicShapes.h"

#include "SpriteMesh.h"
#include "Sprite.h"

#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3d9.h>
#elif defined( EAEENGINE_PLATFORM_GL )
//#include <gl/GL.h>
//#include <gl/GLU.h>
//#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif

namespace EAE_Engine
{
	namespace Graphics
	{
		struct MaterialDesc;
		class SpriteRender 
		{
			SpriteRender();
		public:
			SpriteRender(Sprite* pSprite, Common::ITransform* _pTransform);
			~SpriteRender();

		private:
			Sprite* _pSprite;
			Common::ITransform* _pTransform;
		};

		class SpriteRenderManager : public Singleton<SpriteRenderManager>
		{
		public:
			SpriteRenderManager();
			~SpriteRenderManager();
			void Init();
			void Clean();

			SpriteRender* AddSpriteRender(const char* pName, Common::ITransform* pTransform);
			void UpdateRenderDataList();
		private:
			MaterialDesc* _pMaterial;
			SpriteMesh* _pSpriteMesh;
			std::vector<SpriteRender*> _spriteRenders;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_SPRITERENDER_H