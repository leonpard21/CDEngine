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

		UniformVariable::UniformVariable(const char* pname, uint32_t bufferSize, ShaderTypes shadertype) :
			_bufferSize(bufferSize), _shaderType(shadertype)
		{
			_name = std::string(pname);
			_pBuffer = new uint8_t[bufferSize];
		}

		bool UniformVariable::operator == (const UniformVariable& i_other)
		{
			bool nameEqual = _name == i_other._name;
			bool typeEqual = _bufferSize == i_other._bufferSize;
			return nameEqual && typeEqual;
		}

		void UniformVariable::AddOwner(Effect* pShader, tUniformHandle location)
		{
			_owner.push_back(pShader);
			_location.push_back(location);
		}

		void UniformVariable::NotifyOwners()
		{
			for (size_t i = 0; i < _owner.size(); ++i)
			{
				_owner[i]->OnNotify(this, _location[i]);
			}
		}

		////////////////////////////////////////UniformVariableManager//////////////////////////////////////
		UniformVariable* UniformVariableManager::GetUniformVariable(const char* pUniformVariable, uint32_t bufferSize, ShaderTypes shaderType)
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