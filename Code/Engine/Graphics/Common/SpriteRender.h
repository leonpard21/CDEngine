#ifndef EAE_ENGINE_GRAPHICS_SPRITERENDER_H
#define EAE_ENGINE_GRAPHICS_SPRITERENDER_H

#include <vector>
#include "Engine/General/Singleton.hpp"
#include "Engine/Math/Vector.h"
#include "Engine/Common/Interfaces.h"
#include "BasicShapes.h"

#include "CommonDeclare.h"
#include "SpriteMesh.h"

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
		struct Sprite 
		{
			Sprite(tTexture texture) : _texture(texture) {}
			tTexture _texture;
			SpriteMesh* _pSpriteMesh;
			uint32_t _index;
			float _width, _height;
			bool SetData(Rectangle* i_position = nullptr, Rectangle* i_texcoords = nullptr);
		};

		struct MaterialDesc;
		class SpriteRender 
		{
			SpriteRender();
		public:
			SpriteRender(Sprite& sprite, Common::ITransform* _pTransform);
			~SpriteRender();


		private:
			MaterialDesc* _pMaterial;
			Sprite _sprite;
			Common::ITransform* _pTransform;
		};

		class SpriteManager : public Singleton<SpriteManager>
		{
		public:
			~SpriteManager() {}
			void Init();
			void Clean();

			SpriteRender* AddSprite(const char* pName, Common::ITransform* pTransform);
			void UpdateRenderDataList();
		private:
			std::vector<SpriteRender*> _spriteRenders;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_SPRITERENDER_H