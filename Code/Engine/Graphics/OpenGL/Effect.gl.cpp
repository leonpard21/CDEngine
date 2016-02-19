#include "../Common/Effect.h"
#include "../Common/GraphicsOp.h"
#include <cstdint>
#include <string>
#include <sstream>
#include <cassert>
#include "Engine/Windows/WindowsFunctions.h"
#include "../../UserOutput/UserOutput.h"


namespace EAE_Engine
{
	namespace Graphics
	{
		bool Effect::Init(const char* i_pVSfilePath, const char* i_pFSfilePath, uint32_t renderState)
		{
			// Create a program
			if (!CreateProgram(_programId))
				return false;
			// Load and attach the shaders
			if (!LoadAttachVertexShader(_programId, i_pVSfilePath))
				return false;
			if (!LoadAttachFragmentShader(_programId, i_pFSfilePath))
				return false;
			// Link the program
			if (!LinkProgramAfterAttached(_programId))
				return false;
			ExtractShaderUniformBlocks();
			ExtractShaderUniforms();
			_renderState = renderState;
			return true;
		}

		bool Effect::Release()
		{
			return DeleteProgram(_programId);
		}

		bool Effect::BeginRender() 
		{
			assert(glGetError() == GL_NO_ERROR);
			return glGetError() == GL_NO_ERROR;
		}

		bool Effect::EndRender()
		{
			assert(glGetError() == GL_NO_ERROR);
			return glGetError() == GL_NO_ERROR;
		}

		bool Effect::LoadAttachVertexShader(GLuint& io_programId, const char* i_pVSFilePath)
		{
			bool wereThereErrors = false;
			GLuint vertexShaderId = 0;
			// Load and compile the vertex shader
			if (!LoadCompileShader(vertexShaderId, i_pVSFilePath, GL_VERTEX_SHADER)) 
			{
				wereThereErrors = true;
				goto OnExit;
			}
			// Attach the shader to the program
			if (!AttachShaderToProgram(io_programId, vertexShaderId))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		OnExit:
			// Even if the shader was successfully compiled
			// once it has been attached to the program we can (and should) delete our reference to it
			// (any associated memory that OpenGL has allocated internally will be freed
			// once the program is deleted)
			DeleteShader(vertexShaderId);
			return !wereThereErrors;
		}
	
		bool Effect::LoadAttachFragmentShader(GLuint& io_programId, const char* i_pFSFilePath)
		{
			bool wereThereErrors = false;
			// Load the source code from file and set it into a shader
			GLuint fragmentShaderId = 0;
			if (!LoadCompileShader(fragmentShaderId, i_pFSFilePath, GL_FRAGMENT_SHADER))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			// Attach the shader to the program
			if (!AttachShaderToProgram(io_programId, fragmentShaderId))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		OnExit:
			// Even if the shader was successfully compiled
			// once it has been attached to the program we can (and should) delete our reference to it
			// (any associated memory that OpenGL has allocated internally will be freed
			// once the program is deleted)
			DeleteShader(fragmentShaderId);
			return !wereThereErrors;
		}

