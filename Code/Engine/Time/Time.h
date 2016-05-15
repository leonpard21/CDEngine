/*
	This file manages time-related functionality
*/

#ifndef EAE_ENGINE__TIME_H
#define EAE_ENGINE__TIME_H

// Header Files
//=============

#include <string>

// Interface
//==========

namespace EAE_Engine
{
	namespace Time
	{
		// Time
		//-----

		float GetFixedTimeStep();
		int GetFixedUpdateRunTimesOnThisFrame();
		float GetFixedUpdateBlendAlphaOnThisFrame();

		float GetTotalSecondsElapsed();
		float GetSecondsElapsedThisFrame();
		void OnNewFrame();

		// Initialization / Shut Down
		//---------------------------

		bool Initialize( std::string* o_errorMessage = NULL );
	}
}

#endif	// EAE_ENGINE__TIME_H
