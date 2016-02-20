#ifndef EAE_ENGINE_TEXTURE_H
#define EAE_ENGINE_TEXTURE_H

#include <map>
#include "CommonDeclare.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		struct TextureInfo
		{
			TextureInfo() = default;
			float _width;
			float _height;
			tTexture _texture;
		};

		struct TextureDesc 
		{
			tSamplerID _samplerID;
			TextureInfo* _pTextureInfo;
			ShaderTypes _shaderType;
			uint32_t _offsetInTexPathBuffer;
			uint32_t _offsetInSamplerNameBuffer;
			TextureDesc() : _samplerID(0), _pTextureInfo(nullptr), _shaderType(ShaderTypes::Fragment), 
				_offsetInTexPathBuffer(0), _offsetInSamplerNameBuffer(0){}
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
			TextureInfo* LoadTexture(const char* texturePath);
			int GetIndex(TextureInfo* pTextureInfo);
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