#include "../Common/AOSMesh.h"
#include "../Common/AOSMeshOp.h"
#include "UserOutput/UserOutput.h"
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
			if (!CreateVertexArrayObj(_vertexArrayId)) 
			{
				wereThereErrors = true;
				goto OnExit;
			}
			if (!CreateVertexBufferElements(elements, pVertices, vertexCount))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			if (!CreateIndexBuffer(pIndexData, indexCount, pSubMeshes, subMeshCount))
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

		bool AOSMesh::CreateVertexBufferElements(MeshGLVertexElements elements, void* pVertices, uint32_t vertexCount)
		{
			bool result = true;
			//Create a buffer for vertex, remember that we need to use GL_ARRAY_BUFFER for vertex elements
			if (!CreateBindBufferObj(_vertexBufferId, GL_ARRAY_BUFFER))
			{
				result = false;
			}
			if (!InitVertexFormat(elements, _stride))
			{
				result = false;
			}
			// Assign the data to the vertex buffer
			 _vertexCount = vertexCount;//record how many vertex in this AOSMesh
			const uint32_t bufferSize = vertexCount * _stride;
			result = FillGLBuffer(GL_ARRAY_BUFFER, pVertices, bufferSize, GL_STATIC_DRAW);
			return result;
		}

		bool AOSMesh::CreateIndexBuffer(uint32_t* pIndexData, uint32_t indexCount, sSubMesh* pSubMeshes, uint32_t subMeshCount)
		{
			bool result = true;
			if (!CreateBindBufferObj(_indexBufferId, GL_ELEMENT_ARRAY_BUFFER))
			{
				result = false;
			}
			// Allocate space and copy the triangle data into the index buffer
			_indexCount = indexCount;
			const GLsizeiptr bufferSize = indexCount * sizeof(uint32_t);
			result = FillGLBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndexData, bufferSize, GL_STATIC_DRAW);
			// Set the SubMeshes Information
			for (uint32_t subMeshIndex = 0; subMeshIndex < subMeshCount; ++subMeshIndex)
			{
				sSubMesh pMesh(pSubMeshes[subMeshIndex]._firstIndex, pSubMeshes[subMeshIndex]._lastIndex);
				_subMeshes.push_back(pMesh);
			}
			return result;
		}

		void AOSMesh::Release()
		{
			if (_vertexArrayId != 0) 
			{
				DeleteVertexArrayObj(_vertexArrayId);
			}
			// Just make sure that we have clean the Vertex and Index buffers.
			if (_vertexBufferId != 0)
			{
				DeleteBufferObj(_vertexBufferId, 1);
			}
			if (_indexBufferId != 0)
			{
				DeleteBufferObj(_indexBufferId, 1);
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
			_vertexCount = vertexCount;
			bool result = FillGLBuffer(GL_ARRAY_BUFFER, pVertices, _vertexCount * _stride, GL_STATIC_DRAW);
			//Update IOA
			if (pIndexData != nullptr) 
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
				_indexCount = indexCount;
				result = FillGLBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndexData, indexCount * sizeof(uint32_t), GL_STATIC_DRAW);
				// Set the SubMeshes Information
				for (uint32_t subMeshIndex = 0; subMeshIndex < subMeshCount; ++subMeshIndex)
				{
					sSubMesh pMesh(pSubMeshes[subMeshIndex]._firstIndex, pSubMeshes[subMeshIndex]._lastIndex);
					_subMeshes.push_back(pMesh);
				}
			}
			else 
			{
				if (_indexBufferId != 0)
				{
					DeleteBufferObj(_indexBufferId, 1);
				}
				_indexCount = 0;
			}
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