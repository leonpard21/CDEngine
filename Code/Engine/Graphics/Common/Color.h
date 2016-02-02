#ifndef EAE_ENGINE_GRAPHICS_COLOR
#define EAE_ENGINE_GRAPHICS_COLOR

#include "Math/Vector.h"

namespace EAE_Engine
{
	namespace Graphics
	{
		class Color 
		{
		public:
			Color() {}
			inline Color(float r, float g, float b, float a) : _r(r), _g(g), _b(b), _a(a){}
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
				float _v[4];
				struct 
				{
					float _r;
					float _g;
					float _b;
					float _a;
				};
				Math::Vector4 _value;
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

		const Color Color::Black(0.0f, 0.0f, 0.0f, 1.f);
		const Color Color::White(1.f, 1.f, 1.f, 1.f);
		const Color Color::Red(1.f, 0.f, 0.f, 1.f);
		const Color Color::Green(0.f, 1.f, 0.f, 1.f);
		const Color Color::Blue(0.f, 0.f, 1.f, 1.f);
	}
}


#endif//EAE_ENGINE_GRAPHICS_COLOR