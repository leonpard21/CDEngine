#ifndef EAE_ENGINE_GRAPHICS_EFFECTS
#define EAE_ENGINE_GRAPHICS_EFFECTS
#include <map>
#include "General/MemoryOp.h"
#include "General/EngineObj.h"

#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3d9.h>
#include <d3dx9shader.h>
#elif defined( EAEENGINE_PLATFORM_GL )
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#endif

#include "UniformVariable.h"
#include "UniformBlock.h"
#include "Texture.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		class Effect;
		class UniformVariable;
#if defined( EAEENGINE_PLATFORM_D3D9 )
		class Effect
		{
		public:
			Effect() : _pVertexShader(NULL), _pFragmentShader(NULL), _pVSConstantTable(NULL), _renderState(0) {}
			bool Init(const char* i_pVSfilePath, const char* i_pFSfilePath, uint32_t renderState);
			bool BeginRender();
			bool EndRender();
			bool Release();
			// this interface will be used in the UniformVariable System and the UniformDesc System.
			void SetUniform(D3DXHANDLE handle, void* pBuffer, uint32_t bufferSize, ShaderTypes shaderType);
			IDirect3DVertexShader9* GetVS() { return _pVertexShader; }
			IDirect3DPixelShader9* GetFS() { return _pFragmentShader; }
			uint32_t GetRenderState(){ return _renderState; }
			tUniformHandle GetHandle(const char* pName, ShaderTypes shaderType);
			tSamplerID GetSamplerID(const char* pName, ShaderTypes shaderType);
			void OnNotify(UniformVariable* pVariable, D3DXHANDLE location);
			void OnNotify(UniformBlock* pBlock);
			void Update();
		private:
			bool LoadFragmentShader(uint8_t* pFSBuffer);
			bool LoadVertexShader(uint8_t* pVSBuffer);
			void ExtratUniformsFromTable(ID3DXConstantTable* pConstantTable, ShaderTypes shaderType);
		private:
			IDirect3DVertexShader9* _pVertexShader;
			IDirect3DPixelShader9* _pFragmentShader;
			ID3DXConstantTable* _pVSConstantTable;
			ID3DXConstantTable* _pFSConstantTable;
			uint32_t _renderState;
			// when we have a new uniform variable to update, we add it to the list.
			std::map<D3DXHANDLE, UniformVariable*> _updateVariableList;
			std::map<const char*, UniformBlock*> _updateBlockList;
		};
#elif defined( EAEENGINE_PLATFORM_GL )
		class Effect
		{
		public:
			Effect() : _programId(0), _renderState(0){}
			bool Init(const char* i_pVSfilePath, const char* i_pFSfilePath, uint32_t renderState);
			bool BeginRender();
			bool EndRender();
			bool Release();
			// this interface will be used in the UniformVariable/UniformDesc System.
			void SetUniform(UniformType type, GLint location, void* pBuffer, size_t bufferSize);
			GLuint GetProgram() { return _programId; }
			uint32_t GetRenderState() { return _renderState; }
			GLint GetLocation(const char* pName);
			tSamplerID GetSamplerID(const char* pName, ShaderTypes shaderType);
			void OnNotify(UniformVariable* pVariable, GLint location);
			void OnNotify(UniformBlock* pBlock);
			void Update();

		private:
			bool LoadAttachVertexShader(GLuint& io_programId, const char* i_pVSFilePath);
			bool LoadAttachFragmentShader(GLuint& io_programId, const char* i_pFSFilePath);
			void ExtractShaderUniformBlocks();
			void ExtractShaderUniforms();
			// OpenGL encapsulates a matching vertex shader and fragment shader into what it calls a "program".

			// A vertex shader is a program that operates on vertices.
			// Its input comes from a C/C++ "draw call" and is:
			//	* Position
			//	* Any other data we want
			// Its output is:
			//	* Position
			//		(So that the graphics hardware knows which pixels to fill in for the triangle)
			//	* Any other data we want

			// The fragment shader is a program that operates on fragments
			// (or potential pixels).
			// Its input is:
			//	* The data that was output from the vertex shader,
			//		interpolated based on how close the fragment is
			//		to each vertex in the triangle.
			// Its output is:
			//	* The final color that the pixel should be
			GLuint _programId;
			uint32_t _renderState;
			std::map<GLint, UniformVariable*> _updateVariableList;
			std::map<const char*, UniformBlock*> _updateBlockList;
		};
#endif

		class EffectsManager 
		{
			EffectsManager() {}
			~EffectsManager() {}
		public:
			static EffectsManager* GetInstance();
			static void CleanInstance();
			Effect* CreateEffect(const char* pKey, const char* i_pVSfilePath, const char* i_pFSfilePath, uint32_t renderState);
			Effect* GetEffect(const char* pKey);
			//bool AddEffect(const char* pKey, ShaderEffect* pEffect);

			std::map<const char*, Effect*> _effectManager;
		private:
			static EffectsManager* s_pEffectsManager;

		};
	}
}

RTTI_DECLARE_META_TYPE(EAE_Engine::Graphics::Effect)

#endif//EAE_ENGINE_GRAPHICS_EFFECTS

