#ifndef EAE_ENGINE_MATH_TOOL_H
#define EAE_ENGINE_MATH_TOOL_H
#include <ctime>

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
          t = clip<float>(t, 0.0f, 1.0f);
          return a * (1.0f - t) + b * t;
        }

        // Same as Lerp, but makes sure the values interpolate correctly when they wrap around 360 degrees.
        // The parameter t is clamped to the range[0, 1].
        // Variables a and b are assumed to be in degrees.
        template<typename Type>
        inline Type LerpAngle(Type a, Type b, float t)
        {
          Type result = Lerp<Type>(a, b, t);
          result = result / (Type)360 * (Type)360;
          return result;
        }

		float ConvertDegreesToRadians(const float i_degrees);
	}
}

#endif//EAE_ENGINE_MATH_TOOL_H
