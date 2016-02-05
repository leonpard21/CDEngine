#include "../Common/MeshOp.h"
#include "Memory/Source/New.h"
#include "UserOutput/UserOutput.h"
#include <string>
#include <sstream>
#include <cassert>


namespace EAE_Engine
{
	namespace Graphics
	{
		bool RenderElements(AOSMesh* pAOSMesh, uint32_t subMeshIndex);
		bool RenderArrays(AOSMesh* pAOSMesh);

		AOSMesh* CreateAOSMeshInternal(MeshGLVertexElements elements,
			void* pVertices, uint32_t vertexCount,
			uint32_t* pIndices, uint32_t indexCount,
			sSubMesh* pSubMeshes, uint32_t subMeshCount)
		{
			AOSMesh* pMesh = new AOSMesh();
			pMesh->CreateBuffers(elements,
				pVertices, vertexCount,
				pIndices, indexCount,
				pSubMeshes, subMeshCount);
			return pMesh;
		}

		bool RenderAOSMeshInternal(AOSMesh* pAOSMesh, uint32_t subMeshIndex)
		{
			bool result = true;
			if (pAOSMesh->GetIndexCount() > 0)
			{
				// The mesh uses indices
				result = RenderElements(pAOSMesh, subMeshIndex);
			}
			else 
			{
				// The mesh doesn't uses indices
				result = RenderArrays(pAOSMesh);
			}
			return result;
		}

		bool RenderElements(AOSMesh* pAOSMesh, uint32_t subMeshIndex)
		{
			// Render objects from the current streams
			{
				// We are using triangles as the "primitive" type,
				// and we have defined the vertex buffer as a triangle list
				// (meaning that every triangle is defined by three vertices)
				const GLenum mode = pAOSMesh->GetPrimitiveMode();
				// We'll use 32-bit indices in this class to keep things simple
				// (i.e. every index will be a 32 bit unsigned integer)
				const GLenum indexType = GL_UNSIGNED_INT;
				const uint32_t sizeOfIndex = sizeof(uint32_t);
				// It is possible to start rendering in the middle of an index buffer
				{
					// Remember the void ptr should point to a byte, 
					// so we need to calculate the offset based on the size of each index
					const GLvoid* const offset = (GLvoid*)(pAOSMesh->GetSubMesh(subMeshIndex)->_firstIndex * sizeOfIndex);
					// Draw the AOSMesh
					const GLsizei vertexCountToRender = pAOSMesh->GetSubMesh(subMeshIndex)->GetIndicesCount();//count of vertices we want to render.
					glDrawElements(mode, vertexCountToRender, indexType, offset);
					assert(glGetError() == GL_NO_ERROR);
				}
			}
			return glGetError() == GL_NO_ERROR;
		}

		bool RenderArrays(AOSMesh* pAOSMesh)
		{
			// Render objects from the current streams
			{
				const GLenum mode = pAOSMesh->GetPrimitiveMode();
				const GLsizei vertexCountToRender = pAOSMesh->GetVertexCount();
				const GLint firstofTheVertices = 0;
				{
					// Draw the AOSMesh
					glDrawArrays(mode, firstofTheVertices, vertexCountToRender);
					assert(glGetError() == GL_NO_ERROR);
				}
			}
			return glGetError() == GL_NO_ERROR;
		}

		bool CreateVertexArrayObj(GLuint& io_vertexArrayId)
		{
			// Create a vertex array object and make it active
			const GLsizei arrayCount = 1;
			glGenVertexArrays(arrayCount, &io_vertexArrayId);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindVertexArray(io_vertexArrayId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the vertex array: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused vertex array ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				return false;
			}
			return true;
		}

		bool DeleteVertexArrayObj(GLuint& o_vertexArrayId)
		{
			if (o_vertexArrayId != 0)
			{
				const GLsizei arrayCount = 1;
				glDeleteVertexArrays(arrayCount, &o_vertexArrayId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to delete the vertex array: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					UserOutput::Print(errorMessage.str().c_str());
				}
				o_vertexArrayId = 0;
			}
			return true;
		}

		bool CreateBindBufferObj(GLuint& o_bufferID, GLenum bufferTarget)
		{
			o_bufferID = 0;
			// Create an index buffer object and make it active
			const GLsizei bufferCount = 1;
			glGenBuffers(bufferCount, &o_bufferID);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindBuffer(bufferTarget, o_bufferID);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the index buffer: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused index buffer ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				return false;
			}
			return true;
		}

		bool DeleteBufferObj(GLuint& io_BufferID, GLsizei bufferCount)
		{
			// if we dpn't delete the vertex buffer object, it will cause the memory leaks.
			if (io_BufferID != 0)
			{
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &io_BufferID);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to delete the vertex buffer: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
				io_BufferID = 0;
			}
			return true;
		}

		bool InitVertexFormat(const MeshGLVertexElements& elements, GLsizei stride)
		{
			MeshGLVertexElement* pVertexElements = elements._pGlements;
			// Initialize the vertex format
			GLvoid* offset = 0;
			for (GLint index = 0; index < elements._elementCount; ++index)
			{
				const GLuint vertexElementLocation = pVertexElements[index]._vertexElementLocation;
				const GLint elementCount = pVertexElements[index]._elementCount;
				const GLenum vertexType = pVertexElements[index]._type;
				const GLboolean normalized = pVertexElements[index]._normalized;
				glVertexAttribPointer(vertexElementLocation, elementCount, vertexType, normalized, stride, offset);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					glEnableVertexAttribArray(vertexElementLocation);
					const GLenum errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						offset = reinterpret_cast<GLvoid*>(reinterpret_cast<uint8_t*>(offset) + (elementCount * pVertexElements[index]._sizeOfType));
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to enable the vertex attribute: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
						return false;
					}
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the vertex attribute: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
			}
			return true;
		}

		bool FillGLBuffer(GLenum bufferTarget, void* pVertices, const uint32_t bufferSize, GLenum usage)
		{
			// Assign the data to the vertex buffer
			glBufferData(bufferTarget, bufferSize, NULL, usage);
			glBufferSubData(bufferTarget, 0, bufferSize, reinterpret_cast<GLvoid*>(pVertices));
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to allocate the vertex buffer: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				return false;
			}
			return true;
		}

	}
}

