#include "BinaryFileLoader.h"
#include <cassert>
#include <sstream>
#include <fstream>
#include "../Common/Effect.h"
#include "../Common/Device.h"
#include "../GraphicsInclude.h"
#include "MeshManager.h"
#include "Engine/Math/Vector.h"
#include "Engine/UserOutput/Source/Assert.h"
#include "Engine/UserOutput/UserOutput.h"
#include "Engine/General/MemoryOp.h"
#include "Engine/Windows/WindowsFunctions.h"

#if defined( EAEENGINE_PLATFORM_GL )
#include <gl/GL.h>
#include <gl/GLU.h>
#endif

// Helper Function Declarations
//=============================

namespace
{
	uint8_t* LoadEffectInfo(const char* i_pFile, EAE_Engine::Graphics::sEffect*& o_pEffect)
	{
		if (!i_pFile)
			return nullptr;
		std::ifstream infile(i_pFile, std::ifstream::binary);
		if (!infile)
		{
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "Failed to load binary effect file: " << i_pFile;
			ErrorMessageBox(decoratedErrorMessage.str().c_str());
			return nullptr;
		}
		// get size of file
		infile.seekg(0, infile.end);
		std::ifstream::pos_type size = infile.tellg();
		infile.seekg(0);
		// allocate memory for file content
		char* pBuffer = new char[(unsigned int)size];
		// read content of infile
		infile.read(pBuffer, size);
		if (!infile)
		{
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "Failed to load all data from: " << i_pFile <<", only " << infile.gcount() << "could be load.";
			ErrorMessageBox(decoratedErrorMessage.str().c_str());
			infile.close();
			delete[] pBuffer;
			return nullptr;
		}
		o_pEffect = new EAE_Engine::Graphics::sEffect();
		size_t offset = 0;
		{
			size_t pathStrLength = strlen(pBuffer + offset);
			o_pEffect->_pSourcePath = _strdup(pBuffer + offset);
			offset += pathStrLength + 1;
			size_t vsStrLength = strlen(pBuffer + offset);
			o_pEffect->_pVS = _strdup(pBuffer + offset);
			offset += vsStrLength + 1;
			size_t fsStrLength = strlen(pBuffer + offset);
			o_pEffect->_pFS = _strdup(pBuffer + offset);
			offset += fsStrLength + 1;
			o_pEffect->_renderState = *reinterpret_cast<uint32_t*>(pBuffer + offset);
			offset += sizeof(uint32_t) + 1;
		}
		infile.close();
		return reinterpret_cast<uint8_t*>(pBuffer);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////


namespace EAE_Engine
{
	namespace Graphics
	{
		bool LoadEffectFromBinary(const char* i_binaryEffectFile)
		{
			Effect* pEffect = LoadEffectData(i_binaryEffectFile);
			if (!pEffect)
			{
				SAFE_RELEASE(pEffect);
				SAFE_DELETE(pEffect);
				return false;
			}
			return true;
		}

		Effect* LoadEffectData(const char* i_binaryEffectFile)
		{
			std::string path(i_binaryEffectFile);
			std::string key = GetFileNameWithoutExtension(path.c_str());
			for (std::map<const char*, Effect*>::const_iterator iter = EffectsManager::GetInstance()->_effectManager.begin(); iter != EffectsManager::GetInstance()->_effectManager.end(); ++iter)
			{
				if (strcmp(iter->first, key.c_str()) == 0)
				{
					return iter->second;
				}
			}
			sEffect* o_pEffect = nullptr;
			uint8_t* pBuffer = LoadEffectInfo(i_binaryEffectFile, o_pEffect);
			if (!pBuffer)
				return nullptr;
			std::string vsPath = std::string(o_pEffect->_pSourcePath) + std::string(o_pEffect->_pVS);
			std::string fsPath = std::string(o_pEffect->_pSourcePath) + std::string(o_pEffect->_pFS);
			Effect* pEffect = EffectsManager::GetInstance()->CreateEffect(key.c_str(), vsPath.c_str(), fsPath.c_str(), o_pEffect->_renderState);
			SAFE_DELETE(o_pEffect);
			SAFE_DELETE_ARRAY(pBuffer);
			return pEffect;
		}

	}
}