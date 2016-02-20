#ifndef EAE_ENGINE_GRAPHICS_IMAGE_H
#define EAE_ENGINE_GRAPHICS_IMAGE_H

#include <map>
#include "Engine/General/Singleton.hpp"
#include "Texture.h"


namespace EAE_Engine
{
	namespace Graphics
	{
		struct Image 
		{
			Image() = default;
			Image(TextureInfo* pTextureInfo) : _pTextureInfo(pTextureInfo), _rows(0), _cols(0){}
			TextureInfo* _pTextureInfo;
			uint32_t _rows, _cols;
		};

		class ImageManager : public Singleton<ImageManager>
		{
		public:
			~ImageManager();
			void Clean();
			Image* LoadCustomImage(const char* pImagePathName, uint32_t rows, uint32_t cols);
			Image* GetImage(const char* pKey);
		private:
			std::map<const char*, Image*> _images;
		};

	}
}

#endif//EAE_ENGINE_GRAPHICS_IMAGE_H