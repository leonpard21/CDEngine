#include "../Common/UniformVariable.h"
#include "../Common/Effect.h"
#include "../GraphicsInclude.h"
#include "Math/ColMatrix.h"


namespace EAE_Engine
{
	namespace Graphics
	{

		void UniformDesc::SetHanlde(const char* pName, Effect* pEffect)
		{
			_handle = pEffect->GetHandle(pName, _shaderType);
		}

		// Because we need to set the UniformVariable via VertexConstantTable or FragmentConstantTable,
		// so we need to save shadertype in D3D9 UniformVariable
		UniformVariable::UniformVariable(const char* pname, uint32_t bufferSize, ShaderTypes shadertype) :
			_bufferSize(bufferSize), _shaderType(shadertype)
		{
			_name = std::string(pname);
			_pBuffer = new uint8_t[bufferSize];
		}

		////////////////////////////////////////UniformVariableManager//////////////////////////////////////
		UniformVariable* UniformVariableManager::AddUniformVariable(const char* pUniformVariable, uint32_t bufferSize, ShaderTypes shaderType)
		{
			UniformVariable* pResult = nullptr;
			for (std::vector<UniformVariable*>::iterator iter = _uniformVariables.begin(); iter != _uniformVariables.end();)
			{
				UniformVariable* pUV = *iter++;
				if (pUV->GetName() == std::string(pUniformVariable))
				{
					return pUV;
				}
			}
			pResult = new UniformVariable(pUniformVariable, bufferSize, shaderType);
			_uniformVariables.push_back(pResult);
			return pResult;
		}
	}
}