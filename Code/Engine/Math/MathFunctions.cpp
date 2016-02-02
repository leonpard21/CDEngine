// Header Files
//=============

#include "MathFunctions.h"

// Static Data Initialization
//===========================

const float EAE_Engine::Math::Pi = 3.141592654f;

// Interface
//==========

float EAE_Engine::Math::ConvertDegreesToRadians( const float i_degrees )
{
	return i_degrees * Pi / 180.0f;
}
