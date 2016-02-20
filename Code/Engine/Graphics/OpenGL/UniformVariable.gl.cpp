#include "../Common/UniformVariable.h"
#include "../Common/Effect.h"
#include "Math/ColMatrix.h"
#include <cassert>

namespace EAE_Engine
{
	namespace Graphics
	{

		void UniformDesc::SetHanlde(const char* pName, Effect* pEffect)
		{
			_handle = pEffect->GetLocation(pName);
		}
		//////////////////////////////////UniformVariable////////////////////////////////////
		// In OpenGL, beacuse we need to use glUniform### interfaces, we need to recored their type.
		// That's why I'm using the UniformType 
		UniformVariable::UniformVariable(const char* pname, GLsizei bufferSize, UniformType type) :
			_bufferSize(bufferSize), _uniformType(type)
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
		UniformVariable* UniformVariableManager::GetUniformVariable(const char* pUniformVariable, GLsizei bufferSize, UniformType type)
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
			pResult = new UniformVariable(pUniformVariable, bufferSize, type);
			_uniformVariables.push_back(pResult);
			return pResult;
		}
	}
}