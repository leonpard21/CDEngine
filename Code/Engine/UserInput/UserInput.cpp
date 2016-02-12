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
		void Input::Init()
		{
			for (int i = 0; i < 0xFE; ++i)
			{
				_keyStates[i] = KeyState::OnReleasing;
			}
		}

		void Input::Update() 
		{
			for (int i = 0; i < 0xFE; ++i)
			{
				const int virtualKeyCode = i;
				bool pressing = false;
				if (IsVirtualKeyPressed(virtualKeyCode))
					pressing = true;
				// Update the KeyStates Based on whether we have pressed the key or not.
				if (pressing) 
				{
					switch (_keyStates[i])
					{
					case KeyState::OnReleasing:
					case KeyState::OnReleased:
						_keyStates[i] = KeyState::OnPressed;
						break;
					case KeyState::OnPressing:
					case KeyState::OnPressed:
						_keyStates[i] = KeyState::OnPressing;
						break;
					}
				}
				else 
				{
					switch (_keyStates[i])
					{
					case KeyState::OnReleasing:
					case KeyState::OnReleased:
						_keyStates[i] = KeyState::OnReleasing;
						break;
					case KeyState::OnPressing:
					case KeyState::OnPressed:
						_keyStates[i] = KeyState::OnReleased;
						break;
					}
				}
			}
		}

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
			return Math::Vector2((float)point.x, (float)point.y);
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
		const short isKeyDownMask = ~1;// ~ is Bitwise NOT
		return ( keyState & isKeyDownMask ) != 0;
	}

	bool IsVirtualKeyReleased(const int i_virtualKeyCode)
	{
		short keyState = GetAsyncKeyState(i_virtualKeyCode);
		const short isKeyDownMask = ~1;// ~ is Bitwise NOT
		return (keyState & isKeyDownMask) == 0;
	}

}
