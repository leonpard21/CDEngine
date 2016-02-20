#ifndef EAE_ENGINE_UNIFORMVARIABLE_H
#define EAE_ENGINE_UNIFORMVARIABLE_H

#include <vector>
#include "UniformDesc.h"
#include "Math/ColMatrix.h"

namespace EAE_Engine
{
	namespace Graphics
	{

		struct CameraMatrices
		{
			Math::ColMatrix44 _worldViewMatrix;
			Math::ColMatrix44 _viewProjMatrix;
		};

#if defined( EAEENGINE_PLATFORM_D3D9 )
		class UniformVariable
		{
		public:
			UniformVariable(const char* pname, uint32_t bufferSize, ShaderTypes shadertype);
			~UniformVariable();
			inline const char* GetName() { return _name.c_str(); }
			bool operator == (const UniformVariable& i_other);
			void* GetBuffer() { return _pBuffer; }
			uint32_t GetBufferSize() { return _bufferSize; }

			void AddOwner(Effect* pShader, tUniformHandle location);
			void NotifyOwners();
			void NotifyOwner(Effect* pEffect);
			void SetValue(void* pValue, uint32_t bufferSize)
			{
				memcpy(_pBuffer, pValue, bufferSize);
			}
			ShaderTypes GetShaderType() { return _shaderType; }

		private:
			std::vector<Effect*> _owner;  // List containing references to the owners needed so they can be notified
			std::vector<tUniformHandle> _location;
			std::string _name; // The name of the uniform variable in the HLSL program
			ShaderTypes _shaderType;
			void* _pBuffer;
			uint32_t _bufferSize;
		};
		
#elif defined( EAEENGINE_PLATFORM_GL )
		enum UniformType
		{
			float1,
			float2,
			float3,
			float4,
			mat4,
		};

		class UniformVariable
		{
		public:
			UniformVariable(const char* pname, GLsizei bufferSize, UniformType type);
			~UniformVariable();
			inline const char* GetName() { return _name.c_str(); }
			bool operator == (const UniformVariable& i_other);

			void AddOwner(Effect* pEffect, tUniformHandle location);
			void NotifyOwners();
			void NotifyOwner(Effect* pEffect);
			inline void SetValue(void* pValue, GLsizei bufferSize)
			{
				memcpy(_pBuffer, pValue, bufferSize);
			}
			UniformType GetUniformType() { return _uniformType; }
			void* GetBuffer() { return _pBuffer; }
			GLsizei GetBufferSize() { return _bufferSize; }
		private:
			std::vector<Effect*> _owner;  // List containing references to the owners needed so they can be notified.
			std::vector<tUniformHandle> _location; // list of locations/handles for the GLSL uniform variable
			std::string _name; // The name of the uniform variable in the GLSL program
			UniformType _uniformType;
			void* _pBuffer;
			GLsizei _bufferSize;
		};

#endif 

		class UniformVariableManager
		{			
		public:
			~UniformVariableManager();
			// This is the interface to communicate with the Grphics itself.
			// The Graphics, we need to know 
#if defined( EAEENGINE_PLATFORM_D3D9 )
			UniformVariable* GetUniformVariable(const char* pUniformVariable, uint32_t bufferSize, ShaderTypes shaderType);
#elif defined( EAEENGINE_PLATFORM_GL )
			UniformVariable* GetUniformVariable(const char* pUniformVariable, GLsizei bufferSize, UniformType type);
#endif 
			void Clean();
			// This is the interface to communicate with the Engine/Core.
			// The Engine/Core just need to take care the variable name and its value.  
			void ChangeValue(const char* pUniformVariable, void* pValues, uint32_t bufferSize);
			void NotifyOwners(const char* pUniformVariable);

			std::vector<UniformVariable*> _uniformVariables;
		/////////////////////static_members////////////////////////////
		private:
			UniformVariableManager() {}
			static UniformVariableManager* s_pInternalInstance;
		public:
			static UniformVariableManager& GetInstance();
			static void CleanInstance();
		};

	}
}

#endif//EAE_ENGINE_UNIFORMVARIABLE_H