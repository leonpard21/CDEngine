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
		/*
		 * Because we need to set the UniformBlock and we should know their structures in advance,
		 * so we list the setter functions for whatever structures we known.

		 * Remember that the alignment in GLSL is different with C/C++,
		 * so we really need to know the structures' alignment in advances.
		 * That's why we cannot copy and write the buffer directly for most of the structures.
		 */
		void SetCameraMatricesBlock(UniformBlock* pUB, CameraMatrices& cameraMatrices);

#endif 



	}
}

#endif//EAE_ENGINE_UNIFORM_BLOCK_H