#include "../Common/GraphicsOp.h"
#include "../Common/Device.h"
#include "Memory/Source/New.h"
#include "UserOutput/UserOutput.h"

namespace EAE_Engine
{
	namespace Graphics
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


