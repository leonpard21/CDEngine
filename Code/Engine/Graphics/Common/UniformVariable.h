#ifndef EAE_ENGINE_UNIFORMVARIABLE_H
#define EAE_ENGINE_UNIFORMVARIABLE_H

#include <vector>
#include "UniformDesc.h"
#include "Math/ColMatrix.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		enum UniformType
		{
			float1,
			float2,
			float3,
			float4,
			mat4,
		};

		struct CameraMatrices
		{
			Math::ColMatrix44 _worldViewMatrix;
			Math::ColMatrix44 _viewProjMatrix;
		};

#if defined( EAEENGINE_PLATFORM_D3D9 )
		class UniformVariable
		{
		public:
			UniformVariable(const char* pname, UniformType type, ShaderTypes shadertype);
			~UniformVariable();
			inline const char* GetName() { return _name.c_str(); }
			bool operator == (const UniformVariable& i_other);

			void AddOwner(Effect* pShader, tUniformHandle location);
			void NotifyOwners();
			void NotifyOwner(Effect* pEffect);
			template<typename T>
			void SetValue(T* values, size_t count)
			{
				memcpy(_pElements, values, count * sizeof(T));
			}
			UniformType GetUniformType() { return _uniformType; }
			ShaderTypes GetShaderType() { return _shaderType; }
			void* GetElements() { return _pElements; }
			size_t GetElementCount() { return _elementCount; }
		private:
			std::vector<Effect*> _owner;  // List containing references to the owners needed so they can be notified
			std::vector<tUniformHandle> _location;
			std::string _name; // The name of the uniform variable in the HLSL program
			UniformType _uniformType;
			ShaderTypes _shaderType;
			void* _pElements;
			size_t _elementCount;
		};
		
#elif defined( EAEENGINE_PLATFORM_GL )
		class UniformVariable
		{
		public:
			UniformVariable(const char* pname, UniformType type);
			~UniformVariable();
			inline const char* GetName() { return _name.c_str(); }
			bool operator == (const UniformVariable& i_other);

			void AddOwner(Effect* pEffect, tUniformHandle location);
			void NotifyOwners();
			void NotifyOwner(Effect* pEffect);
			template<typename T>
			void SetValue(T* values, size_t count)
			{
				GLsizei glcounts = (GLsizei)(count * sizeof(T));
				memcpy(_pElements, values, glcounts);
			}
			UniformType GetUniformType() { return _uniformType; }
			void* GetElements() { return _pElements; }
			GLsizei GetElementCount() { return _elementCount; }
		private:
			std::vector<Effect*> _owner;  // List containing references to the owners needed so they can be notified.
			std::vector<tUniformHandle> _location; // list of locations/handles for the GLSL uniform variable
			std::string _name; // The name of the uniform variable in the GLSL program
			UniformType _uniformType;
			void* _pElements;
			GLsizei _elementCount;
		};

#endif 

		class UniformVariableManager
		{			
		public:
			~UniformVariableManager();
			// This is the interface to communicate with the Grphics itself.
			// The Graphics, we need to know 
#if defined( EAEENGINE_PLATFORM_D3D9 )
			UniformVariable* GetUniformVariable(const char* pUniformVariable, UniformType type, ShaderTypes shaderType);
#elif defined( EAEENGINE_PLATFORM_GL )
			UniformVariable* GetUniformVariable(const char* pUniformVariable, UniformType type);
#endif 
			void Clean();
			// This is the interface to communicate with the Engine/Core.
			// The Engine/Core just need to take care the variable name and its value.  
			template<typename T>
			void ChangeValue(const char* pUniformVariable, T* pValues, size_t count);
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

#include "UniformVariable.inl"
#endif//EAE_ENGINE_UNIFORMVARIABLE_H