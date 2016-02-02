#ifndef EAEENGINE_Graphics_SCREEN_H
#define EAEENGINE_Graphics_SCREEN_H
#include "Engine/Windows/WindowsFunctions.h"

namespace EAE_Engine
{
	namespace Graphics 
	{
		struct Rect 
		{
			float _x, _y;
			float _width, _height;
			Rect() = default;
		};

		Rect GetScreenRect(const HWND i_renderingWindow);
	}
};

#endif//EAEENGINE_Graphics_SCREEN_H

