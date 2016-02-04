#include "../Common/Sprite.h"
#include "../Common/Texture.h"
#include "Windows/WindowsFunctions.h"


namespace EAE_Engine
{
	namespace Graphics
	{


		////////////////////////SpriteManager//////////////////////////////////

		SpriteManager::~SpriteManager()
		{
			Clean();
		}
		
		void SpriteManager::Clean()
		{
			for (std::map<const char*, Sprite*>::const_iterator iter = _sprites.begin(); iter != _sprites.end(); )
			{
				char* pKey = const_cast<char*>(iter->first);
				SAFE_DELETE(pKey);
				Sprite* pValue = iter++->second;
				SAFE_DELETE(pValue);
			}
			_sprites.clear();
		}

		Sprite* SpriteManager::LoadSprite(const char* pName, uint32_t index, float width, float height)
		{
			std::string key(pName);
			for (std::map<const char*, Sprite*>::const_iterator iter = _sprites.begin(); iter != _sprites.end(); ++iter)
			{
				if (strcmp(iter->first, key.c_str()) == 0)
				{
					return iter->second;
				}
			}
			tTexture textureHandle = TextureManager::GetInstance()->LoadTexture(pName);
			Sprite* pSprite = new Sprite(textureHandle);
			pSprite->_index = index;
			pSprite->_width = width;
			pSprite->_height = height;
			_sprites.insert(std::pair<const char*, Sprite*>(_strdup(key.c_str()), pSprite));
			return pSprite;
		}


	}
}
