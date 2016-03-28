#include "MathTool.h"
#include "Vector.h"
#include <cmath>

namespace EAE_Engine 
{
	namespace Math 
	{
		// Static Data Initialization
		//===========================

		const float Pi = 3.141592654f;
    const float DegreeToRadian = Pi / 180.0f;
    const float RadianToDegree = 180.0f / Pi;

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

    float ConvertRadiansToDegrees(const float i_radians)
    {
      return i_radians * 180.0f / Pi;
    }

    // dot_product = a.x * b.x + a.y * b.y + a.z * b.z = a.len() * b.len * cos(angle)
    // thus: cos(angle) = dot_product / (a.len * b.len)
    // if we just use acos, we cannot get the positive or nagetive of the angle.
    float Radian(const Vector3& from, const Vector3& to) 
    {
      float dot = Vector3::Dot(from, to);
      float sqmagnitude1 = from.SqMagnitude();
      float sqmagnitude2 = to.SqMagnitude();
      float cosValue = dot / std::sqrt(sqmagnitude1 * sqmagnitude2);
      // clamp the cosValue
      cosValue = clamp<float>(cosValue, -1.0f, 1.0f);
      float radian = std::acosf(cosValue);
      return radian;
    }

    float Degree(const Vector3& from, const Vector3& to)
    {
      float angle = ConvertRadiansToDegrees(Radian(from, to));
      return angle;
    }

	}
}
