#include "../Common/BinaryFileLoader.h"
#include "../Common/Material.h"
#include "../Common/Effect.h"
#include "../Common/UniformVariable.h"
#include "../Common/Texture.h"
#include "../GraphicsInclude.h"
#include <cassert>
#include <sstream>
#include <fstream>
#include "UserOutput/Source/Assert.h"
#include "Windows/WindowsFunctions.h"

#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3d9.h>
#elif defined( EAEENGINE_PLATFORM_GL )
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif


namespace EAE_Engine 
{
	namespace Graphics 
	{
		Effect* CreateEffect(const char* i_pBinaryMaterialFile)
		{
			// Load effects
			Effect* pResult = LoadEffectData(i_pBinaryMaterialFile);
			if (pResult)
				return pResult;
			else
				return nullptr;
		}

		uint8_t* LoadMaterialInfo(const char* i_pFile, uint32_t& o_lengthOfWholerMaterialBuffer, uint32_t& o_lengthOfEffectPath)
		{
			o_lengthOfWholerMaterialBuffer = 0;
			o_lengthOfEffectPath = 0;
			if (!i_pFile)
				return nullptr;
			std::ifstream infile(i_pFile, std::ifstream::binary);
			if (!infile)
			{
				std::stringstream decoratedErrorMessage;
				decoratedErrorMessage << "Failed to load binary material file: " << i_pFile;
				ErrorMessageBox(decoratedErrorMessage.str().c_str());
				return nullptr;
			}
			// get size of file
			infile.seekg(0, infile.end);
			std::ifstream::pos_type size = infile.tellg();
			infile.seekg(0);
			// allocate memory for file content
			o_lengthOfWholerMaterialBuffer = (uint32_t)size;
			char* pBuffer = new char[o_lengthOfWholerMaterialBuffer];
			// read content of infile
			infile.read(pBuffer, size);
			if (!infile)
			{
				std::stringstream decoratedErrorMessage;
				decoratedErrorMessage << "Failed to load all data from: " << i_pFile << ", only " << infile.gcount() << "could be load.";
				ErrorMessageBox(decoratedErrorMessage.str().c_str());
				infile.close();
				delete[] pBuffer;
				return nullptr;
			}
			uint32_t offset = 0;
			// First, set the convert the buffer to MaterialDesc*
			MaterialDesc* pMaterialDesc = (MaterialDesc*)((uint8_t*)pBuffer + offset);
			// Second, load the path of the effect file
			{
				// Remember that I did a tricky solution in the MaterialBuilder 
				// that I use the _pEffect to save the offset of the Name in Effect
				// Because the size of the _handler will be different on x64 and x86,
				// so I really really should be careful about it.
				size_t offsetForEffectPathName = *(size_t*)(&pMaterialDesc->_pEffect);
				char* pathOfEffect = _strdup(pBuffer + offsetForEffectPathName);
				o_lengthOfEffectPath = (uint32_t)strlen(pBuffer + offsetForEffectPathName) + 1;
				pMaterialDesc->_pEffect = CreateEffect(pathOfEffect);
				free(pathOfEffect);
			}
			// Set the buffer of each segement
			UniformBlockDesc* pUniformBlockDescBuffer = nullptr;
			uint8_t* pUniformBlockNameBuffer = nullptr;
			UniformDesc* pUniformDescBuffer = nullptr;
			uint8_t* pUniformVariableValueBuffer = nullptr;
			uint8_t* pUniformVariableNameBuffer = nullptr;
			TextureDesc* pTextureDescBuffer = nullptr;
			uint8_t* pTexturePathBuffer = nullptr;
			uint8_t* pTextureSamplerNameBuffer = nullptr;
			{
				if (pMaterialDesc->_uniformBlockCount > 0)
				{
					pUniformBlockDescBuffer = pMaterialDesc->GetUniformBlockDesc();
					pUniformBlockNameBuffer = (uint8_t*)pMaterialDesc + pMaterialDesc->_offsetOfUniformBlockNameBuffer;
				}
				if (pMaterialDesc->_uniformCount > 0)
				{
					pUniformDescBuffer = pMaterialDesc->GetUniformDesc();
					pUniformVariableValueBuffer = (uint8_t*)pMaterialDesc + pMaterialDesc->_offsetOfUniformVariableValueBuffer;
					pUniformVariableNameBuffer = (uint8_t*)pMaterialDesc + pMaterialDesc->_offsetOfUniformVariableNameBuffer;
				}
				if (pMaterialDesc->_textureCount > 0)
				{
					pTextureDescBuffer = pMaterialDesc->GetTextureDesc();
					pTexturePathBuffer = (uint8_t*)pMaterialDesc + pMaterialDesc->_offsetOfTexturePathBuffer;
					pTextureSamplerNameBuffer = (uint8_t*)pMaterialDesc + pMaterialDesc->_offsetOfTextureSamplerBuffer;
				}
			}
			// Third, Set each UniformDesc of this MaterialDesc
			for (uint32_t ubIndex = 0; ubIndex < pMaterialDesc->_uniformBlockCount; ++ubIndex)
			{
				UniformBlockDesc* pUBD = &pUniformBlockDescBuffer[ubIndex];
				size_t offsetInNameBuffer = pUBD->_offsetInUniformBlockNameBuffer;
				const char* pUBName = (char*)(pUniformBlockNameBuffer + offsetInNameBuffer);
				uint32_t startIndex = pUBD->_startUniformDescIndex;
				uint32_t endIndex = pUBD->_endUniformDescIndex;
				// Add the default block members to be the Uniform Variables.
				if (strcmp(pUBName, "Default") == 0)
				{
					for (uint32_t uIndex = startIndex; uIndex <= endIndex; ++uIndex)
					{
						UniformDesc* pUD = &pUniformDescBuffer[uIndex];
						
						size_t offsetInNameBuffer = pUD->_offsetInNameBuffer;
						const char* pUniformName = (char*)(pUniformVariableNameBuffer + offsetInNameBuffer);
						pUD->SetHanlde(pUniformName, pMaterialDesc->_pEffect);
						size_t t = 0;
					}
				}
				// Add the undefault block members to be Uniform Blocks
				else 
				{
#if defined( EAEENGINE_PLATFORM_D3D9 )
					for (uint32_t uIndex = startIndex; uIndex <= endIndex; ++uIndex)
					{
						UniformDesc* pUD = &pUniformDescBuffer[uIndex];

						size_t offsetInNameBuffer = pUD->_offsetInNameBuffer;
						const char* pUniformName = (char*)(pUniformVariableNameBuffer + offsetInNameBuffer);
						pUD->SetHanlde(pUniformName, pMaterialDesc->_pEffect);
						size_t t = 0;
					}
#elif defined( EAEENGINE_PLATFORM_GL )
					// For the UniformBlock, We should get the instance of it,
					// Since we have scaned the Uniform Blocks when we were creating Effect,
					// We should be able to get their reference.
					UniformBlock* pUniformBlock = UniformBlockManager::GetInstance()->GetUniformBlock(pUBName);
					pUBD->_pUniformBlock = pUniformBlock;
#endif
				}
			}
			// Forth, Set each TextureDesc of this MaterialDesc
			for (uint32_t index = 0; index < pMaterialDesc->_textureCount; ++index)
			{
				TextureDesc* pTex = &pTextureDescBuffer[index];
				{
					// Set the value of the Texture.
					size_t offsetInTexturePathBuffer = pTex->_offsetInSamplerNameBuffer;
					const char* pTexName = (char*)(pTexturePathBuffer + offsetInTexturePathBuffer);
					pTex->_pTextureInfo = TextureManager::GetInstance()->LoadTexture(pTexName);
					// Set the value of the samplerID
					tSamplerID offsetInSamplerNameBuffer = *(tSamplerID*)(&pTex->_samplerID);
					const char* pTexSamplerName = (char*)(pTextureSamplerNameBuffer + offsetInSamplerNameBuffer);
					pTex->_samplerID = pMaterialDesc->_pEffect->GetSamplerID(pTexSamplerName, pTex->_shaderType);
				}
			}
			//Finally, don't forget to close the file
			infile.close();
			return reinterpret_cast<uint8_t*>(pBuffer);
		}

