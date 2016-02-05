#ifndef EAE_ENGINE_GRAPHICS_SPRITE_H
#define EAE_ENGINE_GRAPHICS_SPRITE_H

#include <map>
#include "Engine/General/Singleton.hpp"
#include "CommonDeclare.h"


namespace EAE_Engine
{
	namespace Graphics
	{
		struct Sprite 
		{
			Sprite() = default;
			Sprite(tTexture texture) : _texture(texture), _width(100.0f), _height(100.0f), _rows(0), _cols(0){}
			tTexture _texture;
			float _width, _height;
			uint32_t _rows, _cols;
		};

		class SpriteManager : public Singleton<SpriteManager>
		{
		public:
			~SpriteManager();
			void Clean();
			Sprite* LoadSprite(const char* pSpritePathName, float width, float height);
			Sprite* GetSprite(const char* pKey);
		private:
			std::map<const char*, Sprite*> _sprites;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_SPRITE_H