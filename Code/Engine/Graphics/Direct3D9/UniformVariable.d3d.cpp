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

		UniformVariable::UniformVariable(const char* pname, UniformType type, ShaderTypes shadertype) :
			_pElements(nullptr), _elementCount(0), _shaderType(shadertype)
		{
			_name = std::string(pname);
			_uniformType = type;
			switch (type)
			{
			case UniformType::float1:
				_pElements = new float;
				_elementCount = 1;
				break;
			case UniformType::float2:
				_pElements = new float[2];
				_elementCount = 2;
				break;
			case UniformType::float3:
				_pElements = new float[3];
				_elementCount = 3;
				break;
			case UniformType::mat4:
				_pElements = new Math::ColMatrix44();
				_elementCount = 1;
				break;
			case UniformType::CameraMatricesStruct:
				_pElements = new CameraMatrices;
				((CameraMatrices*)_pElements)->_worldViewMatrix = Math::ColMatrix44::Identity;
				((CameraMatrices*)_pElements)->_viewProjMatrix = Math::ColMatrix44::Identity;
				_elementCount = 1;
				break;
			}
		}

		bool UniformVariable::operator == (const UniformVariable& i_other)
		{
			bool nameEqual = _name == i_other._name;
			bool typeEqual = _uniformType == i_other._uniformType;
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
				//_owner[i]->SetUniform(_type, _location[i], _pElements, _elementCount);
				_owner[i]->OnNotify(this, _location[i]);
			}
		}

		////////////////////////////////////////UniformVariableManager//////////////////////////////////////
		UniformVariable* UniformVariableManager::GetUniformVariable(const char* pUniformVariable, UniformType type, ShaderTypes shaderType)
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
			pResult = new UniformVariable(pUniformVariable, type, shaderType);
			_uniformVariables.push_back(pResult);
			return pResult;
		}
	}
}