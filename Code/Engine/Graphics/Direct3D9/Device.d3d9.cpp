#include "../Common/Device.h"
#include "../Common/Screen.h"
#include "../../UserOutput/UserOutput.h"
#include "General/MemoryOp.h"
#include <d3d9.h>

namespace EAE_Engine
{
	namespace Graphics
	{
		class D3DDevice;
	}
}

namespace
{
	EAE_Engine::Graphics::D3DDevice* s_pDMInstance = nullptr;
	void CleanDMInstance();
	EAE_Engine::Graphics::D3DDevice* GetDMInstance();
}

namespace EAE_Engine
{
	namespace Graphics
	{
		class D3DDevice
		{
		public:
			~D3DDevice()
			{
				ReleaseDM();
			}
			D3DDevice() : _renderingWindow(NULL), _pD3DInterface(NULL), _pD3DDevice(NULL) {}
			IDirect3DDevice9* GetDevice() { return _pD3DDevice; }
			bool InitDM(const HWND i_renderingWindow);
			void ReleaseDM();
			HWND GetHWND() { return _renderingWindow; }
		private:
			bool CreateInterface();
			bool CreateDevice();
			HWND _renderingWindow;
			IDirect3D9* _pD3DInterface;
			IDirect3DDevice9* _pD3DDevice;
		};

		bool D3DDevice::InitDM(const HWND i_renderingWindow)
		{
			_renderingWindow = i_renderingWindow;
			// Initialize the interface to the Direct3D9 library
			if (!CreateInterface())
			{
				return false;
			}
			// Create an interface to a Direct3D device
			if (!CreateDevice())
			{
				return false;
			}
			return true;
		}

		void D3DDevice::ReleaseDM()
		{
			SAFE_RELEASE(_pD3DDevice);
			SAFE_RELEASE(_pD3DInterface);
			_renderingWindow = NULL;
		}

		bool D3DDevice::CreateInterface()
		{
			// D3D_SDK_VERSION is #defined by the Direct3D header files,
			// and is just a way to make sure that everything is up-to-date
			_pD3DInterface = Direct3DCreate9(D3D_SDK_VERSION);
			if (_pD3DInterface)
			{
				return true;
			}
			else
			{
				EAE_Engine::UserOutput::Print("DirectX failed to create a Direct3D9 interface");
				return false;
			}
		}

		bool D3DDevice::CreateDevice()
		{
			const UINT useDefaultDevice = D3DADAPTER_DEFAULT;
			const D3DDEVTYPE useHardwareRendering = D3DDEVTYPE_HAL;
			const DWORD useHardwareVertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
			D3DPRESENT_PARAMETERS presentationParameters = { 0 };
			{
				{
					const unsigned int useRenderingWindowDimensions = 0;
					presentationParameters.BackBufferWidth = useRenderingWindowDimensions;
					presentationParameters.BackBufferHeight = useRenderingWindowDimensions;
				}
				presentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
				presentationParameters.BackBufferCount = 1;
				presentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
				presentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
				presentationParameters.hDeviceWindow = _renderingWindow;
				presentationParameters.Windowed = TRUE;
				presentationParameters.EnableAutoDepthStencil = TRUE;
				presentationParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
				presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
			}
			HRESULT result = _pD3DInterface->CreateDevice(useDefaultDevice, useHardwareRendering,
				_renderingWindow, useHardwareVertexProcessing, &presentationParameters, &_pD3DDevice);
			if (SUCCEEDED(result))
			{
				result = _pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
				result = _pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
				result = _pD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
				return true;
			}
			else
			{
				UserOutput::Print("Direct3D failed to create a Direct3D9 device");
				return false;
			}
		}

///////////////////////////////////Function Interfaces/////////////////////////////

		IDirect3DDevice9* GetD3DDevice()
		{
			D3DDevice* pDeviceManager = GetDMInstance();
			if (!pDeviceManager) return NULL;
			return pDeviceManager->GetDevice();
		}

		bool CreateDevice(const HWND i_renderingWindow) 
		{
			D3DDevice* pDM = GetDMInstance();
			bool result = pDM->InitDM(i_renderingWindow);
			if (!result)
				CleanDMInstance();
			return result;
		}

		void CleanDevice() 
		{
			CleanDMInstance();
		}

		Rect GetScreenRect()
		{
			return GetScreenRect(GetDMInstance()->GetHWND());
		}
	}
}

namespace
{
	void CleanDMInstance()
	{
		SAFE_DELETE(s_pDMInstance);
	}
	EAE_Engine::Graphics::D3DDevice* GetDMInstance()
	{
		if (!s_pDMInstance)
			s_pDMInstance = new EAE_Engine::Graphics::D3DDevice();
		return s_pDMInstance;
	}
}