		void Effect::ExtractShaderUniformBlocks()
		{
			GLint numBlocks;
			glGetProgramiv(_programId, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
			for (int blockIx = 0; blockIx < numBlocks; ++blockIx)
			{
				GLint nameLen;
				glGetActiveUniformBlockiv(_programId, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);
				std::vector<GLchar> name; //Yes, not std::string. There's a reason for that.
				name.resize(nameLen);
				glGetActiveUniformBlockName(_programId, blockIx, nameLen, NULL, &name[0]);
				std::string blockName;
				blockName.assign(name.begin(), name.end() - 1);
				GLint uniformBlockSize;
				glGetActiveUniformBlockiv(_programId, blockIx, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);
				if (!UniformBlockManager::GetInstance()->Contains(blockName.c_str()))
				{
					UniformBlock* pUniformBlock = new UniformBlock(blockName.c_str(), uniformBlockSize);
					// the 2nd parameter is the index of the index Of uniform block,
					// the 3nd parameter is the openGL binding point, which is not the buffer we want to use.
					// the same UniformBlock in different shader, should bind to the same Binding point,
					// we also bind this binding point to the BufferObj, 
					// which contains the data we will offer to the graphics card.
					// in this case, same uniform block in different shader are sharing data in the same buffer.
					glUniformBlockBinding(_programId, blockIx, UniformBlockManager::GetInstance()->GetUniformBlockCount());
					pUniformBlock->AddOwner(this);
					UniformBlockManager::GetInstance()->AddUniformBlock(pUniformBlock);
				}
			}
		}


		void Effect::ExtractShaderUniforms()
		{
			const int BUFF_SIZE = 64;
			GLint count;
			glGetProgramiv(_programId, GL_ACTIVE_UNIFORMS, &count);
			for (int i = 0; i < count; ++i)
			{
				char name[BUFF_SIZE]; // for holding the variable name
				GLint size = BUFF_SIZE;
				GLenum type;
				GLsizei length;
				GLsizei bufSize = BUFF_SIZE;
				glGetActiveUniform(_programId, i, bufSize, &length, &size, &type, name);
				// initialize the location to - 1, and be aware that glGetUniformLocation() returns - 1 if it didn't find the uniform.
				// (Note that if you declare a uniform but don't use it in the shader,
				// it will be optimized out and glGetUniformLocation() won't find it.)
				GLint location = glGetUniformLocation(_programId, name);
				// if the uniform name doesn't exist or it is a member of UniformBlock,
				// then it'll have a location -1, which is invalid.
				if (location == -1)
					continue;
				if (type == GL_FLOAT_MAT4)
				{
					UniformVariable* pUV = UniformVariableManager::GetInstance().GetUniformVariable(name, UniformType::mat4);
					pUV->AddOwner(this, location);
				}
				else if (type == GL_FLOAT_VEC4)
				{
					UniformVariable* pUV = UniformVariableManager::GetInstance().GetUniformVariable(name, UniformType::float4);
					pUV->AddOwner(this, location);
				}
				else if (type == GL_FLOAT_VEC3)
				{
					UniformVariable* pUV = UniformVariableManager::GetInstance().GetUniformVariable(name, UniformType::float3);
					pUV->AddOwner(this, location);
				}
				else if (type == GL_FLOAT)
				{
					UniformVariable* pUV = UniformVariableManager::GetInstance().GetUniformVariable(name, UniformType::float1);
					pUV->AddOwner(this, location);
				}
			}
		}

		void Effect::SetUniform(UniformType type, GLint location, void* pArray, size_t count)
		{
			if (location == -1) 
				return;
			if (type == UniformType::float1)
			{
				// Set a single 3-dimensional float at that location
				glUniform1fv(location, 1, (float*)pArray);
			}
			if (type == UniformType::float2)
			{
				// Set a single 2-dimensional float at that location
				glUniform2fv(location, count / 2, (float*)pArray);
			}
			if (type == UniformType::float3)
			{
				// Set a single 3-dimensional float at that location
				glUniform3fv(location, 1, (float*)pArray);
			}
			else if (type == UniformType::mat4)
			{
				const GLboolean dontTranspose = false; // Matrices are already in the correct format
				const GLsizei uniformCountToSet = 1;
				// OpenGL does store matrices as columns, and so in this case we don't want the API to transpose anything.
				// reinterpret_cast trick is used as in Direct3D. 
				// This wouldn't work if GLfloat wasn't a float
				glUniformMatrix4fv(location, uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(pArray));
				GLenum errorCode = glGetError();
				assert(GL_NO_ERROR == glGetError());
			}
		}

		void Effect::SetUniform(GLint location, void* pArray, size_t count)
		{
			if (location == -1) return;
			if (count == 1)
			{
				// Set a single 3-dimensional float at that location
				glUniform1fv(location, 1, (float*)pArray);
			}
			if (count == 2)
			{
				// Set a single 2-dimensional float at that location
				glUniform2fv(location, count / 2, (float*)pArray);
			}
			if (count == 3)
			{
				// Set a single 3-dimensional float at that location
				glUniform3fv(location, 1, (float*)pArray);
			}
			else if (count == 16)
			{
				const GLboolean dontTranspose = false; // Matrices are already in the correct format
				const GLsizei uniformCountToSet = 1;
				// OpenGL does store matrices as columns, and so in this case we don't want the API to transpose anything.
				// reinterpret_cast trick is used as in Direct3D. 
				// This wouldn't work if GLfloat wasn't a float
				glUniformMatrix4fv(location, uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(pArray));
			}
		}

		GLint Effect::GetLocation(const char* pName)
		{
			GLint location = glGetUniformLocation(_programId, pName);
			const GLenum errorCode = glGetError();
			return location;
		}

		tSamplerID Effect::GetSamplerID(const char* pName, ShaderTypes shaderType)
		{
			GLint location = glGetUniformLocation(_programId, pName);
			return location;
		}

		void Effect::OnNotify(UniformVariable* pVariable, GLint location)
		{
			// if any uniform variable has been changed, we need to record it
			_updateVariableList[location] = pVariable;
		}

		void Effect::OnNotify(UniformBlock* pBlock)
		{
			_updateBlockList[pBlock->GetName()] = pBlock;
		}

		void Effect::Update()
		{
			// for the changed uniform variables, we should update them for the graphic cards.
			for (std::map<GLint, UniformVariable*>::iterator iter = _updateVariableList.begin(); iter != _updateVariableList.end(); ++iter)
			{
				GLint location = iter->first;
				if (location == -1) return;
				UniformVariable* pValue = iter->second;
				SetUniform(pValue->GetUniformType(), location, pValue->GetElements(), pValue->GetElementCount());
			}
			_updateVariableList.clear();
			for (std::map<const char*, UniformBlock*>::iterator iter = _updateBlockList.begin(); iter != _updateBlockList.end(); ++iter)
			{
				UniformBlock* pBlock = iter->second;
				GLuint bindingPoint = UniformBlockManager::GetInstance()->GetIndexOfUniformBlock(iter->first);
				glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, pBlock->GetUboId());
				assert(glGetError() == GL_NO_ERROR);
				pBlock->UpdateUniformBlockBuffer();
			}
			_updateBlockList.clear();
		}


		////////////////////////////////////////////////////////////////////
	}
}



