#ifndef EAE_ENGINE_UNIFORM_BLOCK_H
#define EAE_ENGINE_UNIFORM_BLOCK_H

#include <vector>
#include "UniformDesc.h"
#include "General/Singleton.hpp"
#include "Math/ColMatrix.h"

#if defined( EAEENGINE_PLATFORM_D3D9 )
#include <d3dx9.h>
#endif

namespace EAE_Engine
{
	namespace Graphics
	{
		/*
		* Because the alignment in GLSL uniform block is different with the C/C++ structure.
		* So we use this structure to set each element of the Uniform Block.
		* _offset is the offset from the beginning of the UniformBlock->_pBuffer,
		* _pMemberBuffer is the buffer of the value of the member, it can be any type.
		* _sizeOfMemberBuffer shows how many bytes we need to copy.
		*/
		struct UniformBlockData
		{
			UniformBlockData() = default;
			uint32_t _offset;
			void* _pMemberBuffer;
			uint32_t _sizeOfMemberBuffer;
		};

		class Effect;
#if defined( EAEENGINE_PLATFORM_D3D9 )
		/*
		 * There is no UniformBlock in D3D9.
		 * We even don't have Constant Buffer in D3D9.
		 * However, we can use Structure to simulate it for now.
		 * The difference of UniformBlock with Uniform Variable is:
		 * Uniform Block will be used for several different Effect efficiently,
		 * because it shares the same chunk of buffer. 
		 * Also we just update the whole chunk of buffer instead of updating its member one by one.
		 */
		class UniformBlock
		{
		public:
			UniformBlock(const char* pName, const int blockSize, D3DXHANDLE handle, ShaderTypes shaderType);
			~UniformBlock();
			void AddOwner(Effect* pEffect);
			void NotifyOwners();
			const char* GetName() { return _pBlockName; }
			ShaderTypes GetType() { return _shaderType; }

			void SetBlockData(UniformBlockData* pUBD, uint32_t count);
			void UpdateUniformBlockBuffer(ID3DXConstantTable* pConstantTable);
		private:
			uint8_t* _pBuffer;
			int _blockSize;
			tUniformHandle _handle;
			ShaderTypes _shaderType;
			std::vector<Effect*> _owners;
			char* _pBlockName;
		};

#elif defined( EAEENGINE_PLATFORM_GL )
		class UniformBlock 
		{
		public:
			UniformBlock(const char* pName, const GLint blockSize);
			~UniformBlock();
			void AddOwner(Effect* pEffect);
			void NotifyOwners();
			const char* GetName() { return _pBlockName; }
			GLuint GetUboId() { return _uboId; }
			GLubyte* GetBuffer() { return _pBuffer; }
			GLint GetBufferSize() {	return _blockSize; }

			void SetBlockData(UniformBlockData* pUBD, uint32_t count);
			void UpdateUniformBlockBuffer();
		private:
			GLubyte* _pBuffer;
			GLint _blockSize;
			GLuint _uboId;
			std::vector<Effect*> _owners;
			char* _pBlockName;
		};

#endif 
		class UniformBlockManager : public Singleton<UniformBlockManager>
		{
		public:
			~UniformBlockManager();
			UniformBlock* AddUniformBlock(UniformBlock* pBlock);
			void Clean();
			bool Contains(const char* pBlockName);
			uint32_t GetIndexOfUniformBlock(const char* pBlockName);
			uint32_t GetUniformBlockCount() { return (uint32_t)_uniformBlocks.size(); }
			UniformBlock* GetUniformBlock(const char* pBlockName);
			void NotifyOwners(const char* pBlockName);
		private:
			std::vector<UniformBlock*> _uniformBlocks;
		};
	}
}

#endif//EAE_ENGINE_UNIFORM_BLOCK_H