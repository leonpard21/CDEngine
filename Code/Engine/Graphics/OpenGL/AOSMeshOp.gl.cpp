#include "../Common/AOSMeshOp.h"
#include "Memory/Source/New.h"

namespace EAE_Engine
{
	namespace Graphics
	{
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
	}
}


