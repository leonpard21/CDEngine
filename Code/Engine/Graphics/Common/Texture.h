#ifndef EAE_ENGINE_TEXTURE_H
#define EAE_ENGINE_TEXTURE_H

#include <map>
#include "CommonTypes.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		struct TextureDesc 
		{
			tSamplerID _samplerID;
			tTexture _texture;
			ShaderTypes _shaderType;
			TextureDesc() : _samplerID(0), _texture(0), _shaderType(ShaderTypes::Fragment){}
#if defined( EAEENGINE_PLATFORM_D3D9 )
			void SetTexture();
#elif defined( EAEENGINE_PLATFORM_GL )
			void SetTexture(uint32_t textureUnit);
#endif
		};

		class TextureManager
		{
		public:
			void Clean();
			tTexture LoadTexture(const char* texturePath);
		public:
			static TextureManager* GetInstance();
			static void CleanInstance();
		private:
			std::map<const char*, tTexture> _textures;
			static TextureManager* s_pTextureManager;
		};
	}
}

#endif