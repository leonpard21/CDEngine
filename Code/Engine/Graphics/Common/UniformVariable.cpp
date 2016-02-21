#include "UniformVariable.h"
#include "../Common/Effect.h"
#include <cassert>

#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3dx9shader.h>
#elif defined( EAEENGINE_PLATFORM_GL )
#include "External/OpenGlExtensions/OpenGlExtensions.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#endif

namespace EAE_Engine
{
	namespace Graphics
	{
		//////////////////////////////////UniformDesc////////////////////////////////////
		void UniformDesc::SetUniformVariable(const char* pName, Effect* pEffect)
		{			
#if defined( EAEENGINE_PLATFORM_D3D9 )
			_handle = pEffect->GetHandle(pName, _shaderType);
#elif defined( EAEENGINE_PLATFORM_GL )
			_handle = pEffect->GetLocation(pName);
#endif
			_pUniformVariable = UniformVariableManager::GetInstance().GetUniformVariable(pName);
			assert(_pUniformVariable != nullptr);
		}

		//////////////////////////////////UniformVariable////////////////////////////////////
		UniformVariable::~UniformVariable()
		{
			SAFE_DELETE_ARRAY(_pBuffer);
		}
		void UniformVariable::NotifyOwner(Effect* pEffect)
		{
			assert(_owner.size() == _location.size());
			for (size_t i = 0; i < _owner.size(); ++i)
			{
				if (pEffect != _owner[i])
					continue;
				// Here we need to find the correct location for this uniform in the Effect.
#if defined( EAEENGINE_PLATFORM_D3D9 )
				_owner[i]->SetUniform( _location[i], _pBuffer, _bufferSize, _shaderType);
#elif defined( EAEENGINE_PLATFORM_GL )
				_owner[i]->SetUniform(_uniformType, _location[i], _pBuffer, _bufferSize);
#endif
			}
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


////////////////////////////////////UniformVariableManager////////////////////////////
		UniformVariableManager::~UniformVariableManager()
		{
			for (std::vector<UniformVariable*>::iterator iter = _uniformVariables.begin(); iter != _uniformVariables.end();)
			{
				UniformVariable* pUV = *iter++;
				SAFE_DELETE(pUV);
			}
			_uniformVariables.clear();
		}

		void UniformVariableManager::Clean()
		{
			for (std::vector<UniformVariable*>::iterator iter = _uniformVariables.begin(); iter != _uniformVariables.end();)
			{
				UniformVariable* pUV = *iter++;
				SAFE_DELETE(pUV);
			}
			_uniformVariables.clear();
		}

		// This is the interface to communicate with the Engine/Core.
		// The Engine/Core just need to take care the variable name and its value.  
		void UniformVariableManager::NotifyOwners(const char* pUniformVariable)
		{
			for (std::vector<UniformVariable*>::iterator iter = _uniformVariables.begin(); iter != _uniformVariables.end();)
			{
				UniformVariable* pUV = *iter++;
				if (strcmp(pUV->GetName(), pUniformVariable) == 0)
				{
					pUV->NotifyOwners();
					break;
				}
			}
		}

		// This is the interface to communicate with the Engine/Core.
		// The Engine/Core just need to take care the variable name and its value.  
		void UniformVariableManager::ChangeValue(const char* pUniformVariable, void* pValues, uint32_t bufferSize)
		{
			for (std::vector<UniformVariable*>::iterator iter = _uniformVariables.begin(); iter != _uniformVariables.end();)
			{
				UniformVariable* pUV = *iter++;
				if (pUV->GetName() == std::string(pUniformVariable))
				{
					pUV->SetValue(pValues, bufferSize);
					break;
				}
			}
		}

		UniformVariable* UniformVariableManager::GetUniformVariable(const char* pUniformVariable)
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
			return nullptr;
		}

////////////////////////////////static_members/////////////////////////////
		UniformVariableManager* UniformVariableManager::s_pInternalInstance = nullptr;

		UniformVariableManager& UniformVariableManager::GetInstance()
		{
			if (!s_pInternalInstance)
				s_pInternalInstance = new UniformVariableManager();
			return *s_pInternalInstance;
		}

		void UniformVariableManager::CleanInstance()
		{
			if (!s_pInternalInstance) return;
			s_pInternalInstance->Clean();
			SAFE_DELETE(s_pInternalInstance);
		}
	}
}