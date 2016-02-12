/*
	This file manages user input from the keyboard or mouse
*/

#ifndef EAE_ENGINE_USERINPUT_H
#define EAE_ENGINE_USERINPUT_H

#include <map>
#include "Engine/Math/Vector.h"
#include "Engine/General/Singleton.hpp"
// Interface
//==========


namespace EAE_Engine
{
	namespace UserInput
	{
		enum KeyState 
		{
			OnPressed,
			OnPressing,
			OnReleased,
			OnReleasing,
		};

		class Input : public Singleton<Input>
		{
		public:
			void Init();
			void Update();
			KeyState& GetKeyState(const int KeyCode) { return _keyStates[KeyCode]; }

		private:
			std::map<const int, KeyState> _keyStates;
		};

		// As of this comment, the codes for these functions can be found at:
		// http://msdn.microsoft.com/en-us/library/dd375731(v=vs.85).aspx
		// (if that doesn't work, try searching for "Virtual Key Codes")

		// For standard letter or number keys, the representative ascii char can be used:
		// IsKeyPressed( 'A' ) or IsKeyPressed( '6' )

		bool IsKeyPressed( const int i_virtualKeyCode );
		bool IsKeyReleased(const int i_virtualKeyCode);
		bool IsMouseButtonPressed( const int i_virtualButtonCode );
		Math::Vector2 GetMousePos();

	}
}

#endif	// EAE_ENGINE_USERINPUT_H
