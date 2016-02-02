#include "BinaryFileLoader.h"
#include <cassert>
#include <sstream>
#include <fstream>
#include "../GraphicsInclude.h"
#include "../Common/Device.h"
#include "UserOutput/Source/Assert.h"
#include "UserOutput/UserOutput.h"
#include "ShaderContent.h"
#include "General/MemoryOp.h"


#if defined( EAEENGINE_PLATFORM_GL )
#include <gl/GL.h>
#include <gl/GLU.h>
#endif

// Helper Function Declarations
//=============================

namespace
{
	bool LoadShaderInfo(const char* i_pFile, EAE_Engine::Graphics::ShaderContent*& o_pShader)
	{
		if (!i_pFile)
			return false;
		std::ifstream infile(i_pFile, std::ifstream::binary);
		if (!infile)
		{
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "Failed to load binary shader file: " << i_pFile;
			ErrorMessageBox(decoratedErrorMessage.str().c_str());
			return false;
		}
		// get size of file
		infile.seekg(0, infile.end);
		std::ifstream::pos_type size = infile.tellg();
		infile.seekg(0);
		// allocate memory for file content
		char* pBuffer = new char[(size_t)size];
		// read content of infile
		infile.read(pBuffer, size);
		if (!infile)
		{
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "Failed to load all data from: " << i_pFile <<", only " << infile.gcount() << "could be load.";
			ErrorMessageBox(decoratedErrorMessage.str().c_str());
			infile.close();
			delete[] pBuffer; 
			return false;
		}
		o_pShader = new EAE_Engine::Graphics::ShaderContent(reinterpret_cast<uint8_t*>(pBuffer), (size_t)size);
		SAFE_DELETE_ARRAY(pBuffer);
		infile.close();
		return true;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////


namespace EAE_Engine
{
	namespace Graphics
	{

		ShaderContent* LoadShader(const char* i_bingaryShaderFile)
		{
			ShaderContent* o_pShader = nullptr;
			bool result = LoadShaderInfo(i_bingaryShaderFile, o_pShader);
			if (!result)
			{
				return nullptr;
			}

			return o_pShader;
		}

	}
}