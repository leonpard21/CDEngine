#include "Device.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		Math::Vector2 ConvertWinToEngineScreenPos(Math::Vector2 winScreenPos)
		{
			ScreenRect screenRect = GetScreenRect();
			Math::Vector2 engineScreenPos = Math::Vector2::Zero;
			engineScreenPos._x = winScreenPos._x;
			engineScreenPos._y = screenRect._height - winScreenPos._y;
			return engineScreenPos;
		}

		Math::Vector2 ConvertEngineToWinScreenPos(Math::Vector2 engineScreenPos) 
		{
			ScreenRect screenRect = GetScreenRect();
			Math::Vector2 winScreenPos = Math::Vector2::Zero;
			winScreenPos._x = engineScreenPos._x;
			winScreenPos._y = screenRect._height - engineScreenPos._y;
			return winScreenPos;
		}

	}
}

