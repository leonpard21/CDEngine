#include "../Common/SpriteMesh.h"
#include "../Common/Device.h"
#include "../Common/AOSMeshOp.h"


namespace EAE_Engine
{
	namespace Graphics
	{
		bool SpriteMesh::InitSprite(IDirect3DDevice9* pDevice, void* pVertices)
		{
			D3DVERTEXELEMENT9 vertexElements[] = 
			{
				// Stream: 0, POSITION: 2 floats == 8 bytes, Offset = 0
				{ 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
				// Stream: 0, TextureCoordinate0: 2 float == 8 bytes, Offset = 8
				{ 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
				// Stream: 0, COLOR0: D3DCOLOR == 4 bytes, Offset = 16
				{ 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
				// The following marker signals the end of the vertex declaration
				D3DDECL_END()
			};
			bool result = CreateVertexDeclaration(pDevice, vertexElements, _pVertexDeclaration);
			// The usage tells Direct3D how this vertex buffer will be used
			DWORD usage = 0;
			{
				// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
				const HRESULT result = GetVertexProcessingUsage(pDevice, usage);
				if (FAILED(result))
				{
					return false;
				}
				//we use D3DUSAGE_DYNAMIC the user can change the contents during run time 
				usage |= ( D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY );
			}
			const uint32_t bufferSize = 4 * sizeof(SpriteVertex);
			result = CreateVertexBuffer(_pVertexBuffer, pDevice, usage, bufferSize);
			result = FillVertexBuffer(_pVertexBuffer, pDevice, pVertices, bufferSize);
			return result;
		}

		void SpriteMesh::ReleaseSprite()
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

	}
}