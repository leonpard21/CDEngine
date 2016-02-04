#include "../Common/AOSMesh.h"
#include "../../UserOutput/UserOutput.h"
#include <string>
#include <sstream>
#include <cassert>

namespace EAE_Engine
{
	namespace Graphics
	{
		AOSMesh::AOSMesh() : _vertexArrayId(0), 
			_vertexBufferId(0), _indexBufferId(0), 
			_vertexCount(0), _indexCount(0), 
			_primitiveMode(GL_TRIANGLES), _stride(0) {}

		AOSMesh::~AOSMesh()
		{
		}
	
		bool AOSMesh::CreateBuffers(MeshGLVertexElements elements,
			void* pVertices, uint32_t vertexCount,
			uint32_t* pIndexData, uint32_t indexCount,
			sSubMesh* pSubMeshes, uint32_t subMeshCount)
		{
			bool wereThereErrors = false;
			_vertexBufferId = 0;
			_indexBufferId = 0;
			_primitiveMode = elements._primitiveMode;
			_stride = elements._stride;
			assert(_stride > 0);
			// Create a vertex array object and make it active
			if (!CreateVertexArrayObj()) 
			{
				wereThereErrors = true;
				goto OnExit;
			}
			if (!CreateVertexBufferObj(_vertexBufferId))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			if (!InitVertexFormat(elements))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			if (!FillVertexBuffer(pVertices, vertexCount))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			if (!CreateIndexBufferObj(_indexBufferId))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			if (!FillIndexBuffer(pIndexData, indexCount, pSubMeshes, subMeshCount))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		OnExit:
			// Delete the buffer object
			// (the vertex array will hold a reference to it)
			if (_vertexArrayId != 0)
			{
				// Unbind the vertex array
				// (this must be done before deleting the vertex buffer)
				glBindVertexArray(0);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (_vertexBufferId != 0)
					{
						// NOTE: If you delete the vertex buffer object here, as I recommend,
						// then gDEBugger won't know about it and you won't be able to examine the data.
						// If you find yourself in a situation where you want to see the buffer object data in gDEBugger
						// comment out this block of code temporarily
						// (doing this will cause a memory leak so make sure to restore the deletion code after you're done debugging).
						//wereThereErrors = !DeleteVertexBufferObj(_vertexBufferId);
					}
					if (_indexBufferId != 0)
					{
						// NOTE: See the same comment above about deleting the vertex buffer object here and gDEBugger
						// holds true for the index buffer
						//wereThereErrors = !DeleteIndexBufferObj(_indexBufferId);
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to unbind the vertex array: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				}
			}
			return !wereThereErrors;
		}

		bool AOSMesh::CreateVertexArrayObj() 
		{
			// Create a vertex array object and make it active
			const GLsizei arrayCount = 1;
			glGenVertexArrays(arrayCount, &_vertexArrayId);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindVertexArray(_vertexArrayId);
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

		bool AOSMesh::CreateVertexBufferObj(GLuint& io_vertexBufferID)
		{
			io_vertexBufferID = 0;
			// Create a vertex buffer object and make it active
			const GLsizei bufferCount = 1;
			glGenBuffers(bufferCount, &io_vertexBufferID);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindBuffer(GL_ARRAY_BUFFER, io_vertexBufferID);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the vertex buffer: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused vertex buffer ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
				return false;
			}
			return true;
		}

