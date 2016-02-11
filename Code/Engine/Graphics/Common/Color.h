#ifndef EAE_ENGINE_GRAPHICS_COLOR
#define EAE_ENGINE_GRAPHICS_COLOR


#include "Engine/Math/MathTool.h"
#include "Engine/Math/Vector.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		class Color 
		{
		public:
			Color() :
				_r(0), _g(0), _b(0), _a(0)
			{}
			inline Color(float r, float g, float b, float a)
			{
				_r = (uint8_t)(Math::clip<float>(r, 0.0f, 1.0f) * 255.0f);
				_g = (uint8_t)(Math::clip<float>(g, 0.0f, 1.0f) * 255.0f);
				_b = (uint8_t)(Math::clip<float>(b, 0.0f, 1.0f) * 255.0f);
				_a = (uint8_t)(Math::clip<float>(a, 0.0f, 1.0f) * 255.0f);
			}
			inline Color(const Color& i_other) : _r(i_other._r), _g(i_other._g), _b(i_other._b), _a(i_other._a) {}
			inline Color& operator=(const Color& i_other) 
			{
				_r = i_other._r;
				_g = i_other._g;
				_b = i_other._b;
				_a = i_other._a;
				return *this;
			}

			union 
			{
				uint8_t _v[4];
				struct 
				{
					uint8_t _r;
					uint8_t _g;
					uint8_t _b;
					uint8_t _a;
				};
			};
			//float _gamma;
			//float _grayscale; //The grayscale value of the color. (Read Only)
		
		public:
			const static Color Black;
			const static Color White;
			const static Color Red;
			const static Color Green;
			const static Color Blue;
		};

	}
}


#endif//EAE_ENGINE_GRAPHICS_COLOR