#include "UniformVariable.h"
#include "../Common/Effect.h"

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
		UniformVariable::~UniformVariable()
		{
			SAFE_DELETE_ARRAY(_pElements);
		}

		void UniformVariable::NotifyOwner(Effect* pEffect)
		{
			assert(_owner.size() == _location.size());
			for (size_t i = 0; i < _owner.size(); ++i)
			{
				if (pEffect != _owner[i])
					continue;
#if defined( EAEENGINE_PLATFORM_D3D9 )
				_owner[i]->SetUniform(_uniformType, _location[i], _pElements, _elementCount, _shaderType);
#elif defined( EAEENGINE_PLATFORM_GL )
				_owner[i]->SetUniform(_uniformType, _location[i], _pElements, _elementCount);
#endif
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