		void LoadMaterial(const char* i_pBinaryMaterialFile)
		{
			// First, Load and set the material buffer 
			uint32_t o_lengthOfWholerMaterialBuffer = 0;
			uint32_t o_lengthOfEffectPath = 0;
			uint8_t* pBuffer = LoadMaterialInfo(i_pBinaryMaterialFile, o_lengthOfWholerMaterialBuffer, o_lengthOfEffectPath);
			if (o_lengthOfWholerMaterialBuffer == 0)
			{
				SAFE_DELETE(pBuffer);
				return;
			}
			// Second, let's copy the buffer for real material
			uint8_t* pOutBuffer = new uint8_t[o_lengthOfWholerMaterialBuffer - o_lengthOfEffectPath];
			SetMem(pOutBuffer, o_lengthOfWholerMaterialBuffer - o_lengthOfEffectPath, 0);
			CopyMem(pBuffer, pOutBuffer, o_lengthOfWholerMaterialBuffer - o_lengthOfEffectPath);
			MaterialDesc* pTtemp = (MaterialDesc*)pOutBuffer;
			assert(pTtemp->_sizeOfMaterialBuffer == o_lengthOfWholerMaterialBuffer);
			// Third, add this MaterialDesc buffer to the Material Map.
			std::string mat_path(i_pBinaryMaterialFile);
			std::string key = GetFileNameWithoutExtension(mat_path.c_str());
			MaterialManager::GetInstance()->AddMaterial(key.c_str(), pOutBuffer);
			SAFE_DELETE(pBuffer);
		}
	}
}
