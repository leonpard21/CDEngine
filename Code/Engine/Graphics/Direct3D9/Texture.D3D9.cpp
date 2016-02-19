#include "../Common/Texture.h"
#include "../Common/Device.h"
#include <d3d9.h>
#include <d3dx9shader.h>
#include <cassert>
#include "Windows/WindowsFunctions.h"
#include "General/MemoryOp.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		bool LoadDDSTextureStatic(const char* texturePath, TextureInfo& o_textureInfo)
		{
			//Get devices context of D3D
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (!pD3DDevice) return false;

			IDirect3DTexture9* pResultTexture = NULL;

			const unsigned int useDimensionsFromFile = D3DX_DEFAULT_NONPOW2;
			const unsigned int useMipMapsFromFile = D3DX_FROM_FILE;
			const DWORD staticTexture = 0;
			const D3DFORMAT useFormatFromFile = D3DFMT_FROM_FILE;
			const D3DPOOL letD3dManageMemory = D3DPOOL_MANAGED;
			const DWORD useDefaultFiltering = D3DX_DEFAULT;
			const D3DCOLOR noColorKey = 0;
			D3DXIMAGE_INFO imageInfo;
			PALETTEENTRY* noColorPalette = NULL;
			const HRESULT result = D3DXCreateTextureFromFileEx(pD3DDevice, texturePath, useDimensionsFromFile, useDimensionsFromFile, useMipMapsFromFile,
				staticTexture, useFormatFromFile, letD3dManageMemory, useDefaultFiltering, useDefaultFiltering, noColorKey, &imageInfo, noColorPalette, &pResultTexture);
			o_textureInfo._width = (float)imageInfo.Width;
			o_textureInfo._height = (float)imageInfo.Height;
			o_textureInfo._texture = (tTexture)pResultTexture;
			return true;

		}

		void TextureData::SetTexture() 
		{
			//Get devices context of D3D
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (!pD3DDevice) return;
			if (_samplerID == UINT_MAX)
				return;
			//Set the Texture
			IDirect3DBaseTexture9* pTexture = (IDirect3DBaseTexture9*)_texture;
			HRESULT result = pD3DDevice->SetTexture(_samplerID, pTexture);
			assert(SUCCEEDED(result));
		}

		/////////////////////////////////////TextureManager///////////////////////////////////
		TextureInfo TextureManager::LoadTexture(const char* pTexturePath)
		{
			std::string key = GetFileNameWithoutExtension(pTexturePath);
			for (std::map<const char*, TextureInfo>::const_iterator iter = _textures.begin(); iter != _textures.end(); ++iter)
			{
				if (strcmp(iter->first, key.c_str()) == 0)
				{
					return iter->second;
				}
			}
			TextureInfo textureInfo = {0.0f, 0.0f, 0};
			bool result = LoadDDSTextureStatic(pTexturePath, textureInfo);
			_textures.insert(std::pair<const char*, TextureInfo>(_strdup(key.c_str()), textureInfo));
			for (std::map<const char*, TextureInfo>::const_iterator iter = _textures.begin(); iter != _textures.end(); ++iter)
			{
				if (strcmp(iter->first, key.c_str()) == 0)
				{
					return iter->second;
				}
			}
			return textureInfo;
		}

		void TextureManager::Clean()
		{
			for (std::map<const char*, TextureInfo>::const_iterator iter = _textures.begin(); iter != _textures.end();)
			{
				char* pKey = const_cast<char*>(iter->first);
				SAFE_DELETE(pKey);
				tTexture pValue = iter++->second._texture;
				IDirect3DBaseTexture9* pTexture = (IDirect3DBaseTexture9*)pValue;
				SAFE_RELEASE(pTexture);
			}
			_textures.clear();
		}

	}
}

