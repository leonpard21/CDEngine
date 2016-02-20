#include "MathTool.h"

namespace EAE_Engine 
{
	namespace Math 
	{
		// Static Data Initialization
		//===========================

		const float Pi = 3.141592654f;

		// Interface
		//==========

		float ConvertDegreesToRadians(const float i_degrees)
		{
			return i_degrees * Pi / 180.0f;
		}

	}
}
