#include "../Common/AOSMeshOp.h"
#include "../Common/Device.h"
#include "Memory/Source/New.h"


namespace EAE_Engine
{
	namespace Graphics
	{
		AOSMesh* CreateAOSMeshInternal(MeshD3DVertexElements elements,
			void* pVertices, uint32_t vertexCount, uint32_t* pIndices, uint32_t indexCount,
			sSubMesh* pSubMeshes, uint32_t subMeshCount)
		{
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (!pD3DDevice) return nullptr;

			AOSMesh* pMesh = new AOSMesh();
			pMesh->CreateBuffers(pD3DDevice, elements,
				pVertices, vertexCount,
				pIndices, indexCount,
				pSubMeshes, subMeshCount);
			return pMesh;
		}

		bool RenderAOSMeshInternal(AOSMesh *pAOSMesh, uint32_t subMeshIndex)
		{
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (!pD3DDevice) return false;

			HRESULT result = true;
			// Render objects from the current streams
			{
				// We are using triangles as the "primitive" type,
				// and we have defined the vertex buffer as a triangle list
				// (meaning that every triangle is defined by three vertices)
				const D3DPRIMITIVETYPE primitiveType = pAOSMesh->GetPrimitiveType();
				// It's possible to start rendering primitives in the middle of the stream
				const unsigned int indexOfFirstVertexToRender = 0;
				// We are drawing the AOSMesh
				{
					const unsigned int indexOfFirstIndexToUse = pAOSMesh->GetSubMesh(subMeshIndex)->_firstIndex;
					const unsigned int vertexCountToRender = pAOSMesh->GetVertexCount();	   // How vertices from the vertex buffer will be used?
					const unsigned int indicesInSubMesh = pAOSMesh->GetSubMesh(subMeshIndex)->GetIndicesCount(); // How many triangles will be drawn?
					unsigned int primitiveCountToRender = 0;
					switch(primitiveType)
					{
					case D3DPT_TRIANGLELIST:
						primitiveCountToRender = indicesInSubMesh / 3;
						break;
					case D3DPT_LINELIST:
						primitiveCountToRender = indicesInSubMesh / 2;
						break;
					default:
						primitiveCountToRender = indicesInSubMesh / 3;
					}
					result = pD3DDevice->DrawIndexedPrimitive(primitiveType,
						indexOfFirstVertexToRender, indexOfFirstVertexToRender, vertexCountToRender,
						indexOfFirstIndexToUse, primitiveCountToRender);
					assert(SUCCEEDED(result));
				}
			}
			return SUCCEEDED(result);
		}

	}
}


