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

			//glGenBuffers(1, &_uboId);
			//glBindBuffer(GL_UNIFORM_BUFFER, _uboId);
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

		void UniformBlock::SetBlockData(UniformBlockData* pUBD, uint32_t count)
		{
			for (uint32_t index = 0; index < count; ++index)
			{
				CopyMem((uint8_t*)pUBD[index]._pMemberBuffer, (uint8_t*)_pBuffer + pUBD[index]._offset, pUBD[index]._sizeOfMemberBuffer);
			}
		}

		void UniformBlock::UpdateUniformBlockBuffer()
		{
			glBindBuffer(GL_UNIFORM_BUFFER, _uboId);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, _blockSize, _pBuffer);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}


		////////////////////////UniformBlocksManager////////////////////////////

		UniformBlockManager::~UniformBlockManager() 
		{
			Clean();
		}

		UniformBlock* UniformBlockManager::AddUniformBlock(UniformBlock* pBlock)
		{
			for (std::vector<UniformBlock*>::iterator iter = _uniformBlocks.begin(); iter != _uniformBlocks.end();)
			{
				UniformBlock* pUB = *iter++;
				if (strcmp(pUB->GetName(), pBlock->GetName()) == 0)
				{
					return pUB;
				}
			}
			_uniformBlocks.push_back(pBlock);
			return pBlock;
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

		bool UniformBlockManager::Contains(const char* pBlockName)
		{
			for (std::vector<UniformBlock*>::iterator iter = _uniformBlocks.begin(); iter != _uniformBlocks.end(); iter++)
			{
				UniformBlock* pUB = *iter;
				if (strcmp(pUB->GetName(), pBlockName) == 0)
				{
					return true;
				}
			}
			return  false;
		}

		GLuint UniformBlockManager::GetIndexOfUniformBlock(const char* pBlockName)
		{
			GLuint index = 0;
			for (std::vector<UniformBlock*>::iterator iter = _uniformBlocks.begin(); iter != _uniformBlocks.end(); iter++, index++)
			{
				UniformBlock* pUB = *iter;
				if (strcmp(pUB->GetName(), pBlockName) == 0)
				{
					return index;
				}
			}
			return index;
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

