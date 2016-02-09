#include "UniformBlock.h"
#include "GraphicsOp.h"
#include "Effect.h"
#include "General/MemoryOp.h"


#if defined( EAEENGINE_PLATFORM_GL )
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#endif

namespace EAE_Engine
{
	namespace Graphics
	{

#if defined( EAEENGINE_PLATFORM_GL )

		UniformBlock::UniformBlock(const char* pName, const GLint blockSize) :
			_pBlockName(_strdup(pName)), _blockSize(blockSize)
		{
			_pBuffer = new GLubyte[_blockSize];

		//	glGenBuffers(1, &_uboId);
		//	glBindBuffer(GL_UNIFORM_BUFFER, _uboId);
			CreateBindBufferObj(_uboId, GL_UNIFORM_BUFFER);
			glBufferData(GL_UNIFORM_BUFFER, _blockSize, NULL, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		UniformBlock::~UniformBlock() 
		{
			SAFE_DELETE_ARRAY(_pBlockName);
			SAFE_DELETE_ARRAY(_pBuffer);
			DeleteBufferObj(_uboId);
		}

		void UniformBlock::InitBlockBuffer(GLuint programID) 
		{/*
			GLuint index = glGetUniformBlockIndex(programID, _pBlockName);

			if (index != GL_INVALID_INDEX)
			{
				glUniformBlockBinding(programID, index, _uboId);

				GLint activeUniformsInBlock;
				glGetActiveUniformBlockiv(programID, index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &activeUniformsInBlock);

				GLint *indices = new GLint[activeUniformsInBlock];
				glGetActiveUniformBlockiv(programID, index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices);

				for (GLuint i = 0; i < activeUniformsInBlock; ++i)
				{
					const GLuint index = (GLuint)indices[i];

					glGetActiveUniformName(shader->id, index, 256, 0, variable);

					glGetActiveUniformsiv(shader->id, 1, &index, GL_UNIFORM_TYPE, &type);
					glGetActiveUniformsiv(shader->id, 1, &index, GL_UNIFORM_OFFSET, &offset);

					// now I can use name, type and offset!
				}
			}*/
		}
		
		void UniformBlock::SetBlockValue(void* pData, const GLuint bufferSize)
		{
			CopyMem((uint8_t*)pData, _pBuffer, bufferSize);
		}

		void UniformBlock::UpdateUniformBlockBuffer()
		{
			/*
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
			uniforms_block * block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER,
				0,
				sizeof(uniforms_block),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

			vmath::mat4 model_matrix = vmath::scale(7.0f);

			block->mv_matrix = view_matrix * model_matrix;
			block->view_matrix = view_matrix;
			block->proj_matrix = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight,
				0.1f, 1000.0f);
			glUnmapBuffer(GL_UNIFORM_BUFFER);
			*/
			glBindBuffer(GL_UNIFORM_BUFFER, _uboId);
			glBufferData(GL_UNIFORM_BUFFER, _blockSize, _pBuffer, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		void UniformBlock::AddOwner(Effect* pEffect)
		{
			_owners.push_back(pEffect);
		}

		void UniformBlock::NotifyOwners()
		{
			for (size_t i = 0; i < _owners.size(); ++i)
			{
				_owners[i]->OnNotify(this);
			}
		}

		////////////////////////UniformBlocksManager////////////////////////////

		UniformBlockManager::~UniformBlockManager() 
		{
			Clean();
		}

		void UniformBlockManager::AddUniformBlock(UniformBlock* pBlock)
		{
			_uniformBlocks.push_back(pBlock);
		}

		void UniformBlockManager::Clean() 
		{
			for (std::vector<UniformBlock*>::iterator it = _uniformBlocks.begin(); it != _uniformBlocks.end(); )
			{
				UniformBlock* pUB = *it++;
				SAFE_DELETE(pUB);
			}
			_uniformBlocks.clear();
		}

		UniformBlock* UniformBlockManager::GetUniformBlock(const char* pBlockName)
		{
			for (std::vector<UniformBlock*>::iterator iter = _uniformBlocks.begin(); iter != _uniformBlocks.end();)
			{
				UniformBlock* pUB = *iter++;
				if (strcmp(pUB->GetName(), pBlockName) == 0)
				{
					return pUB;
				}
			}
			return nullptr;
		}

		void UniformBlockManager::NotifyOwners(const char* pBlockName)
		{
			for (std::vector<UniformBlock*>::iterator iter = _uniformBlocks.begin(); iter != _uniformBlocks.end();)
			{
				UniformBlock* pUB = *iter++;
				if (strcmp(pUB->GetName(), pBlockName) == 0)
				{
					pUB->NotifyOwners();
					break;
				}
			}
		}

#endif 

	}
}

