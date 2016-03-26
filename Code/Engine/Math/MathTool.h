#ifndef EAE_ENGINE_MATH_TOOL_H
#define EAE_ENGINE_MATH_TOOL_H
#include <ctime>
#include <cmath>

namespace EAE_Engine{
	namespace Math{
		extern const float Pi;

		template<class T>
		const T& max(const T& a, const T& b)
		{
			return (a < b) ? b : a;
		}
		template<class T>
		const T& min(const T& a, const T& b)
		{
			return (a > b) ? b : a;
		}
		template <typename T>
		T clamp(const T& n, const T& lower, const T& upper) {
			return max(lower, min(n, upper));
		}

		//floor a float number
		inline static float Floor(float f)
		{
			int i = static_cast<int>(f);
			return f >= 0 ? static_cast<float>(i) : static_cast<float>(i - 1);
		}

		//return a abs number
		template<typename Type>
		inline static Type Abs(Type t)
		{
			return t >= 0 ? t : -t;
		}

    // Linearly interpolates between a and b by t.
    // The parameter t is clamped to the range[0, 1].
    // t = 0 is a, t = 1 is b
    template<typename Type>
    inline Type Lerp(Type a, Type b, float t)
    {
      t = clamp<float>(t, 0.0f, 1.0f);
      return a * (1.0f - t) + b * t;
    }

    // Same as Lerp, but makes sure the values interpolate correctly when they wrap around 360 degrees.
    // The parameter t is clamped to the range[0, 1].
    // result is the smallest angle on circle, which is in [0, 180]
    inline float LerpDegree(float from, float to, float t)
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

		float ConvertDegreesToRadians(const float i_degrees);
	}
}

#endif//EAE_ENGINE_MATH_TOOL_H
