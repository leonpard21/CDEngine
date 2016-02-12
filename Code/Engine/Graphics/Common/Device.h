#ifndef EAE_ENGINE_DEVICEMANAGER_H
#define EAE_ENGINE_DEVICEMANAGER_H

#include "../Graphics.h"
#include "Screen.h"
#include "Math/Vector.h"
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
		ScreenRect GetScreenRect();
#if defined( EAEENGINE_PLATFORM_D3D9 )
		IDirect3DDevice9* GetD3DDevice();
#elif defined( EAEENGINE_PLATFORM_GL )
		HDC GetDeviceContext();
#endif
		Math::Vector2 ConvertWinToEngineScreenPos(Math::Vector2 winScreenPos);
		Math::Vector2 ConvertEngineToWinScreenPos(Math::Vector2 engineScreenPos);

	}
}

#endif//EAE_ENGINE_DEVICEMANAGER_H