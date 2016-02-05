#include "Sprite.h"
#include "Texture.h"
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

		Sprite* SpriteManager::LoadSprite(const char* pSpritePathName, float width, float height)
		{
			std::string key = GetFileNameWithoutExtension(pSpritePathName);
			for (std::map<const char*, Sprite*>::const_iterator iter = _sprites.begin(); iter != _sprites.end(); ++iter)
			{
				if (strcmp(iter->first, key.c_str()) == 0)
				{
					return iter->second;
				}
			}
			TextureInfo textureHandle = TextureManager::GetInstance()->LoadTexture(pSpritePathName);
			Sprite* pSprite = new Sprite(textureHandle._texture);
			// First, we need to clip the sprite into rows and heights based on its size.
			pSprite->_rows = textureHandle._width / width;
			pSprite->_cols = textureHandle._height / height;
			// Second, we scale the sprite based on the window size.
			pSprite->_width = width;
			pSprite->_height = height;
			_sprites.insert(std::pair<const char*, Sprite*>(_strdup(key.c_str()), pSprite));
			return pSprite;
		}

		Sprite* SpriteManager::GetSprite(const char* pKey)
		{
			std::string key = GetFileNameWithoutExtension(pKey);
			for (std::map<const char*, Sprite*>::const_iterator iter = _sprites.begin(); iter != _sprites.end(); ++iter)
			{
				if (strcmp(iter->first, key.c_str()) == 0)
				{
					return iter->second;
				}
			}
			return nullptr;
		}

	}
}
