#include "../Common/UniformVariable.h"
#include "../Common/Effect.h"
#include "Math/ColMatrix.h"
#include <cassert>

namespace EAE_Engine
{
	namespace Graphics
	{

		//////////////////////////////////UniformVariable////////////////////////////////////
		// In OpenGL, beacuse we need to use glUniform### interfaces, we need to recored their type.
		// That's why I'm using the UniformType 
		UniformVariable::UniformVariable(const char* pname, GLsizei bufferSize, UniformType type) :
			_bufferSize(bufferSize), _uniformType(type)
		{
			_name = std::string(pname);
			_pBuffer = new uint8_t[bufferSize];
		}

		////////////////////////////////////////UniformVariableManager//////////////////////////////////////
		UniformVariable* UniformVariableManager::AddUniformVariable(const char* pUniformVariable, GLsizei bufferSize, UniformType type)
		{
			UniformVariable* pResult = nullptr;
			for (std::vector<UniformVariable*>::iterator iter = _uniformVariables.begin(); iter != _uniformVariables.end();)
			{
				UniformVariable* pUV = *iter++;
				if (strcmp(pUV->GetName(), pUniformVariable) == 0)
				{
				 	return pUV;
				}
			}
			pResult = new UniformVariable(pUniformVariable, bufferSize, type);
			_uniformVariables.push_back(pResult);
			return pResult;
		}

	}
}