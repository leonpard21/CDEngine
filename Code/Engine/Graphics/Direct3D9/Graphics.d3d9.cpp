// Header Files
//=============
#include "../Common/GraphicsInternal.h"
#include "../Common/Device.h"
#include "../Common/AOSMesh.h"
#include "../Common/Effect.h"
#include "../GraphicsInclude.h"
#include <cassert>
#include <d3d9.h>


namespace EAE_Engine
{
	namespace Graphics
	{
		void CleanBuffer(DWORD clearFlag)
		{
			// Every frame an entirely new image will be created.
			// Before drawing anything, then, the previous image will be erased
			// by "clearing" the image buffer (filling it with a solid color)
			const D3DRECT* subRectanglesToClear = NULL;
			const DWORD subRectangleCount = 0;
			const DWORD clearTheRenderTarget = (clearFlag & ClearFlag::COLOR ? D3DCLEAR_TARGET : 0x0) |
				(clearFlag & ClearFlag::DEPTH ? D3DCLEAR_ZBUFFER : 0x0);// | (clearFlag & ClearFlag::STENCIL ? D3DCLEAR_STENCIL : 0x0);
			D3DCOLOR clearColor;
			{
				//we can also use D3DCOLOR_XRGB(128, 128, 255) for testing;
				clearColor = D3DCOLOR_XRGB(0, 0, 0);
			}
			// We need to reset the zBuffer to be 1.0f for the less-than-or-equal mode.
			// because in the depth buffer, 1.0 is the largest number,
			// so that in next frame the new pixels will not be blocked by the value in last frame.
			const float zBuffer = 1.0f;
			const DWORD noStencilBuffer = 0;
			HRESULT result = GetD3DDevice()->Clear(subRectangleCount, subRectanglesToClear,
				clearTheRenderTarget, clearColor, zBuffer, noStencilBuffer);
			assert(SUCCEEDED(result));
		}

		void SwapBuffer()
		{
			// Everything has been drawn to the "back buffer", which is just an image in memory.
			// In order to display it, the contents of the back buffer must be "presented"
			// (to the front buffer)
			{
				const RECT* noSourceRectangle = NULL;
				const RECT* noDestinationRectangle = NULL;
				const HWND useDefaultWindow = NULL;
				const RGNDATA* noDirtyRegion = NULL;
				HRESULT result = GetD3DDevice()->Present(noSourceRectangle, noDestinationRectangle, useDefaultWindow, noDirtyRegion);
				assert(SUCCEEDED(result));
			}
		}

		bool SetCurrentRenderMesh(const AOSMesh *pAOSMesh)
		{
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			HRESULT result;
			// Set Vertex Declaration
			{
				pD3DDevice->SetVertexDeclaration(pAOSMesh->GetVertexDeclaration());
			}
			// Bind a specific vertex buffer to the device as a data source
			{
				// There can be multiple streams of data feeding the display adaptor at the same time
				const unsigned int streamIndex = 0;
				// It's possible to start streaming data in the middle of a vertex buffer
				const unsigned int bufferOffset = 0;
				// The "stride" defines how large a single vertex is in the stream of data
				const unsigned int bufferStride = pAOSMesh->GetVertexStride();
				result = pD3DDevice->SetStreamSource(streamIndex, pAOSMesh->GetVertexBuffer(), bufferOffset, bufferStride);
				assert(SUCCEEDED(result));
			}
			// Bind a specific index buffer to the device as a data source
			{
				result = pD3DDevice->SetIndices(pAOSMesh->GetIndexBuffer());
				assert(SUCCEEDED(result));
			}
			return SUCCEEDED(result);
		}

		bool BindCurrentEffect(Effect* pEffect)
		{
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			assert(pD3DDevice != nullptr);
			// Set the shaders
			HRESULT result = pD3DDevice->SetVertexShader(pEffect->GetVS());
			assert(SUCCEEDED(result));
			result = pD3DDevice->SetPixelShader(pEffect->GetFS());
			assert(SUCCEEDED(result));
			return SUCCEEDED(result);
		}

		void ChangeEffectRenderState(uint32_t renderState)
		{
			IDirect3DDevice9* pD3DDevice = GetD3DDevice();
			if (renderState & RenderState::Alpha)
			{
				pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			}
			else 
			{
				pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			}
			if (renderState & RenderState::DepthTesting)
			{
				pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
				pD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			}
			else 
			{
				pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			}
			if (renderState & RenderState::DepthWriting)
			{
				pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			}
			else 
			{
				pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			}
			if (renderState & RenderState::FaceCulling)
			{
				pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			}
			else 
			{
				pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			}
		}
	}
}