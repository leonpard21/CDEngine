#include "ImageMesh.h"
#include "Device.h"
#include "GraphicsOp.h"


namespace EAE_Engine
{
	namespace Graphics
	{
#if defined(EAEENGINE_PLATFORM_D3D9)
		bool ImageMesh::Init(void* pVertices)
		{
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (!pD3DDevice) return false;

			D3DVERTEXELEMENT9 vertexElements[] =
			{
				// Stream: 0, POSITION: 3 floats == 12 bytes, Offset = 0
				{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				// Stream: 0, TextureCoordinate0: 2 float == 8 bytes, Offset = 8
				{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				// Stream: 0, COLOR0: D3DCOLOR == 4 bytes, Offset = 16
				{ 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
				// The following marker signals the end of the vertex declaration
				D3DDECL_END()
			};
			bool result = CreateVertexDeclaration(pD3DDevice, vertexElements, _pVertexDeclaration);
			// The usage tells Direct3D how this vertex buffer will be used
			DWORD usage = 0;
			{
				// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
				const HRESULT result = GetVertexProcessingUsage(pD3DDevice, usage);
				if (FAILED(result))
				{
					return false;
				}
				//we use D3DUSAGE_DYNAMIC the user can change the contents during run time 
				usage |= (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);
			}
			const uint32_t bufferSize = 4 * sizeof(ImageVertex);
			result = CreateVertexBuffer(_pVertexBuffer, pD3DDevice, usage, bufferSize);
			result = FillVertexBuffer(_pVertexBuffer, pD3DDevice, pVertices, bufferSize);
			return result;
		}

		void ImageMesh::Release()
		{
			IDirect3DDevice9* pDevice = GetD3DDevice();
			if (_pVertexBuffer)
			{
				_pVertexBuffer->Release();
				_pVertexBuffer = NULL;
			}
			if (_pVertexDeclaration)
			{
				pDevice->SetVertexDeclaration(NULL);
				_pVertexDeclaration->Release();
				_pVertexDeclaration = NULL;
			}
		}

		void ImageMesh::ChangeVertexBuffer(void* pVertices)
		{
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (!pD3DDevice) 
				return;
			const uint32_t bufferSize = 4 * sizeof(ImageVertex);
			FillVertexBuffer(_pVertexBuffer, pD3DDevice, pVertices, bufferSize);
		}

		///////////////////////////////////////////////Internal Function////////////////////////////////////////////////////
		bool RenderImageMeshInternal(ImageMesh *pImageMesh)
		{
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (!pD3DDevice) return false;

			HRESULT result = true;
			// Render objects from the current streams
			{
				// We are using triangles as the "primitive" type,
				// and we have defined the vertex buffer as a triangle list
				// (meaning that every triangle is defined by three vertices)
				const D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLEFAN;
				// It's possible to start rendering primitives in the middle of the stream
				const unsigned int indexOfFirstVertexToRender = 0;
				// We are drawing the AOSMesh
				{
					const unsigned int vertexCountToRender = 2;
					result = pD3DDevice->DrawPrimitive(primitiveType, indexOfFirstVertexToRender, vertexCountToRender);
					assert(SUCCEEDED(result));
				}
			}
			return SUCCEEDED(result);
		}

#elif defined(EAEENGINE_PLATFORM_GL)

#endif
	}
}

