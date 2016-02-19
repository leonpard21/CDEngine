#ifndef EAE_ENGINE_TEXTURE_H
#define EAE_ENGINE_TEXTURE_H

#include <map>
#include "CommonDeclare.h"

namespace EAE_Engine
{
	namespace Graphics
	{

		struct TextureData 
		{
			tSamplerID _samplerID;
			tTexture _texture;
			ShaderTypes _shaderType;
			TextureData() : _samplerID(0), _texture(0), _shaderType(ShaderTypes::Fragment){}
#if defined( EAEENGINE_PLATFORM_D3D9 )
			void SetTexture();
#elif defined( EAEENGINE_PLATFORM_GL )
			void SetTexture(uint32_t textureUnit);
#endif
		};

		struct TextureInfo
		{
			TextureInfo() = default;
			float _width;
			float _height;
			tTexture _texture;
		};

		class TextureManager
		{
		public:
			void Clean();
			TextureInfo LoadTexture(const char* texturePath);
		public:
			static TextureManager* GetInstance();
			static void CleanInstance();
		private:
			std::map<const char*, TextureInfo> _textures;
			static TextureManager* s_pTextureManager;
		};
	}
}

#endif