#include "MathTool.h"
#include <cmath>

namespace EAE_Engine 
{
	namespace Math 
	{
		// Static Data Initialization
		//===========================

		const float Pi = 3.141592654f;

		// Interface
		//==========

    // Same as Lerp, but makes sure the values interpolate correctly when they wrap around 360 degrees.
    // The parameter t is clamped to the range[0, 1].
    // result is the smallest angle on circle, which is in [0, 180]
    float LerpDegree(float from, float to, float t)
    {
      t = clamp<float>(t, 0.0f, 1.0f);
      // get the difference no matter how many loops the differnce is.
      // range will be [-360,  360].
      float differenceInRange = std::fmodf(to - from, 360.0f);
      // +540 and %360 convert the differce to [180, 360]
      // then -180 so converts the result to [0, 180]
      float shortest_difference = std::fmodf(differenceInRange + 540.0f, 360.0f) - 180.0f;
      return shortest_difference * t;
    }


		float ConvertDegreesToRadians(const float i_degrees)
		{
			return i_degrees * Pi / 180.0f;
		}

	}
}