		bool AOSMesh::InitVertexFormat(const MeshGLVertexElements& elements) const
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
				glVertexAttribPointer(vertexElementLocation, elementCount, vertexType, normalized, _stride, offset);
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
		bool AOSMesh::FillVertexBuffer(void* pVertices, uint32_t vertexCount)
		{
			// Assign the data to the vertex buffer
			_vertexCount = vertexCount;//record how many vertex in this AOSMesh
			glBufferData(GL_ARRAY_BUFFER, vertexCount * _stride, NULL, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * _stride, reinterpret_cast<GLvoid*>(pVertices));
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

		bool AOSMesh::CreateIndexBufferObj(GLuint& io_indexBufferID)
		{
			io_indexBufferID = 0;
			// Create an index buffer object and make it active
			const GLsizei bufferCount = 1;
			glGenBuffers(bufferCount, &io_indexBufferID);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, io_indexBufferID);
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

		bool AOSMesh::FillIndexBuffer(uint32_t* pIndexData, uint32_t indexCount, sSubMesh* pSubMeshes, uint32_t subMeshCount)
		{
			// Allocate space and copy the triangle data into the index buffer
			{
				_indexCount = indexCount;
				const GLsizeiptr bufferSize = indexCount * sizeof(uint32_t);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bufferSize, reinterpret_cast<const GLvoid*>(pIndexData));
				/*glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, reinterpret_cast<const GLvoid*>(pIndexData),
					GL_STREAM_DRAW);*/
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to allocate the index buffer: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
			}
			// Set the SubMeshes Information
			for (uint32_t subMeshIndex = 0; subMeshIndex < subMeshCount; ++subMeshIndex)
			{
				sSubMesh pMesh(pSubMeshes[subMeshIndex]._firstIndex, pSubMeshes[subMeshIndex]._lastIndex);
				_subMeshes.push_back(pMesh);
			}
			return true;
		}

		bool AOSMesh::DeleteVertexBufferObj(GLuint& io_vertexBufferID)
		{
			// if we dpn't delete the vertex buffer object, it will cause the memory leaks.
			if (io_vertexBufferID != 0)
			{
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &io_vertexBufferID);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to delete the vertex buffer: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
				io_vertexBufferID = 0;
			}
			return true;
		}
		
		bool AOSMesh::DeleteIndexBufferObj(GLuint& io_indexBufferID) 
		{
			// if we don't delete the index buffer object, it will cause the memory leaks.
			if (io_indexBufferID != 0)
			{
				// NOTE: See the same comment above about deleting the vertex buffer object here and gDEBugger
				// holds true for the index buffer
				const GLsizei bufferCount = 1;
				glDeleteBuffers(bufferCount, &io_indexBufferID);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "\nOpenGL failed to delete the index buffer: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					EAE_Engine::UserOutput::Print(errorMessage.str().c_str());
					return false;
				}
				io_indexBufferID = 0;
			}
			return true;
		}

		void AOSMesh::Release()
		{
			if (_vertexArrayId != 0)
			{
				const GLsizei arrayCount = 1;
				glDeleteVertexArrays(arrayCount, &_vertexArrayId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to delete the vertex array: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					UserOutput::Print(errorMessage.str().c_str());
				}
				_vertexArrayId = 0;
			}
			// Just make sure that we have clean the Vertex and Index buffers.
			if (_vertexBufferId != 0)
			{
				DeleteVertexBufferObj(_vertexBufferId);
			}
			if (_indexBufferId != 0)
			{
				DeleteIndexBufferObj(_indexBufferId);
			}
			//clean all of the submeshes.
			_subMeshes.clear();
		}

		void AOSMesh::ChangeWholeBuffers(void* pVertices, uint32_t vertexCount, uint32_t* pIndexData, uint32_t indexCount, sSubMesh* pSubMeshes, uint32_t subMeshCount)
		{
			//clean all of the submeshes.
			_subMeshes.clear();
			// We can use glBufferSubData() or glMapBuffer to change the buffer.
			// However, because we want to change the Whole chunk of the memory, 
			// So it is good to use glBufferData(), 
			// which means we call the FillVertexBuffer and FillIndexBuffer again.
			glBindVertexArray(_vertexArrayId);
			//Update VOA
			glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
			FillVertexBuffer(pVertices, vertexCount);
			//Update IOA
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
			FillIndexBuffer(pIndexData, indexCount, pSubMeshes, subMeshCount);		
			glBindVertexArray(0);
			// Check the errors so far.
			GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to update the mesh buffers " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				UserOutput::Print(errorMessage.str().c_str());
			}
		}

		sSubMesh* AOSMesh::GetSubMesh(uint32_t subMeshIndex)
		{
			if (subMeshIndex > _subMeshes.size())
			{
				return nullptr;
			}
			return &_subMeshes[subMeshIndex];
		};

		void AOSMesh::SetSubMeshes(std::vector<sSubMesh>& subMeshes)
		{
			_subMeshes.clear();
			_subMeshes = subMeshes;
		}

	}
}