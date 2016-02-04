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
			Sprite(tTexture texture) : _texture(texture), _index(0), _width(100.0f), _height(100.0f){}
			tTexture _texture;
			uint32_t _index;
			float _width, _height;
		};


		class SpriteManager : public Singleton<SpriteManager>
		{
		public:
			~SpriteManager();
			void Clean();
			Sprite* LoadSprite(const char* pName, uint32_t index, float width, float height);
			//Sprite* GetSprite(const char* pName);
		private:
			std::map<const char*, Sprite*> _sprites;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_SPRITE_H