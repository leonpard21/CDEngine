#ifndef EAE_ENGINE_DEVICEMANAGER_H
#define EAE_ENGINE_DEVICEMANAGER_H

#include "../Graphics.h"
#if defined( EAEENGINE_PLATFORM_D3D9 )
struct IDirect3DDevice9;
#elif defined( EAEENGINE_PLATFORM_GL )
#include <gl/GL.h>
#endif

namespace EAE_Engine
{
	namespace Graphics
	{
		bool CreateDevice(const HWND i_renderingWindow);
		void CleanDevice();
		struct Rect;
		Rect GetScreenRect();
#if defined( EAEENGINE_PLATFORM_D3D9 )
		IDirect3DDevice9* GetD3DDevice();
#elif defined( EAEENGINE_PLATFORM_GL )
		HDC GetDeviceContext();
#endif
	}
}

#endif//EAE_ENGINE_DEVICEMANAGER_H