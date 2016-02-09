#ifndef EAE_ENGINE_UNIFORM_BLOCK_H
#define EAE_ENGINE_UNIFORM_BLOCK_H

#include <vector>
#include "UniformDesc.h"
#include "General/Singleton.hpp"

namespace EAE_Engine
{
	namespace Graphics
	{

#if defined( EAEENGINE_PLATFORM_D3D9 )
		class UniformBlock
		{
		};
#elif defined( EAEENGINE_PLATFORM_GL )
		class Effect;
		class UniformBlock 
		{
		public:
			UniformBlock(const char* pName, const GLint blockSize);
			~UniformBlock();
			const char* GetName() { return _pBlockName; }
			GLuint GetUboId() { return _uboId; }

			void InitBlockBuffer(GLuint programID);
			void SetBlockValue(void* pData, const GLuint bufferSize);
			void UpdateUniformBlockBuffer();
			void AddOwner(Effect* pEffect);
			void NotifyOwners();

		private:
			char* _pBlockName;
			GLint _blockSize;
			GLubyte* _pBuffer;
			GLuint _uboId;
			std::vector<Effect*> _owners;
		};

#endif 
	
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


	}
}

#endif//EAE_ENGINE_UNIFORM_BLOCK_H