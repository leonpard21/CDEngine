// Header Files
//=============

#include "UserInput.h"

#include "Engine/Windows/WindowsIncludes.h"

// Helper Function Declarations
//=============================

namespace
{
	bool IsVirtualKeyPressed( const int i_virtualKeyCode );
	bool IsVirtualKeyReleased( const int i_virtualKeyCode );
}

// Interface
//==========

bool EAE_Engine::UserInput::IsKeyPressed( const int i_virtualKeyCode )
{
	return IsVirtualKeyPressed( i_virtualKeyCode );
}

bool EAE_Engine::UserInput::IsKeyReleased(const int i_virtualKeyCode)
{
	return IsVirtualKeyReleased(i_virtualKeyCode);
}

bool EAE_Engine::UserInput::IsMouseButtonPressed( const int i_virtualButtonCode )
{
	return IsVirtualKeyPressed( i_virtualButtonCode );
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
