#ifndef MATH_TOOL_H
#define MATH_TOOL_H
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
		T clip(const T& n, const T& lower, const T& upper) {
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
	}
}

#endif//MATH_TOOL_H
