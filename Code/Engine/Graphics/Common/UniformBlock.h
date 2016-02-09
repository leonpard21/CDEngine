#ifndef EAE_ENGINE_UNIFORM_BLOCK_H
#define EAE_ENGINE_UNIFORM_BLOCK_H

#include <vector>
#include "UniformDesc.h"
#include "General/Singleton.hpp"
#include "Math/ColMatrix.h"

namespace EAE_Engine
{
	namespace Graphics
	{

#if defined( EAEENGINE_PLATFORM_D3D9 )
		class UniformBlock
		{
		};

		class UniformBlockManager : public Singleton<UniformBlockManager>
		{
		public:
			~UniformBlockManager() {}
		};

#elif defined( EAEENGINE_PLATFORM_GL )

		struct CameraMatrices
		{
			Math::ColMatrix44 _worldViewMatrix;
			Math::ColMatrix44 _viewProjMatrix;
		};

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
			char* _pBlockName;
			GLint _blockSize;
			GLubyte* _pBuffer;
			GLuint _uboId;
			std::vector<Effect*> _owners;
		};


		class UniformBlockManager : public Singleton<UniformBlockManager>
		{
		public:
			~UniformBlockManager();
			void AddUniformBlock(UniformBlock* pBlock);
			void Clean();

			UniformBlock* GetUniformBlock(const char* pBlockName);
			void NotifyOwners(const char* pBlockName);
		private:
			std::vector<UniformBlock*> _uniformBlocks;
		};

		
		/////////////////////////////////////////////////////////////////

#endif 



	}
}

#endif//EAE_ENGINE_UNIFORM_BLOCK_H