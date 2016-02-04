#ifndef EAE_ENGINE_GRAPHICS_SPRITE_H
#define EAE_ENGINE_GRAPHICS_SPRITE_H

#include <vector>
#include <map>
#include "Engine/General/Singleton.hpp"
#include "Engine/Math/Vector.h"
#include "BasicMeshes.h"

#include "CommonTypes.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		class SpriteRender 
		{
		public:
			SpriteRender();
			~SpriteRender();

			bool Init(const char* pName);

		private:
			const char* _pName;
		};

		class SpriteManager : public Singleton<SpriteManager>
		{
		public:
			tTexture LoadSprite(const char* spritePath);
		private:
			std::vector<SpriteRender*> _spriteRenders;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_SPRITE_H