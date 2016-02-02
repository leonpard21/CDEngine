
namespace EAE_Engine
{
	namespace Graphics
	{
		// This is the interface to communicate with the Engine/Core.
		// The Engine/Core just need to take care the variable name and its value.  
		template<typename T>
		void UniformVariableManager::ChangeValue(const char* pUniformVariable, T* pValues, size_t count)
		{
			for (std::vector<UniformVariable*>::iterator iter = _uniformVariables.begin(); iter != _uniformVariables.end();)
			{
				UniformVariable* pUV = *iter++;
				if (pUV->GetName() == std::string(pUniformVariable))
				{
					pUV->SetValue<T>(pValues, count);
					break;
				}
			}
		}
	}
}

