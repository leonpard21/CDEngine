#include "../Common/AOSMesh.h"
#include "../Common/Device.h"
#include "../Common/AOSMeshOp.h"
#include "UserOutput/UserOutput.h"
#include <d3d9.h>
#include <cassert>

namespace 
{
	HRESULT GetVertexProcessingUsage(IDirect3DDevice9* pDevice, DWORD& o_usage)
	{
		D3DDEVICE_CREATION_PARAMETERS deviceCreationParameters;
		const HRESULT result = pDevice->GetCreationParameters(&deviceCreationParameters);
		if (SUCCEEDED(result))
		{
			DWORD vertexProcessingType = deviceCreationParameters.BehaviorFlags &
				(D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
			o_usage = (vertexProcessingType != D3DCREATE_SOFTWARE_VERTEXPROCESSING) ? 0 : D3DUSAGE_SOFTWAREPROCESSING;
		}
		else
		{
			EAE_Engine::UserOutput::Print("Direct3D failed to get the device's creation parameters");
		}
		return result;
	}

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

		/*
			An example of vertexElement:
			D3DVERTEXELEMENT9 vertexElements[] =
			{
				// Stream: 0, POSITION: 3 floats == 12 bytes, Offset = 0
				{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				// Stream: 0, Normal: 3 floats == 12 bytes, Offsets = 12
				{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
				// Stream: 0, TextureCoordinate0: 2 float == 8 bytes, Offset = 20DDDDDDW
				{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				// Stream: 0, COLOR0: D3DCOLOR == 4 bytes, Offset = 12
				{ 0, 32, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
				// The following marker signals the end of the vertex declaration
				D3DDECL_END()
			};
		*/
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
			// Get stride of the vertx
			_stride = vertexElement._stride;  //the byte offset between consecutive generic vertex attributes
			// Set Mesh PrimitiveType
			_primitiveType = vertexElement._primitiveType;
			bool result = CreateVertexDeclaration(pDevice, pElements, _pVertexDeclaration);
			SAFE_DELETE(pElements);
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
				usage |= D3DUSAGE_WRITEONLY;
			}
			_vertexCount = vertexCount;//record vertexCount of this AOSMesh.
			assert(_stride > 0);//we assert that the _stride has been set.
			const uint32_t bufferSize = _vertexCount * _stride;
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
			assert(_stride > 0);//we assert that the _stride has been set.
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

	}
}