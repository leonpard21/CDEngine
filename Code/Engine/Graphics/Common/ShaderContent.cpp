#include "ShaderContent.h"
#include "General/MemoryOp.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		ShaderContent::~ShaderContent()
		{
			SAFE_DELETE_ARRAY(_pShaderBuffer);
		}
		ShaderContent::ShaderContent(uint8_t* pBuffer, size_t sizeOfBuffer) :
			_sizeOfBuffer(sizeOfBuffer)
		{
			_pShaderBuffer = new uint8_t[_sizeOfBuffer + 1];
			SetMem(_pShaderBuffer, _sizeOfBuffer, 0);
			CopyMem(pBuffer, _pShaderBuffer, _sizeOfBuffer);
		}
	}
}


