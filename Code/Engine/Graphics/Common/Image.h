#ifndef EAE_ENGINE_GRAPHICS_IMAGE_H
#define EAE_ENGINE_GRAPHICS_IMAGE_H

#include <map>
#include "Engine/General/Singleton.hpp"
#include "CommonDeclare.h"


namespace EAE_Engine
{
	namespace Graphics
	{
		struct Image 
		{
			Image() = default;
			Image(tTexture texture) : _texture(texture), _width(100.0f), _height(100.0f), _rows(0), _cols(0){}
			tTexture _texture;
			float _width, _height;
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