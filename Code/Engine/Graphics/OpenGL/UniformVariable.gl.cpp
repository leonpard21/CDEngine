#include "../Common/UniformVariable.h"
#include "../Common/Effect.h"
#include "Math/ColMatrix.h"
#include <cassert>

namespace EAE_Engine
{
	namespace Graphics
	{
		void UniformData::SetHanlde(const char* pName, Effect* pEffect)
		{
			_handle = pEffect->GetLocation(pName);
		}
		//////////////////////////////////UniformVariable////////////////////////////////////
		UniformVariable::UniformVariable(const char* pname, UniformType type) :
			_pElements(nullptr), _elementCount(0)
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
			case UniformType::float4:
				_pElements = new float[4];
				_elementCount = 4;
				break;
			case UniformType::mat4:
				_pElements = new Math::ColMatrix44();
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

		void UniformVariable::AddOwner(Effect* pEffect, tUniformHandle location)
		{
			_owner.push_back(pEffect);
			_location.push_back(location);
		}

		void UniformVariable::NotifyOwners()
		{
			assert(_owner.size() == _location.size());
			for (size_t i = 0; i < _owner.size(); ++i)
			{
				_owner[i]->OnNotify(this, _location[i]);
			}
		}


		////////////////////////////////////////UniformVariableManager//////////////////////////////////////
		UniformVariable* UniformVariableManager::GetUniformVariable(const char* pUniformVariable, UniformType type)
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
			pResult = new UniformVariable(pUniformVariable, type);
			_uniformVariables.push_back(pResult);
			return pResult;
		}
	}
}