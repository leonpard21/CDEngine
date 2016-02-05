#include "../Common/AOSMesh.h"
#include "../Common/Device.h"
#include "../Common/MeshOp.h"
#include "UserOutput/UserOutput.h"
#include <d3d9.h>
#include <cassert>

namespace 
{

}

namespace EAE_Engine 
{
	namespace Graphics
	{
		AOSMesh::~AOSMesh() 
		{
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

		bool AOSMesh::CreateBuffers(IDirect3DDevice9* pDevice, MeshD3DVertexElements vertexElement,
			void* pVertices, uint32_t vertexDataCount,
			uint32_t* pIndeices, uint32_t indexCount,
			sSubMesh* pSubMeshes, uint32_t subMeshCount)
		{
			if (!InitVertexElements(pDevice, vertexElement))
			{
				goto OnError;
			}
			if (!InitVertexBuffer(pDevice, pVertices, vertexDataCount))
			{
				goto OnError;
			}
			if (!InitIndexBuffer(pDevice, pIndeices, indexCount, pSubMeshes, subMeshCount))
			{
				goto OnError;
			}
			return true;
		OnError:
			Release();
			return false;
		}

		bool AOSMesh::InitVertexElements(IDirect3DDevice9* pDevice, MeshD3DVertexElements vertexElement)
		{
			const uint32_t elementCount = vertexElement._elementCount + 1;
			D3DVERTEXELEMENT9* pElements = new D3DVERTEXELEMENT9[elementCount];
			uint32_t index = 0;
			for (; index < elementCount - 1; ++index)
			{
				pElements[index] = vertexElement._pGlements[index];
			}
			pElements[index] = D3DDECL_END(); //Remember to add this DeclearationEnd for D3D.
			_bufferInfo = vertexElement._bufferInfo;
			bool result = CreateVertexDeclaration(pDevice, pElements, _pVertexDeclaration);
			SAFE_DELETE(pElements);
			return true;
		}

		bool AOSMesh::InitUsage(IDirect3DDevice9* pDevice, UsageInfo usageInfo)
		{

			return true;
		}

		bool AOSMesh::InitVertexBuffer(IDirect3DDevice9* pDevice, void* pVertices, uint32_t vertexCount)
		{
			// The usage tells Direct3D how this vertex buffer will be used
			DWORD usage = 0;
			{
				// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
				const HRESULT result = GetVertexProcessingUsage(pDevice, usage);
				if (FAILED(result))
				{
					return false;
				}
				// Our code will only ever write to the buffer
				usage |= _bufferInfo._usage | D3DUSAGE_WRITEONLY;
			}

			// Get stride of the vertx
			WORD stride = _bufferInfo._stride;  //the byte offset between consecutive generic vertex attributes
			assert(stride > 0);//we assert that the _stride has been set.
			// Set Mesh PrimitiveType
			D3DPRIMITIVETYPE primitiveType = _bufferInfo._primitiveType;
			_vertexCount = vertexCount;//record vertexCount of this AOSMesh.
			const uint32_t bufferSize = _vertexCount * stride;
			bool result = CreateVertexBuffer(_pVertexBuffer, pDevice, usage, bufferSize);
			result = FillVertexBuffer(_pVertexBuffer, pDevice, pVertices, bufferSize);
			return result;
		}

		bool AOSMesh::InitIndexBuffer(IDirect3DDevice9* pDevice,
			uint32_t* pIndices, uint32_t indexCount, 
			sSubMesh* pSubMeshes, uint32_t subMeshCount)
		{
			if (pIndices == nullptr)
				return true;
			// The usage tells Direct3D how this vertex buffer will be used
			DWORD usage = 0;
			{
				// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
				const HRESULT result = GetVertexProcessingUsage(pDevice, usage);
				if (FAILED(result))
				{
					return false;
				}
				// Our code will only ever write to the buffer
				usage |= D3DUSAGE_WRITEONLY;
			}
			// Create an index buffer
			_indexCount = indexCount;
			const unsigned int bufferSize = indexCount * sizeof(uint32_t);
			bool result = CreateIndexBuffer(_pIndexBuffer, pDevice, usage, bufferSize);
			result = FillIndexBuffer(_pIndexBuffer, pDevice, pIndices, bufferSize);
			if (pSubMeshes == nullptr)
				return result;
			// Set the SubMeshes Information
			for (uint32_t subMeshIndex = 0; subMeshIndex < subMeshCount; ++subMeshIndex)
			{
				sSubMesh subMesh;
				subMesh._firstIndex = pSubMeshes[subMeshIndex]._firstIndex;
				subMesh._lastIndex = pSubMeshes[subMeshIndex]._lastIndex;
				_subMeshes.push_back(subMesh);
			}
			return result;
		}

		void AOSMesh::Release()
		{
			IDirect3DDevice9* pDevice = GetD3DDevice();
			if (_pVertexBuffer)
			{
				_pVertexBuffer->Release();
				_pVertexBuffer = NULL;
			}
			if (_pIndexBuffer)
			{
				SAFE_RELEASE(_pIndexBuffer);
			}
			if (_pVertexDeclaration)
			{
				pDevice->SetVertexDeclaration(NULL);
				_pVertexDeclaration->Release();
				_pVertexDeclaration = NULL;
			}
			//clean all of the submeshes.
			_subMeshes.clear();
		}

		void AOSMesh::ChangeWholeBuffers(void* pVertices, uint32_t vertexDataCount, uint32_t* pIndeices, uint32_t indexCount, sSubMesh* pSubMeshes, uint32_t subMeshCount)
		{
			IDirect3DDevice9* pDevice = GetD3DDevice();
			if (_pVertexBuffer)
			{
				_pVertexBuffer->Release();
				_pVertexBuffer = NULL;
			}
			if (_pIndexBuffer)
			{
				SAFE_RELEASE(_pIndexBuffer);
			}
			//clean all of the submeshes.
			_subMeshes.clear();
			InitVertexBuffer(pDevice, pVertices, vertexDataCount);
			InitIndexBuffer(pDevice, pIndeices, indexCount, pSubMeshes, subMeshCount);
		}

		//////////////////////////////////////Interal Functions//////////////////////////////////////////////
		bool RenderIndexPremitives(AOSMesh *pAOSMesh, uint32_t subMeshIndex);
		bool RenderNoIndexPremitives(AOSMesh *pAOSMesh);


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
			bool result = true;
			if (pAOSMesh->GetIndexCount() > 0)
			{
				// The mesh uses indices
				result = RenderIndexPremitives(pAOSMesh, subMeshIndex);
			}
			else
			{
				// The mesh doesn't uses indices
				result = RenderNoIndexPremitives(pAOSMesh);
			}
			return result;
		}

		bool RenderIndexPremitives(AOSMesh *pAOSMesh, uint32_t subMeshIndex)
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
					switch (primitiveType)
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

		bool RenderNoIndexPremitives(AOSMesh *pAOSMesh)
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
				const unsigned int vertexCountToRender = pAOSMesh->GetVertexCount();// How vertices from the vertex buffer will be used?
				// We are drawing the AOSMesh
				{
					unsigned int primitiveCountToRender = vertexCountToRender;
					switch (primitiveType)
					{
					case D3DPT_TRIANGLEFAN:
						primitiveCountToRender = vertexCountToRender - 2;
						break;
					default:
						primitiveCountToRender = vertexCountToRender;
					}
					result = pD3DDevice->DrawPrimitive(primitiveType, indexOfFirstVertexToRender, primitiveCountToRender);
					assert(SUCCEEDED(result));
				}
			}
			return SUCCEEDED(result);
		}

	}
}