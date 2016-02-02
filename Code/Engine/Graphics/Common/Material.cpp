#include "Material.h"
#include "General/MemoryOp.h"
#include "UniformVariable.h"
#include "Texture.h"

#include "Effect.h"
#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3dx9shader.h>
#elif defined( EAEENGINE_PLATFORM_GL )
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif

namespace EAE_Engine 
{
	namespace Graphics 
	{
		void MaterialDesc::SetUniformForEffect()
		{
			uint8_t* pUniformVariableValueBuffer = nullptr;
			pUniformVariableValueBuffer = (uint8_t*)this + _offsetOfUniformVariableValueBuffer;
			UniformDesc* pUniformDescBuffer = GetUniformDesc();
			if (!pUniformDescBuffer)
				return;
			// Set each UniformDesc of this MaterialDesc
			for (size_t index = 0; index < _uniformCount; ++index)
			{
				UniformDesc* pUD = &pUniformDescBuffer[index];
				uint8_t* pValue = pUniformVariableValueBuffer + pUD->_offsetInValueBuffer;
				uint32_t count = pUD->_valueBufferSize / sizeof(float);
#if defined( EAEENGINE_PLATFORM_D3D9 )
				_pEffect->SetUniform(pUD->_handle, (float*)pValue, count, pUD->_shaderType);
#elif defined( EAEENGINE_PLATFORM_GL )
				_pEffect->SetUniform(pUD->_handle, (float*)pValue, count);
#endif
			}
		}

		void MaterialDesc::SetTexturesForEffect()
		{
			TextureDesc* pTexDescBuffer = GetTextureDesc();
			if (pTexDescBuffer == nullptr)
				return;
			// Set each UniformDesc of this MaterialDesc
			for (uint32_t index = 0; index < _textureCount; ++index)
			{
#if defined( EAEENGINE_PLATFORM_D3D9 )
				(pTexDescBuffer + index)->SetTexture();
#elif defined( EAEENGINE_PLATFORM_GL )
				// In OpenGL, rather than associating the texture directly with the sampler ID,
				// there is an indirect step involving what OpenGL calls a "texture unit". 
				// We have to associate the texture with an arbitrary texture unit, 
				// and then assign that texture unit to the sampler.
				// For now,  I just use the index of the texture in the material, 
				// and this guarantees that each texture in material uses a different texture unit. 
				uint32_t textureUnit = index;
				(pTexDescBuffer + index)->SetTexture(textureUnit);
#endif
			}
		}

		TextureDesc* MaterialDesc::GetTextureDesc()
		{
			if (_textureCount == 0)
				return nullptr;
			return (TextureDesc*)((uint8_t*)this + sizeof(MaterialDesc) + sizeof(UniformDesc) * _uniformCount);
		}

////////////////////////MaterialManager static members//////////////////
		MaterialManager* MaterialManager::s_pMaterialManager = nullptr;

		MaterialManager* MaterialManager::GetInstance()
		{
			if (!s_pMaterialManager)
				s_pMaterialManager = new MaterialManager();
			return s_pMaterialManager;
		}
		void MaterialManager::CleanInstance()
		{
			if (!s_pMaterialManager) 
				return;
			s_pMaterialManager->Clean();
			SAFE_DELETE(s_pMaterialManager);
		}

////////////////////////////////////member function////////////////////////////
		void MaterialManager::AddMaterial(const char* key, uint8_t* pValue)
		{
			MaterialKeyValuePair keyValuePair = { _strdup(key), pValue };
			_materials.push_back(keyValuePair);
		}
		void MaterialManager::Clean()
		{
			for (std::vector<MaterialKeyValuePair>::const_iterator iter = _materials.begin(); iter != _materials.end();)
			{
				char* pKey = const_cast<char*>(iter->_pKey);
				SAFE_DELETE(pKey);
				uint8_t* pValue = iter++->_pMaterialDesc;
				SAFE_DELETE_ARRAY(pValue);
			}
			_materials.clear();
		}
		MaterialDesc* MaterialManager::GetMaterial(const char* key)
		{
			for (std::vector<MaterialKeyValuePair>::const_iterator iter = _materials.begin(); iter != _materials.end(); ++iter)
			{
				if (strcmp(iter->_pKey, key) == 0)
				{
					return (MaterialDesc*)iter->_pMaterialDesc;
				}
			}
			return nullptr;
		}

	}
}