// Header Files
//=============

#include "UserInput.h"

#include "Engine/Windows/WindowsIncludes.h"
#include "Engine/Windows/WindowsFunctions.h"
// Helper Function Declarations
//=============================

namespace
{
	bool IsVirtualKeyPressed( const int i_virtualKeyCode );
	bool IsVirtualKeyReleased( const int i_virtualKeyCode );
}

// Interface
//==========


namespace EAE_Engine
{
	namespace UserInput
	{

		bool IsKeyPressed(const int i_virtualKeyCode)
		{
			return IsVirtualKeyPressed(i_virtualKeyCode);
		}

		bool IsKeyReleased(const int i_virtualKeyCode)
		{
			return IsVirtualKeyReleased(i_virtualKeyCode);
		}

		bool IsMouseButtonPressed(const int i_virtualButtonCode)
		{
			return IsVirtualKeyPressed(i_virtualButtonCode);
		}

		Math::Vector2 GetMousePos()
		{
			POINT point;
			GetCursorPosInActiveWindow(point);
			return Math::Vector2(point.x, point.y);
		}



	}
}

// Helper Function Definitions
//============================

namespace
{
	bool IsVirtualKeyPressed( const int i_virtualKeyCode )
	{
		short keyState = GetAsyncKeyState( i_virtualKeyCode );
		const short isKeyDownMask = ~1;
		return ( keyState & isKeyDownMask ) != 0;
	}

	bool IsVirtualKeyReleased(const int i_virtualKeyCode)
	{
		short keyState = GetAsyncKeyState(i_virtualKeyCode);
		const short isKeyDownMask = ~1;
		return (keyState & isKeyDownMask) == 0;
	}

}
