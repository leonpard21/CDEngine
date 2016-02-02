/*
	This file manages user input from the keyboard or mouse
*/

#ifndef EAE6320_USERINPUT_H
#define EAE6320_USERINPUT_H

// Interface
//==========

namespace EAE_Engine
{
	namespace UserInput
	{
		// As of this comment, the codes for these functions can be found at:
		// http://msdn.microsoft.com/en-us/library/dd375731(v=vs.85).aspx
		// (if that doesn't work, try searching for "Virtual Key Codes")

		// For standard letter or number keys, the representative ascii char can be used:
		// IsKeyPressed( 'A' ) or IsKeyPressed( '6' )

		bool IsKeyPressed( const int i_virtualKeyCode );
		bool IsKeyReleased(const int i_virtualKeyCode);
		bool IsMouseButtonPressed( const int i_virtualButtonCode );
	}
}

#endif	// EAE6320_USERINPUT_H
