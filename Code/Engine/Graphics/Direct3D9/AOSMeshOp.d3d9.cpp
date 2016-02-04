#include "../Common/AOSMeshOp.h"
#include "../Common/Device.h"
#include "Memory/Source/New.h"
#include "UserOutput/UserOutput.h"

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


		bool CreateVertexDeclaration(IDirect3DDevice9* pDevice, D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9*& o_pVertexDeclaration)
		{
			// Initialize the vertex format
			HRESULT result = pDevice->CreateVertexDeclaration(pVertexElements, &o_pVertexDeclaration);
			if (SUCCEEDED(result))
			{
				result = pDevice->SetVertexDeclaration(o_pVertexDeclaration);
				if (FAILED(result))
				{
					EAE_Engine::UserOutput::Print("Direct3D failed to set the vertex declaration");
					return false;
				}
			}
			else
			{
				EAE_Engine::UserOutput::Print("Direct3D failed to create a Direct3D9 vertex declaration");
				return false;
			}
			return true;
		}

		// Create a vertex buffer
		bool CreateVertexBuffer(IDirect3DVertexBuffer9*& o_pVertexBuffer, IDirect3DDevice9* pDevice, const DWORD usage, const uint32_t bufferSize)
		{
			// We will define our own vertex format
			const DWORD useSeparateVertexDeclaration = 0;
			// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* const notUsed = NULL;
			const HRESULT result = pDevice->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
				&o_pVertexBuffer, notUsed);
			if (FAILED(result))
			{
				EAE_Engine::UserOutput::Print("Direct3D failed to create a vertex buffer");
				return false;
			}
			return true;
		}

		// Fill the vertex buffer with the triangle's vertices
		bool FillVertexBuffer(IDirect3DVertexBuffer9*& io_pVertexBuffer, IDirect3DDevice9* pDevice, void* pVertices, const uint32_t bufferSize)
		{
			// Before the vertex buffer can be changed it must be "locked"
			void* pTargetVertexData;
			{
				const unsigned int lockEntireBuffer = 0;
				const DWORD useDefaultLockingBehavior = 0;
				const HRESULT result = io_pVertexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&pTargetVertexData), useDefaultLockingBehavior);
				if (FAILED(result))
				{
					EAE_Engine::UserOutput::Print("Direct3D failed to lock the vertex buffer");
					return false;
				}
			}
			// Fill the buffer
			{
				CopyMem(reinterpret_cast<const uint8_t*>(pVertices), reinterpret_cast<uint8_t*>(pTargetVertexData), bufferSize);
			}
			// The buffer must be "unlocked" before it can be used
			{
				const HRESULT result = io_pVertexBuffer->Unlock();
				if (FAILED(result))
				{
					EAE_Engine::UserOutput::Print("Direct3D failed to unlock the vertex buffer");
					return false;
				}
			}
			return true;
		}

		//Create Index Buffer
		bool CreateIndexBuffer(IDirect3DIndexBuffer9*& o_pIndexBuffer, IDirect3DDevice9* pDevice, const DWORD usage, const uint32_t bufferSize)
		{
			// We'll use 32-bit indices in this class to keep things simple
			// (i.e. every index will be a 32 bit unsigned integer)
			const D3DFORMAT format = D3DFMT_INDEX32;
			// Place the index buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* notUsed = NULL;
			const HRESULT result = pDevice->CreateIndexBuffer(bufferSize, usage, format, useDefaultPool,
				&o_pIndexBuffer, notUsed);
			if (FAILED(result))
			{
				EAE_Engine::UserOutput::Print("Direct3D failed to create an index buffer");
				return false;
			}
			return true;
		}

		// Fill the index buffer with the triangles' connectivity data
		bool FillIndexBuffer(IDirect3DIndexBuffer9*& io_pIndexBuffer, IDirect3DDevice9* pDevice, void* pIndices, const uint32_t bufferSize) 
		{
			// Before the index buffer can be changed it must be "locked"
			uint32_t* pTargetIndexData;
			{
				const unsigned int lockEntireBuffer = 0;
				const DWORD useDefaultLockingBehavior = 0;
				const HRESULT result = io_pIndexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&pTargetIndexData), useDefaultLockingBehavior);
				if (FAILED(result))
				{
					EAE_Engine::UserOutput::Print("Direct3D failed to lock the index buffer");
					return false;
				}
			}
			// Fill the buffer
			{
				CopyMem(reinterpret_cast<const uint8_t*>(pIndices), reinterpret_cast<uint8_t*>(pTargetIndexData), bufferSize);
			}
			// The buffer must be "unlocked" before it can be used
			{
				const HRESULT result = io_pIndexBuffer->Unlock();
				if (FAILED(result))
				{
					EAE_Engine::UserOutput::Print("Direct3D failed to unlock the index buffer");
					return false;
				}
			}
			return true;
		}

	}
}


