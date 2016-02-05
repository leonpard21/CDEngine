#include "Image.h"
#include "Texture.h"
#include "Windows/WindowsFunctions.h"


namespace EAE_Engine
{
	namespace Graphics
	{

		////////////////////////ImageManager//////////////////////////////////
		ImageManager::~ImageManager()
		{
			Clean();
		}
		
		void ImageManager::Clean()
		{
			for (std::map<const char*, Image*>::const_iterator iter = _images.begin(); iter != _images.end(); )
			{
				char* pKey = const_cast<char*>(iter->first);
				SAFE_DELETE(pKey);
				Image* pValue = iter++->second;
				SAFE_DELETE(pValue);
			}
			_images.clear();
		}

		Image* ImageManager::LoadCustomImage(const char* pImagePathName, float width, float height)
		{
			std::string key = GetFileNameWithoutExtension(pImagePathName);
			for (std::map<const char*, Image*>::const_iterator iter = _images.begin(); iter != _images.end(); ++iter)
			{
				if (strcmp(iter->first, key.c_str()) == 0)
				{
					return iter->second;
				}
			}
			TextureInfo textureHandle = TextureManager::GetInstance()->LoadTexture(pImagePathName);
			Image* pImage = new Image(textureHandle._texture);
			// First, we need to clip the sprite into rows and heights based on its size.
			pImage->_rows = textureHandle._width / width;
			pImage->_cols = textureHandle._height / height;
			// Second, we scale the sprite based on the window size.
			pImage->_width = width;
			pImage->_height = height;
			_images.insert(std::pair<const char*, Image*>(_strdup(key.c_str()), pImage));
			return pImage;
		}

		Image* ImageManager::GetImage(const char* pKey)
		{
			std::string key = GetFileNameWithoutExtension(pKey);
			for (std::map<const char*, Image*>::const_iterator iter = _images.begin(); iter != _images.end(); ++iter)
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
